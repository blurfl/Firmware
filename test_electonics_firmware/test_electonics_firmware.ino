
#define IN1 6   //9
#define IN2 4   //8
#define IN3 9   //11
#define IN4 7   //10
#define IN5 29  //12
#define IN6 30  //13

#define ENA 5  //6
#define ENB 8  //7
#define ENC 12 //5

#define motor_l 0
#define motor_r 1
#define motor_z 2

const int ledPin = 13;

void setup(){
    Serial.begin(57600); 
    pinMode(ledPin, OUTPUT);
   
}


void loop(){
    // pick a speed
    int _speed = random(0,255);
//    float volts = 12 * (float(_speed) / 255.0);
    float volts = 13.29 * (float(_speed) / 255.0);
//    float volts = 3.3 * (float(_speed) / 255.0);
    Serial.print ("_speed ");
    Serial.print(_speed);
    Serial.print (" = ");
    Serial.print(volts);
    Serial.println(" volts");
    //enable all motors
    
    //Command motors to go one way
    moveMotor(motor_l, _speed, 0);
    moveMotor(motor_r, _speed, 0);
    moveMotor(motor_z, _speed, 0);

    Serial.println("Direction A");
    digitalWrite(ledPin, HIGH);   // set the LED on
    delay(10000);
    
    //stop all motors
    stopMotor(motor_l);
    stopMotor(motor_r);
    stopMotor(motor_z);
    
    Serial.println("Stopped");
    delay(1000);
    
    //enable all motors
    
    //Command motors to go the other way
    moveMotor(motor_l, _speed, 1);
    moveMotor(motor_r, _speed, 1);
    moveMotor(motor_z, _speed, 1);
    
    Serial.println("Direction B");
    digitalWrite(ledPin, LOW);   // set the LED on
    delay(10000);
    
    //stop all motors
    stopMotor(motor_l);
    stopMotor(motor_r);
    stopMotor(motor_z);
    
    Serial.println("Stopped");
    delay(1000);

}

void moveMotor(int motor, int _speed, int _direction) {
  int mA = 0;
  int mB = 0;
  int pwmA, pwmB;
  switch (motor) {
  case motor_l:
    mA = IN1;
    mB = IN2;
    break;
  case motor_r:
    mA = IN3;
    mB = IN4;
    break;
  case motor_z:
    mA = IN5;
    mB = IN6;
    break;
  }
  
  if (_direction == 0) {
    pwmA = 0;
    pwmB = _speed;
  } else {
    pwmA = _speed;
    pwmB = 0;
  }
    analogWrite(mA, pwmA);
    analogWrite(mB, pwmB);  
    Serial.print(mA);
    Serial.print(" = ");
    Serial.println(pwmA);
    Serial.print(mB);
    Serial.print(" = ");
    Serial.println(pwmB);
}

void stopMotor(int motor) {
  moveMotor(motor, 0, 0);
}


