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

/******** MQTT AUTH TOKEN ********/
const char* mqtt_username = "token";
const char* mqtt_token = "ESP_TOKEN_123";

/******** LCD ********/
LiquidCrystal_I2C lcd(0x27,16,2);

/******** LM35 ********/
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

  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWIFI_CONNECTED");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");

  delay(2000);
}

/******** MQTT RECONNECT ********/
void reconnect() {

  while (!client.connected()) {

    Serial.println("MQTT_CONNECTING");

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MQTT Connecting");

    if (client.connect("ESP32Client", mqtt_username, mqtt_token)) {

      Serial.println("MQTT_CONNECTED");
      Serial.print("AUTH_TOKEN:");
      Serial.println(mqtt_token);

      lcd.setCursor(0,1);
      lcd.print("Connected");

      delay(2000);
      lcd.clear();

    } else {

      Serial.print("MQTT_FAILED:");
      Serial.println(client.state());

      lcd.setCursor(0,1);
      lcd.print("Retrying...");

      delay(3000);
    }
  }
}

/******** SETUP ********/
void setup(){

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
}

/******** LOOP ********/
void loop(){

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  /******** READ LM35 ********/
  int adcValue = analogRead(sensorPin);
  float voltage = adcValue * (3.3 / 4095.0);
  float temp = voltage * 100.0;

  /******** WIFI SIGNAL ********/
  int rssi = WiFi.RSSI();

  /******** LCD DISPLAY ********/
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(temp,1);
  lcd.print("C   ");

  lcd.setCursor(0,1);
  lcd.print("RSSI:");
  lcd.print(rssi);
  lcd.print("dBm ");

  /******** SERIAL OUTPUT ********/
  Serial.print("TEMP:");
  Serial.println(temp,2);

  Serial.print("RSSI:");
  Serial.println(rssi);

  /******** MQTT PAYLOAD ********/
  char payload[80];

  sprintf(payload,
  "{\"token\":\"ESP_TOKEN_123\",\"temp\":%.2f,\"rssi\":%d}",
  temp, rssi);

  if(client.publish("esp32/sensor", payload)){
    Serial.println("MQTT_PUBLISH_SUCCESS");
  } else {
    Serial.println("MQTT_PUBLISH_FAILED");
  }

  delay(5000);
}