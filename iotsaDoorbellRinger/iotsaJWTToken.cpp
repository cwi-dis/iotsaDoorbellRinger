#include "iotsaUser.h"
#include "iotsaConfigFile.h"
#include "IotsaJWTToken.h"
#include <ArduinoJWT.h>
#include <ArduinoJson.h>

#define IFDEBUGX if(1)

// This module requires the ArduinoJWT library from https://github.com/yutter/ArduinoJWT

IotsaJWTTokenMod::IotsaJWTTokenMod(IotsaApplication &_app, IotsaAuthMod &_chain)
:	chain(_chain),
	IotsaAuthMod(_app)
{
	configLoad();
}
	
void
IotsaJWTTokenMod::handler() {
  if (needsAuthentication("tokens")) return;
  bool anyChanged = false;
  if (server.hasArg("issuer")) {
    trustedIssuer = server.arg("issuer");
    anyChanged = true;
  }
  if (server.hasArg("issuerKey")) {
   issuerKey = server.arg("issuerKey");
    anyChanged = true;
  }
  if (anyChanged) configSave();

  String message = "<html><head><title>JWT Keys</title></head><body><h1>JWT Keys</h1>";
  message += "<form method='get'>Trusted JWT Issuer: <input name='issuer' value='";
  message += trustedIssuer;
  message += "'>";
  message += "<br>Secret Key: <input name='issuerKey' value='";
  message += issuerKey;
  message += "'>";

  message += "<br><input type='submit'></form>";
  server.send(200, "text/html", message);
}

void IotsaJWTTokenMod::setup() {
  configLoad();
}

void IotsaJWTTokenMod::serverSetup() {
  server.on("/jwt", std::bind(&IotsaJWTTokenMod::handler, this));
}

void IotsaJWTTokenMod::configLoad() {
  IotsaConfigFileLoad cf("/config/jwt.cfg");
  cf.get("trustedIssuer", trustedIssuer, "");
  cf.get("issuerKey", issuerKey, "");
}

void IotsaJWTTokenMod::configSave() {
  IotsaConfigFileSave cf("/config/jwt.cfg");
  cf.put("trustedIssuer", trustedIssuer);
  cf.put("issuerKey", issuerKey);
}

void IotsaJWTTokenMod::loop() {
}

String IotsaJWTTokenMod::info() {
  String message = "<p>JWT tokens enabled.";
  message += " See <a href=\"/jwt\">/tokens</a> to change settings.";
  message += "</p>";
  return message;
}

bool IotsaJWTTokenMod::needsAuthentication(const char *right) {
  if (server.hasHeader("Authorization")) {
    String authHeader = server.header("Authorization");
    if (authHeader.startsWith("Bearer ")) {
      IFDEBUGX IotsaSerial.println("Found Authorization bearer");
      String token = authHeader.substring(7);
      ArduinoJWT decoder(issuerKey);
      String payload;
      bool ok = decoder.decodeJWT(token, payload);
      // If decode returned false the token wasn't signed with they key.
      if (!ok) {
        IFDEBUGX IotsaSerial.println("Did not decode correctly with key");
        return true;
      }
      
      // Token decoded correctly.
      // decode JSON from payload
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(payload);
      
      // check that issuer matches
      String issuer = root["iss"];
      if (issuer != trustedIssuer) {
        IFDEBUGX IotsaSerial.println("Issuer did not match");
        return true;
      }
      // xxxjack check that audience matches, if present
      if (root.containsKey("aud")) {
        String audience = root["aud"];
        String myUrl("http://");
        myUrl += hostName;
        if (audience != myUrl) {
          IFDEBUGX IotsaSerial.println("Audience did not match");
          return true;
        }
      }
      // check that right matches
      String capRights = root["right"];
      if (capRights != right && capRights != "*") {
        IFDEBUGX IotsaSerial.println("Rights did not match");
        return true;
      }
      IFDEBUGX IotsaSerial.println("JWT accepted");
      return false;
    }
    
  }
  IotsaSerial.println("No token match, try user/password");
  // If no rights fall back to username/password authentication
  return chain.needsAuthentication(right);
}
