#include <SoftwareSerial.h>

const byte rxPin = 2; // Wire this to Tx Pin of ESP8266
const byte txPin = 3; // Wire this to Rx Pin of ESP8266

int inPin = 5;         // the number of the input pin
int outPin = 13;       // the number of the output pin
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

int initialize = 0;
int responseCount = 0;
String inData;

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
  reading = digitalRead(inPin);

  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH) state = LOW;
    else state = HIGH;
    time = millis();    
  }
  
  digitalWrite(outPin, state);
  previous = reading;

  
  if(initialize == 0){
    responseCount = 0;
    Serial.println("AT...");
    ESP8266.println("AT");
    while(responseCount < 5){
      inData = ESP8266.readStringUntil('\n');
      if(inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
      responseCount++;
    }
    initialize = 1;
  }
  if(initialize == 1){
    responseCount = 0;
    Serial.println("AT+CWMODE=1...");
    ESP8266.println("AT+CWMODE=1");
    while(responseCount < 5){
      inData = ESP8266.readStringUntil('\n');
      if(inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
      responseCount++;
    }
    initialize = 2;
  }  
  if(initialize == 2){
    responseCount = 0;
    Serial.println("AT+CWJAP=”esp”,”testpass”...");
    ESP8266.println("AT+CWJAP=\"esp\",\"testpass\"");
    while(responseCount < 10){
      inData = ESP8266.readStringUntil('\n');
      if(inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
      responseCount++;
    }
    initialize = 3;
  }  
  if(initialize == 3){
    responseCount = 0;
    Serial.println("AT+CIPSTART=\"TCP\",\"168.235.103.76\",3001");
    ESP8266.println("AT+CIPSTART=\"TCP\",\"168.235.103.76\",3001");
    while(responseCount < 10){
      inData = ESP8266.readStringUntil('\n');
      if(inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
      responseCount++;
    }
    initialize = 4;
  }  
  if(initialize == 4){
    responseCount = 0;
    Serial.println("AT+CIPSTATUS");
    ESP8266.println("AT+CIPSTATUS");
    while(responseCount < 10){
      inData = ESP8266.readStringUntil('\n');
      if(inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
      responseCount++;
    }
    initialize = 5;
  }  
  if(initialize == 5){
    responseCount = 0;
    Serial.println("AT+CIPSEND=4");
    ESP8266.println("AT+CIPSEND=4");   
    while(responseCount < 20){
      inData = ESP8266.readStringUntil('\n');
      if(inData.length() > 1) Serial.println("--- Got reponse from ESP8266: " + inData);
      if(inData[0] == '>'){
        Serial.println("test");
        ESP8266.println("test");
      }
      responseCount++;
    }
    initialize = 6;
  }  
  if(initialize == 6){
    Serial.println("Done");
    initialize = 7;
  }
}
