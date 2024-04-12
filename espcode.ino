#include <ThingerESP8266.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);

#define USERNAME "sunny_0315"
#define DEVICE_ID "sunny"
#define DEVICE_CREDENTIAL "nothing"

int heartbeat_value, spo2_value , ecg_value;
double temperature_value;
int test;


//timer
unsigned long previousMillis = 0;
unsigned long currentMillis;
const unsigned long period = 100;

const char* ssid = "MANIKYA RAO"; //--> Your wifi name or SSID.
const char* password = "no password";

#define ON_Board_LED 2
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup()
{
  nodemcu.begin(9600);
  Serial.begin(9600);
  test = 6;
  pinMode(ON_Board_LED, OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH);
  thing.add_wifi(ssid, password);
  thing["dht11"] >> [](pson & out) {
    out["temperature"] = temperature_value;
    out["heartbeat"] = heartbeat_value;
    out["spo2"] = spo2_value;
    out["ecg"] = ecg_value;
    out["test"] = test;
  };

}

void loop()
{
  currentMillis = millis();

  if ((currentMillis - previousMillis >= period)) {

    thing.handle();

    StaticJsonBuffer<1000> jsonBuffer;
    JsonObject& doc = jsonBuffer.parseObject(nodemcu);

    if (doc == JsonObject::invalid()) {
      //Serial.println("Invalid Json Object");
      jsonBuffer.clear();
      return;
    }
    Serial.println("JSON Object Recieved");
    Serial.print("Recieved Humidity:  ");
    heartbeat_value = doc["bpm"];
    spo2_value = doc["spo2"];
    temperature_value = doc["temperature"];
    ecg_value = doc["ecgvalue"];

    previousMillis = previousMillis + period;
  }
}
