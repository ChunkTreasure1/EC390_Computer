#include "classes.h"
#include "defines.h"
#include <Wire.h>
#include <UTFT.h>
#include <ArxContainer.h>

//Declare fonts
extern uint8_t BigFont[];
extern uint8_t Grotesk24x48[];

//Create LCD
UTFT g_GLCD(TFTM040_1_16, 7, 38, 9, 10);

arx::vector<Message*> g_Messages;
arx::vector<Message*> g_PreviousMessages;

Menu g_Menu;
Menu g_LastMenu;

uint16_t g_Width;
uint16_t g_Height;
float g_LastFrameTime;
float g_TargetUpdateTime = 10.f;

bool g_EngPressed = false;
bool g_BackPressed = false;
bool g_HydPressed = false;

void Message::Print()
{
  g_GLCD.fillScr(255, 255, 255);

  if(m_MessageLevel == MessageLevel::Info)
  {
    g_GLCD.setColor(110, 110, 110);
    g_GLCD.print("[Info]", CENTER, 150);
  }
  else if(m_MessageLevel == MessageLevel::Warning)
  {
    g_GLCD.setColor(255, 154, 0);
    g_GLCD.print("[Warning]", CENTER, 150);
  }
  else if(m_MessageLevel == MessageLevel::Error)
  {
    g_GLCD.setColor(255, 90, 90);
    g_GLCD.print("[Error]", CENTER, 150);
  }
  else if(m_MessageLevel == MessageLevel::Critical)
  {
    g_GLCD.setColor(255, 0, 0);
    g_GLCD.print("[Critical]", CENTER, 150);
  }

  g_GLCD.setColor(0, 0, 0);
  g_GLCD.print(m_Message, CENTER, 200);
}

float ToRads(const float& f)
{
  float r;
  r = f * PI / 180;

  return r;
}

void setup() 
{
  g_Menu = Menu::Main;
  g_LastMenu = Menu::None;

  g_GLCD.InitLCD();
  g_GLCD.setFont(Grotesk24x48);
  g_GLCD.fillScr(255, 255, 255);
  g_GLCD.setColor(0, 0, 0);
  g_GLCD.setBackColor(255, 255, 255);
  
  g_Width = g_GLCD.getDisplayXSize();
  g_Height = g_GLCD.getDisplayYSize();
  
  Wire.begin(10);
  Wire.onReceive(recieveEvent);
  Serial.begin(9600);

  pinMode(ENTER, INPUT); //Enter
  pinMode(BACK, INPUT); //Back
  pinMode(ENGINE, INPUT); //Engine
  pinMode(HYDRAULICS, INPUT); //Hydraulics

  pinMode(ENGINE_OIL_TEMP, INPUT); //Engine oil temp
  pinMode(HYDRAULIC_OIL_TEMP, INPUT); //Hydraulic oil temp
  pinMode(DIESEL_AMOUNT, INPUT); //Diesel amount
  pinMode(COOLANT_TEMP, INPUT); //Coolant temp
  pinMode(ENGINE_RMP, INPUT); //Engine RPM

  g_Messages.push_back(new Message(MessageLevel::Warning, "Test"));
  DrawMain();
}

void recieveEvent(int howMany)
{
  Serial.println("Recieved!");
  
  char c = Wire.read();
  if(c == '{')
  {
    MessageLevel msgLevel = (MessageLevel)Wire.read();

    char* string = new char[howMany - 3];
    int i = 0;

    while (1 < Wire.available())
    {
      if (i < howMany - 2)
      {
        string[i] = Wire.read();
      }
      else
      {
        break;
      }
      i++;
    }

    g_Messages.push_back(new Message(msgLevel, string));  
    Serial.println(string);
  }
}

void CheckMessages()
{
  if(g_Messages.size() > 0)
  {
    if (g_Menu != Menu::Messages)
    {
      g_Menu = Menu::Messages;    
    }
  }
}

