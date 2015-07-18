#include <TimerOne.h>

int THROTTLEPIN = A4;
int CURRENTPIN = A0;
int MOSFETPIN = 15;
const int numReadings = 3;

unsigned long thistime;
unsigned long lasttime;
unsigned long timepassed;

long readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int averageC = 0;                // the average

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(0, OUTPUT);
  Timer1.initialize(62);
  Timer1.start();
  Timer1.pwm(MOSFETPIN, 0);
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
  thistime = millis();
}
int tttout = 0;
int goaltttout = 0;
int lasttttout = 0;
int DEBUG = 0;
int imin = 600;
int AMPS = 0;
long TANK = 70000;

int cl = 0;

void loop() {
  lasttime = thistime;
  thistime = millis();
  timepassed = thistime - lasttime;

  total = total - readings[index];
  readings[index] = analogRead(CURRENTPIN);
  if (readings[index] == 1023)
  {
    Timer1.setPwmDuty(MOSFETPIN, 0);
    digitalWrite(2, HIGH);
  }
  else
  {
    digitalWrite(2, LOW);
  }
  total = total + readings[index];
  index = index + 1;

  if (index >= numReadings)
    index = 0;

  averageC = total / numReadings;

  int ttt = analogRead(THROTTLEPIN);
  tttout = map(ttt, 200, 850, 0, 1024);
  tttout = constrain(tttout, 0, 1024);

  AMPS = map(averageC, 512, 1021, 0, 83);
  AMPS = constrain(AMPS, 0, 1000);

  long i2t = (((AMPS - 40) * (AMPS - 40)) * (timepassed)) / 100;

  if (AMPS > 40)
  {
    TANK = TANK - i2t;
    digitalWrite(0, HIGH);
  }
  else
  {
    TANK = TANK + (i2t);
    digitalWrite(0, LOW);
  }

  if (TANK > 50000)
  {
    TANK = 50000;
    digitalWrite(3, HIGH);
  }
  else
  {
    digitalWrite(3, LOW);
  }

  if ((AMPS > 71) || (TANK < 0))
  {
    Timer1.setPwmDuty(MOSFETPIN, 0);
    digitalWrite(1, HIGH);
  }
  else
  {
    Timer1.setPwmDuty(MOSFETPIN, tttout);
    digitalWrite(1, LOW);
  }
  if (DEBUG)
  {
    Serial.print(AMPS);
    Serial.print(" - ");
    Serial.print(timepassed);
    Serial.print(" - ");
    Serial.print(TANK);
    Serial.print(" - ");
    Serial.print(i2t);
    Serial.println();
    delay(1);
  }
}




