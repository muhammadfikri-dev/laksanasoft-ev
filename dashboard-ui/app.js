/**
 * ============================================================================
 * LAKSANASOFT-EV DASHBOARD HMI CLUSTER CONTROLLER (SMART EDITION 2.0)
 * Scale 160 km/h | 18kW Power | 6-Axis Lean Meter | Dual-Battery BMS
 * (c) 2026 PT. Laksana Digital Industri
 * ============================================================================
 */

// DOM Elements
const elSpeedDisplay = document.getElementById('speedDisplay');
const elSpeedArcFill = document.getElementById('speedArcFill');
const elSocValue = document.getElementById('socValue');
const elSocRing = document.getElementById('socRing');
const elRangeValue = document.getElementById('rangeValue');

const elPackABar = document.getElementById('packABar');
const elPackAPct = document.getElementById('packAPct');
const elPackBBar = document.getElementById('packBBar');
const elPackBPct = document.getElementById('packBPct');

const elValVoltage = document.getElementById('valVoltage');
const elValCurrent = document.getElementById('valCurrent');
const elValPower = document.getElementById('valPower');
const elValSoh = document.getElementById('valSoh');

const elValRpm = document.getElementById('valRpm');
const elValThrottle = document.getElementById('valThrottle');
const elPowerBar = document.getElementById('powerBar');
const elStateMsgDisplay = document.getElementById('stateMsgDisplay');

const elValLeanAngle = document.getElementById('valLeanAngle');
const elLeanDot = document.getElementById('leanDot');

const elValMotorTemp = document.getElementById('valMotorTemp');
const elValInverterTemp = document.getElementById('valInverterTemp');
const elValBmsTemp = document.getElementById('valBmsTemp');
const elValTcsStatus = document.getElementById('valTcsStatus');
const elDiagFaultCode = document.getElementById('diagFaultCode');
const elDiagFaultDesc = document.getElementById('diagFaultDesc');

const elIndKeyless = document.getElementById('indicatorKeyless');
const elIndTcs = document.getElementById('indicatorTcs');
const elIndReady = document.getElementById('indicatorReady');
const elIndStand = document.getElementById('indicatorStand');
const elIndBrake = document.getElementById('indicatorBrake');
const elIndRegen = document.getElementById('indicatorRegen');
const elIndFault = document.getElementById('indicatorFault');

const elWsDot = document.getElementById('wsStatusDot');
const elWsText = document.getElementById('wsStatusText');

const elModeEco = document.getElementById('modeEco');
const elModeUrban = document.getElementById('modeUrban');
const elModeSport = document.getElementById('modeSport');
const elModeReverse = document.getElementById('modeReverse');

const elValOdo = document.getElementById('valOdo');
const elValTrip = document.getElementById('valTrip');
const elValUptime = document.getElementById('valUptime');

// WebSocket Connection
let ws = null;
let reconnectTimer = null;
const WS_URL = `ws://${window.location.hostname || 'localhost'}:5000/ws/telemetry`;

function connectWebSocket() {
  try {
    ws = new WebSocket(WS_URL);

    ws.onopen = () => {
      console.log('[HMI] Terhubung ke Telemetry Server WebSocket');
      elWsDot.classList.add('connected');
      elWsText.innerText = 'ONLINE (LIVE)';
      if (reconnectTimer) clearInterval(reconnectTimer);
    };

    ws.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);
        renderTelemetry(data);
      } catch (err) {
        console.error('[HMI] Error parse payload:', err);
      }
    };

    ws.onclose = () => {
      elWsDot.classList.remove('connected');
      elWsText.innerText = 'OFFLINE (RETRY)';
      scheduleReconnect();
    };

    ws.onerror = () => {
      elWsDot.classList.remove('connected');
      elWsText.innerText = 'OFFLINE';
      scheduleReconnect();
    };
  } catch (e) {
    scheduleReconnect();
  }
}

function scheduleReconnect() {
  if (!reconnectTimer) {
    reconnectTimer = setTimeout(() => {
      reconnectTimer = null;
      connectWebSocket();
    }, 2500);
  }
}

