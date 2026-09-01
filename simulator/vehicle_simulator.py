#!/usr/bin/env python3
"""
============================================================================
LAKSANASOFT-EV: HARDWARE-IN-THE-LOOP (HIL) VEHICLE DYNAMICS SIMULATOR
Simulator Fisika Kendaraan Listrik Modular Berbasis Model Matematis
(c) 2026 PT. Laksana Digital Industri
============================================================================
"""

import time
import json
import math
import random
import sys
import paho.mqtt.client as mqtt

# Parameter Fisik Kendaraan Laksanasoft-EV
VIN = "LAKSANAEV2026ID001"
VEHICLE_MASS = 210.0      # Massa Total Kendaraan + Rider (kg)
WHEEL_RADIUS = 0.267      # Jari-jari roda belakang 14 inci (m)
GEAR_RATIO = 4.2          # Rasio reduksi gir rantai (14T : 58T)
AIR_DENSITY = 1.225       # Densitas udara (kg/m^3)
DRAG_COEFF = 0.65         # Cd
FRONTAL_AREA = 0.75       # Luas penampang depan (m^2)
ROLLING_RESISTANCE = 0.015# Crr

# Parameter Baterai & Motor
BATT_CAPACITY_WH = 3240.0 # 72V 45Ah = 3.24 kWh
BATT_NOMINAL_VOLT = 72.0
MAX_MOTOR_RPM = 4800.0
MAX_MOTOR_TORQUE = 45.0   # Nm pada shaft motor

MQTT_BROKER = "broker.emqx.io"
MQTT_PORT = 1883
MQTT_TOPIC = f"laksanasoft/ev/v1/{VIN}/telemetry"

class ElectricMotorcycleSimulator:
    def __init__(self):
        self.state = "STATE_DRIVE"
        self.mode = "SPORT"
        self.speed_kmh = 0.0
        self.speed_ms = 0.0
        self.motor_rpm = 0
        self.throttle_pct = 0.0
        self.brake_engaged = False
        self.side_stand_down = False
        
        # Baterai
        self.soc = 92.0
        self.pack_voltage = 75.6
        self.pack_current = 0.0
        self.power_watts = 0.0
        self.energy_used_wh = 120.0
        
        # Jarak
        self.odometer_km = 1420.8
        self.trip_km = 18.4
        
        # Termal (°C)
        self.motor_temp = 42.0
        self.inverter_temp = 38.0
        self.bms_temp = 31.5
        
        # State mesin waktu
        self.uptime_sec = 1800

    def update_physics(self, dt_sec=0.1):
        self.uptime_sec += dt_sec
        
        # Interlock check
        if self.side_stand_down:
            self.throttle_pct = 0.0
            self.state = "STATE_STANDBY"
        
        # Gaya dorong motor (Tractive Force)
        if self.brake_engaged:
            self.state = "STATE_REGEN" if self.speed_kmh > 5.0 and self.soc < 95 else "STATE_DRIVE"
            motor_torque = 0.0
            regen_force = 250.0 if self.state == "STATE_REGEN" else 0.0
            friction_brake_force = 500.0
        else:
            self.state = "STATE_DRIVE"
            regen_force = 0.0
            friction_brake_force = 0.0
            
            # Kurva torsi motor berdasarkan mode
            mode_multiplier = 1.0 if self.mode == "SPORT" else (0.75 if self.mode == "URBAN" else 0.4)
            motor_torque = (self.throttle_pct / 100.0) * MAX_MOTOR_TORQUE * mode_multiplier
            
            # Field weakening / torque drop pada RPM tinggi
            if self.motor_rpm > 3500:
                motor_torque *= max(0.2, 1.0 - ((self.motor_rpm - 3500) / 2000.0))

        wheel_torque = motor_torque * GEAR_RATIO
        tractive_force = wheel_torque / WHEEL_RADIUS
        
        # Gaya hambat aerodinamika & rolling resistance
        f_rr = VEHICLE_MASS * 9.81 * ROLLING_RESISTANCE
        f_aero = 0.5 * AIR_DENSITY * DRAG_COEFF * FRONTAL_AREA * (self.speed_ms ** 2)
        
        net_force = tractive_force - f_rr - f_aero - regen_force - friction_brake_force
        if self.speed_ms <= 0.05 and net_force < 0:
            net_force = 0.0
            self.speed_ms = 0.0

        # Akselerasi (F = m * a)
        accel = net_force / VEHICLE_MASS
        self.speed_ms = max(0.0, self.speed_ms + (accel * dt_sec))
        self.speed_kmh = self.speed_ms * 3.6

        # Kalkulasi RPM Motor
        wheel_rps = self.speed_ms / (2 * math.pi * WHEEL_RADIUS)
        self.motor_rpm = min(MAX_MOTOR_RPM, wheel_rps * GEAR_RATIO * 60.0)

        # Kalkulasi Daya Listrik (Power Watts) & Baterai
        mech_power = tractive_force * self.speed_ms
        if self.state == "STATE_REGEN":
            elec_power = - (regen_force * self.speed_ms * 0.7) # Efisiensi regen ~70%
        else:
            elec_power = (mech_power / 0.88) + 35.0 # Efisiensi motor/inverter ~88% + 35W sistem bantu
        
        self.power_watts = elec_power
        self.pack_current = elec_power / self.pack_voltage
        
        # Konsumsi Baterai (Ah & SOC)
        energy_delta_wh = (elec_power * (dt_sec / 3600.0))
        self.energy_used_wh += max(0.0, energy_delta_wh)
        self.soc = max(5.0, min(100.0, self.soc - (energy_delta_wh / BATT_CAPACITY_WH * 100.0)))
        
        # Estimasi Tegangan Pack berbasis SOC & Load Drop (Internal Resistance 0.08 Ohm)
        open_circuit_volt = 64.0 + (self.soc / 100.0) * 18.0 # 64V (kosong) - 82V (penuh)
        self.pack_voltage = open_circuit_volt - (self.pack_current * 0.08)

        # Odometer
        dist_km = (self.speed_ms * dt_sec) / 1000.0
        self.odometer_km += dist_km
        self.trip_km += dist_km

        # Termal Dinamis
        if self.power_watts > 2000:
            self.motor_temp += 0.04 * dt_sec
            self.inverter_temp += 0.03 * dt_sec
        else:
            self.motor_temp = max(38.0, self.motor_temp - 0.01 * dt_sec)
            self.inverter_temp = max(34.0, self.inverter_temp - 0.01 * dt_sec)

    def get_telemetry_dict(self):
        remaining_wh = (self.soc / 100.0) * BATT_CAPACITY_WH
        range_km = remaining_wh / 30.0 # 30 Wh/km

        return {
            "vin": VIN,
            "timestamp": int(time.time()),
            "system": {
                "state": self.state,
                "mode": self.mode,
                "ready": True,
                "side_stand": self.side_stand_down,
                "brake": self.brake_engaged
            },
            "dynamics": {
                "speed_kmh": round(self.speed_kmh, 1),
                "motor_rpm": int(self.motor_rpm),
                "throttle_pct": round(self.throttle_pct, 1),
                "odometer_km": round(self.odometer_km, 2),
                "trip_km": round(self.trip_km, 2)
            },
            "energy": {
                "pack_voltage": round(self.pack_voltage, 1),
                "pack_current": round(self.pack_current, 1),
                "power_watts": round(self.power_watts, 1),
                "soc": int(self.soc),
                "soh": 99,
                "range_km": round(range_km, 1)
            },
            "temperatures": {
                "motor_c": round(self.motor_temp, 1),
                "inverter_c": round(self.inverter_temp, 1),
                "bms_c": round(self.bms_temp, 1)
            },
            "diagnostics": {
                "fault_code": 0,
                "fault_msg": "NORMAL"
            }
        }

