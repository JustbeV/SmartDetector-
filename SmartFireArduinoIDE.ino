#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Firebase Realtime Database
const char* databaseURL = "YOUR_FIREBASE_URL";
const char* authToken = "YOUR_FB_AUTH_TOKEN";  

//for php dashboard
const char* serverURL = "YOUR_PHP_SERVER_URL";

// Sensor pins
#define SMOKE_DIGITAL_PIN 32
#define FLAME_DO_PIN 35   // digital DO pin of flame sensor

// Track the last known state
String lastState = "normal";

void setup() {
  Serial.begin(115200);
  pinMode(SMOKE_DIGITAL_PIN, INPUT);
  pinMode(FLAME_DO_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n WiFi connected");
}

void loop() {
  int smoke = digitalRead(SMOKE_DIGITAL_PIN);
  int flame = digitalRead(FLAME_DO_PIN);  // digital output of flame sensor

  String currentState = "normal";

  if (flame == LOW) { 
    // Flame sensor DO goes LOW when fire is detected
    currentState = "fire";
  } 
  else if (smoke == HIGH) {
    currentState = "smoke";
  }

  // Only send when the state changes
  if (currentState != lastState) {
    if (currentState == "fire") {
      logEvent("Fire Detected");
    } 
    else if (currentState == "smoke") {
      logEvent("Smoke Detected");
    } 
    else if (currentState == "normal" && (lastState == "fire" || lastState == "smoke")) {
      logEvent("Safe");
    }

    lastState = currentState;
  }

  delay(500); // check every 0.5s
}

void logEvent(String eventType) {
  if (WiFi.status() == WL_CONNECTED) {

    // POST TO FIREBASE all events 
    HTTPClient httpFirebase;
    String url = String(databaseURL) + "/fire_events.json";
    if (strlen(authToken) > 0) {
      url += "?auth=" + String(authToken);
    }
    httpFirebase.begin(url);
    httpFirebase.addHeader("Content-Type", "application/json");
    String jsonData = "{\"event\":\"" + eventType + "\",\"timestamp\":" + String(millis()) + "}";
    int httpResponseCodeFirebase = httpFirebase.POST(jsonData);
    if (httpResponseCodeFirebase > 0) {
      Serial.print("[Firebase] Logged: ");
      Serial.println(eventType);
    } else {
      Serial.print("[Firebase] Error posting: ");
      Serial.println(httpResponseCodeFirebase);
    }
    httpFirebase.end();

    //  POST TO PHP BACKEND (only fire/smoke) 
    if (eventType == "Fire Detected" || eventType == "Smoke Detected") {
      HTTPClient httpPHP;
      httpPHP.begin(serverURL);
      httpPHP.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String postData = "event=" + eventType;
      int httpResponseCodePHP = httpPHP.POST(postData);
      if (httpResponseCodePHP > 0) {
        Serial.print("[PHP] Logged: ");
        Serial.println(eventType);
      } else {
        Serial.print("[PHP] Error posting: ");
        Serial.println(httpResponseCodePHP);
      }
      httpPHP.end();
    }

  } else {
    Serial.println("WiFi not connected");
  }
}
