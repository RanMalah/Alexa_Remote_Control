
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <Wire.h>           
#include <LiquidCrystal_I2C.h>    

#define RCV_PIN   14 // D5
#define SDA 4 // D2
#define SCL 5 // D1


LiquidCrystal_I2C lcd(0x27,16,2); 
IRrecv irRcv(RCV_PIN);
decode_results res;

void initIR();
void handleIR();
void print64(uint64_t);
void printlcd64(uint64_t);

void setup() {
  lcd.init();      
  lcd.backlight(); 
  initIR();
  lcd.setCursor(0,0);  
  lcd.print("IR  ");  
  lcd.setCursor(0,1);  
  lcd.print("Scan "); 
}

void loop() {
  handleIR();
}


void initIR(){
  irRcv.enableIRIn();
}

void handleIR(){
  if(irRcv.decode(&res)){
    print64(res.value);
    printlcd64(res.value);
    lcd.setCursor(0, 1);
    lcd.print("Type = ");
    lcd.print(res.decode_type);
    irRcv.resume();
  }
  delay(100);
}

void print64(uint64_t val){
  Serial.print("0x");
  Serial.print(uint32_t(val >> 32), HEX);
  Serial.println(uint32_t(val & 0xFFFFFFFF), HEX);
}
void printlcd64(uint64_t val){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("0x");
  for(int i = 0 ; i < 8 ; i++){
    lcd.setCursor(9 - i, 0);
    lcd.print(val & 0xf, HEX);
    val >>= 4;
  }
}
