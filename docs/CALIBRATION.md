/*
 * Copyright 2026 Absolute Tech
 * Licensed under the Apache License, Version 2.0
 * Project: S.C.O.U.T. — Version v0.27.0 "OP"
 * File: CALIBRATION.md · Purpose: Gas sensor calibration procedure documentation
 */

# 🧪 MQ Gas Sensor Calibration Procedure

## Calibration Formula

- MQ2 default $R_o$: `9.83 kΩ`
- MQ135 default $R_o$: `10.25 kΩ`
- Load Resistance $R_L$: `10.0 kΩ`

## Pre-heating Requirement

1. Power the gas sensors for **at least 24 hours** prior to first calibration.
2. Ensure sensors are in fresh ambient air (outside or clean ventilated room).

## Automated API Calibration

Trigger auto-calibration via HTTP API:

```bash
curl -X POST http://<SCOUT_IP>/api/calibrate -H "Content-Type: application/json" -d '{"sensor":"all","ro_kohm":0}'
```
