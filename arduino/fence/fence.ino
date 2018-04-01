#include <SPI.h>
#include <Controllino.h>
#include <Ethernet.h> // Make sure to use Controllino's Ethernet module (see compilation logs)
#include <WebServer.h> // https://github.com/sirleech/Webduino

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// IPAddress ip(192, 168, 168, 115);
IPAddress ip(192, 168, 167, 102);
WebServer webserver("", 80);

// WARNING: to be customized!
const char API_HOST[] = "192.168.167.101";
const char API_AUTH_HEADER[] = "Authorization: Basic YWRtaW46YWRtaW4=";
const int API_PORT = 5000;

int STOP_OUTPUT = CONTROLLINO_RELAY_03;

// Fence
int FENCE_OUTPUT = CONTROLLINO_RELAY_04;
int FENCE_LED_OUTPUT = CONTROLLINO_DO0;

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

// Alarm
int alarmSwitchState = LOW;
bool alarmActivated = false;
int ALARM_BUZZER_DELAY = 10000;
unsigned long alarmBuzzerTimer = 0;
int ALARM_DETECTOR_INPUT = CONTROLLINO_DI3;
int ALARM_BUZZER_OUTPUT = CONTROLLINO_DO3;
int ALARM_LIGHT_OUTPUT = CONTROLLINO_DO1;
int ALARM_SWITCH_INPUT = CONTROLLINO_DI2;
int ALARM_ACTIVATED_OUTPUT = CONTROLLINO_DO4;


void switchOutput(int pin) {
    if(digitalRead(pin) == HIGH) {
        digitalWrite(pin, LOW);
    } else {
        digitalWrite(pin, HIGH);
    }
}

void homeRoute(WebServer &server, WebServer::ConnectionType type, char *, bool) {
    server.httpSuccess();
    if (type == WebServer::GET) {
        P(msg) = "https://github.com/tartopum/fence";
        server.printP(msg);
    }
}

void fenceRoute(WebServer &server, WebServer::ConnectionType type, char *, bool) {
    server.httpSuccess();

    if (type == WebServer::POST) {
        switchOutput(FENCE_OUTPUT);
        switchOutput(FENCE_LED_OUTPUT);
    }

    if (digitalRead(FENCE_OUTPUT)) {
        P(msg) = "0";
        server.printP(msg);
    } else {
        P(msg) = "1";
        server.printP(msg);
    }
}

void lightIn1Route(WebServer &server, WebServer::ConnectionType type, char *, bool) {
    server.httpSuccess();
    
    if (type == WebServer::POST) {
        switchOutput(LIGHT_IN1_OUTPUT);
    }

    if (digitalRead(LIGHT_IN1_OUTPUT)) {
        P(msg) = "1";
        server.printP(msg);
    } else {
        P(msg) = "0";
        server.printP(msg);
    }
}

void lightIn2Route(WebServer &server, WebServer::ConnectionType type, char *, bool) {
    server.httpSuccess();
    
    if (type == WebServer::POST) {
        switchOutput(LIGHT_IN2_OUTPUT);
    }

    if (digitalRead(LIGHT_IN2_OUTPUT)) {
        P(msg) = "1";
        server.printP(msg);
    } else {
        P(msg) = "0";
        server.printP(msg);
    }
}

void lightOutRoute(WebServer &server, WebServer::ConnectionType type, char *, bool) {
    server.httpSuccess();
    
    if (type == WebServer::POST) {
        switchOutput(LIGHT_OUT_OUTPUT);
    }

    if (digitalRead(LIGHT_OUT_OUTPUT)) {
        P(msg) = "1";
        server.printP(msg);
    } else {
        P(msg) = "0";
        server.printP(msg);
    }
}

void alarmRoute(WebServer &server, WebServer::ConnectionType type, char *, bool) {
    server.httpSuccess();
    
    if (type == WebServer::POST) {
        alarmActivated = !alarmActivated;
    }

    if (alarmActivated) {
        P(msg) = "1";
        server.printP(msg);
    } else {
        P(msg) = "0";
        server.printP(msg);
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
            switchOutput(LIGHT_OUT_OUTPUT); 
            light1Active = false;
            light2Active = false;
        }
    } else {
        if(light1 == HIGH && light1Active && mill - light1LastHigh > LIGHT_PRESS_DELAY) {
            switchOutput(LIGHT_IN1_OUTPUT); 
            light1Active = false;
        }
        if(light2 == HIGH && light2Active && mill - light2LastHigh > LIGHT_PRESS_DELAY) {
            switchOutput(LIGHT_IN2_OUTPUT); 
            light2Active = false;
        }
    }
    
    light1State = light1;
    light2State = light2;
}

