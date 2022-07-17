#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"

#define PIN_ROTARY_CLK 5
#define PIN_ROTARY_DT 4
#define PIN_ROTARY_SW 3
#define LCD_BACKL 1
#define BACKL_TOUT 5

Encoder enc1(5, 4, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int floodValue = 0;
int drainValue = 0;
int curValue = 0;
boolean FDFlag = true;
boolean backlState = true;
uint32_t backlTimer;

void swapValues(int& first, int& second) {
  int temp = first;
  first = second;
  second = temp;
}



void getCurVariable() {
  if (FDFlag == true) {
    curValue = floodValue;
  }
  if (FDFlag == false) {
    curValue = drainValue;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  //lcd.print("TEST STRINg");
  enc1.setType(TYPE1);
}

void backlTick() {
  if (LCD_BACKL && backlState && millis() - backlTimer >= BACKL_TOUT * 1000) {
    backlState = false;
    lcd.noBacklight();
    //lcd.clear();
  }
}

void backlOn() {
  backlState = true;
  backlTimer = millis();
  lcd.backlight();
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (FDFlag == true) {
    lcd.print("Flood time = ");
    lcd.print(curValue);
    swapValues(curValue, floodValue);
    
  }
  if (FDFlag == false) {
    lcd.print("Drain time = ");
    lcd.print(curValue);
    swapValues(curValue, drainValue);
  }
}

void encoderTick() {
  enc1.tick();
  if (enc1.isTurn()){
    if (backlState) {
      backlTimer = millis();
      getCurVariable();
      if (enc1.isRight()) {
        curValue++;
        updateLCD();
      }
      if (enc1.isLeft()) {
        curValue--;
        updateLCD();
      }

    } else {backlOn();}
    
  }
  if (enc1.isClick()) {
    backlOn();
    FDFlag = not FDFlag;
    if (backlState) {
      if (FDFlag == true) {
        swapValues(floodValue, curValue);
        }
      else if (FDFlag == false) {
        swapValues(drainValue, curValue);
        }
      updateLCD();
    }
  }     
}

void loop() {
  // put your main code here, to run repeatedly:
  encoderTick();
  backlTick();
}