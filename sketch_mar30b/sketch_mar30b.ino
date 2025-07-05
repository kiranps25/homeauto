#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

// Firebase Credentials
#define FIREBASE_HOST "homeauto-4c25b-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyBLTQKUzS9nKvAfkJqYOi8I2mkcP_Rs4lY"

// GPIO Pin Definitions
#define LED_PIN        2   // D4
#define SWITCH1_PIN    0   // D3
#define SWITCH2_PIN   14   // D5
#define SWITCH3_PIN   12   // D6

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Polling interval
unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 200; // milliseconds

void setup() {
  Serial.begin(115200);

  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH1_PIN, OUTPUT);
  pinMode(SWITCH2_PIN, OUTPUT);
  pinMode(SWITCH3_PIN, OUTPUT);

  // Default all to LOW
  digitalWrite(LED_PIN, LOW);
  digitalWrite(SWITCH1_PIN, LOW);
  digitalWrite(SWITCH2_PIN, LOW);
  digitalWrite(SWITCH3_PIN, LOW);

  // Setup WiFi with WiFiManager
  WiFiManager wm;
  // wm.resetSettings();  // ← Uncomment to clear saved WiFi settings
  if (!wm.autoConnect("MyESP8266", "12345678")) {
    Serial.println("❌ Failed to connect. Restarting...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("✅ WiFi Connected. IP: " + WiFi.localIP().toString());

  // Firebase Setup
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  unsigned long currentTime = millis();

  if (Firebase.ready() && (currentTime - lastCheckTime >= checkInterval)) {
    lastCheckTime = currentTime;

    if (Firebase.getJSON(fbdo, "/")) {
      FirebaseJson& json = fbdo.jsonObject();
      FirebaseJsonData result;

      if (json.get(result, "LED_STATUS/status") && result.success) {
        digitalWrite(LED_PIN, result.to<int>());
        Serial.println("LED: " + result.to<String>());
      }

      if (json.get(result, "LED_STATUS1/status") && result.success) {
        digitalWrite(SWITCH1_PIN, result.to<int>());
        Serial.println("Switch1: " + result.to<String>());
      }

      if (json.get(result, "LED_STATUS2/status") && result.success) {
        digitalWrite(SWITCH2_PIN, result.to<int>());
        Serial.println("Switch2: " + result.to<String>());
      }

      if (json.get(result, "LED_STATUS3/status") && result.success) {
        digitalWrite(SWITCH3_PIN, result.to<int>());
        Serial.println("Switch3: " + result.to<String>());
      }

    } else {
      Serial.println("❌ Firebase JSON Fetch Failed: " + fbdo.errorReason());
    }
  }
}