void CheckPreviousMessages()
{
  if(g_PreviousMessages.size() > 10)
  {
    delete g_PreviousMessages[10];
    g_PreviousMessages[10] = nullptr;
    g_PreviousMessages.erase(g_PreviousMessages.begin() + 10);  
  }  
}

void loop() 
{
  float time = (float)millis();
  Timestep timestep = time / 1000 - g_LastFrameTime;
  g_LastFrameTime = time / 1000;
  g_TargetUpdateTime -= timestep;

  if(g_TargetUpdateTime <= 0.f)
  {
    Update();
    g_TargetUpdateTime = 10.f;
  }

  CheckMessages();
  HandleInput();
}

void HandleInput()
{
  short state = digitalRead(HYDRAULICS);
  if(state == HIGH)
  {
    Serial.println("Hydraulic Pressed");  
  }
  
  if(state == HIGH && !g_HydPressed)
  {
    if(g_Menu != Menu::Hydraulics)
    {
      g_Menu = Menu::Hydraulics;
      Update();
    }
    g_HydPressed = true;
    Serial.println("Hydraulic Pressed");
  }
  else if(state == LOW)
  {
    g_HydPressed = false;  
  }

  state = LOW;
  state = digitalRead(ENGINE);
  if(state == HIGH && !g_EngPressed)
  {
    if(g_Menu != Menu::Engine)
    {
      g_Menu = Menu::Engine;
      Update();
    }
    Serial.println("Engine Pressed");
    g_EngPressed = true; 
  }
  else if(state == LOW)
  {
    g_EngPressed = false;  
  }
  
  state = LOW;
  state = digitalRead(BACK);
  if(state == HIGH && !g_BackPressed)
  {
    if(g_Menu != Menu::Main)
    {
      g_Menu = Menu::Main;      
      Update();
    }
  
    Serial.println("Back Pressed");
    g_BackPressed = true;
  }
  else if(state == LOW)
  {
    g_BackPressed = false;  
  }
}

void Update()
{
  if(g_LastMenu != g_Menu)
  {
    g_GLCD.fillScr(255, 255, 255);
    g_LastMenu = g_Menu;
  }
  
  if (g_Menu == Menu::Main)
  {
    DrawMain();
    //Serial.println("Main");
  }
  else if (g_Menu == Menu::Engine)
  {
    DrawEngine();
    //Serial.println("Engine");
  }
  else if (g_Menu == Menu::Hydraulics)
  {
    DrawHydraulics();
  }
  else if (g_Menu == Menu::Settings)
  {
    DrawSettings();
  }
  else if(g_Menu == Menu::Messages)
  {
    DrawMessages();
  }
}

