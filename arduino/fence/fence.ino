#include <SPI.h>
#include <Controllino.h>
#include <Ethernet.h> // Make sure to use Controllino's Ethernet module (see compilation logs)
#include <WebServer.h> // https://github.com/sirleech/Webduino

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 167, 100);
WebServer webserver("", 80);

int STOP_OUTPUT = CONTROLLINO_RELAY_03;

// Fence
int FENCE_OUTPUT = CONTROLLINO_RELAY_04;
int FENCE_LED_OUTPUT = CONTROLLINO_D0;

// Light
int LIGHT1_INPUT = CONTROLLINO_DI0;
int LIGHT2_INPUT = CONTROLLINO_DI1;

int LIGHT_IN1_OUTPUT = CONTROLLINO_RELAY_09;
int LIGHT_IN2_OUTPUT = CONTROLLINO_RELAY_08;
int LIGHT_OUT_OUTPUT = CONTROLLINO_RELAY_06;

unsigned long lightLastDebounceTime = 0;
unsigned long lightDebounceDelay = 1000;
unsigned long LIGHT_PRESS_DELAY = 2000;
bool light1Active = true;
bool light2Active = true;
unsigned long light1LastHigh = 0;
unsigned long light2LastHigh = 0;
int light1State = LOW;
int light2State = LOW;


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
  digitalWrite(FENCE_LED_OUTPUT, LOW);
  digitalWrite(FENCE_OUTPUT, LOW);
}

void fenceOff(WebServer &server, WebServer::ConnectionType type, char *, bool) {
  server.httpSuccess();
  digitalWrite(FENCE_LED_OUTPUT, HIGH);
  digitalWrite(FENCE_OUTPUT, HIGH);
}

void switchRelay(int pin) {
  if(digitalRead(pin) == HIGH) {
    digitalWrite(pin, LOW);
  } else {
    digitalWrite(pin, HIGH);
  }
}

void light() {
  int light1 = digitalRead(LIGHT1_INPUT);
  int light2 = digitalRead(LIGHT2_INPUT);
  unsigned long mill = millis();

  if(mill - lightLastDebounceTime <= lightDebounceDelay) {
    return;
  }
  lightLastDebounceTime = mill;

  if(light1State == LOW && light1 == HIGH) {
    light1LastHigh = mill;
    light1Active = true;
  }
  if(light2State == LOW && light2 == HIGH) {
    light2LastHigh = mill;
    light2Active = true;
  }

  if(light1 == HIGH && light2 == HIGH) { // Outdoor
    if (mill - light1LastHigh > LIGHT_PRESS_DELAY && mill - light2LastHigh > LIGHT_PRESS_DELAY && light1Active && light2Active) {
      switchRelay(LIGHT_OUT_OUTPUT); 
      light1Active = false;
      light2Active = false;
    }
  } else {
    if(light1 == HIGH && light1Active && mill - light1LastHigh > LIGHT_PRESS_DELAY) {
      switchRelay(LIGHT_IN1_OUTPUT); 
      light1Active = false;
    }
    if(light2 == HIGH && light2Active && mill - light2LastHigh > LIGHT_PRESS_DELAY) {
      switchRelay(LIGHT_IN2_OUTPUT); 
      light2Active = false;
    }
  }
  
  light1State = light1;
  light2State = light2;
}

void setup() {
  pinMode(FENCE_OUTPUT, OUTPUT);

  pinMode(LIGHT1_INPUT, INPUT);
  pinMode(LIGHT2_INPUT, INPUT);
  pinMode(LIGHT_IN1_OUTPUT, OUTPUT);
  pinMode(LIGHT_IN2_OUTPUT, OUTPUT);
  pinMode(LIGHT_OUT_OUTPUT, OUTPUT);

  light1State = digitalRead(LIGHT_IN1_OUTPUT);
  light2State = digitalRead(LIGHT_IN2_OUTPUT);
  
  Ethernet.begin(mac, ip);
  webserver.setDefaultCommand(&home);
  webserver.addCommand("fence/on", &fenceOn);
  webserver.addCommand("fence/off", &fenceOff);
  webserver.addCommand("fence/state", &fenceState);
  webserver.begin();
}

void loop() {
  /*
  char buff[64];
  int len = 64;
  webserver.processConnection(buff, &len);
  */
  light(); 
}
