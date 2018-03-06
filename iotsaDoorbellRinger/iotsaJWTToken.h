#ifndef _IOTSAJWTTOKEN_H_
#define _IOTSAJWTTOKEN_H_
#include "iotsa.h"

class IotsaJWTTokenMod : public IotsaAuthMod {
public:
  IotsaJWTTokenMod(IotsaApplication &_app, IotsaAuthenticationProvider &_chain);
  void setup();
  void serverSetup();
  void loop();
  String info();
  bool allows(const char *right=NULL);
  bool allows(const char *obj, IotsaApiOperation verb);
protected:
  void configLoad();
  void configSave();
  void handler();
  
  IotsaAuthenticationProvider &chain;
  String trustedIssuer;
  String issuerKey;
};

#endif