// Render Telemetry Data onto UI
function renderTelemetry(data) {
  if (!data) return;

  const dyn = data.dynamics || {};
  const sys = data.system || {};
  const nrg = data.energy || {};
  const tmp = data.temperatures || {};
  const dia = data.diagnostics || {};
  const imu = data.imu || {};

  // 1. Kecepatan & Speedometer Gauge (Skala 0 - 160 km/h)
  const speed = dyn.speed_kmh || 0;
  elSpeedDisplay.innerText = Math.round(speed);
  
  // Speed Arc calculation (Stroke dashoffset from 450 to 0)
  const maxSpeed = 160;
  const speedRatio = Math.min(speed / maxSpeed, 1.0);
  const arcOffset = 450 - (speedRatio * 450);
  elSpeedArcFill.style.strokeDashoffset = arcOffset;

  if (speed >= 100) {
    elSpeedArcFill.style.stroke = 'var(--accent-red)';
  } else {
    elSpeedArcFill.style.stroke = 'var(--cyan-primary)';
  }

  // 2. Dual Battery Packs & Combined SOC
  const packA = nrg.pack_a_soc !== undefined ? nrg.pack_a_soc : 92;
  const packB = nrg.pack_b_soc !== undefined ? nrg.pack_b_soc : 90;
  elPackABar.style.width = `${packA}%`;
  elPackAPct.innerText = `${packA}%`;
  elPackBBar.style.width = `${packB}%`;
  elPackBPct.innerText = `${packB}%`;

  const soc = nrg.soc !== undefined ? nrg.soc : Math.round((packA + packB) / 2);
  elSocValue.innerText = soc;
  const socOffset = 314 - ((soc / 100) * 314);
  elSocRing.style.strokeDashoffset = socOffset;
  elRangeValue.innerText = Math.round(nrg.range_km || (soc * 1.05));

  // 3. Metrik Baterai
  elValVoltage.innerHTML = `${(nrg.pack_voltage || 75.6).toFixed(1)} <small>V</small>`;
  elValCurrent.innerHTML = `${(nrg.pack_current || 0.0).toFixed(1)} <small>A</small>`;
  const pwrKw = ((nrg.power_watts || (nrg.pack_voltage * nrg.pack_current) || 0) / 1000).toFixed(2);
  elValPower.innerHTML = `${pwrKw} <small>kW</small>`;
  elValSoh.innerHTML = `${nrg.soh || 99} <small>%</small>`;

  // 4. Dinamika Motor & Throttle
  elValRpm.innerText = dyn.motor_rpm || 0;
  const throttlePct = dyn.throttle_pct || 0;
  elValThrottle.innerText = `${Math.round(throttlePct)}%`;
  elPowerBar.style.width = `${Math.min(throttlePct, 100)}%`;

  // 5. Lean Angle (IMU 6-Axis)
  const lean = imu.lean_angle_deg !== undefined ? imu.lean_angle_deg : ((speed > 30) ? (Math.sin(Date.now() / 1500) * 18) : 0);
  elValLeanAngle.innerText = `${Math.abs(lean).toFixed(1)}° ${lean < 0 ? 'L' : (lean > 0 ? 'R' : '')}`;
  // Map lean angle from -50°..+50° to 0%..100%
  const leanPct = Math.max(5, Math.min(95, 50 + (lean / 50) * 45));
  elLeanDot.style.left = `${leanPct}%`;

  // 6. Mode Berkendara
  const mode = (sys.mode || 'SPORT').toUpperCase();
  [elModeEco, elModeUrban, elModeSport, elModeReverse].forEach(el => el.classList.remove('active'));
  if (mode.includes('ECO')) elModeEco.classList.add('active');
  else if (mode.includes('URBAN')) elModeUrban.classList.add('active');
  else if (mode.includes('SPORT')) elModeSport.classList.add('active');
  else if (mode.includes('REVERSE')) elModeReverse.classList.add('active');

  // 7. Indikator Status, Keyless, TCS & Interlock
  elIndKeyless.classList.toggle('active', sys.keyless_unlocked !== false);
  elIndTcs.classList.toggle('active', Boolean(sys.tcs_active));
  elValTcsStatus.innerText = sys.tcs_active ? 'INTERVENSI' : 'STANDBY';
  elValTcsStatus.className = sys.tcs_active ? 't-val text-red' : 't-val text-green';

  elIndReady.classList.toggle('active', sys.ready === true);
  elIndStand.classList.toggle('active', sys.side_stand === true);
  elIndBrake.classList.toggle('active', sys.brake === true);
  elIndRegen.classList.toggle('active', sys.state === 'STATE_REGEN');
  
  const isFault = (dia.fault_code && dia.fault_code !== 0) || sys.state === 'STATE_FAULT';
  elIndFault.classList.toggle('active', Boolean(isFault));

  elStateMsgDisplay.innerText = `STATUS: ${sys.state || 'DRIVE READY (120 KM/H ED.)'}`;
  if (isFault) {
    elStateMsgDisplay.style.color = 'var(--accent-red)';
    elDiagFaultCode.innerText = `FAULT: ${dia.fault_msg || 'SYSTEM INTERLOCK'}`;
    elDiagFaultCode.style.color = 'var(--accent-red)';
    elDiagFaultDesc.innerText = 'Periksa sensor APPS / IMU crash lock sebelum melanjutkan.';
  } else {
    elStateMsgDisplay.style.color = 'var(--accent-green)';
    elDiagFaultCode.innerText = 'FAULTS: NONE (NORMAL)';
    elDiagFaultCode.style.color = 'var(--accent-green)';
    elDiagFaultDesc.innerText = 'Sistem cerdas beroperasi optimal dalam batas aman 120 km/jam.';
  }

  // 8. Suhu
  elValMotorTemp.innerText = `${(tmp.motor_c || 44.0).toFixed(1)}°C`;
  elValInverterTemp.innerText = `${(tmp.inverter_c || 39.5).toFixed(1)}°C`;
  elValBmsTemp.innerText = `${(tmp.bms_c || 32.0).toFixed(1)}°C`;

  // 9. Odo & Trip
  if (dyn.odometer_km) elValOdo.innerText = dyn.odometer_km.toFixed(1);
  if (dyn.trip_km) elValTrip.innerText = dyn.trip_km.toFixed(1);
  
  if (data.timestamp) {
    const sec = Math.floor(data.timestamp % 86400);
    const hrs = String(Math.floor(sec / 3600)).padStart(2, '0');
    const mins = String(Math.floor((sec % 3600) / 60)).padStart(2, '0');
    const s = String(sec % 60).padStart(2, '0');
    elValUptime.innerText = `${hrs}:${mins}:${s}`;
  }
}

