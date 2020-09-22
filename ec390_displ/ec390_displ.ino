#include "classes.h"
#include <Wire.h>
#include <UTFT.h>
#include <ArxContainer.h>

//Declare fonts
extern uint8_t BigFont[];
extern uint8_t Grotesk24x48[];

//Create LCD
UTFT g_GLCD(TFTM040_1_16, 7, 38, 9, 10);

arx::vector<Message*> g_Messages;
Menu g_Menu;
Menu g_LastMenu;

uint32_t g_Width;
uint32_t g_Height;
float g_LastFrameTime;
float g_TargetUpdateTime = 10.f;

void Message::Print()
{
  g_GLCD.print(m_Message, CENTER, 100);  
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
  g_LastMenu = Menu::Engine;

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

  pinMode(14, INPUT); //Enter
  pinMode(15, INPUT); //Back
  pinMode(16, INPUT); //Engine
  pinMode(17, INPUT); //Hydraulics

  pinMode(A0, INPUT); //Engine oil temp
  pinMode(A1, INPUT); //Hydraulic oil temp
  pinMode(A2, INPUT); //Diesel amount
  pinMode(A3, INPUT); //Coolant temp

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

void loop() 
{
  float time = (float)millis();
  Timestep timestep = time / 1000 - g_LastFrameTime;
  g_LastFrameTime = time / 1000;
  g_TargetUpdateTime -= timestep;
  
  if(g_Messages.size() > 0)
  {
    for(int i = 0; i < g_Messages.size(); i++)
    {
      g_Messages[i]->Print();
      
      bool accepted = false;
      do
      {
        int enterState = digitalRead(14);
        if (enterState == HIGH)
        {
          accepted = true;
          delete g_Messages[i];
        } 
      } 
      while(!accepted);
    }

    g_Messages.clear();
  }

  if(g_TargetUpdateTime <= 0.f)
  {
    Update();
    g_TargetUpdateTime = 10.f;
  }

  int engineState = digitalRead(16);
  if(engineState == HIGH)
  {
    g_Menu = Menu::Engine;
  }

  int hydraulicsState = digitalRead(17);
  if(hydraulicsState == HIGH) 
  {
    g_Menu = Menu::Hydraulics;
  }

  if(g_Menu == Menu::Engine || g_Menu == Menu::Hydraulics)
  {
    int backState = digitalRead(15);
    if(backState == HIGH)
    {
      g_Menu = Menu::Main;
    }  
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
  }
  else if (g_Menu == Menu::Engine)
  {
    DrawEngine();
  }
  else if (g_Menu == Menu::Hydraulics)
  {
    DrawHydraulics();
  }
  else if (g_Menu == Menu::Settings)
  {
    DrawSettings();
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

    int sensorValue = analogRead(A0);
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

    int sensorValue = analogRead(A1);
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

    int sensorValue = analogRead(A2);
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
  int dieselSensor = analogRead(A2);
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

void DrawSettings()
{}
