/*
  Digital Ruler - Integrated With MQTT
  Powered by FTEK UKSW

  Semangat belajarnya yaa! :)
*/



#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>

// --- KONFIGURASI WIFI & MQTT ---
const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_topic = "workshop/ftek/namatopic";

WiFiClient espClient;
PubSubClient client(espClient);

// -------- Global Variabel & Define ------------------- //
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int echoPin = 18;   
const int trigPin = 5;    
const int btnPin  = 17;   

long duration;
float distanceCm;
bool lastBtnState = HIGH; 


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {

  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(btnPin, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("Gagal mendeteksi OLED."));
    for(;;);
  }

  // Tampilan Awal
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("Digital Ruler");
  display.println("Connecting WiFi...");
  display.display();

  // Setup Koneksi
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  
  // Tampilan Siap
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("WiFi Connected!");
  display.println("MQTT Ready.");
  display.display();
  delay(1000);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // --- 1. Proses Ukur Jarak ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * 0.034 / 2;

  // --- 2. Tampilkan di OLED ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Powered by - ");
  display.print("FTEK UKSW");
  
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.print(distanceCm);
  display.setTextSize(1);
  display.print(" cm");
  
  display.setCursor(0, 55);
  display.print("[Btn] -> Publish");
  display.display();

  // --- 3. Logic Push Button -> Publish MQTT ---
  int btnState = digitalRead(btnPin);

  if (btnState == LOW && lastBtnState == HIGH) {
    Serial.print("Push Button Ditekan! Jarak: ");
    Serial.print(distanceCm);
    Serial.println(" cm");

    String payload = String(distanceCm);
    
    if (client.publish(mqtt_topic, payload.c_str())) {
      Serial.println("Data terkirim ke MQTT Broker!");
      display.setCursor(80, 55);
      display.print("SENT!");
    } else {
      Serial.println("Gagal mengirim data.");
    }
    
    display.invertDisplay(true);
    delay(500);
    display.invertDisplay(false);
  }
  
  lastBtnState = btnState;
  delay(100); 
}