#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// WiFi Credentials
#define WIFI_SSID "FabLab UKFCET"
#define WIFI_PASSWORD "Ucars@2024"

// Firebase Credentials
#define FIREBASE_HOST "https://homeauto-4c25b-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyBLTQKUzS9nKvAfkJqYOi8I2mkcP_Rs4lY"

// Corrected Pin Definitions (using GPIO numbers)
#define LED_PIN 2        // GPIO2 (D4 on NodeMCU)
#define SWITCH1_PIN 0    // GPIO0 (D3 on NodeMCU)
#define SWITCH2_PIN 14   // GPIO14 (D5 on NodeMCU)
#define SWITCH3_PIN 12   // GPIO12 (D6 on NodeMCU)

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH1_PIN, OUTPUT);
  pinMode(SWITCH2_PIN, OUTPUT);
  pinMode(SWITCH3_PIN, OUTPUT);

  // Set all pins to LOW initially
  digitalWrite(LED_PIN, LOW);
  digitalWrite(SWITCH1_PIN, LOW);
  digitalWrite(SWITCH2_PIN, LOW);
  digitalWrite(SWITCH3_PIN, LOW);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected: " + WiFi.localIP().toString());

  // Set Firebase configuration
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Check and update LED status (using LED_STATUS)
  if (Firebase.getInt(fbdo, "/LED_STATUS")) {
    int status = fbdo.intData();
    digitalWrite(LED_PIN, status);
    Serial.println("Main LED Status: " + String(status));
  } else {
    Serial.println("Main LED Error: " + fbdo.errorReason());
  }

  // Check and update Switch 1 status (using LED_STATUS1)
  if (Firebase.getInt(fbdo, "/LED_STATUS1")) {
    int status = fbdo.intData();
    digitalWrite(SWITCH1_PIN, status);
    Serial.println("Switch 1 Status: " + String(status));
  } else {
    Serial.println("Switch 1 Error: " + fbdo.errorReason());
  }

  // Check and update Switch 2 status (using LED_STATUS2)
  if (Firebase.getInt(fbdo, "/LED_STATUS2")) {
    int status = fbdo.intData();
    digitalWrite(SWITCH2_PIN, status);
    Serial.println("Switch 2 Status: " + String(status));
  } else {
    Serial.println("Switch 2 Error: " + fbdo.errorReason());
  }

  // Check and update Switch 3 status (using LED_STATUS3)
  if (Firebase.getInt(fbdo, "/LED_STATUS3")) {
    int status = fbdo.intData();
    digitalWrite(SWITCH3_PIN, status);
    Serial.println("Switch 3 Status: " + String(status));
  } else {
    Serial.println("Switch 3 Error: " + fbdo.errorReason());
  }

  delay(500); // Check every 0.5 seconds
}
