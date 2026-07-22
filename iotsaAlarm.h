#ifndef _IOTSAALARM_H_
#define _IOTSAALARM_H_
#include "iotsa.h"
#include "iotsaApi.h"
#include "iotsaLed.h"

extern IotsaLedMod ledMod;

#define PIN_ALARM 4 // GPIO4 connects to the buzzer

//
// Buzzer module: sounds the buzzer (and flashes the LED) for a configurable duration.
//
class IotsaAlarmMod : public IotsaApiMod {
public:
  IotsaAlarmMod(IotsaApplication &_app, IotsaAuthMod *_auth=NULL)
  : IotsaApiMod(_app, _auth),
    alarmEndTime(0)
  {}
  void setup() override;
  void serverSetup() override;
  void loop() override;
  String info() override;
  using IotsaBaseMod::needsAuthentication;
protected:
  bool getHandler(const char *path, JsonObject& reply) override;
  bool putHandler(const char *path, const JsonVariant& request, JsonObject& reply) override;
  void handler();
  unsigned long alarmEndTime;
};

#endif
