#include <Wire.h>
#include "defines.h"
#include "helpers.h"

enum class EngineState
{
  Idle,
  Work
};

EngineState g_CurrentEngineState = EngineState::Idle;
bool g_IsAutoRPM = false;

uint16_t g_WorkRPM = 1500;
uint16_t g_IdleRPM = 600;
uint16_t g_CurrentRPM = 0;
float g_LastFrameTime = 0.f;
float g_TargetTimeToIdle = 10.f;

void setup() 
{
  Wire.begin();

  Serial.begin(9600);
  SetupPins();
}

void SetupPins()
{
  pinMode(P_TURN_EFFECT, INPUT);
  pinMode(M_TURN_BREAK, OUTPUT);
  pinMode(M_HLD, OUTPUT);
  pinMode(R_START_HEATER, OUTPUT);
  
  pinMode(P_EXTRA, OUTPUT);
  pinMode(P_SSC, OUTPUT);
  pinMode(M_FLOAT_MODE, OUTPUT);
  pinMode(R_FUEL_VALVE, OUTPUT);

  pinMode(R_WIPER_UPPER, OUTPUT);
  pinMode(R_WIPER_LOWER, OUTPUT);
  pinMode(N_ENGINE_OIL, INPUT_PULLUP);
  pinMode(N_HYDRAULIC_OIL, INPUT_PULLUP);

  pinMode(LEVER_SENSE, INPUT);
  pinMode(PEDAL_SENSE, INPUT);
  pinMode(DRIVE_PEDAL_SENSE, INPUT);

  pinMode(R_ENGINE_HEATER, OUTPUT);
  pinMode(R_FAN, OUTPUT);
  pinMode(N_WATERSEPERATOR, INPUT_PULLUP);

  pinMode(RPM_ENGINE, INPUT);
  pinMode(RPM_TURN, INPUT);
  pinMode(MANUEVER_LOCK, INPUT);

  pinMode(QUICK_LOCK_SENSE, INPUT);
  pinMode(S_LOW_GEAR, INPUT);
  pinMode(S_HLD, INPUT);
  
  pinMode(S_LEFT_LEVER, INPUT);
  pinMode(S_OVERLOAD_ALARM, INPUT);
  pinMode(S_START_HEATER, INPUT);
  
  pinMode(S_AUTOMATIC_RPM, INPUT);
  pinMode(N_COOLANT, INPUT);
  pinMode(TV_AIR_FILTER, INPUT);

  pinMode(V_START_HEATER, INPUT);
  pinMode(N_LUBRICATION_PUMP, INPUT);
  pinMode(TV_OVERLOAD, INPUT);

  pinMode(S_WIPER_UPPER, INPUT);
  pinMode(S_AUTO_TURN_BRAKE, INPUT);
  pinMode(S_AUTOMATIC_RPM2, INPUT);

  pinMode(S_WIPER_LOWER, INPUT);
  pinMode(L_CENTRAL, OUTPUT);
  pinMode(L_WARNING, OUTPUT);
  pinMode(L_OVERLOAD, OUTPUT);
  pinMode(L_SHOVEL_LOCK, OUTPUT);
  pinMode(L_HLD, OUTPUT);
  pinMode(L_FLOAT_MODE, OUTPUT);
  pinMode(L_AIRCONDITIONING, OUTPUT);

  pinMode(TG_MANOUVER_PRESSURE, INPUT);
  pinMode(TEMP_COOLANT, INPUT);
  pinMode(TEMP_HYDRAULIC_OIL, INPUT);

  pinMode(NG_FUEL, INPUT);
  pinMode(TG_ENGINE_OIL, INPUT);
}

void loop() 
{
  float time = (float)millis();
  Timestep timestep = time / 1000 - g_LastFrameTime;
  g_LastFrameTime = time / 1000;
  g_TargetTimeToIdle -= timestep;

  if(g_IsAutoRPM && g_CurrentEngineState == EngineState::Work)
  {
    if (g_TargetTimeToIdle <= 0.f)
    {
      g_CurrentEngineState = EngineState::Idle;
      g_TargetTimeToIdle = 10.f;
    }
  }
  
  CheckSwitches();
  CheckGuards();

  delay(500);
}

