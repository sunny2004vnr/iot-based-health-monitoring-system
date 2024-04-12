#include <ArduinoJson.h>
#include "MAX30100_PulseOximeter.h"
#include <Wire.h>
#include <SoftwareSerial.h>

PulseOximeter pox;
#define REPORTING_PERIOD_MS     1000
uint32_t tsLastReport = 0;

//gsm module
SoftwareSerial gsmSerial(47, 49); //RX, TX

//node mcu sending data
SoftwareSerial nodemcu(5, 6); //has to be edited as  per the pin diageram

//max30102
int heartbeat;
int spo2;

//ad8232
int ecg;


//lcd
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//mlx
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


#define button 53
#define buzzer 51

//setup
void setup() {

  lcd.init();
  lcd.backlight();
  nodemcu.begin(9600);
  gsmSerial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);

  Serial.begin(9600);

  //ad8232
  pinMode(22, INPUT);
  pinMode(23, INPUT);

  //mlx
  Serial.println("Arduino MLX90614 Testing");
  mlx.begin();

  Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  //pox.setOnBeatDetectedCallback(onBeatDetected);
}

//loop

void loop() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& doc = jsonBuffer.createObject();

  Wire.setClock(100000UL);
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float bpm = pox.getHeartRate();
    int spo2 = pox.getSpO2();
    int ecg;

    if (digitalRead(23) && digitalRead(22) == HIGH) {
    }
    ecg = analogRead(A0);

    //temperature
    int temperature;
    temperature = mlx.readObjectTempC();
    Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
    //serial commiunication

    

    doc["ecgvalue"] = ecg;
    doc["temperature"] = temperature;
    doc["bpm"] = bpm;
    doc["spo2"] = spo2;

    lcd.setCursor(0, 0);
    lcd.print("TEMP  BPM  SPO2");
    lcd.setCursor(1, 1);
    lcd.print(temperature);lcd.print('C');
    lcd.setCursor(5, 1);
    lcd.print(bpm);
    lcd.setCursor(12, 1);
    lcd.print(spo2);

    tsLastReport = millis();
    doc.printTo(nodemcu);
//    jsonBuffer.clear();

    if (button == HIGH){
      message();
      caretaker();
      digitalWrite(buzzer, HIGH);
      delay(1000);
      digitalWrite(buzzer, LOW);
      delay(1000);
    }

  }
}

void message() {
  Serial.println("Setting the GSM in text mode");
  gsmSerial.println("AT+CMGF=1\r");
  delay(2000);
  Serial.println("Sending SMS to the desired phone number!");
  gsmSerial.println("AT+CMGS=\"+917659960815\"\r");
  // Replace x with mobile number
  delay(2000);
  gsmSerial.println("Somethings wrong with the patient");    // SMS Text
  delay(100);
  gsmSerial.println((char)26);
  delay(2000);
}

void caretaker() {
  Serial.println("Setting the GSM in text mode");
  gsmSerial.println("AT+CMGF=1\r");
  delay(2000);
  Serial.println("Sending SMS to the desired phone number!");
  gsmSerial.println("AT+CMGS=\"+916309414483\"\r");
  delay(2000);
  gsmSerial.println("need some help to be done");    // SMS Text
  delay(100);
  gsmSerial.println((char)26);
  delay(2000);
}

void onBeatDetected()
{
  Serial.println("Beat!");
}
