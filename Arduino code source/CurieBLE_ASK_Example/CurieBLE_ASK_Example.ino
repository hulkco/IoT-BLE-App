/*
 * Arduino Curie BLE (Arduino 101 Board) example for the
 * Arduino Sensor Kit https://store.arduino.cc/usa/sensor-kit-base
 * 
 * This example code is in the public domain and is based on arduino.cc 
 * 
 * The Arduino_SensorKit.h library is used to communicate with the sensors 
 * and the CurieBLE.h library is used to communicate with the "IoT BLE App" 
 * application created in App Inventor
 * 
 * Note: This sketch was created for the BLE stack of the Arduino 101 board, 
 * if you use another, you will have to make the necessary changes.
 * 
 */
#include <CurieBLE.h>
#include "Arduino_SensorKit.h"

#define BUZZER 5
#define BUTTON  4
#define LED 6
#define POT A0
#define LIGHT A3

int pot_value;
bool button_state;

//DHT dht(DHTPIN, DHT11);

// "¡Please Dont forget to change with yout own UUID!"
// Can you create in: https://www.uuidgenerator.net/

BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService ASKIT("19B10010-E8F2-537E-4F6C-D104768A1214"); // BLE Service of ASKIT (Arduino Sensor Kit)
//ASKIT = Arduino Sensor Kit
//ASK = Arduino Sensor Kit"
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central 
BLEShortCharacteristic DHT11_TEMP( "19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEShortCharacteristic DHT11_HUMI( "19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEShortCharacteristic LED_ASK( "19B10013-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEShortCharacteristic LIGHT_ASK( "19B10014-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

long previousMillis = 0;

int send_data_temp;
int send_data_humi;
int send_data_light;

void setup() {
  Serial.begin(9600);
  
  // set advertised local name and service UUID:
  blePeripheral.setLocalName("Arduino Sensor Kit");
  blePeripheral.setAdvertisedServiceUuid(ASKIT.uuid());

  // add service and characteristic:
  blePeripheral.addAttribute(ASKIT);
  blePeripheral.addAttribute(DHT11_TEMP);
  blePeripheral.addAttribute(DHT11_HUMI);
  blePeripheral.addAttribute(LED_ASK);
  blePeripheral.addAttribute(LIGHT_ASK);

  LED_ASK.setValue(0);
  //BUTTON_ASK.setValue(0);  

  // begin advertising BLE service:
  blePeripheral.begin();

  Serial.println("BLE Read service start.");
  
  //Setup ASK
  Environment.begin(); 
  
  //pinMode(MIC , INPUT);
  pinMode(LIGHT , INPUT);
  //pinMode(BUTTON , INPUT);
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(BUZZER, OUTPUT);

  Oled.begin();
  Oled.setFlipMode(false); 
}

void loop() {
  
  //Setup Oled Display
  Oled.setFont(u8x8_font_8x13B_1x2_f);
  
  //cursor values are in characters, not pixels
  Oled.setCursor(0, 0);
  Oled.print("Loading...");
  
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();
  
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected... ");
    Oled.setFont(u8x8_font_amstrad_cpc_extended_r);
    Oled.setCursor(0, 2);
    Oled.print("Connected...");
    // print the central's MAC address:
    Serial.println(central.address());    
    delay(3000);
    Oled.clear();

    // while the central is still connected to peripheral:
    while (central.connected()) {
      long currentMillis = millis();
      // if 10s have passed, check the sensor:
      if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
        //Oled.clear();
        Oled.setFont(u8x8_font_8x13B_1x2_f);  
        //cursor values are in characters, not pixels
        Oled.setCursor(0, 0);
        Oled.print("Sensor Values");                        
        updateTemp();              
        updateHumi();
        updateLed();
        updateLight();
      }
    }
    

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    Oled.clear();
    Oled.setFont(u8x8_font_8x13B_1x2_f);
    Oled.setCursor(0, 0);
    Oled.print("Loading...");
      
  }
}

void updateTemp(){
      //Environment.readTemperature();            
      //int t = dht.readTemperature();
      int temp = Environment.readTemperature();
      send_data_temp = temp; //send humidity
      DHT11_TEMP.setValue(send_data_temp);
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" *C ");
      Serial.println();
      Oled.setCursor(0, 2);
      Oled.print("Temp: ");
      Oled.print(temp);
      Oled.print("*C");
      
      //delay(1000);
}
void updateHumi(){
      //Environment.readHumidity();  
      //int h = dht.readHumidity();
      int humi = Environment.readHumidity();
      send_data_humi = humi;//send humidity
      DHT11_HUMI.setValue(send_data_humi);
      Serial.print("Humidity: ");
      Serial.print(humi);
      Serial.print(" %\t");
      Serial.println();
      //Oled.setFont(u8x8_font_amstrad_cpc_extended_r);
      Oled.setCursor(0, 4);
      Oled.print("Hum: ");
      Oled.print(humi);
      Oled.print("%");
      
}

void updateLed(){
  //if (LED_ASK.written() || BUTTON_Changed) {
      
  if (LED_ASK.written()) {
    // update LED, either central has written to characteristic or button state has changed
    if (LED_ASK.value()) {
      Serial.println("LED on");
      digitalWrite(LED, HIGH);
      Oled.setCursor(9, 6);
      Oled.print("LED on ");      
    } else {
      Serial.println("LED off");
      digitalWrite(LED, LOW);      
      Oled.setCursor(9, 6);
      Oled.print("LED off ");
    }
  }
}
void updateLight(){  
  int light = analogRead(LIGHT);
  send_data_light = light;
  LIGHT_ASK.setValue(send_data_light);
  Serial.println("Lux: ");
  Serial.println(light); 
  }
