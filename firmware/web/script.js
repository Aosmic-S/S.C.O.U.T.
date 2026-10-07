/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: script.js · Purpose: Vanilla JS frontend dashboard logic
 */

document.addEventListener('DOMContentLoaded', () => {
  const state = {
    connected: false,
    speed: 70,
    activeDirection: null,
    moveInterval: null,
    eStop: false,
    token: localStorage.getItem('SCOUT_TOKEN') || 'scout-client-' + Math.random().toString(36).substr(2, 9)
  };

  localStorage.setItem('SCOUT_TOKEN', state.token);

  // DOM Elements
  const connBadge = document.getElementById('conn-badge');
  const connStatus = document.getElementById('conn-status');
  const alertBanner = document.getElementById('alert-banner');
  const alertMessage = document.getElementById('alert-message');
  const estopBtn = document.getElementById('estop-btn');

  // MQ2
  const mq2Ppm = document.getElementById('mq2-ppm');
  const mq2Raw = document.getElementById('mq2-raw');
  const mq2Rsro = document.getElementById('mq2-rsro');
  const mq2Volts = document.getElementById('mq2-volts');
  const mq2Bar = document.getElementById('mq2-bar');
  const mq2Badge = document.getElementById('mq2-badge');

  // MQ135
  const mq135Ppm = document.getElementById('mq135-ppm');
  const mq135Raw = document.getElementById('mq135-raw');
  const mq135Rsro = document.getElementById('mq135-rsro');
  const mq135Volts = document.getElementById('mq135-volts');
  const mq135Bar = document.getElementById('mq135-bar');
  const mq135Badge = document.getElementById('mq135-badge');

  // AQI
  const aqiVal = document.getElementById('aqi-val');
  const aqiMarker = document.getElementById('aqi-marker');
  const aqiBadge = document.getElementById('aqi-badge');

  // Battery
  const battPct = document.getElementById('batt-pct');
  const battVolts = document.getElementById('batt-volts');
  const battRuntime = document.getElementById('batt-runtime');
  const battBar = document.getElementById('batt-bar');
  const battBadge = document.getElementById('batt-badge');

  // DHT
  const dhtiTemp = document.getElementById('dhti-temp');
  const dhtiHum = document.getElementById('dhti-hum');
  const dhtiStatus = document.getElementById('dhti-status');
  const dhteTemp = document.getElementById('dhte-temp');
  const dhteHum = document.getElementById('dhte-hum');
  const dhteStatus = document.getElementById('dhte-status');

  // System
  const sysIp = document.getElementById('sys-ip');
  const sysUptime = document.getElementById('sys-uptime');
  const sysRssi = document.getElementById('sys-rssi');
  const sysHeap = document.getElementById('sys-heap');
  const sysWdt = document.getElementById('sys-wdt');

  // BT
  const btStatus = document.getElementById('bt-status');
  const btBatt = document.getElementById('bt-batt');
  const btInput = document.getElementById('bt-input');

  // Controls
  const speedSlider = document.getElementById('speed-slider');
  const speedVal = document.getElementById('speed-val');

  // Helper Toast
  function showToast(msg) {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = 'toast';
    toast.textContent = msg;
    container.appendChild(toast);
    setTimeout(() => toast.remove(), 3000);
  }

  // API Client helper
  async function apiFetch(endpoint, method = 'GET', body = null) {
    const headers = {
      'X-SCOUT-Token': state.token
    };
    if (body) headers['Content-Type'] = 'application/json';

    try {
      const res = await fetch(endpoint, {
        method,
        headers,
        body: body ? JSON.stringify(body) : null
      });

      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      return await res.json();
    } catch (err) {
      console.warn(`API Error [${endpoint}]:`, err);
      return null;
    }
  }

  // Poll Functions
  async function pollStatus() {
    const data = await apiFetch('/api/status');
    if (data && data.ok) {
      state.connected = true;
      connBadge.classList.add('connected');
      connStatus.textContent = 'ONLINE';

      sysIp.textContent = data.ip || '0.0.0.0';
      sysUptime.textContent = Math.floor(data.uptime_ms / 1000) + 's';
      sysRssi.textContent = data.wifi_rssi + ' dBm';
      sysHeap.textContent = Math.round(data.free_heap / 1024) + ' KB';
      sysWdt.textContent = data.watchdog_resets;

      state.eStop = data.emergency_stop;
      if (state.eStop) {
        estopBtn.classList.add('active');
        estopBtn.textContent = 'RELEASE';
      } else {
        estopBtn.classList.remove('active');
        estopBtn.textContent = 'E-STOP';
      }

      if (data.controller_connected) {
        btStatus.textContent = 'CONNECTED';
        btStatus.className = 'badge badge-normal';
      } else {
        btStatus.textContent = 'DISCONNECTED';
        btStatus.className = 'badge badge-danger';
      }
      btBatt.textContent = (data.controller_battery || 0) + '%';

      // Update Mode buttons
      document.querySelectorAll('.mode-btn').forEach(btn => {
        if (btn.dataset.mode === data.mode) {
          btn.classList.add('active');
        } else {
          btn.classList.remove('active');
        }
      });
    } else {
      state.connected = false;
      connBadge.classList.remove('connected');
      connStatus.textContent = 'OFFLINE';
    }
  }

  async function pollSensors() {
    const data = await apiFetch('/api/sensors');
    if (!data) return;

    // MQ2
    mq2Ppm.textContent = data.mq2.ppm;
    mq2Raw.textContent = data.mq2.raw;
    mq2Rsro.textContent = data.mq2.rs_ro_ratio.toFixed(2);
    mq2Volts.textContent = data.mq2.voltage.toFixed(2) + 'V';
    mq2Bar.style.width = Math.min(100, (data.mq2.ppm / 1000) * 100) + '%';
    mq2Badge.textContent = data.mq2.status.toUpperCase();
    mq2Badge.className = `badge badge-${data.mq2.status}`;

    // MQ135
    mq135Ppm.textContent = data.mq135.ppm;
    mq135Raw.textContent = data.mq135.raw;
    mq135Rsro.textContent = data.mq135.rs_ro_ratio.toFixed(2);
    mq135Volts.textContent = data.mq135.voltage.toFixed(2) + 'V';
    mq135Bar.style.width = Math.min(100, (data.mq135.ppm / 1200) * 100) + '%';
    mq135Badge.textContent = data.mq135.status.toUpperCase();
    mq135Badge.className = `badge badge-${data.mq135.status}`;

    // AQI
    aqiVal.textContent = data.air_quality_index;
    aqiMarker.style.left = Math.min(100, (data.air_quality_index / 500) * 100) + '%';

    // DHT
    dhtiTemp.textContent = data.dht_internal.temperature_c.toFixed(1);
    dhtiHum.textContent = data.dht_internal.humidity_pct.toFixed(1);
    dhtiStatus.textContent = data.dht_internal.status.toUpperCase();

    dhteTemp.textContent = data.dht_external.temperature_c.toFixed(1);
    dhteHum.textContent = data.dht_external.humidity_pct.toFixed(1);
    dhteStatus.textContent = data.dht_external.status.toUpperCase();

    // Alert Banner Level
    alertBanner.className = `alert-banner level-${data.alert_level}`;
    if (data.alert_level === 'normal') {
      alertMessage.textContent = 'System Nominal — All environmental parameters within safe thresholds.';
    } else if (data.alert_level === 'warning') {
      alertMessage.textContent = 'WARNING — Elevated gas concentration detected in operating zone.';
    } else if (data.alert_level === 'danger') {
      alertMessage.textContent = 'DANGER — High toxic gas / smoke concentration detected!';
    } else if (data.alert_level === 'critical') {
      alertMessage.textContent = 'CRITICAL — Extreme hazard level! Initiating safety protocols.';
    }
  }

  async function pollBattery() {
    const data = await apiFetch('/api/battery');
    if (!data) return;

    battPct.textContent = data.percentage;
    battVolts.textContent = data.voltage.toFixed(2) + 'V';
    battRuntime.textContent = data.estimated_runtime_min + ' min';
    battBar.style.width = data.percentage + '%';

    if (data.is_low) {
      battBadge.textContent = 'LOW';
      battBadge.className = 'badge badge-danger';
    } else {
      battBadge.textContent = 'GOOD';
      battBadge.className = 'badge badge-normal';
    }
  }

  // Drive Commands
  function sendMove(dir) {
    if (state.eStop) return;
    const pwmSpeed = Math.round((state.speed / 100) * 255);
    apiFetch('/api/move', 'POST', {
      direction: dir,
      speed: pwmSpeed,
      duration_ms: 300
    });
  }

  function startDriving(dir) {
    if (state.activeDirection === dir) return;
    stopDriving();
    state.activeDirection = dir;
    sendMove(dir);
    state.moveInterval = setInterval(() => {
      sendMove(dir);
    }, 250);
  }

  function stopDriving() {
    if (state.moveInterval) {
      clearInterval(state.moveInterval);
      state.moveInterval = null;
    }
    state.activeDirection = null;
  }

  // D-Pad Bindings
  const btnUp = document.getElementById('btn-up');
  const btnDown = document.getElementById('btn-down');
  const btnLeft = document.getElementById('btn-left');
  const btnRight = document.getElementById('btn-right');
  const btnRotLeft = document.getElementById('btn-rot-left');
  const btnRotRight = document.getElementById('btn-rot-right');

  const setupHoldBtn = (btn, dir) => {
    btn.addEventListener('mousedown', () => startDriving(dir));
    btn.addEventListener('mouseup', stopDriving);
    btn.addEventListener('mouseleave', stopDriving);
    btn.addEventListener('touchstart', (e) => { e.preventDefault(); startDriving(dir); });
    btn.addEventListener('touchend', stopDriving);
  };

  setupHoldBtn(btnUp, 'forward');
  setupHoldBtn(btnDown, 'backward');
  setupHoldBtn(btnLeft, 'left');
  setupHoldBtn(btnRight, 'right');
  setupHoldBtn(btnRotLeft, 'rotate_left');
  setupHoldBtn(btnRotRight, 'rotate_right');

  document.getElementById('btn-center').addEventListener('click', () => {
    stopDriving();
    apiFetch('/api/move', 'POST', { direction: 'stop', speed: 0, duration_ms: 0 });
  });

  // Speed Slider
  speedSlider.addEventListener('input', (e) => {
    state.speed = parseInt(e.target.value, 10);
    speedVal.textContent = state.speed;
  });

  // E-Stop Button
  estopBtn.addEventListener('click', async () => {
    if (state.eStop) {
      const res = await apiFetch('/api/release', 'POST');
      if (res && res.ok) {
        state.eStop = false;
        showToast('Emergency stop released');
      }
    } else {
      stopDriving();
      const res = await apiFetch('/api/stop', 'POST');
      if (res && res.ok) {
        state.eStop = true;
        showToast('EMERGENCY STOP ACTIVATED');
      }
    }
  });

  // Mode Buttons
  document.querySelectorAll('.mode-btn').forEach(btn => {
    btn.addEventListener('click', async () => {
      const mode = btn.dataset.mode;
      const res = await apiFetch('/api/mode', 'POST', { mode });
      if (res && res.ok) {
        showToast(`Mode changed to: ${mode.toUpperCase()}`);
        pollStatus();
      }
    });
  });

  // Actions
  document.getElementById('btn-beep').addEventListener('click', () => {
    apiFetch('/api/buzzer', 'POST', { pattern: 'beep', duration_ms: 200 });
  });

  document.getElementById('btn-alarm').addEventListener('click', () => {
    apiFetch('/api/buzzer', 'POST', { pattern: 'alarm', duration_ms: 1000 });
  });

  document.getElementById('btn-silence').addEventListener('click', () => {
    apiFetch('/api/buzzer', 'POST', { pattern: 'silence', duration_ms: 0 });
  });

  document.getElementById('btn-download').addEventListener('click', () => {
    window.location.href = '/api/logs.csv';
  });

  document.getElementById('btn-clear-log').addEventListener('click', async () => {
    if (confirm('Are you sure you want to clear mission logs?')) {
      const res = await apiFetch('/api/logs', 'DELETE');
      if (res && res.ok) {
        showToast('Mission logs cleared');
      }
    }
  });

  // Keyboard Shortcuts
  window.addEventListener('keydown', (e) => {
    if (e.repeat) return;
    if (e.key === 'ArrowUp') startDriving('forward');
    else if (e.key === 'ArrowDown') startDriving('backward');
    else if (e.key === 'ArrowLeft') startDriving('left');
    else if (e.key === 'ArrowRight') startDriving('right');
    else if (e.key === ' ') {
      e.preventDefault();
      estopBtn.click();
    }
  });

  window.addEventListener('keyup', (e) => {
    if (['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight'].includes(e.key)) {
      stopDriving();
    }
  });

  // Auto-stop on tab hidden
  document.addEventListener('visibilitychange', () => {
    if (document.hidden) {
      stopDriving();
    }
  });

  // Start Polling Loops
  setInterval(pollStatus, 3000);
  setInterval(pollSensors, 2000);
  setInterval(pollBattery, 2000);

  pollStatus();
  pollSensors();
  pollBattery();
});
