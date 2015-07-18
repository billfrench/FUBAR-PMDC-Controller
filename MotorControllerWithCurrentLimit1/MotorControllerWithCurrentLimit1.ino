#include <TimerOne.h>

const int THROTTLEPIN = A4;
const int CURRENTPIN = A0;
const int MOSFETPIN = 15;
const int numReadings = 3;  // Number of readings to average
const long TANKMAX = 100000; //fuse is rated at 10,000 ... times that by 10 for the equation
const int MAXAMPS = 71;

unsigned long thistime;
unsigned long lasttime;
unsigned long timepassed;

long readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int averageC = 0;                // the average

void setup() {

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
long TANK = TANKMAX;

int cl = 0;

void loop() {
  lasttime = thistime;
  thistime = micros();
  timepassed = thistime - lasttime;

  total = total - readings[index];
  readings[index] = analogRead(CURRENTPIN);
  if (readings[index] == 1023)   // See if we've maxed the ADC; if so, shut down the PWM!
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

  AMPS = map(averageC, 512, 1021, 0, 83);  //83 is close to the max amps that can be measures with a 50 amp shunt
  AMPS = constrain(AMPS, 0, 1000);

  long i2t = (((AMPS - 40) * (AMPS - 40)) * (timepassed)) / 100000;

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

  if (TANK > TANKMAX)
  {
    TANK = TANKMAX;
    digitalWrite(3, HIGH); // Tank is full!
  }
  else
  {
    digitalWrite(3, LOW);
  }

  if ((AMPS > MAXAMPS) || (TANK <= 0)) //Check for max amps or empty tank, shutdown PWM if so
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




