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
    
#include "CNC_Functions.h"
#include "TimerOne.h"

void setup(){
    Serial.begin(57600);
    
    readyCommandString.reserve(128);           //Allocate memory so that this string doesn't fragment the heap as it grows and shrinks
    gcodeLine.reserve(128);
    
    if(pcbRevisionIndicator == 0){
        Serial.println(F("PCB v1.1 Detected"));
    } 
    if(pcbRevisionIndicator == 1){
        Serial.println(F("Beta PCB v1.0 Detected"));
    }
    
    Serial.println(F("ready"));
    _signalReady();
    
    Timer1.initialize(LOOPINTERVAL);
    Timer1.attachInterrupt(runsOnATimer);
    
    Serial.println(F("Grbl v1.00"));
    
#ifdef __AVR_ATmega2560__ //ARDUINO_AVR_MEGA2560
//Mega 2560 specific code
    #define MEGA_2560
    Serial.println(F("compiled for Mega"));
#elif __MK64FX512__ // TEENSY35
//TEENSY 3.5 specific code
    #define TEENSY35
    #define TEENSY
    Serial.println(F("compiled for Teensy 3.5"));
#elif __MK66FX1M0__ //TEENSY36 
//TEENSY 3.6 specific code
    #define TEENSY36
    #define TEENSY
    Serial.println(F("compiled for Teensy 3.6"));
#elif ARDUINO_SAM_DUE 
//Due specific code
#else
    Serial.println(F("compiled for some other board"));
//#error Unsupported hardware
#endif

}

void runsOnATimer(){
    #if misloopDebug > 0
    if (inMovementLoop && !movementUpdated){
        movementFail = true;
    }
    #endif
    movementUpdated = false;
    leftAxis.computePID();
    rightAxis.computePID();
    zAxis.computePID();
}

void loop(){
    
    readyCommandString = ringBuffer.readLine();
    
    if (readyCommandString.length() > 0){
        readyCommandString.trim();  // remove leading and trailing white space
        readyCommandString.toUpperCase();
        interpretCommandString(readyCommandString);
        readyCommandString = "";
    }
    
    holdPosition();
    
    readSerialCommands();
    
    returnPoz(xTarget, yTarget, zAxis.read());
    
    _watchDog();
}
