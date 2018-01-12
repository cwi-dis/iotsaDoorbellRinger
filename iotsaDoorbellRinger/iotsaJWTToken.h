#ifndef _IOTSAJWTTOKEN_H_
#define _IOTSAJWTTOKEN_H_
#include "iotsa.h"

class JWTToken;

class IotsaJWTTokenMod : public IotsaAuthMod {
public:
  IotsaJWTTokenMod(IotsaApplication &_app, IotsaAuthMod &_chain);
  void setup();
  void serverSetup();
  void loop();
  String info();
  bool needsAuthentication(const char *right=NULL);
protected:
  void configLoad();
  void configSave();
  void handler();
  
  IotsaAuthMod &chain;
  int ntoken;
  JWTToken *tokens;
};

#endif