void DrawMain()
{
  /////Draw engine oil temp/////
  {
    vec2 pos(150.f, g_Height - 50.f);
    
    for (int j = 0; j < 3; j++)
    {
      for(int i = 202; i < 338; i++)
      {
        g_GLCD.drawPixel(pos.x + (float)(100 - j) * cos(ToRads(i)), pos.y + (float)(100 - j) * sin(ToRads(i)));
      }
    }

    for (int j = 0; j < 3; j++)
    {
      for(int i = 202; i < 338; i++)
      {
        g_GLCD.drawPixel(pos.x + (float)(80 - j) * cos(ToRads(i)), pos.y + (float)(80 - j) * sin(ToRads(i)));
      }
    }

    for(int i = 318; i < 338; i++)
    {
      for(int j = 0; j < 17; j++)
      {
        g_GLCD.setColor(VGA_LIME);
        g_GLCD.drawPixel(pos.x + (float)(97 - j) * cos(ToRads(i)), pos.y + (float)(97 - j) * sin(ToRads(i)));
        g_GLCD.setColor(VGA_BLACK);
      } 
    }

    float aOne = cos(ToRads(202));
    float aTwo = sin(ToRads(202));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    aOne = cos(ToRads(338));
    aTwo = sin(ToRads(338));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    g_GLCD.setFont(BigFont);
    g_GLCD.print("E.O.T", pos.x + 20.f, pos.y);
    g_GLCD.setFont(Grotesk24x48);

    int sensorValue = analogRead(ENGINE_OIL_TEMP);
    float val = (float)sensorValue * (5.f / 1023.f);
    val = ((val * (338 - 202)) / 5) + 202;

    static float lastVal = val;

    if(lastVal != val)
    {
      g_GLCD.setColor(255, 255, 255);
      for(int i = -5; i < 5; i++)
      {
        g_GLCD.drawLine(pos.x + i, pos.y, pos.x + 90 * cos(ToRads(lastVal)), pos.y + 90 * sin(ToRads(lastVal)));
      }
      g_GLCD.setColor(0, 0, 0);
      lastVal = val;
    }
    
    for(int i = -5; i < 5; i++)
    {
      g_GLCD.drawLine(pos.x + i, pos.y, pos.x + 90 * cos(ToRads(val)), pos.y + 90 * sin(ToRads(val)));
    }

    g_GLCD.fillCircle(pos.x, pos.y, 10.f);
  }
  //////////////////////////////

  ////Draw hydraulic oil temp///
  {
    vec2 pos(400.f, g_Height - 50.f);    

    for (int j = 0; j < 3; j++)
    {
      for(int i = 202; i < 338; i++)
      {
        g_GLCD.drawPixel(pos.x + (float)(100 - j) * cos(ToRads(i)), pos.y + (float)(100 - j) * sin(ToRads(i)));
      }
    }

    for (int j = 0; j < 3; j++)
    {
      for(int i = 202; i < 338; i++)
      {
        g_GLCD.drawPixel(pos.x + (float)(80 - j) * cos(ToRads(i)), pos.y + (float)(80 - j) * sin(ToRads(i)));
      }
    }

    for(int i = 318; i < 338; i++)
    {
      for(int j = 0; j < 17; j++)
      {
        g_GLCD.setColor(VGA_LIME);
        g_GLCD.drawPixel(pos.x + (float)(97 - j) * cos(ToRads(i)), pos.y + (float)(97 - j) * sin(ToRads(i)));
        g_GLCD.setColor(VGA_BLACK);
      } 
    }

    float aOne = cos(ToRads(202));
    float aTwo = sin(ToRads(202));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    aOne = cos(ToRads(338));
    aTwo = sin(ToRads(338));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    g_GLCD.setFont(BigFont);
    g_GLCD.print("H.O.T", pos.x + 20.f, pos.y);
    g_GLCD.setFont(Grotesk24x48);

    int sensorValue = analogRead(HYDRAULIC_OIL_TEMP);
    float val = (float)sensorValue * (5.f / 1023.f);
    val = ((val * (338 - 202)) / 5) + 202;

    static float lastVal = val;

    if(lastVal != val)
    {
      g_GLCD.setColor(255, 255, 255);
      for(int i = -5; i < 5; i++)
      {
        g_GLCD.drawLine(pos.x + i, pos.y, pos.x + 90 * cos(ToRads(lastVal)), pos.y + 90 * sin(ToRads(lastVal)));
      }
      g_GLCD.setColor(0, 0, 0);
      lastVal = val;
    }
    
    for(int i = -5; i < 5; i++)
    {
      g_GLCD.drawLine(pos.x + i, pos.y, pos.x + 90 * cos(ToRads(val)), pos.y + 90 * sin(ToRads(val)));
    }

    g_GLCD.fillCircle(pos.x, pos.y, 10.f);
  }
  /////////////////////////////

  ////Draw fuel level////
  {
    vec2 pos(650.f, g_Height - 50.f);    

    for (int j = 0; j < 3; j++)
    {
      for(int i = 202; i < 338; i++)
      {
        g_GLCD.drawPixel(pos.x + (float)(100 - j) * cos(ToRads(i)), pos.y + (float)(100 - j) * sin(ToRads(i)));
      }
    }

    for (int j = 0; j < 3; j++)
    {
      for(int i = 202; i < 338; i++)
      {
        g_GLCD.drawPixel(pos.x + (float)(80 - j) * cos(ToRads(i)), pos.y + (float)(80 - j) * sin(ToRads(i)));
      }
    }

    for(int i = 318; i < 338; i++)
    {
      for(int j = 0; j < 17; j++)
      {
        g_GLCD.setColor(VGA_LIME);
        g_GLCD.drawPixel(pos.x + (float)(97 - j) * cos(ToRads(i)), pos.y + (float)(97 - j) * sin(ToRads(i)));
        g_GLCD.setColor(VGA_BLACK);
      } 
    }

    float aOne = cos(ToRads(202));
    float aTwo = sin(ToRads(202));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    aOne = cos(ToRads(338));
    aTwo = sin(ToRads(338));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    g_GLCD.fillCircle(pos.x, pos.y, 10.f);

    g_GLCD.setFont(BigFont);
    g_GLCD.print("D.A", pos.x + 20.f, pos.y);
    g_GLCD.setFont(Grotesk24x48);

    int sensorValue = analogRead(DIESEL_AMOUNT);
    Serial.println(sensorValue);
    float val = (float)sensorValue * (5.f / 1023.f);
    val = ((val * (338 - 202)) / 5) + 202;

    static float lastVal = val;

    if(lastVal != val)
    {
      g_GLCD.setColor(255, 255, 255);
      for(int i = -5; i < 5; i++)
      {
        g_GLCD.drawLine(pos.x + i, pos.y, pos.x + 90 * cos(ToRads(lastVal)), pos.y + 90 * sin(ToRads(lastVal)));
      }
      g_GLCD.setColor(0, 0, 0);
      lastVal = val;
    }
    
    for(int i = -5; i < 5; i++)
    {
      g_GLCD.drawLine(pos.x + i, pos.y, pos.x + 90 * cos(ToRads(val)), pos.y + 90 * sin(ToRads(val)));
    }

    g_GLCD.fillCircle(pos.x, pos.y, 10.f);
  }
  /////////////////////////////

  /////RPM Counter////
  {
    g_GLCD.print("1000", g_Width - 200.f, 100);
  }
  ////////////////////
}

