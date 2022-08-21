#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"
#include <ezOutput.h>

#define PIN_ROTARY_CLK 5
#define PIN_ROTARY_DT 4
#define PIN_ROTARY_SW 3
#define LCD_BACKL 1
#define BACKL_TOUT 5

Encoder enc1(5, 4, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);
ezOutput led(A3);
int floodValue = 200; //turn pump on to time in seconds
int drainValue = 1200; // turn pump off to time in seconds (20 min)
long floodOnceADay[] = {200, 43000}; // thats to option turn pump one once a 12 hours
int columnsLCD = 16;

const char* MenuLine[] = {"Turn on/off", "Set propeties", "Once per 12h"};
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
bool onceTwelveHours = false;

unsigned long time=0;
uint32_t backlTimer;
int step = 5;

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
}
void setOnceTwelveHours() {
  floodValue = floodOnceADay[0];
  drainValue = floodOnceADay[1];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print('Settings updated');
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

    //led.high();
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
      if (CursorLine > 2)
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
        if (CursorLine == 2)
        {
          setOnceTwelveHours();
        }
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
          if (enc1.isDouble())
          {
            if (step == 5)
            {
              step = 30;
            }
            else if (step == 30)
            {
              step = 5;
            }
            
          }
        if (floodSelector && !drainSelector)
        { 
          enc1.tick();
          if (enc1.isRight())
          {
            floodValue += step;
            drawPumpSettings();
          }
          if (enc1.isLeft())
          {
            floodValue -= step;
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
            drainValue += step;
            drawPumpSettings();
          }
          if (enc1.isLeft())
          {
            drainValue -= step;
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
          
            if (!inMenu && floodValue != 0 && drainValue != 0)
            {
              led.blink(floodValue * 1000, drainValue * 1000);
              Serial.println(floodValue * 100);
              Serial.println("\n");
              Serial.println(time);
            }
          }
          if (!turnFlag)
          {
            led.high();
            //led.blink(0, 0);
            Serial.println("MUST OFF");
            Serial.println(turnFlag);

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
  led.loop();
  mainMenuTracer();
  backlTick();

}