// Simulasi Demo Cepat
function sendDemoThrottle(pct) {
  const speed = (pct / 100) * 122.5; // Skala 100% gas = 122.5 km/jam
  const rpm = Math.round((pct / 100) * 5800);
  const current = (pct / 100) * 195.0; // 195A peak
  const power = current * 75.0;
  const lean = (pct > 50) ? 22.5 : 8.0;
  
  const mockPayload = {
    vin: "LAKSANAEV2026ID001",
    timestamp: Math.floor(Date.now() / 1000),
    system: { state: "STATE_DRIVE", mode: "SPORT", ready: true, side_stand: false, brake: false, keyless_unlocked: true, tcs_active: (pct > 90) },
    dynamics: { speed_kmh: speed, motor_rpm: rpm, throttle_pct: pct },
    energy: { pack_voltage: 75.2, pack_current: current, power_watts: power, soc: 91, soh: 99, range_km: 88.5, pack_a_soc: 92, pack_b_soc: 90 },
    imu: { lean_angle_deg: lean, pitch_angle_deg: 2.0 },
    temperatures: { motor_c: 54.0, inverter_c: 48.0, bms_c: 34.0 },
    diagnostics: { fault_code: 0, fault_msg: "NORMAL" }
  };
  renderTelemetry(mockPayload);
}

function sendDemoBrake() {
  const mockPayload = {
    vin: "LAKSANAEV2026ID001",
    timestamp: Math.floor(Date.now() / 1000),
    system: { state: "STATE_REGEN", mode: "SPORT", ready: true, side_stand: false, brake: true, keyless_unlocked: true, tcs_active: false },
    dynamics: { speed_kmh: 35.0, motor_rpm: 1200, throttle_pct: 0 },
    energy: { pack_voltage: 76.8, pack_current: -45.0, power_watts: -3400, soc: 91, soh: 99, range_km: 88.5, pack_a_soc: 92, pack_b_soc: 90 },
    imu: { lean_angle_deg: 0.0, pitch_angle_deg: -3.5 },
    temperatures: { motor_c: 53.0, inverter_c: 47.5, bms_c: 34.0 },
    diagnostics: { fault_code: 0, fault_msg: "NORMAL" }
  };
  renderTelemetry(mockPayload);
}

// Inisialisasi awal
window.addEventListener('DOMContentLoaded', () => {
  connectWebSocket();
});
