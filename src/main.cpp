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
int columnsLCD = 16;
char* MenuLine[] = {"Turn on/off", "Set propeties"};
int CursorLine = 0;

bool inMenu = true;
bool turnPumpSelected = false;
bool setPumpSelected = false;

boolean FDFlag = true;
boolean backlState = true;
uint32_t backlTimer;

void swapValues(int& first, int& second) {
  int temp = first;
  first = second;
  second = temp;
}


void drawMainMenu() {

}

void drawFloodAndDrainMenu() {

}

void getCurVariable() {
  if (FDFlag == true) {
    curValue = floodValue;
  }
  if (FDFlag == false) {
    curValue = drainValue;
  }
}
void backlOn() {
  backlState = true;
  backlTimer = millis();
  lcd.backlight();
}
void printMenu() {
  backlOn();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Menu    ");
  lcd.setCursor(0, 1);
  lcd.print(">");
  lcd.setCursor(columnsLCD - 1, 1); 

  lcd.setCursor(1, 1);
  lcd.print(MenuLine[CursorLine]);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  //lcd.print("TEST STRINg");
  enc1.setType(TYPE1);
  printMenu();
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
  if (!inMenu) 
  {
    if (enc1.isTurn())
    {
      if (backlState) 
      {
        backlTimer = millis();
        getCurVariable();
        if (enc1.isRight()) 
        {
          curValue++;
          updateLCD();
        }
        if (enc1.isLeft()) 
        {
          curValue--;
          updateLCD();
        }
      }
      else 
      {
        backlOn();
      }
    }
    if (enc1.isClick()) 
    {
      backlOn();
      FDFlag = not FDFlag;
      if (backlState)
      {
        if (FDFlag == true) 
        {
          swapValues(floodValue, curValue);
          updateLCD();
        }
        else if (FDFlag == false) 
        {
          swapValues(drainValue, curValue);
          updateLCD();
        }
      }
    }   
    
  }
  if (inMenu)
  {
    if (enc1.isTurn()) {
      if (enc1.isRight() && CursorLine != 1) 
      {
        CursorLine++;
        if (CursorLine >= 1) {
          CursorLine = 1;
          printMenu();
        }
      }
      else if (enc1.isLeft() && CursorLine != 0) {
        CursorLine--;
        if (CursorLine <= 0) {
          CursorLine = 0;
          printMenu();
        }
      }
    else backlOn();
    }
    if (enc1.isClick()) 
    {
      backlOn();
      switch (CursorLine)
      {
      case 1:
        /* code */
        setPumpSelected = true;
        turnPumpSelected = false;
        inMenu = false;
        updateLCD();
        break;
      
      case 0:
        setPumpSelected = false;
        turnPumpSelected = true;
        inMenu = false;
        break;
      }
    }  
  
  }
  if (enc1.isHolded())
    {
      inMenu = true;
      setPumpSelected = false;
      turnPumpSelected = false;
      printMenu();
    }  
}

void selectorMenu() {
  enc1.tick();
  if (enc1.isTurn() && inMenu) {
    if (enc1.isRight() && CursorLine != 1) {
      CursorLine++;
      if (CursorLine >= 1) {
        CursorLine = 1;
        printMenu();
      }
    }
    else if (enc1.isLeft() && CursorLine != 0) {
      CursorLine--;
      if (CursorLine <= 0) {
        CursorLine = 0;
        printMenu();
      }
    }
    else backlOn();
  }
  if (enc1.isClick() && inMenu) 
  {
    backlOn();
    switch (CursorLine)
    {
    case 1:
      /* code */
      setPumpSelected = true;
      turnPumpSelected = false;
      inMenu = false;
      updateLCD();
      break;
    
    case 0:
      setPumpSelected = false;
      turnPumpSelected = true;
      inMenu = false;
      break;
    }
  }  
  if (enc1.isHolded() && inMenu)
  {
    inMenu = true;
    setPumpSelected = false;
    turnPumpSelected = false;
    printMenu();
  }  
}



void backlTick() {
  if (LCD_BACKL && backlState && millis() - backlTimer >= BACKL_TOUT * 1000) {
    backlState = false;
    lcd.noBacklight();
    //lcd.clear();
  }
}







void loop() {
  // put your main code here, to run repeatedly:
  //encoderTick();
  encoderTick();
  backlTick();
}