#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Network Credentials
#define WIFI_SSID "Shivam"
#define WIFI_PASSWORD "Shivam123"
#define API_KEY "AIzaSyDUofqIy2OKaMZft2GOb6LWYjejqFuub4s"
#define DATABASE_URL "https://ev-charging-ee60f-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// IR Sensor pins for 3 ports
#define IR_1A 12
#define IR_1B 14
#define IR_1C 27

// LED pins for 3 ports
// Port 1A LEDs
#define LED_1A_GREEN 13
#define LED_1A_RED 15

// Port 1B LEDs
#define LED_1B_GREEN 2
#define LED_1B_RED 4

// Port 1C LEDs
#define LED_1C_GREEN 16
#define LED_1C_RED 17

// State variables (false=Available, true=Occupied)
bool state1A = false;
bool state1B = false;
bool state1C = false;

// Previous sensor readings for edge detection
bool prevIR_1A = HIGH;
bool prevIR_1B = HIGH;
bool prevIR_1C = HIGH;

bool signupOK = false;

// Function to update Firebase and LEDs
void updatePortStatus(String port, bool isOccupied, int greenLED, int redLED) {
  String path = "stations/1/ports/" + port;
  String status = isOccupied ? "Occupied" : "Available";
  
  // Update LEDs
  if (isOccupied) {
    digitalWrite(greenLED, LOW);   // Turn OFF green
    digitalWrite(redLED, HIGH);    // Turn ON red
  } else {
    digitalWrite(greenLED, HIGH);  // Turn ON green
    digitalWrite(redLED, LOW);     // Turn OFF red
  }
  
  // Update Firebase
  if (Firebase.RTDB.setString(&fbdo, path.c_str(), status)) {
    Serial.println("✅ " + port + " set to " + status);
  } else {
    Serial.println("❌ Failed to update " + port);
    Serial.println("Reason: " + fbdo.errorReason());
  }
}

void setup() {
  Serial.begin(115200);

  // Configure IR sensor pins as INPUT
  pinMode(IR_1A, INPUT);
  pinMode(IR_1B, INPUT);
  pinMode(IR_1C, INPUT);

  // Configure LED pins as OUTPUT
  pinMode(LED_1A_GREEN, OUTPUT);
  pinMode(LED_1A_RED, OUTPUT);
  pinMode(LED_1B_GREEN, OUTPUT);
  pinMode(LED_1B_RED, OUTPUT);
  pinMode(LED_1C_GREEN, OUTPUT);
  pinMode(LED_1C_RED, OUTPUT);

  // Initialize all ports as Available (Green ON, Red OFF)
  digitalWrite(LED_1A_GREEN, HIGH);
  digitalWrite(LED_1A_RED, LOW);
  digitalWrite(LED_1B_GREEN, HIGH);
  digitalWrite(LED_1B_RED, LOW);
  digitalWrite(LED_1C_GREEN, HIGH);
  digitalWrite(LED_1C_RED, LOW);

  // Connect WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.println("\nWiFi Connected ✅");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Firebase config
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Sign up (anonymous authentication)
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Sign Up OK ✅");
    signupOK = true;
  } else {
    Serial.printf("Sign Up Error: %s\n", config.signer.signupError.message.c_str());
  }

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  Serial.println("Setup Complete!");
  Serial.println("Station 1 - Ports: 1A, 1B, 1C");
  Serial.println("IR Sensors & LEDs Active - Detecting vehicles...");
  Serial.println("🟢 Green LED = Available");
  Serial.println("🔴 Red LED = Occupied");
}

void loop() {
  // Only proceed if Firebase is ready
  if (!Firebase.ready() || !signupOK) {
    Serial.println("Firebase not ready!");
    delay(1000);
    return;
  }

  // Read IR sensor values (LOW = object detected, HIGH = no object)
  bool currentIR_1A = digitalRead(IR_1A);
  bool currentIR_1B = digitalRead(IR_1B);
  bool currentIR_1C = digitalRead(IR_1C);

  // Port 1A - Check for state change
  if (currentIR_1A != prevIR_1A) {
    delay(50); // Debounce delay
    currentIR_1A = digitalRead(IR_1A); // Re-read after debounce
    
    if (currentIR_1A != prevIR_1A) {
      prevIR_1A = currentIR_1A;
      
      if (currentIR_1A == LOW) {
        // Object detected - Port Occupied
        state1A = true;
        Serial.println("🚗 Vehicle detected at Port 1A - RED LED ON");
        updatePortStatus("1A", state1A, LED_1A_GREEN, LED_1A_RED);
      } else {
        // Object removed - Port Available
        state1A = false;
        Serial.println("✅ Vehicle left Port 1A - GREEN LED ON");
        updatePortStatus("1A", state1A, LED_1A_GREEN, LED_1A_RED);
      }
    }
  }

  // Port 1B - Check for state change
  if (currentIR_1B != prevIR_1B) {
    delay(50); // Debounce delay
    currentIR_1B = digitalRead(IR_1B); // Re-read after debounce
    
    if (currentIR_1B != prevIR_1B) {
      prevIR_1B = currentIR_1B;
      
      if (currentIR_1B == LOW) {
        // Object detected - Port Occupied
        state1B = true;
        Serial.println("🚗 Vehicle detected at Port 1B - RED LED ON");
        updatePortStatus("1B", state1B, LED_1B_GREEN, LED_1B_RED);
      } else {
        // Object removed - Port Available
        state1B = false;
        Serial.println("✅ Vehicle left Port 1B - GREEN LED ON");
        updatePortStatus("1B", state1B, LED_1B_GREEN, LED_1B_RED);
      }
    }
  }

  // Port 1C - Check for state change
  if (currentIR_1C != prevIR_1C) {
    delay(50); // Debounce delay
    currentIR_1C = digitalRead(IR_1C); // Re-read after debounce
    
    if (currentIR_1C != prevIR_1C) {
      prevIR_1C = currentIR_1C;
      
      if (currentIR_1C == LOW) {
        // Object detected - Port Occupied
        state1C = true;
        Serial.println("🚗 Vehicle detected at Port 1C - RED LED ON");
        updatePortStatus("1C", state1C, LED_1C_GREEN, LED_1C_RED);
      } else {
        // Object removed - Port Available
        state1C = false;
        Serial.println("✅ Vehicle left Port 1C - GREEN LED ON");
        updatePortStatus("1C", state1C, LED_1C_GREEN, LED_1C_RED);
      }
    }
  }

  delay(100); // Small delay for loop stability
}
