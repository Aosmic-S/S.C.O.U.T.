/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: API.md · Purpose: REST API specification documentation
 */

# 🌐 S.C.O.U.T. REST API Specification

All `/api/*` endpoints respond in JSON format. When an error occurs, the standard error schema is returned:

```json
{
  "ok": false,
  "error": "ERROR_CODE",
  "message": "Detailed error message"
}
```

---

## GET Endpoints

### 1. `GET /api/status`
Returns overall system telemetry, uptime, memory, WiFi, headlights, and controller status.

### 2. `GET /api/sensors`
Returns sensor readings:
```json
{
  "mq2": {"raw":412,"voltage":1.32,"rs_ro_ratio":3.41,"ppm":245,"status":"normal"},
  "mq135": {"raw":638,"voltage":2.05,"rs_ro_ratio":1.82,"ppm":512,"status":"warning"},
  "dht_internal": {"temperature_c":27.4,"humidity_pct":54.2,"status":"ok"},
  "dht_external": {"temperature_c":32.1,"humidity_pct":41.8,"status":"ok"},
  "air_quality_index": 112,
  "alert_level": "warning",
  "timestamp_ms": 124567
}
```

### 3. `GET /api/battery`
Returns battery voltage, percentage, and estimated remaining runtime.

### 4. `GET /api/logs` & `GET /api/logs.csv`
Downloads LittleFS mission logs in CSV format.

---

## POST Endpoints

- `POST /api/move`: Request body `{"direction": "forward|backward|left|right|rotate_left|rotate_right|stop", "speed": 0-255, "duration_ms": 250}`
- `POST /api/mode`: Request body `{"mode": "manual|sniffer|perimeter|rtl"}`
- `POST /api/stop`: Activates software Emergency Stop.
- `POST /api/release`: Releases Emergency Stop.
- `POST /api/headlights`: Request body `{"mode": "off|low|high|hazard|auto", "brightness": 0-255}`
- `POST /api/buzzer`: Request body `{"pattern": "beep|alarm|silence", "duration_ms": 200}`
- `POST /api/calibrate`: Request body `{"sensor": "mq2|mq135|all", "ro_kohm": 9.83}`

---

## DELETE Endpoints

- `DELETE /api/logs`: Clears mission logs stored in LittleFS.
