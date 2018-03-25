#include <SoftwareSerial.h>

const byte rxPin = 2; // Wire this to Tx Pin of ESP8266
const byte txPin = 3; // Wire this to Rx Pin of ESP8266

int inPin = 7;         // the number of the input pin
int outPin = 13;       // the number of the output pin
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

int initialize = 0;
int responseCount = 0;
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
   if(overWrite == 0){
      reading = digitalRead(inPin);
   }
  if(initialize == 7 || initialize == 0){
    if (reading == HIGH && previous == LOW && millis() - time > debounce) {
      if (state == HIGH){
        state = LOW;
        outData = "off";
      }
      else {
        state = HIGH;
        outData = "on";
      }
      if(overWrite == 0) initialize = 5;
      time = millis();   
      overWrite = 0; 
    }
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
       if(inData.indexOf("+IPD") > -1){
        received = inData;
        responseCount = 20;
       }
      if(inData[0] == '>'){
        Serial.println(outData);
        ESP8266.println(outData);
      }
      responseCount++;
    }
    initialize = 6;
  }  
  if(initialize == 6){
    Serial.println("Done");
    initialize = 7;
    if(received.length() > 1) Serial.println("--- Got reponse from server: " + received);
    if(received.indexOf("on") > -1){
      overWrite = 1;
      reading = HIGH;
       previous = LOW;
      state = LOW;
      Serial.println("Turned LED on");
    }
    else if(received.indexOf("off") > -1){
      overWrite = 1;
      reading = HIGH;
      previous = LOW;
      state = HIGH;
      Serial.println("Turned LED off");
    }
  }
}
