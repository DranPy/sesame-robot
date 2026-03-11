#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include <Arduino.h>
#include <VL6180X.h>

// ---- Debug ----
#define TOF_DEBUG 1 // set to 0 to disable Serial distance logging

// ---- Thresholds ----
#define TOF_PROX_THRESHOLD_MM 130 // within sensor range — triggers scared if hand stays still
#define TOF_WAVE_NEAR_MM 160      // hand "in" zone (16 cm, based on sensor range)
#define TOF_OUT_OF_RANGE 255      // VL6180X returns 255 when nothing detected
#define TOF_WAVE_MIN_CROSSINGS 3  // in→out transitions needed for wave
#define TOF_WAVE_WINDOW_MS 2500   // time window for wave gesture
#define TOF_POLL_INTERVAL_MS 50   // poll every 50 ms (20 Hz)
#define TOF_PROX_DEBOUNCE_MS 3000 // longer than wave window — fires if hand stays still
#define TOF_COOLDOWN_MS 3000      // silence sensor after any reaction

enum TofWaveState : uint8_t
{
  TOF_WAVE_IDLE,    // waiting; nothing in range
  TOF_WAVE_OBJ_IN,  // hand entered near zone
  TOF_WAVE_OBJ_OUT, // hand moved out of range
};

class DistanceSensor
{
public:
  DistanceSensor();

  bool begin();               // init sensor; returns false if not found
  void update();              // call every loop() — non-blocking
  bool isWaveDetected();      // true once per wave event (auto-clears)
  bool isProximityDetected(); // true once per proximity event (auto-clears)
  uint8_t getDistance();      // last raw reading in mm (255 = nothing)
  bool isPresent();           // true if begin() succeeded

private:
  VL6180X _sensor;
  bool _present;

  uint8_t _lastDistance;
  unsigned long _lastPollMs;

  // Proximity debounce
  bool _proxPending;
  unsigned long _proxStartMs;
  bool _proxFired;

  // Wave state machine
  TofWaveState _waveState;
  uint8_t _waveCrossings;
  unsigned long _waveWindowStartMs;
  bool _waveFired;

  // Post-reaction cooldown
  unsigned long _cooldownUntilMs;

  void _updateProximity(uint8_t dist);
  void _updateWave(uint8_t dist);
};

#endif
