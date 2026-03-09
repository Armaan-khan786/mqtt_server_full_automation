#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/******** WIFI DETAILS ********/
const char* ssid = "@armaan_khxn__";
const char* password = "786786786j";

/******** MQTT SERVER ********/
const char* mqtt_server = "192.168.222.100";
const int mqtt_port = 1885;

/******** MQTT AUTH ********/
const char* mqtt_username = "token";
const char* mqtt_token = "ESP_TOKEN_123";

/******** LCD ********/
LiquidCrystal_I2C lcd(0x27, 16, 2);

/******** LM35 SENSOR ********/
int sensorPin = 34;

/******** MQTT CLIENT ********/
WiFiClient espClient;
PubSubClient client(espClient);

/******** WIFI CONNECT ********/
void setup_wifi() {

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");

  Serial.println("WIFI_CONNECTING");

  WiFi.begin(ssid, password);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
    attempts++;

    if(attempts > 20){
      Serial.println("\nWIFI_TIMEOUT");
      return;
    }
  }

  Serial.println("\nWIFI_CONNECTED");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");

  delay(2000);
}

/******** MQTT CONNECT ********/
void connectMQTT() {

  Serial.println("MQTT_CONNECTING");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MQTT Connecting");

  if (client.connect("ESP32Client", mqtt_username, mqtt_token)) {

    Serial.println("MQTT_CONNECTED");

    lcd.setCursor(0,1);
    lcd.print("Connected");

    delay(2000);
    lcd.clear();
  }

  else {

    int errorCode = client.state();

    Serial.print("MQTT_FAILED:");
    Serial.println(errorCode);

    lcd.setCursor(0,1);
    lcd.print("Error:");
    lcd.print(errorCode);

    delay(3000);
  }
}

/******** SETUP ********/
void setup() {

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
}

/******** LOOP ********/
void loop() {

  /******** WIFI STATUS CHECK ********/
  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WIFI_DISCONNECTED");

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WiFi Lost");

    setup_wifi();
    Serial.println("WIFI_RECONNECTED");
  }

  /******** MQTT STATUS CHECK ********/
  if (!client.connected()) {

    Serial.println("MQTT_CONNECTION_LOST");

    connectMQTT();
    delay(3000);
    return;
  }

  client.loop();

  /******** READ LM35 ********/
  int adcValue = analogRead(sensorPin);

  float voltage = adcValue * (3.3 / 4095.0);

  float temp = voltage * 100.0;

  /******** WIFI SIGNAL ********/
  int rssi = WiFi.RSSI();

  if(rssi > -50){
    Serial.println("RSSI_STRONG");
  }
  else if(rssi > -70){
    Serial.println("RSSI_GOOD");
  }
  else{
    Serial.println("RSSI_WEAK");
  }

  /******** LCD DISPLAY ********/
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(temp,1);
  lcd.print("C");

  lcd.setCursor(0,1);
  lcd.print("RSSI:");
  lcd.print(rssi);

  /******** SERIAL OUTPUT ********/
  Serial.print("TEMP:");
  Serial.println(temp,2);

  Serial.print("RSSI:");
  Serial.println(rssi);

  /******** MQTT PAYLOAD ********/
  char payload[100];

  sprintf(payload,
  "{\"token\":\"%s\",\"temp\":%.2f,\"rssi\":%d}",
  mqtt_token, temp, rssi);

  /******** MQTT PUBLISH ********/
  if (client.publish("esp32/sensor", payload)) {

    Serial.println("MQTT_PUBLISH_SUCCESS");
  }

  else {

    Serial.println("MQTT_PUBLISH_FAILED");
  }

  delay(5000);
}