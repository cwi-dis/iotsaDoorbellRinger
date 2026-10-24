#ifndef _IOTSAALARM_H_
#define _IOTSAALARM_H_

#include "iotsa.h"
#include "iotsaApi.h"

//
// Alarm module, the same as iotsa's examples/Ringer. Sounds a buzzer on the
// given pin for a while: on the web page /alarm, with PUT /api/alarm, or with
// the trigger /ring?alarm=20, a plain URL for curl or another device (an
// iotsaDoorbellButton). Durations are in tenths of a second. Two additions:
// the status LED pulses blue while the buzzer sounds, and ringing or stopping
// needs the right "alarm", from a user or a capability token: this device is
// the testbed for those. Reading the time left is open.
//
class IotsaAlarmMod : public IotsaModule {
public:
  IotsaAlarmMod(IotsaApplication &_app, int _pin)
  : IotsaModule(_app),
    pin(_pin)
  {}
  void setup() override;
  void lateSetup() override;
  void loop() override;
  String info() override;
protected:
  void putHandler(IotsaApiContext& ctx) override;
  void getHandler(IotsaApiContext& ctx) override;
#ifdef IOTSA_WITH_WEB
  String webTitle() override { return "Alarm Server"; }
  void formHandler_args(IotsaWebContext& ctx) override;
  void formHandler_fields(IotsaWebContext& ctx) override;
  void ringTrigger(IotsaWebContext& ctx);
#endif
private:
  void _ring(int dur);
  int pin;
  unsigned long alarmStart = 0;
  unsigned long alarmDuration = 0;  // milliseconds, 0 when not ringing
};

#endif
