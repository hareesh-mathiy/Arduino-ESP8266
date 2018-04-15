#include <SoftwareSerial.h>

const byte rxPin = 2; // Wire this to Tx Pin of ESP8266
const byte txPin = 3; // Wire this to Rx Pin of ESP8266

int inPin = 8;         // the number of the input pin
int outPin = 13;       // the number of the output pin
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

int initialize = 0;
String inData;
String outData = "off";
String received;
int overWrite = 0;

// We'll use a software serial interface to connect to ESP8266
SoftwareSerial ESP8266 (rxPin, txPin);

void setup() {
  pinMode(inPin, INPUT);
  pinMode(outPin, OUTPUT);

  Serial.begin(9600);
  ESP8266.begin(9600);
  delay(1000); // Let the module self-initialize
}

void loop() {
  // read the state of the pushbutton value:
  if(overWrite == 0) reading = digitalRead(inPin);
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH) {
      state = LOW;
      outData = "off";
      if(overWrite == 0) initialize = 5;
    }
    else {
      state = HIGH;
      outData = "on";
      if(overWrite == 0) initialize = 5;
    }
    overWrite = 0;
    time = millis();
  }

  digitalWrite(outPin, state);
  previous = reading;

  inData = ESP8266.readStringUntil('\n');
  if (inData.indexOf("+IPD") > -1) {
    received = inData;
    initialize = 6;
  }


  if (initialize == 0) {
    inData = "";
    Serial.println("AT...");
    ESP8266.println("AT");
    while (inData.indexOf("OK") == -1) {
      inData = ESP8266.readStringUntil('\n');
      if (inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
    }
    initialize = 1;
  }
  if (initialize == 1) {
    inData = "";
    Serial.println("AT+CWMODE=1...");
    ESP8266.println("AT+CWMODE=1");
    while (inData.indexOf("OK") == -1) {
      inData = ESP8266.readStringUntil('\n');
      if (inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
    }
    initialize = 2;
  }
  if (initialize == 2) {
    inData = "";
    Serial.println("AT+CWJAP=”esp”,”testpass”...");
    ESP8266.println("AT+CWJAP=\"esp\",\"testpass\"");
    while (inData.indexOf("OK") == -1) {
      inData = ESP8266.readStringUntil('\n');
      if (inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
    }
    initialize = 3;
  }
  if (initialize == 3) {
    inData = "";
    Serial.println("AT+CIPSTART=\"TCP\",\"168.235.103.76\",3002");
    ESP8266.println("AT+CIPSTART=\"TCP\",\"168.235.103.76\",3002");
    while (inData.indexOf("OK") == -1) {
      inData = ESP8266.readStringUntil('\n');
      if (inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
    }
    initialize = 4;
  }
  if (initialize == 4) {
    inData = "";
    Serial.println("AT+CIPSTATUS");
    ESP8266.println("AT+CIPSTATUS");
    while (inData.indexOf("OK") == -1) {
      inData = ESP8266.readStringUntil('\n');
      if (inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
    }
    initialize = 6;
  }
  if (initialize == 5) {
    inData = "";
    Serial.println("AT+CIPSEND=4");
    ESP8266.println("AT+CIPSEND=4");
    while (inData.indexOf("SEND OK") == -1) {
      inData = ESP8266.readStringUntil('\n');
      if (inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
      if (inData.indexOf("+IPD") > -1) {
        received = inData;
        inData = "SEND OK";
      }
      if (inData[0] == '>') {
        Serial.println(outData);
        ESP8266.println(outData);
      }
    }
    initialize = 6;
  }
  if (initialize == 6) {
    Serial.println("Done");
    initialize = 7;
    if (received.length() > 1) Serial.println("--- Got reponse from server: " + received);
    if (received.indexOf("on") > -1) {
      overWrite = 1;
      reading = HIGH;
      previous = LOW;
      state = LOW;
      Serial.println("Turned LED on");
    }
    else if (received.indexOf("off") > -1) {
      overWrite = 1;
      reading = HIGH;
      previous = LOW;
      state = HIGH;
      Serial.println("Turned LED off");
    }
  }
}
