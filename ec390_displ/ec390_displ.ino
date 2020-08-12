#include "classes.h"
#include <Wire.h>
#include <ArxContainer.h>

arx::vector<Message*> g_Messages;
Menu g_Menu;

void setup() 
{
  g_Menu = Menu::Main;

  Wire.begin(4);
  Wire.onReceive(RecieveEvent);
  Serial.begin(9600);
}

void RecieveEvent(int amount)
{
  char c = Wire.read();
  if(c == '{')
  {
    MessageLevel msgLevel = (MessageLevel)Wire.read();

    char* string = new char[amount - 3];
    int i = 0;

    while (1 < Wire.available())
    {
      if (i < amount - 2)
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
  }

  Serial.println("Recieved!");
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
}

void DrawMain()
{}

void DrawEngine()
{}

void DrawHydraulics()
{}

void DrawSettings()
{}
