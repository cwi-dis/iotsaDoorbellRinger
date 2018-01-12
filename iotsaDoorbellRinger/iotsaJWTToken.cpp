#include "iotsaUser.h"
#include "iotsaConfigFile.h"
#include "IotsaJWTToken.h"

#undef PASSWORD_DEBUGGING	// Enables admin1/admin1 to always log in

class JWTToken {
public:
  String token;
  String rights;
};

IotsaJWTTokenMod::IotsaJWTTokenMod(IotsaApplication &_app, IotsaAuthMod &_chain)
:	chain(_chain),
	IotsaAuthMod(_app)
{
	configLoad();
}
	
void
IotsaJWTTokenMod::handler() {
  if (needsAuthentication("tokens")) return;
  if (server.hasArg("ntoken")) {
    ntoken = server.arg("ntoken").toInt();
    if (tokens) free(tokens);
    tokens = (JWTToken *)calloc(ntoken, sizeof(JWTToken));
     
    for (int i=0; i<ntoken; i++) {
      String tokenValue ;
      String tokenRights;
      tokens[i].token = server.arg("token" + String(i));
      tokens[i].rights = server.arg("rights" + String(i));
    }
    configSave();
  }

  String message = "<html><head><title>Edit tokens</title></head><body><h1>Edit tokens</h1>";
  message += "<form method='get'>Number of tokens: <input name='ntoken' type='number' min='0' value='";
  message += String(ntoken);
  message += "'>";
  for (int i=0; i<ntoken; i++) {
    message += "<br>Token: <input name='token" + String(i) + "' value='" + tokens[i].token + "'>";
    message += "Rights (/right1/right2/right3/) <input name='rights" + String(i) + "' value='" + tokens[i].rights + "'>";
  }

  message += "<br><input type='submit'></form>";
  server.send(200, "text/html", message);
}

void IotsaJWTTokenMod::setup() {
  configLoad();
}

void IotsaJWTTokenMod::serverSetup() {
  server.on("/tokens", std::bind(&IotsaJWTTokenMod::handler, this));
}

void IotsaJWTTokenMod::configLoad() {
  IotsaConfigFileLoad cf("/config/statictokens.cfg");
  cf.get("ntoken", ntoken, 0);
  if (tokens) free(tokens);
  if (ntoken <= 0) return;
  tokens = (JWTToken *)calloc(ntoken, sizeof(JWTToken));
   
  for (int i=0; i<ntoken; i++) {
    String tokenValue;
    String tokenRights;
    cf.get("token" + String(i), tokens[i].token, "");
  	cf.get("rights" + String(i), tokens[i].rights, "");
  	// xxxjack store into a token object
  }
}

void IotsaJWTTokenMod::configSave() {
  IotsaConfigFileSave cf("/config/statictokens.cfg");
  cf.put("ntoken", ntoken);
  for (int i=0; i<ntoken; i++) {
	  cf.put("token" + String(i), tokens[i].token);
  	cf.put("rights" + String(i), tokens[i].rights);
  }
}

void IotsaJWTTokenMod::loop() {
}

String IotsaJWTTokenMod::info() {
  String message = "<p>Static tokens enabled.";
  message += " See <a href=\"/tokens\">/tokens</a> to change.";
  message += "</p>";
  return message;
}

bool IotsaJWTTokenMod::needsAuthentication(const char *right) {
  if (server.hasHeader("Authorization")) {
    String authHeader = server.header("Authorization");
    if (authHeader.startsWith("Bearer ")) {
      String token = authHeader.substring(7);
      String rightField("/");
      rightField += right;
      rightField += "/";
      //IotsaSerial.print("Bearer token: "); IotsaSerial.println(token);
      //IotsaSerial.print("Rights needed: "); IotsaSerial.println(rightField);
      // Loop over all tokens.
      
      for (int i=0; i<ntoken; i++) {
        if (tokens[i].token == token) {
          // The token matches. Check the rights.
          if (tokens[i].rights == "*" || tokens[i].rights.indexOf(rightField) >= 0) {
            //IotsaSerial.print("Matched with: "); IotsaSerial.println(tokens[i].rights);
            return false;
          }
        }
      }
    }
  }
  IotsaSerial.println("No token match, try user/password");
  // If no rights fall back to username/password authentication
  return chain.needsAuthentication(right);
}
