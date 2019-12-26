#include <QTRSensors.h>
#define rightMaxSpeed 120
#define leftMaxSpeed 120
#define rightBaseSpeed 80
#define leftBaseSpeed 80
#define turnSpeed 65
float Kp = 0.014;
float Kd = 0.140;
int blk = 100; //Black Line threshold(sensor value)
int wht = 70;
int KpSent = 0, KdSent = 0;
#define NUM_SENSORS   8   
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   QTR_NO_EMITTER_PIN 
#define r1 4
#define r2 5
#define l1 6
#define l2 7
#define rt 3
#define lt 8
#define junCheck 200 // delay for foreward movement
QTRSensorsRC qtrrc((unsigned char[]) {31, 33, 35, 37, 39, 41,43,45}, NUM_SENSORS, TIMEOUT, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];int error, prop, derv, motorSpeed, leftMotorSpeed, rightMotorSpeed, lastError = 0, pos;
int switchOfBot = 1;
char PID()
{
  pos = qtrrc.readLine(sensorValues);
  error = pos - 3500;
  prop = Kp * error;
  derv = Kd * (error - lastError);
  lastError = error;
  motorSpeed = prop + derv;
  rightMotorSpeed = rightBaseSpeed - motorSpeed;
  leftMotorSpeed = leftBaseSpeed + motorSpeed;
  if (rightMotorSpeed > rightMaxSpeed ) rightMotorSpeed = rightMaxSpeed; // prevent the motor from going beyond max speed
  if (leftMotorSpeed > leftMaxSpeed ) leftMotorSpeed = leftMaxSpeed; // prevent the motor from going beyond max speed
  if (rightMotorSpeed < 0) rightMotorSpeed = 0; // keep the motor speed positive
  if (leftMotorSpeed < 0) leftMotorSpeed = 0; // keep the motor speed positive
  digitalWrite(r1, HIGH);digitalWrite(r2, LOW);
  digitalWrite(l1, HIGH);digitalWrite(l2, LOW);
  analogWrite(rt, rightMotorSpeed);
  analogWrite(lt, leftMotorSpeed);
  printAll();
  if(sensorValues[0]>blk && sensorValues[1]>blk && sensorValues[2]>blk && sensorValues[7]>blk && sensorValues[6]>blk && sensorValues[5]>blk)
  { digitalWrite(A8, HIGH);digitalWrite(A10, HIGH);return 'B';}
  else if(sensorValues[0]>blk && sensorValues[1]>blk && sensorValues[2]>blk && sensorValues[3]>blk)
  { digitalWrite(A8, HIGH);return 'L';}
  else if(sensorValues[7]>blk && sensorValues[6]>blk && sensorValues[5]>blk && sensorValues[4]>blk)
  {digitalWrite(A10, HIGH);return 'R';}
  else if(sensorValues[0]<wht && sensorValues[1]<wht && sensorValues[2]<wht && sensorValues[3]<wht && sensorValues[4]<wht && sensorValues[5]<wht && sensorValues[6]<wht && sensorValues[7]<wht)
  {digitalWrite(A8, LOW);digitalWrite(A10, LOW);return 'E';} // E signifies End of line
  else {digitalWrite(A8, LOW);digitalWrite(A10, LOW);return 'N';}//N for none
}

void printAll()
{
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.print("error:");
  Serial.print(error);Serial.print("\t");
  // Serial.print("prop:");
  // Serial.print(prop);Serial.print("\t");
  // Serial.print("derv:");
  // Serial.print(derv);Serial.print("\t");
  // Serial.print("motorSpeed:");
  // Serial.print(motorSpeed);Serial.print("\t");
  // Serial.print("left:");
  // Serial.print(leftMotorSpeed);Serial.print("\t");
  // Serial.print("right:");
  // Serial.print(rightMotorSpeed);Serial.print("\t");
  Serial.println();
}

void printMinMax()
{
  Serial.println("Minimum Values: ");
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtrrc.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println("Maximum Values: ");
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtrrc.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
}