void CheckSwitches()
{
  short switchState = digitalRead(S_AUTO_TURN_BRAKE);
  if (switchState == HIGH)
  {
    /* code */
  }
  else
  {
    /* code */
  }
  
  switchState = LOW;
  switchState = digitalRead(S_AUTOMATIC_RPM);
  if(switchState == HIGH)
  {
    g_IsAutoRPM = true;
  }
  else
  {
    g_IsAutoRPM = false;
  }
  
  switchState = LOW;
  switchState = digitalRead(S_AUTOMATIC_RPM2);
  if(switchState == HIGH)
  {

  }
  else
  {
    /* code */
  }
  
  switchState = LOW;
  switchState = digitalRead(S_HLD);
  if(switchState == HIGH)
  {
    digitalWrite(M_HLD, 1);
    digitalWrite(L_HLD, 1);
  }
  else
  {
    digitalWrite(M_HLD, 0);
    digitalWrite(L_HLD, 0);
  }
  
  switchState = LOW;
  switchState = digitalRead(S_LOW_GEAR);
  if(switchState == HIGH)
  {

  }
  else
  {
    /* code */
  }

  switchState = LOW;
  switchState = digitalRead(S_OVERLOAD_ALARM);
  if(switchState == HIGH)
  {
    digitalWrite(L_OVERLOAD, 1);
  }
  else
  {
    digitalWrite(L_OVERLOAD, 0);
  }
  
  switchState = LOW;
  switchState = digitalRead(S_START_HEATER);
  if(switchState == HIGH)
  {

  }
  else
  {
    /* code */
  }
  
  switchState = LOW;
  switchState = digitalRead(S_WIPER_LOWER);
  if(switchState == HIGH)
  {

  }
  else
  {
    /* code */
  }
  
  switchState = LOW;
  switchState = digitalRead(S_WIPER_UPPER);
  if(switchState == HIGH)
  {

  }
  else
  {
    /* code */
  }
  
  switchState = LOW;
  switchState = digitalRead(S_LOW_GEAR);
  if(switchState == HIGH)
  {

  }
  else
  {
    
  }
}

void CheckGuards()
{
  /////Engine Oil/////
  short state = digitalRead(N_ENGINE_OIL);
  static bool engineOilLevel = false;
  if (state == LOW && !engineOilLevel)
  {
    engineOilLevel = true;
    Wire.beginTransmission(10);
    Wire.write("{3Låg motorolja!}");
    Wire.endTransmission();

    //Turn off the engine
    digitalWrite(R_FUEL_VALVE, 1);
  }
  ////////////////////

  /////Hydraulic Oil//////
  state = HIGH;
  state = digitalRead(N_HYDRAULIC_OIL);
  static bool hydraulicOilLevel = false;
  if (state == LOW && !hydraulicOilLevel)
  {
      hydraulicOilLevel = true;
      Wire.beginTransmission(10);
      Wire.write("{3Låg hydraulolja!}");
      Wire.endTransmission();
  }
  ////////////////////////

  /////Coolant level/////
  state = LOW;
  state = digitalRead(N_COOLANT);
  static bool coolantLevel = false;
  if (state == HIGH && !coolantLevel)
  {
    coolantLevel = true;
    Wire.beginTransmission(10);
    Wire.write("{3Låg kylvätska!}");
    Wire.endTransmission();
  }
  ///////////////////////

  /////Water seperator/////
  state = HIGH;
  state = digitalRead(N_WATERSEPERATOR);
  static bool waterSeperator = false;
  if (state == LOW && !waterSeperator)
  {
    waterSeperator = true;
    Wire.beginTransmission(10);
    Wire.write("{2Vatten seperator full!}");
    Wire.endTransmission();
  }
  /////////////////////////

  /////Overload warning/////
  state = LOW;
  state = digitalRead(S_OVERLOAD_ALARM);
  if (state == HIGH)
  {
    state = LOW;
    state = digitalRead(TV_OVERLOAD);
    static bool overloadWarning = false;
    if (state == HIGH && !overloadWarning)
    {
      overloadWarning = true;
      Wire.beginTransmission(10);
      Wire.write("{3Varning! Överlast!}");
      Wire.endTransmission();

      digitalWrite(L_OVERLOAD, 1);
    }
    else
    {
      digitalWrite(L_OVERLOAD, 0);
    }  
  }
  //////////////////////////

  /////Air filter/////
  state = LOW;
  state = digitalRead(TV_AIR_FILTER);
  static bool airFilter = false;
  if (state == HIGH && !airFilter)
  {
    airFilter = true;
    Wire.beginTransmission(10);
    Wire.write("{2Luftfilter igensatt!}");
    Wire.endTransmission();
  }
  
  //////////////////////////////
}

void CheckSensors()
{
  /////Engine oil sensor/////
  float sensorValue = analogRead(TG_ENGINE_OIL);
  float voltage = (float)sensorValue * (5.f / 1023.f);

  float val = (voltage * 0.7f) / 5.f;
  static bool engineOilSensor = false;
  if ((g_CurrentRPM > 400 && g_CurrentRPM < 1000) && val < 0.07f)
  {
    engineOilSensor = true;
    Wire.beginTransmission(10);
    Wire.write("{3Lågt oljetryck!}");
    Wire.endTransmission();

    //Turn off the engine
    digitalWrite(R_FUEL_VALVE, 1);
  }
  ///////////////////////////

  /////Engine RPM/////

  ////////////////////

  if (g_IsAutoRPM)
  {
    short sensorState = digitalRead(LEVER_SENSE);
    if (sensorState == HIGH)
    {
      g_CurrentEngineState = EngineState::Work;
    }

    sensorState = LOW;
    sensorState = digitalRead(PEDAL_SENSE);
    if (sensorState == HIGH)
    {
      g_CurrentEngineState = EngineState::Work;
    }

    sensorState = LOW;
    sensorState = digitalRead(DRIVE_PEDAL_SENSE);
    if (sensorState == HIGH)
    {
      g_CurrentEngineState = EngineState::Work;
    }
  }
}
