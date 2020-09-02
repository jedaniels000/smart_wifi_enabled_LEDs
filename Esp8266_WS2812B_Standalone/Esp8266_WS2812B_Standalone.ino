// ESP8266
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// LED Control
#include <FastLED.h>

#include "private.h"

// ESP8266
#define BLYNK_PRINT Serial

// LED Control
#define LED_PIN     4   // output pin for the LED strip
#define NUM_LEDS    60  // number of LEDs being controlled
CRGB leds[NUM_LEDS];
CRGB ledsTemp[NUM_LEDS];

int redVal = 0;
int greenVal = 0;
int blueVal = 0;

int pinValue = 0;

void setup()
{
  // LEDs
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  pinMode(LED_BUILTIN, OUTPUT);
  
  // Setup blink pin
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  
  // Debug console
  Serial.begin(115200);

  // auth, ssid, pass defined in private.h
  Blynk.begin(auth, ssid, pass);

  // Used for OTA (over the air) updates
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setHostname("LED Lights Esp8266");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  turnOffLights();
  digitalWrite(LED_BUILTIN, LOW);
}

// On off pin
BLYNK_WRITE(V0){
  pinValue = param.asInt();

  if(pinValue == 1){
    runLightAnim();
  }else{
    turnOffLights();
  }
}

BLYNK_WRITE(V1){
  redVal = param.asInt();
  
  runLightAnim();
  Blynk.virtualWrite(V0, HIGH);
}

BLYNK_WRITE(V2){
  greenVal = param.asInt();
  
  runLightAnim();
  Blynk.virtualWrite(V0, HIGH);
}

BLYNK_WRITE(V3){
  blueVal = param.asInt();
  
  runLightAnim();
  Blynk.virtualWrite(V0, HIGH);
}

void loop()
{
  ArduinoOTA.handle();
  
  Blynk.run();
}

void runLightAnim(){
  for (int i = 0; i < 60; i++) {
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }
  FastLED.show();
}

void turnOffLights(){
  for (int i = 0; i < 60; i++) {
    leds[i] = CRGB (0, 0, 0);
  }
  FastLED.show();
}