void requestAlarmDetector(bool on) {
    EthernetClient client;
    if(!client.connect(API_HOST, API_PORT)) {
        return;
    }
    client.println("POST /alarm_detector HTTP/1.1");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println("Authorization: Basic YWRtaW46YWRtaW4=");
    if(on) {
        client.println("Content-Length: 4");
        client.println();
        client.println("true");
    } else {
        client.println("Content-Length: 5");
        client.println();
        client.println("false");
    }
    client.println();
    client.stop();
}

void alarm() {
    requestAlarmDetector(false);

    digitalWrite(ALARM_ACTIVATED_OUTPUT, alarmActivated ? HIGH : LOW);
    digitalWrite(STOP_OUTPUT, alarmActivated ? LOW : HIGH);

    int switchState = digitalRead(ALARM_SWITCH_INPUT);
    if(switchState != alarmSwitchState) {
        alarmActivated = switchState == HIGH;
    }
    alarmSwitchState = switchState;

    if(!alarmActivated) {
        digitalWrite(ALARM_BUZZER_OUTPUT, LOW);
        digitalWrite(ALARM_LIGHT_OUTPUT, LOW);
        digitalWrite(LIGHT_OUT_OUTPUT, LOW);
        alarmBuzzerTimer = 0;
        return;
    }

    unsigned long mill = millis();
    if(digitalRead(ALARM_DETECTOR_INPUT)) {
        if(!alarmBuzzerTimer) {
            alarmBuzzerTimer = mill;
        } else if(mill - alarmBuzzerTimer > ALARM_BUZZER_DELAY) {
            digitalWrite(ALARM_BUZZER_OUTPUT, HIGH);
            digitalWrite(ALARM_LIGHT_OUTPUT, HIGH);
            digitalWrite(LIGHT_OUT_OUTPUT, HIGH);
            alarmBuzzerTimer = 0;
            requestAlarmDetector(true);
        }
    } else {
        alarmBuzzerTimer = 0;
    }
}

void setup() {
    pinMode(FENCE_OUTPUT, OUTPUT);
    pinMode(FENCE_LED_OUTPUT, OUTPUT);
    pinMode(LIGHT1_INPUT, INPUT);
    pinMode(LIGHT2_INPUT, INPUT);
    pinMode(LIGHT_IN1_OUTPUT, OUTPUT);
    pinMode(LIGHT_IN2_OUTPUT, OUTPUT);
    pinMode(LIGHT_OUT_OUTPUT, OUTPUT);
    pinMode(ALARM_SWITCH_INPUT, INPUT);
    pinMode(ALARM_DETECTOR_INPUT, INPUT);
    pinMode(ALARM_BUZZER_OUTPUT, OUTPUT);
    pinMode(ALARM_LIGHT_OUTPUT, OUTPUT);
    pinMode(ALARM_ACTIVATED_OUTPUT, OUTPUT);
    pinMode(STOP_OUTPUT, OUTPUT);

    light1State = digitalRead(LIGHT_IN1_OUTPUT);
    light2State = digitalRead(LIGHT_IN2_OUTPUT);
    alarmSwitchState = digitalRead(ALARM_SWITCH_INPUT);
    alarmActivated = alarmSwitchState == HIGH;
    
    Ethernet.begin(mac, ip);
    webserver.setDefaultCommand(&homeRoute);
    webserver.addCommand("fence", &fenceRoute);
    webserver.addCommand("light_in1", &lightIn1Route);
    webserver.addCommand("light_in2", &lightIn2Route);
    webserver.addCommand("light_out", &lightOutRoute);
    webserver.addCommand("alarm", &alarmRoute);
    webserver.begin();
}

void loop() {
    char buff[64];
    int len = 64;
    webserver.processConnection(buff, &len);
    light(); 
    alarm();
}
