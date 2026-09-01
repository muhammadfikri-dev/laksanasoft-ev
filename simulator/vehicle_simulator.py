#!/usr/bin/env python3
"""
============================================================================
LAKSANASOFT-EV: HARDWARE-IN-THE-LOOP (HIL) VEHICLE DYNAMICS SIMULATOR
Smart ADAS Edition (120+ km/h, 6-DOF IMU Lean Angle, TCS & Dual-Battery BMS)
(c) 2026 PT. Laksana Digital Industri
============================================================================
"""

import time
import json
import math
import sys
import paho.mqtt.client as mqtt

# Parameter Fisik Kendaraan Laksanasoft-EV (120 km/jam High-Speed Setup)
VIN = "LAKSANAEV2026ID001"
VEHICLE_MASS = 210.0      # Massa Total Kendaraan + Rider (kg)
WHEEL_RADIUS = 0.267      # Jari-jari roda belakang 14 inci (m)
GEAR_RATIO = 3.0          # Rasio reduksi gir 15T : 45T
AIR_DENSITY = 1.225       # Densitas udara (kg/m^3)
DRAG_COEFF = 0.62         # Cd
FRONTAL_AREA = 0.68       # Luas penampang depan (m^2)
ROLLING_RESISTANCE = 0.015# Crr