void DrawEngine()
{
  int dieselSensor = analogRead(DIESEL_AMOUNT);
  float dieselVal = (float)dieselSensor * (5.f / 1023.f);
  dieselVal = ((dieselVal * 100) / 5);
  
  g_GLCD.print("Diesel amount: ", 50.f, 30.f);
  g_GLCD.print("Hours left: 12h", 50.f, 130.f);
  g_GLCD.print("Cooling temperature: 60 C", 50.f, 230.f);
  g_GLCD.print("Hydraulic temperature: 70 C", 50.f, 330.f);

  g_GLCD.setColor(0, 0, 0);
  g_GLCD.fillRect(0.f, g_Height - 30.f, g_Width, g_Height);

  g_GLCD.setFont(BigFont);
  g_GLCD.print("Back", 10.f, g_Height - 20.f);
  g_GLCD.setFont(Grotesk24x48);
}

void DrawHydraulics()
{
  g_GLCD.print("HLD: On", 50.f, 30.f);
  g_GLCD.print("Floating mode: Off", 50.f, 130.f);
  g_GLCD.print("Turn prioritizing: 2", 50.f, 230.f);

    g_GLCD.setColor(0, 0, 0);
  g_GLCD.fillRect(0.f, g_Height - 30.f, g_Width, g_Height);

  g_GLCD.setFont(BigFont);
  g_GLCD.print("Back", 10.f, g_Height - 20.f);
  g_GLCD.setFont(Grotesk24x48);
}

void DrawMessages()
{
  Serial.println("Drawing messages!");
  for(int i = 0; i < g_Messages.size(); i++)
  {
    bool enterPressed = false;

    g_Messages[i]->Print();
    while(!enterPressed)
    {
      short enterState = digitalRead(ENTER);
      if (enterState == HIGH && !enterPressed)
      {
        enterPressed = true;
      } 
    }
    g_PreviousMessages.push_back(g_Messages[i]);
    g_Messages.erase(g_Messages.begin() + i);
    enterPressed = false;
  }

  g_Menu == Menu::Main;
}

void DrawSettings()
{}
