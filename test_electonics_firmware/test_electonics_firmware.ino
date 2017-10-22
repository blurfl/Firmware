
#define IN1 6   //9
#define IN2 4   //8
#define IN3 9   //11
#define IN4 7   //10
#define IN5 10  //12
#define IN6 11  //13

#define ENA 5  //6
#define ENB 8  //7
#define ENC 12 //5

#define motor_l 0
#define motor_r 1
#define motor_z 2

void setup(){
    Serial.begin(57600);    
}


void loop(){
    // pick a speed
    int _speed = random(0,255);
    float volts = 12.0 * (float(_speed) / 255.0);
    Serial.print(_speed);
    Serial.print (" = ");
    Serial.println(volts);
    //enable all motors
    
    //Command motors to go one way
    moveMotor(motor_l, _speed, 0);
    moveMotor(motor_r, _speed, 0);
    moveMotor(motor_z, _speed, 0);

    Serial.println("Direction A");
    delay(2000);
    
    //stop all motors
    stopMotor(motor_l);
    stopMotor(motor_r);
    stopMotor(motor_z);
    
    Serial.println("Stopped");
    delay(2000);
    
    //enable all motors
    
    //Command motors to go the other way
    moveMotor(motor_l, _speed, 1);
    moveMotor(motor_r, _speed, 1);
    moveMotor(motor_z, _speed, 1);
    
    Serial.println("Direction B");
    delay(2000);
    
    //stop all motors
    stopMotor(motor_l);
    stopMotor(motor_r);
    stopMotor(motor_z);
    
    Serial.println("Stopped");
    delay(2000);

}

void moveMotor(int motor, int _speed, int _direction) {
  int mA = 0;
  int mB = 0;
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
    analogWrite(mA, 0);
    analogWrite(mB, _speed);
  } else {
    analogWrite(mB, 0);
    analogWrite(mA, _speed);    
  }
}

void stopMotor(int motor) {
  moveMotor(motor, 0, 0);
}