def run_simulation():
    print("=" * 60)
    print(" ⚡ LAKSANASOFT-EV: HARDWARE-IN-THE-LOOP SIMULATOR")
    print(f" 📡 Menghubungkan ke MQTT Broker: {MQTT_BROKER}:{MQTT_PORT}")
    print(f" 📦 Topik Telemetri: {MQTT_TOPIC}")
    print("=" * 60)

    client = mqtt.Client()
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_start()
        print(" [OK] Terhubung ke Broker MQTT!")
    except Exception as e:
        print(f" [PERINGATAN] Gagal terhubung ke MQTT: {e}")
        print(" Simulator akan tetap berjalan dan menampilkan log lokal.")

    sim = ElectricMotorcycleSimulator()
    step = 0

    try:
        while True:
            # Simulasi Siklus Berkendara Otomatis (Drive Cycle)
            cycle_phase = (step // 50) % 5
            if cycle_phase == 0:
                # Akselerasi Halus
                sim.throttle_pct = min(60.0, sim.throttle_pct + 2.0)
                sim.brake_engaged = False
            elif cycle_phase == 1:
                # Cruising Kecepatan Konstan
                sim.throttle_pct = 45.0
                sim.brake_engaged = False
            elif cycle_phase == 2:
                # Akselerasi Penuh (Sport Boost)
                sim.throttle_pct = min(95.0, sim.throttle_pct + 3.0)
                sim.brake_engaged = False
            elif cycle_phase == 3:
                # Pengereman Regeneratif (Deselerasi)
                sim.throttle_pct = 0.0
                sim.brake_engaged = True
            elif cycle_phase == 4:
                # Berhenti di Lampu Merah (Idle)
                sim.throttle_pct = 0.0
                sim.brake_engaged = False

            sim.update_physics(dt_sec=0.1)
            payload = sim.get_telemetry_dict()

            if client.is_connected():
                client.publish(MQTT_TOPIC, json.dumps(payload))

            # Cetak status live di terminal
            sys.stdout.write(
                f"\r\033[K[EV TELEMETRY] Speed: {payload['dynamics']['speed_kmh']:>4.1f} km/h | "
                f"RPM: {payload['dynamics']['motor_rpm']:>4} | "
                f"Power: {payload['energy']['power_watts']:>6.1f} W | "
                f"SOC: {payload['energy']['soc']:>3}% | "
                f"Volt: {payload['energy']['pack_voltage']:>4.1f}V | "
                f"State: {payload['system']['state']}"
            )
            sys.stdout.flush()

            step += 1
            time.sleep(0.1)

    except KeyboardInterrupt:
        print("\n\n[SIMULATOR] Dihentikan oleh pengguna.")
        client.loop_stop()
        client.disconnect()

if __name__ == "__main__":
    run_simulation()
