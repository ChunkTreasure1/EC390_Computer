#include "classes.h"
#include <Wire.h>
#include <UTFT.h>
#include <ArxContainer.h>

//Declare fonts
extern uint8_t SmallFont[];

//Create LCD
UTFT g_GLCD(TFTM040_1_16, 7, 38, 9, 10);

arx::vector<Message*> g_Messages;
Menu g_Menu;

uint32_t g_Width;
uint32_t g_Height;

float ToRads(const float& f)
{
  float r;
  r = f * PI / 180;

  return r;
}

void setup() 
{
  g_Menu = Menu::Main;

  g_GLCD.InitLCD();
  g_GLCD.setFont(SmallFont);
  g_GLCD.fillScr(255, 255, 255);
  g_GLCD.setColor(0, 0, 0);

  g_Width = g_GLCD.getDisplayXSize();
  g_Height = g_GLCD.getDisplayYSize();
  
  Wire.begin(10);
  Wire.onReceive(recieveEvent);
  Serial.begin(9600);
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
  if(g_Messages.size() > 0)
  {
    for(int i = 0; i < g_Messages.size(); i++)
    {
      g_Messages[i]->Print();
      
      bool accepted = false;
      do
      {
        //Check if enter pressed
        if (true)
        {
          accepted = true;
          delete g_Messages[i];
        } 
      } 
      while(!accepted);
    }

    g_Messages.clear();
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

  delay(100);
}

void DrawMain()
{
  /////Draw engine oil temp/////
  {
    vec2 pos(150.f, g_Height - 50.f);
      
    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 100 * cos(ToRads(i)), pos.y + 100 * sin(ToRads(i)));
    }
    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 99 * cos(ToRads(i)), pos.y + 99 * sin(ToRads(i)));
    }

    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 80 * cos(ToRads(i)), pos.y + 80 * sin(ToRads(i)));
    } 
    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 79 * cos(ToRads(i)), pos.y + 79 * sin(ToRads(i)));
    } 

    float aOne = cos(ToRads(202));
    float aTwo = sin(ToRads(202));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    aOne = cos(ToRads(338));
    aTwo = sin(ToRads(338));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    g_GLCD.setColor(VGA_GRAY);
    g_GLCD.fillCircle(pos.x, pos.y, 20.f);
    g_GLCD.setColor(0, 0, 0);
  }
  //////////////////////////////

  ////Draw hydraulic oil temp///
  {
    vec2 pos(400.f, g_Height - 50.f);    

    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 100 * cos(ToRads(i)), pos.y + 100 * sin(ToRads(i)));
    }
    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 99 * cos(ToRads(i)), pos.y + 99 * sin(ToRads(i)));
    }

    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 80 * cos(ToRads(i)), pos.y + 80 * sin(ToRads(i)));
    }
    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 79 * cos(ToRads(i)), pos.y + 79 * sin(ToRads(i)));
    }

    float aOne = cos(ToRads(202));
    float aTwo = sin(ToRads(202));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    aOne = cos(ToRads(338));
    aTwo = sin(ToRads(338));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    g_GLCD.setColor(VGA_GRAY);
    g_GLCD.fillCircle(pos.x, pos.y, 20.f);
    g_GLCD.setColor(0, 0, 0);
  }
  /////////////////////////////

  ////Draw fuel level////
  {
    vec2 pos(650.f, g_Height - 50.f);    

    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 100 * cos(ToRads(i)), pos.y + 100 * sin(ToRads(i)));
    }
    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 99 * cos(ToRads(i)), pos.y + 99 * sin(ToRads(i)));
    }

    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 80 * cos(ToRads(i)), pos.y + 80 * sin(ToRads(i)));
    }
    for(int i = 202; i < 338; i++)
    {
      g_GLCD.drawPixel(pos.x + 79 * cos(ToRads(i)), pos.y + 79 * sin(ToRads(i)));
    }

    float aOne = cos(ToRads(202));
    float aTwo = sin(ToRads(202));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    aOne = cos(ToRads(338));
    aTwo = sin(ToRads(338));
    g_GLCD.drawLine(pos.x + 79 * aOne, pos.y + 79 * aTwo, pos.x + 100 * aOne, pos.y + 100 * aTwo);

    g_GLCD.setColor(VGA_GRAY);
    g_GLCD.fillCircle(pos.x, pos.y, 20.f);
    g_GLCD.setColor(0, 0, 0);
  }
  /////////////////////////////

  /////RPM Counter////
  {
    g_GLCD.printNumI(1000, CENTER, 100);
  }
  ////////////////////
}

void DrawEngine()
{}

void DrawHydraulics()
{}

void DrawSettings()
{}
