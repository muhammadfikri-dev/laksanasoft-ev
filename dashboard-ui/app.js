/**
 * ============================================================================
 * LAKSANASOFT-EV DASHBOARD HMI CLUSTER CONTROLLER
 * (c) 2026 PT. Laksana Digital Industri
 * ============================================================================
 */

// DOM Elements
const elSpeedDisplay = document.getElementById('speedDisplay');
const elSpeedArcFill = document.getElementById('speedArcFill');
const elSocValue = document.getElementById('socValue');
const elSocRing = document.getElementById('socRing');
const elRangeValue = document.getElementById('rangeValue');

const elValVoltage = document.getElementById('valVoltage');
const elValCurrent = document.getElementById('valCurrent');
const elValPower = document.getElementById('valPower');
const elValSoh = document.getElementById('valSoh');

const elValRpm = document.getElementById('valRpm');
const elValThrottle = document.getElementById('valThrottle');
const elPowerBar = document.getElementById('powerBar');
const elStateMsgDisplay = document.getElementById('stateMsgDisplay');

const elValMotorTemp = document.getElementById('valMotorTemp');
const elValInverterTemp = document.getElementById('valInverterTemp');
const elValBmsTemp = document.getElementById('valBmsTemp');
const elDiagFaultCode = document.getElementById('diagFaultCode');
const elDiagFaultDesc = document.getElementById('diagFaultDesc');

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

  // 1. Kecepatan & Speedometer Gauge (Maksimal skala 120 km/h)
  const speed = dyn.speed_kmh || 0;
  elSpeedDisplay.innerText = Math.round(speed);
  
  // Speed Arc calculation (Stroke dashoffset from 450 to 0)
  const maxSpeed = 120;
  const speedRatio = Math.min(speed / maxSpeed, 1.0);
  const arcOffset = 450 - (speedRatio * 450);
  elSpeedArcFill.style.strokeDashoffset = arcOffset;

  // 2. State of Charge (SOC) Circle (Dasharray: 314)
  const soc = nrg.soc !== undefined ? nrg.soc : 85;
  elSocValue.innerText = soc;
  const socOffset = 314 - ((soc / 100) * 314);
  elSocRing.style.strokeDashoffset = socOffset;
  elRangeValue.innerText = Math.round(nrg.range_km || (soc * 0.95));

  // 3. Metrik Baterai
  elValVoltage.innerHTML = `${(nrg.pack_voltage || 74.0).toFixed(1)} <small>V</small>`;
  elValCurrent.innerHTML = `${(nrg.pack_current || 0.0).toFixed(1)} <small>A</small>`;
  const pwrKw = ((nrg.power_watts || (nrg.pack_voltage * nrg.pack_current) || 0) / 1000).toFixed(2);
  elValPower.innerHTML = `${pwrKw} <small>kW</small>`;
  elValSoh.innerHTML = `${nrg.soh || 99} <small>%</small>`;

  // 4. Dinamika Motor & Throttle
  elValRpm.innerText = dyn.motor_rpm || 0;
  const throttlePct = dyn.throttle_pct || 0;
  elValThrottle.innerText = `${Math.round(throttlePct)}%`;
  elPowerBar.style.width = `${Math.min(throttlePct, 100)}%`;

  // 5. Mode Berkendara
  const mode = (sys.mode || 'SPORT').toUpperCase();
  [elModeEco, elModeUrban, elModeSport, elModeReverse].forEach(el => el.classList.remove('active'));
  if (mode === 'ECO') elModeEco.classList.add('active');
  else if (mode === 'URBAN') elModeUrban.classList.add('active');
  else if (mode === 'SPORT') elModeSport.classList.add('active');
  else if (mode === 'REVERSE') elModeReverse.classList.add('active');

  // 6. Indikator Status & Interlock
  elIndReady.classList.toggle('active', sys.ready === true);
  elIndStand.classList.toggle('active', sys.side_stand === true);
  elIndBrake.classList.toggle('active', sys.brake === true);
  elIndRegen.classList.toggle('active', sys.state === 'STATE_REGEN');
  
  const isFault = (dia.fault_code && dia.fault_code !== 0) || sys.state === 'STATE_FAULT';
  elIndFault.classList.toggle('active', Boolean(isFault));

  elStateMsgDisplay.innerText = `STATUS: ${sys.state || 'DRIVE READY'}`;
  if (isFault) {
    elStateMsgDisplay.style.color = 'var(--accent-red)';
    elDiagFaultCode.innerText = `FAULT: ${dia.fault_msg || 'SYSTEM INTERLOCK'}`;
    elDiagFaultCode.style.color = 'var(--accent-red)';
    elDiagFaultDesc.innerText = 'Periksa sensor APPS / interlock standar sebelum melanjutkan.';
  } else {
    elStateMsgDisplay.style.color = 'var(--accent-green)';
    elDiagFaultCode.innerText = 'FAULTS: NONE (NORMAL)';
    elDiagFaultCode.style.color = 'var(--accent-green)';
    elDiagFaultDesc.innerText = 'Sistem beroperasi optimal dalam batas aman.';
  }

  // 7. Suhu
  elValMotorTemp.innerText = `${(tmp.motor_c || 42.0).toFixed(1)}°C`;
  elValInverterTemp.innerText = `${(tmp.inverter_c || 38.0).toFixed(1)}°C`;
  elValBmsTemp.innerText = `${(tmp.bms_c || 32.0).toFixed(1)}°C`;

  // 8. Odo & Trip
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

// Simulasi Lokal Manual untuk Pengujian Langsung dari Tombol
function sendDemoThrottle(pct) {
  const mockPayload = {
    vin: "LAKSANAEV2026ID001",
    timestamp: Math.floor(Date.now() / 1000),
    system: { state: "STATE_DRIVE", mode: "SPORT", ready: true, side_stand: false, brake: false },
    dynamics: { speed_kmh: pct * 0.85, motor_rpm: Math.round(pct * 45), throttle_pct: pct },
    energy: { pack_voltage: 73.8, pack_current: pct * 0.6, power_watts: pct * 44.2, soc: 84, soh: 99, range_km: 81.2 },
    temperatures: { motor_c: 46.5, inverter_c: 41.2, bms_c: 33.0 },
    diagnostics: { fault_code: 0, fault_msg: "NORMAL" }
  };
  renderTelemetry(mockPayload);
}

function sendDemoBrake() {
  const mockPayload = {
    vin: "LAKSANAEV2026ID001",
    timestamp: Math.floor(Date.now() / 1000),
    system: { state: "STATE_REGEN", mode: "SPORT", ready: true, side_stand: false, brake: true },
    dynamics: { speed_kmh: 12.0, motor_rpm: 600, throttle_pct: 0 },
    energy: { pack_voltage: 74.8, pack_current: -12.5, power_watts: -935, soc: 84, soh: 99, range_km: 81.2 },
    temperatures: { motor_c: 47.0, inverter_c: 41.8, bms_c: 33.0 },
    diagnostics: { fault_code: 0, fault_msg: "NORMAL" }
  };
  renderTelemetry(mockPayload);
}

// Inisialisasi awal
window.addEventListener('DOMContentLoaded', () => {
  connectWebSocket();
});
