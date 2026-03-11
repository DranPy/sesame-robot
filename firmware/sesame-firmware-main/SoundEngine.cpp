#include "SoundEngine.h"

static const SoundNote soundRest[] = {{600, 120}, {500, 200}};
static const SoundNote soundStand[] = {{800, 100}, {1000, 120}, {1200, 150}};
static const SoundNote soundWave[] = {{1200, 80}, {0, 40}, {1200, 80}, {0, 40}, {1400, 120}};
static const SoundNote soundDance[] = {{900, 80}, {1200, 80}, {1500, 80}, {1200, 80}, {900, 120}};
static const SoundNote soundSwim[] = {{700, 120}, {900, 120}, {700, 120}, {500, 150}};
static const SoundNote soundPoint[] = {{1500, 100}, {1800, 200}};
static const SoundNote soundPushup[] = {{500, 100}, {400, 100}, {300, 150}};
static const SoundNote soundBow[] = {{1000, 120}, {800, 120}, {600, 200}};
static const SoundNote soundCute[] = {{1600, 80}, {1800, 80}, {1600, 80}, {1400, 150}};
static const SoundNote soundFreaky[] = {{400, 70}, {1800, 70}, {600, 70}, {1600, 120}};
static const SoundNote soundWorm[] = {{400, 100}, {500, 100}, {600, 100}, {700, 150}};
static const SoundNote soundShake[] = {{1400, 60}, {0, 40}, {1400, 60}, {0, 40}, {1400, 100}};
static const SoundNote soundShrug[] = {{900, 100}, {1100, 100}, {900, 150}};
static const SoundNote soundDead[] = {{700, 200}, {500, 300}, {300, 400}};
static const SoundNote soundCrab[] = {{1000, 80}, {1300, 80}, {1600, 80}, {1300, 80}, {1000, 150}};

SoundEngine::SoundEngine(uint8_t pin, uint8_t channel)
{
  _pin = pin;
  _channel = channel;
}

void SoundEngine::begin()
{
  ledcSetup(_channel, 2000, 10);   // 2kHz base, 10-bit resolution
  ledcAttachPin(_pin, _channel);
  stopTone();
}

void SoundEngine::play(SoundID id)
{

  static const SoundNote soundRest[] = {{600, 120}, {500, 200}};
  static const SoundNote soundStand[] = {{800, 100}, {1000, 120}, {1200, 150}};
  static const SoundNote soundWave[] = {{1200, 80}, {0, 40}, {1200, 80}, {0, 40}, {1400, 120}};
  static const SoundNote soundDance[] = {{900, 80}, {1200, 80}, {1500, 80}, {1200, 80}, {900, 120}};
  static const SoundNote soundSwim[] = {{700, 120}, {900, 120}, {700, 120}, {500, 150}};
  static const SoundNote soundPoint[] = {{1500, 100}, {1800, 200}};
  static const SoundNote soundPushup[] = {{500, 100}, {400, 100}, {300, 150}};
  static const SoundNote soundBow[] = {{1000, 120}, {800, 120}, {600, 200}};
  static const SoundNote soundCute[] = {{1600, 80}, {1800, 80}, {1600, 80}, {1400, 150}};
  static const SoundNote soundFreaky[] = {{400, 70}, {1800, 70}, {600, 70}, {1600, 120}};
  static const SoundNote soundWorm[] = {{400, 100}, {500, 100}, {600, 100}, {700, 150}};
  static const SoundNote soundShake[] = {{1400, 60}, {0, 40}, {1400, 60}, {0, 40}, {1400, 100}};
  static const SoundNote soundShrug[] = {{900, 100}, {1100, 100}, {900, 150}};
  static const SoundNote soundDead[] = {{700, 200}, {500, 300}, {300, 400}};
  static const SoundNote soundCrab[] = {{1000, 80}, {1300, 80}, {1600, 80}, {1300, 80}, {1000, 150}};
  static const SoundNote soundScared[] = {{300, 100}, {200, 150}, {150, 300}};

  switch (id)
  {
  case SOUND_REST:
    play(soundRest, 2);
    break;
  case SOUND_STAND:
    play(soundStand, 3);
    break;
  case SOUND_WAVE:
    play(soundWave, 5);
    break;
  case SOUND_DANCE:
    play(soundDance, 5);
    break;
  case SOUND_SWIM:
    play(soundSwim, 4);
    break;
  case SOUND_POINT:
    play(soundPoint, 2);
    break;
  case SOUND_PUSHUP:
    play(soundPushup, 3);
    break;
  case SOUND_BOW:
    play(soundBow, 3);
    break;
  case SOUND_CUTE:
    play(soundCute, 4);
    break;
  case SOUND_FREAKY:
    play(soundFreaky, 4);
    break;
  case SOUND_WORM:
    play(soundWorm, 4);
    break;
  case SOUND_SHAKE:
    play(soundShake, 5);
    break;
  case SOUND_SHRUG:
    play(soundShrug, 3);
    break;
  case SOUND_DEAD:
    play(soundDead, 3);
    break;
  case SOUND_CRAB:
    play(soundCrab, 5);
    break;
  case SOUND_SCARED:
    play(soundScared, 3);
    break;
  }
}

void SoundEngine::play(const SoundNote *melody, uint8_t length)
{
  _melody = melody;
  _length = length;
  _index = 0;
  _noteStart = millis();
  _playing = true;

  startTone(_melody[_index].freq);
}

void SoundEngine::update()
{
  if (!_playing)
    return;

  if (millis() - _noteStart >= _melody[_index].duration)
  {
    _index++;

    if (_index >= _length)
    {
      stop();
      return;
    }

    _noteStart = millis();
    startTone(_melody[_index].freq);
  }
}

void SoundEngine::stop()
{
  stopTone();
  _playing = false;
}

bool SoundEngine::isPlaying()
{
  return _playing;
}

void SoundEngine::startTone(uint16_t freq)
{
  if (freq == 0)
  {
    stopTone();
  }
  else
  {
    ledcWriteTone(_channel, freq);
  }
}

void SoundEngine::stopTone()
{
  ledcWrite(_channel, 0);
}