# Parameter Dual-Battery & Motor Upgrade (18kW Peak)
BATT_CAPACITY_WH = 4320.0 # Dual 72V 30Ah Packs = 72V 60Ah (4.32 kWh)
BATT_NOMINAL_VOLT = 72.0
MAX_MOTOR_RPM = 6200.0    # Putaran motor maksimal dengan Field Weakening
MAX_MOTOR_TORQUE = 72.0   # Nm pada shaft motor QS 138 90H V3

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
        self.keyless_unlocked = True
        self.tcs_active = False
        
        # Sensor Inersia IMU 6-DOF
        self.lean_angle_deg = 0.0
        self.pitch_angle_deg = 0.0
        
        # Dual Baterai Pack
        self.pack_a_soc = 94.0
        self.pack_b_soc = 92.0
        self.soc = 93.0
        self.pack_voltage = 76.8
        self.pack_current = 0.0
        self.power_watts = 0.0
        self.energy_used_wh = 50.0
        
        # Jarak
        self.odometer_km = 1420.8
        self.trip_km = 18.4
        
        # Termal (°C)
        self.motor_temp = 44.0
        self.inverter_temp = 39.5
        self.bms_temp = 32.0
        
        # State mesin waktu
        self.uptime_sec = 1800

    def update_physics(self, dt_sec=0.1):
        self.uptime_sec += dt_sec
        
        # Interlock check
        if self.side_stand_down:
            self.throttle_pct = 0.0
            self.state = "STATE_STANDBY"
        
        # Simulasi Dinamika Kemiringan Menikung (Lean Angle)
        if self.speed_kmh > 20.0 and not self.brake_engaged:
            self.lean_angle_deg = math.sin(self.uptime_sec * 0.8) * min(38.0, self.speed_kmh * 0.32)
        else:
            self.lean_angle_deg = 0.0

        # Gaya dorong motor (Tractive Force)
        if self.brake_engaged:
            self.state = "STATE_REGEN" if self.speed_kmh > 5.0 and self.soc < 95 else "STATE_DRIVE"
            motor_torque = 0.0
            regen_force = 450.0 if self.state == "STATE_REGEN" else 0.0
            friction_brake_force = 850.0
            self.tcs_active = False
        else:
            self.state = "STATE_DRIVE"
            regen_force = 0.0
            friction_brake_force = 0.0
            
            # Kurva torsi motor berdasarkan mode
            mode_multiplier = 1.0 if self.mode == "SPORT" else (0.70 if self.mode == "URBAN" else 0.35)
            motor_torque = (self.throttle_pct / 100.0) * MAX_MOTOR_TORQUE * mode_multiplier
            
            # Deteksi & Intervensi TCS jika akselerasi terlalu tajam saat miring
            if self.throttle_pct > 85.0 and abs(self.lean_angle_deg) > 25.0:
                self.tcs_active = True
                motor_torque *= 0.60 # Reduksi torsi TCS
            else:
                self.tcs_active = False

            # Field weakening compensation untuk menjaga torsi tetap kuat hingga 6000 RPM
            if self.motor_rpm > 4200:
                motor_torque *= max(0.45, 1.0 - ((self.motor_rpm - 4200) / 3200.0))

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

        # Kalkulasi RPM Motor pada rasio 1:3.0
        wheel_rps = self.speed_ms / (2 * math.pi * WHEEL_RADIUS)
        self.motor_rpm = min(MAX_MOTOR_RPM, wheel_rps * GEAR_RATIO * 60.0)

        # Kalkulasi Daya Listrik (Power Watts) & Baterai
        mech_power = tractive_force * self.speed_ms
        if self.state == "STATE_REGEN":
            elec_power = - (regen_force * self.speed_ms * 0.75)
        else:
            elec_power = (mech_power / 0.86) + 40.0
        
        self.power_watts = elec_power
        self.pack_current = elec_power / self.pack_voltage
        
        # Konsumsi Dual Baterai
        energy_delta_wh = (elec_power * (dt_sec / 3600.0))
        self.energy_used_wh += max(0.0, energy_delta_wh)
        self.pack_a_soc = max(5.0, min(100.0, self.pack_a_soc - (energy_delta_wh / (BATT_CAPACITY_WH/2) * 50.0)))
        self.pack_b_soc = max(5.0, min(100.0, self.pack_b_soc - (energy_delta_wh / (BATT_CAPACITY_WH/2) * 50.0)))
        self.soc = (self.pack_a_soc + self.pack_b_soc) / 2.0
        
        # Estimasi Tegangan Pack berbasis SOC & Load Drop
        open_circuit_volt = 64.0 + (self.soc / 100.0) * 18.0
        self.pack_voltage = open_circuit_volt - (self.pack_current * 0.04)

        # Odometer
        dist_km = (self.speed_ms * dt_sec) / 1000.0
        self.odometer_km += dist_km
        self.trip_km += dist_km

        # Termal Dinamis
        if self.power_watts > 8000:
            self.motor_temp += 0.08 * dt_sec
            self.inverter_temp += 0.06 * dt_sec
        elif self.power_watts > 3000:
            self.motor_temp += 0.03 * dt_sec
            self.inverter_temp += 0.02 * dt_sec
        else:
            self.motor_temp = max(38.0, self.motor_temp - 0.015 * dt_sec)
            self.inverter_temp = max(34.0, self.inverter_temp - 0.015 * dt_sec)

    def get_telemetry_dict(self):
        remaining_wh = (self.soc / 100.0) * BATT_CAPACITY_WH
        range_km = remaining_wh / 35.0

        return {
            "vin": VIN,
            "timestamp": int(time.time()),
            "system": {
                "state": self.state,
                "mode": self.mode,
                "ready": True,
                "side_stand": self.side_stand_down,
                "brake": self.brake_engaged,
                "keyless_unlocked": self.keyless_unlocked,
                "tcs_active": self.tcs_active
            },
            "dynamics": {
                "speed_kmh": round(self.speed_kmh, 1),
                "motor_rpm": int(self.motor_rpm),
                "throttle_pct": round(self.throttle_pct, 1),
                "odometer_km": round(self.odometer_km, 2),
                "trip_km": round(self.trip_km, 2)
            },
            "imu": {
                "lean_angle_deg": round(self.lean_angle_deg, 1),
                "pitch_angle_deg": round(self.pitch_angle_deg, 1)
            },
            "energy": {
                "pack_voltage": round(self.pack_voltage, 1),
                "pack_current": round(self.pack_current, 1),
                "power_watts": round(self.power_watts, 1),
                "soc": int(self.soc),
                "soh": 99,
                "range_km": round(range_km, 1),
                "pack_a_soc": round(self.pack_a_soc, 1),
                "pack_b_soc": round(self.pack_b_soc, 1)
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
    print("=" * 75)
    print(" ⚡ LAKSANASOFT-EV: SMART ADAS SIMULATOR (120+ KM/H, 6-DOF IMU, TCS, DUAL BMS)")
    print(f" 📡 Menghubungkan ke MQTT Broker: {MQTT_BROKER}:{MQTT_PORT}")
    print(f" 📦 Topik Telemetri: {MQTT_TOPIC}")
    print("=" * 75)

    client = mqtt.Client()
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_start()
        print(" [OK] Terhubung ke Broker MQTT!")
    except Exception as e:
        print(f" [PERINGATAN] Gagal terhubung ke MQTT: {e}")

    sim = ElectricMotorcycleSimulator()
    step = 0

    try:
        while True:
            # Simulasi Siklus Berkendara Kecepatan Tinggi & Tikungan (TCS & Lean Angle Demo)
            cycle_phase = (step // 60) % 5
            if cycle_phase == 0:
                # Akselerasi Halus (0 - 60 km/jam)
                sim.throttle_pct = min(50.0, sim.throttle_pct + 1.5)
                sim.brake_engaged = False
            elif cycle_phase == 1:
                # Cruising Kota & Menikung (60 - 75 km/jam)
                sim.throttle_pct = 55.0
                sim.brake_engaged = False
            elif cycle_phase == 2:
                # FULL THROTTLE BOOST MENUJU 120+ KM/JAM
                sim.throttle_pct = 100.0
                sim.brake_engaged = False
            elif cycle_phase == 3:
                # Pengereman Regeneratif Berkecepatan Tinggi
                sim.throttle_pct = 0.0
                sim.brake_engaged = True
            elif cycle_phase == 4:
                # Idle Berhenti
                sim.throttle_pct = 0.0
                sim.brake_engaged = False

            sim.update_physics(dt_sec=0.1)
            payload = sim.get_telemetry_dict()

            if client.is_connected():
                client.publish(MQTT_TOPIC, json.dumps(payload))

            # Cetak status live di terminal
            sys.stdout.write(
                f"\r\033[K[SMART EV] Spd: {payload['dynamics']['speed_kmh']:>5.1f} km/h | "
                f"RPM: {payload['dynamics']['motor_rpm']:>4} | "
                f"Pwr: {payload['energy']['power_watts']/1000:>4.1f} kW | "
                f"Lean: {payload['imu']['lean_angle_deg']:>5.1f}° | "
                f"TCS: {'AKTIF' if payload['system']['tcs_active'] else 'OFF':>5} | "
                f"Dual-SOC: {payload['energy']['pack_a_soc']:.0f}%/{payload['energy']['pack_b_soc']:.0f}%"
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