int checkForeward()
{
  instantStop();
  delay(100);
  
//  digitalWrite(r1, HIGH); digitalWrite(r2, LOW);
//  analogWrite(rt, 150);
//  digitalWrite(l1, HIGH);digitalWrite(l2, LOW);  
//  analogWrite(lt, 150);
//  delay(50);
  analogWrite(rt, 0);
  analogWrite(lt, 0);
  pos = qtrrc.readLine(sensorValues);
  if(sensorValues[0]>blk && sensorValues[1]>blk && sensorValues[2]>blk && sensorValues[3]>blk && sensorValues[4]>blk && sensorValues[5]>blk && sensorValues[6]>blk && sensorValues[7]>blk)
  {return 2;}
  else if(sensorValues[3]>blk || sensorValues[4]>blk){return 1;}
  else return 0;
}

void turn(char dir)
{
  switch(dir)
  {
    case 'L':    
      digitalWrite(r1, HIGH); digitalWrite(r2, LOW);
      analogWrite(rt, turnSpeed);
      digitalWrite(l1, LOW);digitalWrite(l2, HIGH);  
      analogWrite(lt, turnSpeed);
      while (sensorValues[0]<wht && sensorValues[1]<wht) 
      {
        pos = qtrrc.readLine(sensorValues);
      }
      analogWrite(rt, 0);  
      analogWrite(lt, 0); 
      digitalWrite(r1, LOW); digitalWrite(r2, HIGH);
      digitalWrite(l1, HIGH);digitalWrite(l2, LOW);
      analogWrite(rt, turnSpeed);analogWrite(lt, turnSpeed);
      delay(69);
      analogWrite(rt, 0);analogWrite(lt, 0); 
      break;
      
   
    case 'R':        
      digitalWrite(l1, HIGH); digitalWrite(l2, LOW); 
      analogWrite(lt, turnSpeed);
      digitalWrite(r1, LOW); digitalWrite(r2, HIGH);  
      analogWrite(rt, turnSpeed);
      while (sensorValues[7]<wht && sensorValues[6]<wht) 
      {
        pos = qtrrc.readLine(sensorValues);
      }
      analogWrite(lt, 0);  
      analogWrite(rt, 0);  
      digitalWrite(r1, LOW); digitalWrite(r2, HIGH);
      digitalWrite(l1, HIGH);digitalWrite(l2, LOW);
      analogWrite(rt, turnSpeed);analogWrite(lt, turnSpeed);
      delay(69);
      analogWrite(rt, 0);analogWrite(lt, 0);    
      break;
  }
}

void selectTurn(char node)
{
  if(node == 'L' || node == 'B' || node == 'R')
    {
      if(node =='L') {turn('L');return 0;}
      int foreward = checkForeward();
      if(foreward == 2) {switchOfBot = 0;analogWrite(rt, 0);analogWrite(lt, 0);delay(5000);switchOfBot = 1;}
      if(node =='L' || node == 'B') {turn('L');}
      else if(foreward == 1) {}
      else if(node == 'R' && foreward == 0) turn('R'); 
    }
  if(node == 'E')
  {
    turn('L');
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(r1, OUTPUT); 
  pinMode(r2, OUTPUT);
  pinMode(l1, OUTPUT); 
  pinMode(l2, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A9, OUTPUT);
  pinMode(A10, OUTPUT);
  pinMode(A8, OUTPUT);
  delay(1000);
  digitalWrite(13, HIGH);    // turn on Arduino's LED to indicate we are in calibration mode
  for (int i = 0; i < 400; i++)  // make the calibration take about 10 seconds
  { 
    qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
  }
  digitalWrite(13, LOW);     // turn off Arduino's LED to indicate we are through with calibration
  printMinMax();
  digitalWrite(A9, LOW);
}

void instantStop()
{
  digitalWrite(r1, LOW); digitalWrite(r2, HIGH);
  digitalWrite(l1, LOW);digitalWrite(l2, HIGH);
  analogWrite(lt, leftMotorSpeed);
  analogWrite(rt, rightMotorSpeed);
  delay(125);
  analogWrite(rt, 0);analogWrite(lt, 0);
}

void loop()
{
  if(Serial.available())
  { 
//    switchOfBot = Serial.parseInt();
    KpSent = Serial.parseInt();
    KdSent = Serial.parseInt();
    char j = Serial.read();
    Kp = KpSent/1000.0;
    Kd = KdSent/1000.0;
  }
  if(switchOfBot != 0)
  {
    char node = PID();
    selectTurn(node);
  }
}
