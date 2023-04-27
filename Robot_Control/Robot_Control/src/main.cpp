#include <Arduino.h>

// define SW
#define SW_LEFT 9
#define SW_DOWN 8
#define SW_RIGHT 10
#define SW_UP 11

// Variáveis
byte WheelR = 0, WheelL = 0, OldWheelR = 0, OldWheelL = 0, UP = 0, DOWN = 0, LEFT = 0, RIGHT = 0;
char Sensor[6];
int Velocidade = 0, TimesPressedUP = 0;
String SensorR, SensorL;
float SensorRL = 0;
bool RunF = false, Run = false, AutoPilot = false, BtnPressed = false;

// Prototypes
void Read_SW();
void Buttons();
void MoveRemote();
void MoveAutoPilot();

// Functions-Remote
void Read_SW()
{
  delay(50);
  UP = digitalRead(SW_UP);
  DOWN = digitalRead(SW_DOWN);
  LEFT = digitalRead(SW_LEFT);
  RIGHT = digitalRead(SW_RIGHT);
}

void Buttons()
{
  while (BtnPressed == false)
  {
    Read_SW();
    if (UP == 1)
    {
      if (TimesPressedUP > 9)
      {
        AutoPilot = true;
        return;
      }else{
        TimesPressedUP++;
      }

      if (Velocidade < 9)
      {
        Velocidade++;
      }
      BtnPressed = true;
    }
    else if (DOWN == 1)
    {
      if (TimesPressedUP >= -9)
      {
        TimesPressedUP--;
        if (TimesPressedUP == -10)
        {
          TimesPressedUP++;
        }
      }

      if (Velocidade > -9)
      {
        Velocidade--;
      }
      BtnPressed = true;
    }

    Read_SW();
    while (RIGHT == 1)
    {
      Read_SW();
      Serial.print("m");
      Serial.print(-Velocidade);
      Serial.print(Velocidade);
      BtnPressed = true;
    }
    Read_SW();
    while (LEFT == 1)
    {
      Read_SW();
      Serial.print("m");
      Serial.print(Velocidade);
      Serial.print(-Velocidade);
      BtnPressed = true;
    }
  }
}

void MoveRemote()
{
  Serial.print("m");
  Serial.print(Velocidade);
  Serial.print(Velocidade);
}

// Functions-AutoPilot
void Read_Down()
{
  DOWN = digitalRead(SW_DOWN);
}

void Read_Sensors()
{
  if (Serial.available() > 0)
  {
    for (byte i = 0; i < 5; i++)
    {
      if (Serial.available() > 0)
      {
        Sensor[i] = Serial.read();
      }
      else
      {
        i--;
      }
    }
    if (Sensor[0] == 'R')
    {
      SensorR = "";
      for (byte i = 1; i < 5; i++)
      {
        SensorR += Sensor[i];
      }
    }
    if (Sensor[0] == 'L')
    {
      SensorL = "";
      for (byte i = 1; i < 5; i++)
      {
        SensorL += Sensor[i];
      }
    }
  }
  if (SensorR.toDouble() > 0.27)
  {
    SensorR = 0.0;
  }
  if (SensorL.toDouble() > 0.27)
  {
    SensorL = 0.0;
  }
}
void MoveAutoPilot()
{
  Read_Down();

  if (DOWN == 1)
  {
    Velocidade = 0;
    Serial.print("m");
    Serial.print(0);
    Serial.print(0);
    AutoPilot = false;
    delay(50);
    return;
  }

  if ((SensorR.toDouble() <= 0.15 && SensorL.toDouble() <= 0.15) && (SensorR.toDouble() > 0 && SensorL.toDouble() > 0) && Run == false)
  {
    Serial.print("m");
    Serial.print(-9);
    Serial.print(9);
    Run = true;
    delay(200);
  }

  if (SensorR.toDouble() == 0 && SensorL.toDouble() == 0 && Run == false)
  {
    Serial.print("m");
    Serial.print(9);
    Serial.print(9);
    Run = true;
  }

  while ((SensorR.toDouble() > 0 && SensorR.toDouble() < 0.27))
  {
    if ((SensorR.toDouble() <= 0.15 && SensorL.toDouble() <= 0.15) && (SensorR.toDouble() > 0 && SensorL.toDouble() > 0))
    {
      return;
    }
    WheelR = int(40 * SensorR.toDouble() - 1);
    if (WheelR != OldWheelR)
    {
      OldWheelR = WheelR;
      Serial.print("m");
      Serial.print(9);
      Serial.print(WheelR);
      Run = false;
    }
    else
    {
      break;
    }
  }

  while ((SensorL.toDouble() > 0 && SensorL.toDouble() < 0.27))
  {
    if ((SensorR.toDouble() <= 0.15 && SensorL.toDouble() <= 0.15) && (SensorR.toDouble() > 0 && SensorL.toDouble() > 0))
    {
      return;
    }
    WheelL = int(40 * SensorL.toDouble() - 1);
    // Serial.print(OldWheelR);
    if (WheelL != OldWheelL)
    {
      OldWheelL = WheelL;
      Serial.print("m");
      Serial.print(WheelL);
      Serial.print(9);
      Run = false;
    }
    else
    {
      break;
    }
  }
}
// y= 19*x+1.6
// m= 19 b = 1.6

void setup()
{
  pinMode(SW_DOWN, INPUT);
  pinMode(SW_LEFT, INPUT);
  pinMode(SW_RIGHT, INPUT);
  pinMode(SW_UP, INPUT);

  Serial.begin(9600);
}

void loop()
{
  if (AutoPilot == true)
  {
    // Auto-pilot
    Read_Sensors();
    MoveAutoPilot();
  }
  else
  {
    Buttons();
    MoveRemote();
    BtnPressed = false;
    Buttons();
  }
}