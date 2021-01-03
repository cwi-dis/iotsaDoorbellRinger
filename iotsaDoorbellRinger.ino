//
// Server for a 4-line display with a buzzer and 2 buttons.
// Messages to the display (and buzzes) can be controlled with a somewhat REST-like interface.
// The buttons can be polled, or can be setup to send a GET request to a preprogrammed URL.
// 
// Support for buttons, LCD and buzzer can be disabled selectively.
//
// Hardware schematics and PCB stripboard layout can be found in the "extras" folder, in Fritzing format,
// for an ESP201-based device.
//
// (c) 2016, Jack Jansen, Centrum Wiskunde & Informatica.
// License TBD.
//

#include "iotsa.h"
#include "iotsaWifi.h"
#include "iotsaOta.h"
#include "iotsaFilesBackup.h"
#include "iotsaSimple.h"
#include "iotsaConfigFile.h"
#include "iotsaUser.h"
#include "iotsaLed.h"
#include "iotsaLogger.h"
#include "iotsaCapabilities.h"

#define PIN_ALARM 4 // GPIO4 connects to the buzzer
#define PIN_NEOPIXEL 15  // pulled-down during boot, can be used for NeoPixel afterwards
#define IFDEBUGX if(0)

IotsaApplication application("Doorbell Ringer Server");

// Configure modules we need
IotsaWifiMod wifiMod(application);  // wifi is always needed
IotsaOtaMod otaMod(application);    // we want OTA for updating the software (will not work with esp-201)
IotsaLedMod ledMod(application, PIN_NEOPIXEL);

IotsaUserMod myUserAuthenticator(application, "owner");  // Our username/password authenticator module
IotsaCapabilityMod myTokenAuthenticator(application, myUserAuthenticator); // Our token authenticator
#ifdef IOTSA_WITH_HTTP_OR_HTTPS
//IotsaLoggerMod myLogger(application, &myTokenAuthenticator);
#endif

//
// Buzzer configuration and implementation
//
unsigned long alarmEndTime;

// Declaration of the Alarm module
class IotsaAlarmMod : public IotsaApiMod {
public:
  IotsaAlarmMod(IotsaApplication &_app, IotsaAuthMod *_auth=NULL) : IotsaApiMod(_app, _auth) {}
  void setup() override;
  void serverSetup() override;
  void loop() override;
  String info() override;
  using IotsaBaseMod::needsAuthentication;
protected:
  bool getHandler(const char *path, JsonObject& reply) override;
  bool putHandler(const char *path, const JsonVariant& request, JsonObject& reply) override;
  void handler();
};

//
// LCD configuration and implementation
//
void IotsaAlarmMod::setup() {
  pinMode(PIN_ALARM, OUTPUT); // Trick: we configure to input so we make the pin go Hi-Z.
}

#ifdef IOTSA_WITH_WEB
void IotsaAlarmMod::handler() {
  if (needsAuthentication("alarm")) return;
  
  String msg;
  for (uint8_t i=0; i<server->args(); i++){
    if (server->argName(i) == "alarm") {
      const char *arg = server->arg(i).c_str();
      if (arg && *arg) {
        int dur = atoi(server->arg(i).c_str());
        if (dur) {
          alarmEndTime = millis() + dur*100;
          IotsaSerial.println("alarm on");
          digitalWrite(PIN_ALARM, HIGH);
          ledMod.set(0x0080ff, dur*100, 0, 1);
        } else {
          alarmEndTime = 0;
        }
      }
    }
  }
  String message = "<html><head><title>Alarm Server</title></head><body><h1>Alarm Server</h1>";
  message += "<form method='get'>";
  message += "Alarm: <input name='alarm' value=''> (times 0.1 second)<br>\n";
  message += "<input type='submit'></form></body></html>";
  server->send(200, "text/html", message);
  
}

String IotsaAlarmMod::info() {
  return "<p>See <a href='/alarm'>/alarm</a> to use the buzzer. REST interface on <a href='/api/alarm'>/api/alarm</a>";
}
#endif // IOTSA_WITH_WEB

bool IotsaAlarmMod::getHandler(const char *path, JsonObject& reply) {
  int dur = 0;
  if (alarmEndTime) {
    dur = (alarmEndTime - millis())/100;
  }
  reply["alarm"] = dur;
  return true;
}

bool IotsaAlarmMod::putHandler(const char *path, const JsonVariant& request, JsonObject& reply) {
  int dur = 0;
  if (request.is<int>()) {
    dur = request.as<int>();
  } else if (request.is<JsonObject>()) {
    JsonObject reqObj = request.as<JsonObject>();
    dur = reqObj["alarm"];
  } else {
    return false;
  }
  if (dur) {
    alarmEndTime = millis() + dur*100;
    IotsaSerial.println("alarm on");
    digitalWrite(PIN_ALARM, HIGH);
    ledMod.set(0x0080ff, dur*100, 0, 1);
  } else {
    alarmEndTime = 0;
  }
  return true;
}

void IotsaAlarmMod::serverSetup() {
  // Setup the web server hooks for this module.
#ifdef IOTSA_WITH_WEB
  server->on("/alarm", std::bind(&IotsaAlarmMod::handler, this));
#endif
  api.setup("/api/alarm", true, true);
  name = "alarm";
}


void IotsaAlarmMod::loop() {
  if (alarmEndTime && millis() > alarmEndTime) {
    alarmEndTime = 0;
    IotsaSerial.println("alarm off");
    digitalWrite(PIN_ALARM, LOW);
  }
}

IotsaAlarmMod alarmMod(application, &myTokenAuthenticator);
//
// Boilerplate for iotsa server, with hooks to our code added.
//
void setup(void) {
  application.setup();
  application.serverSetup();
  ESP.wdtEnable(WDTO_120MS);
}
 
void loop(void) {
  application.loop();
}
