/////////////////////////////////////////////////////////////////////////
//                                                                     //
//     Remote control codes operated by Alexa                          //
//     Fan control by temperature sensor                               //
//                                                                     //
//                                                                     //
//     Written by Ran Malah                              June 2022     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include <Espalexa.h>
/////////////////////////////////////////////////////////////////////////
// ESP8266 Pin defines
/////////////////////////////////////////////////////////////////////////
#define IR_LED    0 // D3
#define BS18B20   2 // D4
#define FAN       5 // D1
/////////////////////////////////////////////////////////////////////////
// Fan defines
/////////////////////////////////////////////////////////////////////////
#define START_FAN_ABOVE   33
#define STOP_FAN_BELOW    30
#define SEC               1000
#define TEMP_CHECK_SEC    120 * SEC
/////////////////////////////////////////////////////////////////////////
// IR code defines
/////////////////////////////////////////////////////////////////////////
#define AMP_ON            0x010E03FC    
#define AMP_OFF           0x010EF906
#define AMP_MHL           0x010EF708
#define AMP_OSD_MENU      0x414E3AC5
#define AMP_OK            0x414E21DE
#define AMP_UP            0x414E9966
#define AMP_DISK          0x010E0BF4
#define PROJECTOR_POWER   0xC1AA09F6
#define AMP_MUTE          0x010E837C
/////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////
uint64_t FANdt = 0;

const char* ssid     = "Shimmy";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "alma2020";     // The password of the Wi-Fi network

IRsend* irSend;
OneWire oneWire(BS18B20);
DallasTemperature sensors(&oneWire);
Espalexa espalexa;
/////////////////////////////////////////////////////////////////////////
// Function declarations
/////////////////////////////////////////////////////////////////////////
void initWIFI();
void initFan();
void handleFan();
void initIR();
void initEspAlexa();

void ampPower(uint8_t onState); String ampPowerName = "Amp";
void ampMHL(uint8_t onState); String ampMHLName = "Tv";
void ampBluetooth(uint8_t onState); String ampBluetoothName = "Music";
void ampDisk(uint8_t onState); String ampDiskName = "Disk";
void ampMute(uint8_t onState); String ampMuteName = "Mute";
/////////////////////////////////////////////////////////////////////////
// Function definitions
/////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  initWIFI();
  initFan();
  initIR();
  initEspAlexa();
}

void loop() {
  handleFan();
  espalexa.loop();
}

void initEspAlexa(){
  if(WiFi.status() == WL_CONNECTED){
    espalexa.addDevice(ampPowerName, ampPower);
    espalexa.addDevice(ampMHLName, ampMHL);
    espalexa.addDevice(ampBluetoothName, ampBluetooth);
    espalexa.addDevice(ampDiskName, ampDisk);
    espalexa.addDevice(ampMuteName, ampMute);
   
    espalexa.begin();
  }
}
void ampMute(uint8_t dummy){
  irSend->sendNEC(AMP_MUTE, 32, 3);
  Serial.println("Mute!");
  delay(100);
}
void ampDisk(uint8_t dummy){
  irSend->sendNEC(AMP_DISK, 32, 3);
  Serial.println("Disk in!");
  delay(100);
}
void ampBluetooth(uint8_t dummy){
  irSend->sendNEC(AMP_OSD_MENU, 32, 3);
  delay(SEC);
  irSend->sendNEC(AMP_OK, 32, 3);
  delay(SEC);
  irSend->sendNEC(AMP_UP, 32, 3);
  delay(SEC);
  irSend->sendNEC(AMP_OK, 32, 3);
  Serial.println("Bluetooth in!");
}
void ampMHL(uint8_t dummy){
  irSend->sendNEC(AMP_MHL, 32, 3);
  Serial.println("MHL in!");
  delay(100);
}

void ampPower(uint8_t onState){
  if(onState){
    irSend->sendNEC(AMP_ON, 32, 3);
    Serial.println("Amp On!");
  } else{
    irSend->sendNEC(AMP_OFF, 32, 3);
    Serial.println("Amp Off!");
  }
  delay(100);
}

void initFan(){
  pinMode(FAN, OUTPUT);
  digitalWrite(FAN, LOW);
  sensors.begin();
  Serial.println("Start\n");
}

void handleFan(){
  if( (millis() - FANdt) > TEMP_CHECK_SEC){
    FANdt = millis();
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);
    
    if(temperature > START_FAN_ABOVE){
      digitalWrite(FAN, HIGH);
      Serial.println("Start Fan");
    } else if(temperature < STOP_FAN_BELOW){
      digitalWrite(FAN, LOW);
      Serial.println("Stop Fan");
    } 
    Serial.print("Temp = ");
    Serial.println(temperature);
  }
}
void initIR(){
  irSend = new IRsend(IR_LED, true);
  irSend->begin();
}

void initWIFI(){
  delay(1000);
  Serial.println('\n');
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

}
