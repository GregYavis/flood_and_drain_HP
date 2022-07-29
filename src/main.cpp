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

int columnsLCD = 16;

const char* MenuLine[] = {"Turn on/off", "Set propeties"};
int CursorLine = 0;

const char* PumpMenuLine[] = {"Flood time: ", "Drain time: "};

bool inMenu = true;
bool turnPumpSelected = false;
bool turmPumpCursor = true;
bool setPumpSelected = false;
bool setPumpCursor = false;

bool floodCursor = false;
bool floodSelector = false;
bool drainCursor = false;
bool drainSelector = false;
bool swapOn = true;
boolean turnFlag = false;
boolean backlState = true;
uint32_t backlTimer;


void backlOn() {
  backlState = true;
  backlTimer = millis();
  lcd.backlight();
}

void updatePumpData() {
  if (!floodCursor && !drainCursor)
    {
      lcd.print(PumpMenuLine[0]);
      lcd.print(floodValue);
      lcd.setCursor(0, 1);
      lcd.print(PumpMenuLine[1]);
      lcd.print(drainValue);
    }
    if (floodCursor && !drainCursor)
    {
      
      lcd.print(PumpMenuLine[0]);
      lcd.print(floodValue);
      lcd.print("<");
      lcd.setCursor(0, 1);
      lcd.print(PumpMenuLine[1]);
      lcd.print(drainValue);
    }
    if (!floodCursor && drainCursor)
    {
      lcd.print(PumpMenuLine[0]);
      lcd.print(floodValue);
      lcd.setCursor(0, 1);
      
      lcd.print(PumpMenuLine[1]);
      lcd.print(drainValue);
      lcd.print("<");
      
    }
}
void drawPumpSettings() {
  if (backlState)
  { 
    backlTimer = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    // отрисовка
    updatePumpData();
  }
  else 
  {
     backlOn();
  }
}

void drawTurnOnOff()
{
  if (backlState)
  {
    backlTimer = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    if (turnFlag)
    {
      
      lcd.print("ON ");
      lcd.print("<");
      lcd.setCursor(0, 1);
      lcd.print("OFF");
    }
    if (!turnFlag)
    {
      lcd.print("ON");
      lcd.setCursor(0, 1);
      lcd.print("OFF");
      lcd.print("<");
    }
  }
  else 
  {
    backlOn();
  }
}


void drawMainMenu() {
    backlTimer = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    if (inMenu)
    { 
      lcd.print("Main menu");
      lcd.setCursor(0, 1);
      const char** currentMenu = MenuLine;
      lcd.print(">");
      lcd.print(currentMenu[CursorLine]);
    }
    if (!inMenu && setPumpSelected)
    {
      drawPumpSettings();
    }
    //if (setPumpCursor && !turmPumpCursor)
    //{
    //  lcd.print(MenuLine[0]);
    //  lcd.setCursor(0, 1);
    //  lcd.print(">");
    //  lcd.print(MenuLine[1]);
    //}
    //else if (!setPumpCursor && turmPumpCursor)
   // {
   //   lcd.print(">");
   //   lcd.print(MenuLine[0]);
   //   lcd.setCursor(0, 1);
   //   lcd.print(MenuLine[1]);
   // }

}

void mainMenuTracer () {
  enc1.tick();
  // Возвращаемся в главное меню
  if (enc1.isHolded())
  {
    inMenu = true;
    setPumpSelected = false;
    turnPumpSelected = false;
    swapOn = true;
    floodSelector = false;
    drainSelector = false;
    drawMainMenu();
  }
  // Включаем екран в любом меню при клике или повороте енкодера если он выключен
  if (!backlState)
  {
    if (enc1.isClick()| enc1.isTurn()| enc1.isHold())
    {
      backlOn();
    }
  }
 
  if (inMenu)
  {
    if (enc1.isTurn()) 
    { 
      if (enc1.isRight())
      {
      CursorLine++;
      if (CursorLine > 1)
      {
        CursorLine = 0;
      }
      drawMainMenu();
      }
    
      if(enc1.isLeft())
      {
        CursorLine--;
        if (CursorLine < 0)
        {
          CursorLine = 1;
        }
        drawMainMenu();
      }
    }
    if (enc1.isClick())
    {
      if (backlState)
      {      
        inMenu = false;
        if (CursorLine == 1)
        {
          setPumpSelected = true;
          turnPumpSelected = false;
          drawPumpSettings();
        }
        if (CursorLine == 0)
        {
          setPumpSelected = false;
          turnPumpSelected = true;
          drawTurnOnOff();
        }
      }
      else
      {
        backlOn();
      }
    }
  }

  if (!inMenu)
  { 
    if (setPumpSelected && !turnPumpSelected)
    { 
      if ((floodCursor && drainCursor)|(!floodCursor && !drainCursor))
      {
        floodCursor = true;
        drainCursor = false;
      }
      if (enc1.isTurn() && swapOn &&(enc1.isRight() | enc1.isLeft()))
      {
        floodCursor = !floodCursor;
        drainCursor = !drainCursor;
        // Привести функции drawPumpSettings(); drawTurnOnOff(); к виду процессоров внутри которых отслеживеются нажатия и изменения 
        drawPumpSettings();
      }
      if (enc1.isClick() && !floodSelector && !drainSelector)
      {
        floodSelector = floodCursor;
        drainSelector = drainCursor;
        swapOn = false;
        drawPumpSettings();
      }
      if (!swapOn)
      { 
        if (floodSelector && !drainSelector)
        { 
          enc1.tick();
          if (enc1.isRight())
          {
            floodValue++;
            drawPumpSettings();
          }
          if (enc1.isLeft())
          {
            floodValue--;
            if (floodValue < 0)
            {
              floodValue = 0;
            }
            drawPumpSettings();
          }
          enc1.tick();
          if (enc1.isClick())
          {
            swapOn = true;
            floodSelector = false;
            drainSelector = false;
            floodCursor = false;
            drainCursor = false;
            drawPumpSettings();
          }

        }
        if (!floodSelector && drainSelector)
        { 
          enc1.tick();
          if (enc1.isRight())
          {
            drainValue++;
            drawPumpSettings();
          }
          if (enc1.isLeft())
          {
            drainValue--;
            if (drainValue < 0)
            {
              drainValue = 0;
            }
            drawPumpSettings();
          }
          enc1.tick();
          if (enc1.isClick())
          {
            swapOn = true;
            floodSelector = false;
            drainSelector = false;
            floodCursor = false;
            drainCursor = false;
            drawPumpSettings();
          }

        }
        
    }
    }
    if (!setPumpSelected && turnPumpSelected)
    { 
      
      if (enc1.isTurn() && (enc1.isRight() | enc1.isLeft()))
      {
        turnFlag = !turnFlag;
        drawTurnOnOff();
      }
      enc1.tick();
      if (enc1.isClick())
      {
        if (turnFlag)
        {
          Serial.println("MUST ON");
          Serial.println(turnFlag);
          digitalWrite(A3, LOW);
        }
        if (!turnFlag)
        {
          Serial.println("MUST OFF");
          Serial.println(turnFlag);
          digitalWrite(A3, HIGH);
        }
      } 
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  //lcd.print("TEST STRINg");
  pinMode(A3, OUTPUT);
  digitalWrite(A3, HIGH);
  enc1.setType(TYPE1);
  drawMainMenu();
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
  mainMenuTracer();
  backlTick();
}