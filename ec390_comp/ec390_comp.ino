#include "classes.h"
#include <ArxContainer.h>

arx::vector<Message*> g_Messages;
Menu g_Menu;

void setup() 
{
  g_Menu = Menu::Main;

  Serial.begin(9600);
  SetupPins();
}

void SetupPins()
{
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  pinMode(12, INPUT);
  pinMode(13, INPUT);
  pinMode(14, INPUT);

  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);

  pinMode(18, INPUT);
  pinMode(19, INPUT);
  pinMode(22, INPUT);

  pinMode(23, INPUT);
  pinMode(24, INPUT);
  pinMode(25, INPUT);
  
  pinMode(26, INPUT);
  pinMode(27, INPUT);
  pinMode(28, INPUT);
  
  pinMode(29, INPUT);
  pinMode(30, INPUT);
  pinMode(31, INPUT);

  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(34, INPUT);

  pinMode(35, INPUT);
  pinMode(36, INPUT);
  pinMode(37, INPUT);

  pinMode(38, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(39, OUTPUT);
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

  CheckSwitches();
  CheckGuards();
}

void DrawMain()
{}

void DrawEngine()
{}

void DrawHydraulics()
{}

void DrawSettings()
{}

void CheckSwitches()
{}

void CheckGuards()
{
  int engineOilState = digitalRead(10);
  static bool engineOilCreated = false;

  if(engineOilState == HIGH && !engineOilCreated)
  {
    engineOilCreated = true;
    g_Messages.push_back(new Message(MessageLevel::Critical, "Låg motoroljenivå!"));
  }

  int hydraulicOilState = digitalRead(11);
  static bool hydraulicOilCreated = false;

  if(hydraulicOilState == HIGH && !hydraulicOilCreated)
  {
    hydraulicOilCreated = true;
    g_Messages.push_back(new Message(MessageLevel::Critical, "Låg hydrailoljenivå!"));
  }
}