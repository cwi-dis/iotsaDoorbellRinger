//
// Doorbell ringer server: a buzzer (and NeoPixel) that can be sounded over a
// somewhat-REST-like interface, typically triggered by an iotsaDoorbellButton.
// Also serves as the testbed for iotsa's token/capability-based access control
// (IotsaCapabilityMod).
//
// Hardware schematics and PCB stripboard layout are in the "extras" folder, in
// Fritzing format, for an ESP-201-based device.
//
// (c) 2016, Jack Jansen, Centrum Wiskunde & Informatica.
// License TBD.
//

#include "iotsa.h"
#include "iotsaUser.h"
#include "iotsaCapabilities.h"
#include "iotsaAlarm.h"

IotsaApplication application("Doorbell Ringer Server");

// Configure modules we need

IotsaUserMod myUserAuthenticator(application, "owner");  // Our username/password authenticator module
IotsaCapabilityMod myTokenAuthenticator(application, myUserAuthenticator); // Our token authenticator

IotsaAlarmMod alarmMod(application);

//
// Boilerplate for iotsa server, with hooks to our code added.
//
void setup(void) {
  application.setAuth(&myTokenAuthenticator);  // every module, the standard ones included, uses this
  application.setup();
  application.lateSetup();
#ifndef ESP32
  ESP.wdtEnable(WDTO_120MS);
#endif
}

void loop(void) {
  application.loop();
}
