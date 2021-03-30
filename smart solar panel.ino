#include <ESP32Servo.h>
Servo servoO;
Servo servoB;
int pos = 0;
int r_pos = 0;
int t_pos = 0;
float v;
int servoPin1 = 13;
int servoPin2 = 12;
int voltPin = 35;
int angle;
int angle2;
int moveRotator;
int moveTilter;

float high = 0.0;
float v_test = 0.0;
int x, y = 0;

int pot = 34;
int ADC_Max = 4095;
int val;
int val1;

int S1 = 14;
int servo;
int knopstatus;
int a = 0;
int S2 = 5;
int knopstatus1;
int b = 0;
int autoMa;

int Led1 = 27;
int Led2 = 26;

int LDRO = 15;
int LDRW = 2;
int LDRZ = 35;
int LDRN = 32;
int topl = 0;
int topr = 0;
int botl = 0;
int botr = 0;

int avgtop = (topr + topl) / 2;
int avgbot = (botr + botl) / 2;
int avgleft = (topl + botl) / 2;
int avgright = (topr + botr) / 2;

int drempel_value = 10;


int diffelev = avgtop - avgbot;
int diffazi = avgright - avgleft;

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
char auth[] = "RXZiOgRa7OOF_lvjpnO2DyD_Fh5tIzHp";
char ssid[] = "SSD_C3150";
char pass[] = "ssdsolo1238";

#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float testVoltage() {
  v = analogRead(voltPin);
  v = (v * 2) / 204.6;
  return v;
}

BlynkTimer timer;

void setup()
{
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servoO.setPeriodHertz(50);
  servoO.attach(servoPin1, 500, 2400);
  servoB.setPeriodHertz(50);
  servoB.attach(servoPin2, 500, 2400);

  pinMode(S1, INPUT_PULLUP);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);

  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 0, 110), 8080);
}

BLYNK_WRITE(V3)
{
  int waarde1 = param.asInt();
  servoO.write(param.asInt());
  Blynk.virtualWrite(V0, waarde1);

}

BLYNK_WRITE(V4)
{

  int waarde2 = param.asInt();
  servoB.write(param.asInt());
  Blynk.virtualWrite(V1, waarde2);

}

void loop()
{
  Blynk.run();
  timer.run();
  float Temp = dht.readTemperature();
  Blynk.virtualWrite(V2, Temp);

  knopstatus = ! servo;
  servo = digitalRead(S1);
  if (servo == LOW & knopstatus == LOW & a == 0)
  {
    knopstatus = ! servo;
    Serial.println("servo onder");
    digitalWrite(Led1, HIGH);
    digitalWrite(Led2, LOW);
    delay(50);
    a = 1;
  }

  if (servo == LOW & knopstatus == LOW & a == 1)
  {
    Serial.println("servo boven");
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, HIGH);
    delay(50);
    a = 0;
  }

  knopstatus = ! autoMa;
  autoMa = digitalRead(S2);
  if (autoMa == LOW & knopstatus == LOW & b == 0)
  {
    knopstatus = ! autoMa;
    Serial.println("calibratie");

    delay(50);
    b = 1;
  }

  if (autoMa == LOW & knopstatus == LOW & b == 1)
  {
    knopstatus = ! autoMa;
    Serial.println("automatisch");

    delay(50);
    b = 2;
  }

  if (autoMa == LOW & knopstatus == LOW & b == 2)
  {
    Serial.println("manueel");

    delay(50);
    b = 0;
  }

  switch (b)
  {
    case 1:
      for (int i = 0; i <= 180; i++) {
        angle2 = i;
        v_test = testVoltage();

        if (v_test > high) {
          high = v_test;
          x = i;
        }
      }

      high = 0.0;

      if (angle2 > r_pos) {
        for (int u = r_pos; u <= angle2; u++) {
          servoO.write(u);
          delay(30);
        }
      }
      else {
        for (int p = r_pos; p >= angle2; p--) {
          servoO.write(p);
          delay(30);
        }
      }
      r_pos = angle2;

      for (int m = 5; m <= 90; m++) {
        angle = m;
        v_test = testVoltage();
        if (v_test > high) {
          high = v_test;
          y = m;
        }
      }

      if (angle > 90) {
        angle = 90;
      }
      else if (angle < 5) {
        angle = 10;
      }
      if (angle > t_pos) {
        for (int l = t_pos; l <= angle; l++) {
          servoB.write(l);
          delay(30);
        }
      }
      else {
        for (int k = t_pos; k >= angle; k--) {
          servoB.write(k);
          delay(30);
        }
      }

      t_pos = angle;
      break;

    case 2:
      topr = analogRead(LDRO);
      topl = analogRead(LDRN);
      botr = analogRead(LDRW);
      botl = analogRead(LDRZ);

      if (abs(diffazi) >= drempel_value) {
        if (diffazi > 0) {
          if (servoO.read() < 180) {
            servoO.write((servoB.read() + 2));
          }
        }
        if (diffazi <  0) {
          if (servoO.read() > 0) {
            servoO.write((servoB.read() - 2));
          }
        }
      }

      if (abs(diffelev) >= drempel_value) {
        if (diffelev > 0) {
          if (servoB.read() < 180) {
            servoB.write((servoO.read() - 2));
          }
        }
        if (diffelev <  0) {
          if (servoB.read() > 0) {
            servoB.write((servoO.read() + 2));
          }
        }
      }
      break;

    case 0:
      if ( a == 1 )
      {
        val = analogRead(pot);
        val = map(val, 0, ADC_Max, 0, 180);
        servoO.write(val);
        delay(200);
        Serial.println(val);
        Blynk.virtualWrite(V0, val);
      }
      else
      {
        val1 = analogRead(pot);
        val1 = map(val1, 0, ADC_Max, 0, 180);
        servoB.write(val1);
        delay(200);
        Serial.println(val1);
        Blynk.virtualWrite(V1, val1);
      }
      break;
  }
}
