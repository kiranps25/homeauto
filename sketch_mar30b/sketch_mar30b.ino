#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <WiFiManager.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Firebase
#define FIREBASE_HOST "homeauto-4c25b-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyBLTQKUzS9nKvAfkJqYOi8I2mkcP_Rs4lY"

#define LED_PIN        2   // D4
#define SWITCH1_PIN    0   // D3
#define SWITCH2_PIN   14   // D5
#define SWITCH3_PIN   12   // D6

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);  // IST offset +5:30

unsigned long lastCheck = 0;
const unsigned long checkInterval = 1000;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH1_PIN, OUTPUT);
  pinMode(SWITCH2_PIN, OUTPUT);
  pinMode(SWITCH3_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(SWITCH1_PIN, LOW);
  digitalWrite(SWITCH2_PIN, LOW);
  digitalWrite(SWITCH3_PIN, LOW);

  WiFiManager wm;
  if (!wm.autoConnect("MyESP8266", "12345678")) {
    Serial.println("❌ Failed to connect. Restarting...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("✅ WiFi Connected: " + WiFi.localIP().toString());

  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  timeClient.begin();
}

void checkAndUpdate(String path, int pin) {
  if (Firebase.getJSON(fbdo, path)) {
    FirebaseJson& json = fbdo.jsonObject();
    FirebaseJsonData statusData, onData, offData;

    json.get(statusData, "status");
    json.get(onData, "scheduled_on");
    json.get(offData, "scheduled_off");

    int currentStatus = statusData.success ? statusData.to<int>() : 0;
    String onTime = onData.success ? onData.to<String>() : "";
    String offTime = offData.success ? offData.to<String>() : "";

    String nowStr = timeClient.getFormattedTime();  // format: HH:MM:SS
    nowStr.trim();

    if (onTime == nowStr) {
      Firebase.setInt(fbdo, path + "/status", 1);
      digitalWrite(pin, HIGH);
      Serial.println(path + " Turned ON at " + nowStr);
    }
    else if (offTime == nowStr) {
      Firebase.setInt(fbdo, path + "/status", 0);
      Firebase.set(fbdo, path + "/scheduled_on", "");
      Firebase.set(fbdo, path + "/scheduled_off", "");
      digitalWrite(pin, LOW);
      Serial.println(path + " Turned OFF at " + nowStr);
    } else {
      digitalWrite(pin, currentStatus);
    }
  }
}

void loop() {
  timeClient.update();
  unsigned long now = millis();

  if (Firebase.ready() && (now - lastCheck >= checkInterval)) {
    lastCheck = now;

    checkAndUpdate("/LED_STATUS", LED_PIN);
    checkAndUpdate("/LED_STATUS1", SWITCH1_PIN);
    checkAndUpdate("/LED_STATUS2", SWITCH2_PIN);
    checkAndUpdate("/LED_STATUS3", SWITCH3_PIN);
  }
}
