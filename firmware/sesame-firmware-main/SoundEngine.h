#ifndef SOUND_ENGINE_H
#define SOUND_ENGINE_H

#include <Arduino.h>

enum SoundID {
  SOUND_REST,
  SOUND_STAND,
  SOUND_WAVE,
  SOUND_DANCE,
  SOUND_SWIM,
  SOUND_POINT,
  SOUND_PUSHUP,
  SOUND_BOW,
  SOUND_CUTE,
  SOUND_FREAKY,
  SOUND_WORM,
  SOUND_SHAKE,
  SOUND_SHRUG,
  SOUND_DEAD,
  SOUND_CRAB,
  SOUND_SCARED
};

struct SoundNote {
  uint16_t freq;
  uint16_t duration;
};

class SoundEngine {
public:
  SoundEngine(uint8_t pin, uint8_t channel = 7);

  void begin();
  void play(const SoundNote* melody, uint8_t length);
  void play(SoundID id);
  void stop();
  void update();
  bool isPlaying();

private:
  uint8_t _pin;
  uint8_t _channel;

  const SoundNote* _melody;
  uint8_t _length;
  uint8_t _index;

  unsigned long _noteStart;
  bool _playing;

  void startTone(uint16_t freq);
  void stopTone();
};

#endif