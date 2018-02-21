#include <SPI.h>
#include <Controllino.h>
#include <Ethernet.h> // Make sure to use Controllino's Ethernet module (see compilation logs)
#include <WebServer.h> // https://github.com/sirleech/Webduino

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 167, 100);
WebServer webserver("", 80);

int FENCE_OUTPUT = CONTROLLINO_D0;
// Maybe a relay: CONTROLLINO_RELAY_00;


void home(WebServer &server, WebServer::ConnectionType type, char *, bool) {
  server.httpSuccess();
  if (type == WebServer::GET) {
    P(msg) = "https://github.com/tartopum/fence";
    server.printP(msg);
  }
}

void fenceState(WebServer &server, WebServer::ConnectionType type, char *, bool) {
  server.httpSuccess();
  if (type == WebServer::GET) {
    if (digitalRead(FENCE_OUTPUT)) {
        P(msg) = "0";
        server.printP(msg);
    } else {
        P(msg) = "1";
        server.printP(msg);
    }
  }
}

void fenceOn(WebServer &server, WebServer::ConnectionType type, char *, bool) {
  server.httpSuccess();
  digitalWrite(FENCE_OUTPUT, LOW);
}

void fenceOff(WebServer &server, WebServer::ConnectionType type, char *, bool) {
  server.httpSuccess();
  digitalWrite(FENCE_OUTPUT, HIGH);
}

void setup() {
  pinMode(FENCE_OUTPUT, OUTPUT);
  
  Ethernet.begin(mac, ip);
  webserver.setDefaultCommand(&home);
  webserver.addCommand("fence/on", &fenceOn);
  webserver.addCommand("fence/off", &fenceOff);
  webserver.addCommand("fence/state", &fenceState);
  webserver.begin();
}

void loop() {
  char buff[64];
  int len = 64;
  webserver.processConnection(buff, &len);
}
