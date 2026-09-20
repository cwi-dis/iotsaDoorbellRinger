#ifndef _IOTSAALARM_H_
#define _IOTSAALARM_H_
#include "iotsa.h"
#include "iotsaApi.h"

#define PIN_ALARM 4 // GPIO4 connects to the buzzer

//
// Buzzer module: sounds the buzzer (and flashes the LED) for a configurable duration.
//
class IotsaAlarmMod : public IotsaModule {
public:
  using IotsaModule::IotsaModule;
  void setup() override;
  void lateSetup() override;
  void loop() override;
  String info() override;
  using IotsaBaseModule::needsAuthentication;
protected:
  bool getHandler(const char *path, JsonObject& reply) override;
  bool putHandler(const char *path, const JsonVariant& request, JsonObject& reply) override;
#ifdef IOTSA_WITH_WEB
  void webHandler() override;
#endif
  unsigned long alarmEndTime = 0;
};

#endif
