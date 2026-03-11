#include "DistanceSensor.h"

DistanceSensor::DistanceSensor() {
  _present          = false;
  _lastDistance     = TOF_OUT_OF_RANGE;
  _lastPollMs       = 0;
  _proxPending      = false;
  _proxStartMs      = 0;
  _proxFired        = false;
  _waveState        = TOF_WAVE_IDLE;
  _waveCrossings    = 0;
  _waveWindowStartMs = 0;
  _waveFired        = false;
  _cooldownUntilMs  = 0;
}

bool DistanceSensor::begin() {
  // Wire.begin() already called in main setup() before this
  // Probe I2C address 0x29 to check if sensor is connected
  Wire.beginTransmission(0x29);
  if (Wire.endTransmission() != 0) {
    Serial.println(F("[TOF] VL6180X not found — distance features disabled"));
    return false;
  }
  _sensor.setTimeout(50);
  _sensor.init();
  _sensor.configureDefault();
  _present = true;
  Serial.println(F("[TOF] VL6180X initialized"));
  return true;
}

void DistanceSensor::update() {
  if (!_present) return;

  unsigned long now = millis();
  if (now < _cooldownUntilMs) return;

  if (now - _lastPollMs < TOF_POLL_INTERVAL_MS) return;
  _lastPollMs = now;

  uint8_t dist = _sensor.readRangeSingle();
  _lastDistance = dist;

#if TOF_DEBUG
  if (dist == TOF_OUT_OF_RANGE) {
    Serial.print(F("[TOF] dist=--- state="));
  } else {
    Serial.print(F("[TOF] dist="));
    Serial.print(dist);
    Serial.print(F("mm state="));
  }
  Serial.println(_waveState);
#endif

  _updateProximity(dist);
  _updateWave(dist);
}

void DistanceSensor::_updateProximity(uint8_t dist) {
  unsigned long now = millis();

  if (dist <= TOF_PROX_THRESHOLD_MM) {
    if (!_proxPending) {
      _proxPending = true;
      _proxStartMs = now;
    } else if (!_proxFired && (now - _proxStartMs >= TOF_PROX_DEBOUNCE_MS)) {
      _proxFired = true;
      _cooldownUntilMs = now + TOF_COOLDOWN_MS;
    }
  } else {
    _proxPending = false;
    _proxStartMs = 0;
  }
}

void DistanceSensor::_updateWave(uint8_t dist) {
  unsigned long now = millis();
  bool inNear = (dist <= TOF_WAVE_NEAR_MM);
  bool inFar  = (dist == TOF_OUT_OF_RANGE);
  // Readings in hysteresis band (NEAR < dist < OUT_OF_RANGE) are ignored

  switch (_waveState) {
    case TOF_WAVE_IDLE:
      if (inNear) {
        _waveState        = TOF_WAVE_OBJ_IN;
        _waveCrossings    = 1;
        _waveWindowStartMs = now;
      }
      break;

    case TOF_WAVE_OBJ_IN:
      if (now - _waveWindowStartMs > TOF_WAVE_WINDOW_MS) {
        // Timed out — reset
        _waveState     = TOF_WAVE_IDLE;
        _waveCrossings = 0;
        break;
      }
      if (inFar) {
        _waveCrossings++;
        _waveState = TOF_WAVE_OBJ_OUT;
      }
      break;

    case TOF_WAVE_OBJ_OUT:
      if (now - _waveWindowStartMs > TOF_WAVE_WINDOW_MS) {
        _waveState     = TOF_WAVE_IDLE;
        _waveCrossings = 0;
        break;
      }
      if (inNear) {
        _waveCrossings++;
        if (_waveCrossings >= TOF_WAVE_MIN_CROSSINGS) {
          _waveFired       = true;
          _waveState       = TOF_WAVE_IDLE;
          _waveCrossings   = 0;
          _cooldownUntilMs = now + TOF_COOLDOWN_MS;
        } else {
          _waveState = TOF_WAVE_OBJ_IN;
        }
      }
      break;
  }
}

bool DistanceSensor::isWaveDetected() {
  bool fired = _waveFired;
  _waveFired = false;
  return fired;
}

bool DistanceSensor::isProximityDetected() {
  bool fired = _proxFired;
  _proxFired = false;
  return fired;
}

uint8_t DistanceSensor::getDistance() {
  return _lastDistance;
}

bool DistanceSensor::isPresent() {
  return _present;
}
