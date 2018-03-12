/*This file is part of the Maslow Control Software.

    The Maslow Control Software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Maslow Control Software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Maslow Control Software.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2014-2017 Bar Smith*/

/*
The Motor module imitates the behavior of the Arduino servo module. It allows a gear motor (or any electric motor)
to be a drop in replacement for a continuous rotation servo.

*/

#include "Maslow.h"

Motor::Motor(){

  _attachedState = 0;


}

int  Motor::setupMotor(const int& pwmPin, const int& pin1, const int& pin2){

  //store pin numbers as private variables
  _pwmPin = pwmPin;
  _pin1  = pin1;
  _pin2  = pin2;
  _attachedState = 0;

  //set pinmodes
  pinMode(_pwmPin,   OUTPUT);
  pinMode(_pin1,     OUTPUT);
  pinMode(_pin2,     OUTPUT);

  //stop the motor
  digitalWrite(_pin1,    HIGH);
  digitalWrite(_pin2,    LOW) ;
  digitalWrite(_pwmPin,  LOW);

  return 1;
}

void Motor::attach(){
    _attachedState = 1;
}

void Motor::detach(){
    _attachedState = 0;

    //stop the motor
    digitalWrite(_pin1,    HIGH);
    digitalWrite(_pin2,    LOW) ;
    digitalWrite(_pwmPin,  LOW);
}

int Motor::lastSpeed(){
    /*
    Returns the last speed(voltage) value sent to the pwm
    */
    return _lastSpeed;
}

void Motor::additiveWrite(int speed){
    /*
    Increases/decreases the motor speed by the passed speed amount
    */
    write(_lastSpeed + speed);
}

void Motor::write(int speed){
    /*
    Sets motor speed from input. Speed = 0 is stopped, -255 is full reverse, 255 is full ahead.
    */
    if (_attachedState == 1){
        // if ((speed > 255) || (speed < -255)){
        //   if (_isOverSpeed){
        //     if (millis() - _overSpeedStartTime >= 20){  // 20 means 3 consecutive PID cycles, change to whatever you like
        //
        //       // Serial.println(F("WARNING OVERSPEED EVENT!"));
        //       String axisName = " ";
        //       switch (_pwmPin) {
        //       case 5:
        //         switch (_pin1) {
        //           case 6: // board v1.1 Right
        //             axisName = "R";
        //           break;
        //           case 4: // board v1.2 Right
        //             axisName = "R";
        //           break;
        //           case 12: // board v1.0 Left
        //             axisName = "L";
        //           break;
        //         }
        //       break;
        //       case 6:
        //         axisName = "R"; // board v1.0 Right
        //       break;
        //       case 7:
        //         axisName = "Z"; // board v1.0 Z
        //       break;
        //       case 8:
        //         switch (_pin1) {
        //           case 9: // board v1.1 Z
        //             axisName = "Z";
        //           break;
        //           case 7: // board v1.2 Z
        //             axisName = "Z";
        //           break;
        //         }
        //       break;
        //       case 10:
        //         axisName = "L"; // board v1.2 Left
        //       break;
        //       case 12:
        //         axisName = "L"; // board v1.1 Left
        //       break;
        //       default:
        //         axisName = "!"; // board undetermined
        //       break;
        //       }
        //       String warningText = "OVERSPEED WARNING - " + axisName;
        //       Serial.println(warningText);
        //       _overSpeedStartTime = millis();
        //     }
        //   }
        //   else {
        //     _isOverSpeed = true;
        //     _overSpeedStartTime = millis();
        //   }
        // }
        // else {
        //   _isOverSpeed = false;
        // }
        speed = constrain(speed, -255, 255);
        _lastSpeed = speed; //saves speed for use in additive write
        bool forward = (speed > 0);
        speed = abs(speed); //remove sign from input because direction is set by control pins on H-bridge

        bool usePin1 = ((_pin1 != 4) && (_pin1 != 13) && (_pin1 != 11) && (_pin1 != 12)); // avoid PWM using timer0 or timer1
        bool usePin2 = ((_pin2 != 4) && (_pin2 != 13) && (_pin2 != 11) && (_pin2 != 12)); // avoid PWM using timer0 or timer1
        if (forward) {
            if (speed > 0) {
              if (usePin2) {
                digitalWrite(_pin1 , HIGH );
                analogWrite(_pin2 , 255 - speed); // invert drive signals - don't alter speed
              } else {
                analogWrite(_pin1 , speed);
                digitalWrite(_pin2 , LOW );
              }
            } else {
              digitalWrite(_pin1 , LOW );
              digitalWrite(_pin2 , LOW );
            }
        } else{
            if (usePin1) {
              analogWrite(_pin1 , 255 - speed); // invert drive signals - don't alter speed
              digitalWrite(_pin2 , HIGH );
            } else {
                analogWrite(_pin2 , speed);
                digitalWrite(_pin1 , LOW );
            }
        }
        digitalWrite(_pwmPin, HIGH);
    }
}

void Motor::directWrite(int voltage){
    /*
    Write directly to the motor, ignoring if the axis is attached or any applied calibration.
    */
    bool forward = (voltage >= 0);
    if (forward) {
        if (voltage > 0) {
          digitalWrite(_pin1 , HIGH );
          analogWrite(_pin2 , 255 - abs(voltage)); // invert drive signals - don't alter speed
        } else {
          digitalWrite(_pin1 , LOW );
          digitalWrite(_pin2 , LOW );
        }
    }
    else{
        analogWrite(_pin1 , 255 - abs(voltage)); // invert drive signals - don't alter speed
        digitalWrite(_pin2 , HIGH );
    }
    digitalWrite(_pwmPin, HIGH);
}

int  Motor::attached(){

    return _attachedState;
}

int  Motor::_convolve(const int& input){
    /*
    This function distorts the input signal in a manner which is the inverse of the way
    the mechanics of the motor distort it to give a linear response.
    */

    int output = input;

    int arrayLen = sizeof(_linSegments)/sizeof(_linSegments[1]);
    for (int i = 0; i <= arrayLen - 1; i++){
        if (input > _linSegments[i].negativeBound and input < _linSegments[i].positiveBound){
            output = (input + _linSegments[i].intercept)/_linSegments[i].slope;
            break;
        }
    }

    return output;
}

void Motor::setSegment(const int& index, const float& slope, const float& intercept, const int& negativeBound, const int& positiveBound){

    //Adds a linearizion segment to the linSegments object in location index

    _linSegments[index].slope          =          slope;
    _linSegments[index].intercept      =      intercept;
    _linSegments[index].positiveBound  =  positiveBound;
    _linSegments[index].negativeBound  =  negativeBound;

}

LinSegment Motor::getSegment(const int& index){
    return _linSegments[index];
}
