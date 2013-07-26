/*
  Author of HomeControl is Tommy Ziegler - http://tommyziegler.com

  This file is a for of the RCSwitch class from https://github.com/denschu/rcswitch-pi
  ------------------------------------------------------------------------------------
  Original license text:

  RCSwitch - Arduino libary for remote control outlet switches
  Copyright (c) 2011 Suat �zg�r.  All right reserved.

  Contributors:
  - Andre Koehler / info(at)tomate-online(dot)de
  - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com
  - Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=48

  Project home: http://code.google.com/p/rc-switch/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  ------------------------------------------------------------------------------------
  When there are questions, please contact me under: me@tommyziegler.com
*/

#include "HomeControl.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned long HomeControl::nReceivedValue = NULL;
unsigned int HomeControl::nReceivedBitlength = 0;
unsigned int HomeControl::nReceivedDelay = 0;
unsigned int HomeControl::nReceivedProtocol = 0;
unsigned int HomeControl::timings[HOMECTRLSWITCH_MAX_CHANGES];
int HomeControl::nReceiveTolerance = 60;

HomeControl::HomeControl() {
  this->nReceiverInterrupt = -1;
  this->nTransmitterPin = -1;
  HomeControl::nReceivedValue = NULL;

  this->setPulseLength(350);
  this->setRepeatTransmit(10);
  this->setReceiveTolerance(60);
  this->setProtocol(1);
}

/**
  * Sets the protocol to send.
  */
void HomeControl::setProtocol(int nProtocol) {
  this->nProtocol = nProtocol;
  if (nProtocol == 1){
    this->setPulseLength(350);
  }
  else if (nProtocol == 2) {
    this->setPulseLength(650);
  }
}

/**
  * Sets the protocol to send with pulse length in microseconds.
  */
void HomeControl::setProtocol(int nProtocol, int nPulseLength) {
  this->nProtocol = nProtocol;
  if (nProtocol == 1){
    this->setPulseLength(nPulseLength);
  }
  else if (nProtocol == 2) {
    this->setPulseLength(nPulseLength);
  }
}


/**
  * Sets pulse length in microseconds
  */
void HomeControl::setPulseLength(int nPulseLength) {
  this->nPulseLength = nPulseLength;
}

/**
 * Sets Repeat Transmits
 */
void HomeControl::setRepeatTransmit(int nRepeatTransmit) {
  this->nRepeatTransmit = nRepeatTransmit;
}

/**
 * Set Receiving Tolerance
 */
void HomeControl::setReceiveTolerance(int nPercent) {
  HomeControl::nReceiveTolerance = nPercent;
}


/**
 * Enable transmissions
 *
 * @param nTransmitterPin    Arduino Pin to which the sender is connected to
 */
void HomeControl::enableTransmit(int nTransmitterPin) {
  this->nTransmitterPin = nTransmitterPin;
  pinMode(this->nTransmitterPin, OUTPUT);
}

/**
  * Disable transmissions
  */
void HomeControl::disableTransmit() {
  this->nTransmitterPin = -1;
}

/**
 * Switch a remote switch on (Type D REV)
 *
 * @param sGroup        Code of the switch group (A,B,C,D)
 * @param nDevice       Number of the switch itself (1..3)
 */
void HomeControl::switchOnD(char sGroup, int nDevice) {
  this->sendTriState( this->getCodeWordD(sGroup, nDevice, true) );
}

/**
 * Switch a remote switch off (Type D REV)
 *
 * @param sGroup        Code of the switch group (A,B,C,D)
 * @param nDevice       Number of the switch itself (1..3)
 */
void HomeControl::switchOffD(char sGroup, int nDevice) {
  this->sendTriState( this->getCodeWordD(sGroup, nDevice, false) );
}

/**
 * Switch a remote switch on (Type C Intertechno)
 *
 * @param sFamily  Familycode (a..f)
 * @param nGroup   Number of group (1..4)
 * @param nDevice  Number of device (1..4)
  */
void HomeControl::switchOnC(char sFamily, int nGroup, int nDevice) {
  this->sendTriState( this->getCodeWordC(sFamily, nGroup, nDevice, true) );
}

/**
 * Switch a remote switch off (Type C Intertechno)
 *
 * @param sFamily  Familycode (a..f)
 * @param nGroup   Number of group (1..4)
 * @param nDevice  Number of device (1..4)
 */
void HomeControl::switchOffC(char sFamily, int nGroup, int nDevice) {
  this->sendTriState( this->getCodeWordC(sFamily, nGroup, nDevice, false) );
}

/**
 * Switch a remote switch on (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
void HomeControl::switchOnB(int nAddressCode, int nChannelCode) {
  this->sendTriState( this->getCodeWordB(nAddressCode, nChannelCode, true) );
}

/**
 * Switch a remote switch off (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
void HomeControl::switchOffB(int nAddressCode, int nChannelCode) {
  this->sendTriState( this->getCodeWordB(nAddressCode, nChannelCode, false) );
}

/**
 * Switch a remote switch on (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param sDevice       Code of the switch device (refers to DIP switches 6..10 (A..E) where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 */
void HomeControl::switchOnA(char* sGroup, char* sDevice) {
  this->sendTriState( this->getCodeWordA(sGroup, sDevice, true) );
}

/**
 * Switch a remote switch off (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param sDevice       Code of the switch device (refers to DIP switches 6..10 (A..E) where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 */
void HomeControl::switchOffA(char* sGroup, char* sDevice) {
    this->sendTriState( this->getCodeWordA(sGroup, sDevice, false) );
}

/**
 * Deprecated, use switchOn(char* sGroup, char* sDevice) instead!
 * Switch a remote switch on (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..5)
 */
void HomeControl::switchOn(char* sGroup, int nChannel) {
  char* code[6] = { "00000", "10000", "01000", "00100", "00010", "00001" };
  this->switchOnA(sGroup, code[nChannel]);
}

/**
 * Deprecated, use switchOff(char* sGroup, char* sDevice) instead!
 * Switch a remote switch off (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..5)
 */
void HomeControl::switchOff(char* sGroup, int nChannel) {
  char* code[6] = { "00000", "10000", "01000", "00100", "00010", "00001" };
  this->switchOffA(sGroup, code[nChannel]);
}

/**
 * Returns a char[13], representing the Code Word to be send.
 *
 * getCodeWordA(char*, char*)
 *
 */
char* HomeControl::getCodeWordA(char* sGroup, char* sDevice, boolean bOn) {
    static char sDipSwitches[13];
    int i = 0;
    int j = 0;

    for (i=0; i < 5; i++) {
        if (sGroup[i] == '0') {
            sDipSwitches[j++] = 'F';
        } else {
            sDipSwitches[j++] = '0';
        }
    }

    for (i=0; i < 5; i++) {
        if (sDevice[i] == '0') {
            sDipSwitches[j++] = 'F';
        } else {
            sDipSwitches[j++] = '0';
        }
    }

    if ( bOn ) {
        sDipSwitches[j++] = '0';
        sDipSwitches[j++] = 'F';
    } else {
        sDipSwitches[j++] = 'F';
        sDipSwitches[j++] = '0';
    }

    sDipSwitches[j] = '\0';

    return sDipSwitches;
}

/**
 * Returns a char[13], representing the Code Word to be send.
 * A Code Word consists of 9 address bits, 3 data bits and one sync bit but in our case only the first 8 address bits and the last 2 data bits were used.
 * A Code Bit can have 4 different states: "F" (floating), "0" (low), "1" (high), "S" (synchronous bit)
 *
 * +-------------------------------+--------------------------------+-----------------------------------------+-----------------------------------------+----------------------+------------+
 * | 4 bits address (switch group) | 4 bits address (switch number) | 1 bit address (not used, so never mind) | 1 bit address (not used, so never mind) | 2 data bits (on|off) | 1 sync bit |
 * | 1=0FFF 2=F0FF 3=FF0F 4=FFF0   | 1=0FFF 2=F0FF 3=FF0F 4=FFF0    | F                                       | F                                       | on=FF off=F0         | S          |
 * +-------------------------------+--------------------------------+-----------------------------------------+-----------------------------------------+----------------------+------------+
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 * @param bStatus       Wether to switch on (true) or off (false)
 *
 * @return char[13]
 */
char* HomeControl::getCodeWordB(int nAddressCode, int nChannelCode, boolean bStatus) {
   int nReturnPos = 0;
   static char sReturn[13];

   char* code[5] = { "FFFF", "0FFF", "F0FF", "FF0F", "FFF0" };
   if (nAddressCode < 1 || nAddressCode > 4 || nChannelCode < 1 || nChannelCode > 4) {
    return '\0';
   }
   for (int i = 0; i<4; i++) {
     sReturn[nReturnPos++] = code[nAddressCode][i];
   }

   for (int i = 0; i<4; i++) {
     sReturn[nReturnPos++] = code[nChannelCode][i];
   }

   sReturn[nReturnPos++] = 'F';
   sReturn[nReturnPos++] = 'F';
   sReturn[nReturnPos++] = 'F';

   if (bStatus) {
      sReturn[nReturnPos++] = 'F';
   } else {
      sReturn[nReturnPos++] = '0';
   }

   sReturn[nReturnPos] = '\0';

   return sReturn;
}

/**
 * Like getCodeWord (Type C = Intertechno)
 */
char* HomeControl::getCodeWordC(char sFamily, int nGroup, int nDevice, boolean bStatus) {
  static char sReturn[13];
  int nReturnPos = 0;

  if ( (byte)sFamily < 97 || (byte)sFamily > 112 || nGroup < 1 || nGroup > 4 || nDevice < 1 || nDevice > 4) {
    return '\0';
  }

  char* sDeviceGroupCode =  dec2binWzerofill(  (nDevice-1) + (nGroup-1)*4, 4  );
  char familycode[16][5] = { "0000", "F000", "0F00", "FF00", "00F0", "F0F0", "0FF0", "FFF0", "000F", "F00F", "0F0F", "FF0F", "00FF", "F0FF", "0FFF", "FFFF" };
  for (int i = 0; i<4; i++) {
    sReturn[nReturnPos++] = familycode[ (int)sFamily - 97 ][i];
  }
  for (int i = 0; i<4; i++) {
    sReturn[nReturnPos++] = (sDeviceGroupCode[3-i] == '1' ? 'F' : '0');
  }
  sReturn[nReturnPos++] = '0';
  sReturn[nReturnPos++] = 'F';
  sReturn[nReturnPos++] = 'F';
  if (bStatus) {
    sReturn[nReturnPos++] = 'F';
  } else {
    sReturn[nReturnPos++] = '0';
  }
  sReturn[nReturnPos] = '\0';
  return sReturn;
}

/**
 * Decoding for the REV Switch Type
 *
 * Returns a char[13], representing the Tristate to be send.
 * A Code Word consists of 7 address bits and 5 command data bits.
 * A Code Bit can have 3 different states: "F" (floating), "0" (low), "1" (high)
 *
 * +-------------------------------+--------------------------------+-----------------------+
 * | 4 bits address (switch group) | 3 bits address (device number) | 5 bits (command data) |
 * | A=1FFF B=F1FF C=FF1F D=FFF1   | 1=0FFF 2=F0FF 3=FF0F 4=FFF0    | on=00010 off=00001    |
 * +-------------------------------+--------------------------------+-----------------------+
 *
 * Source: http://www.the-intruder.net/funksteckdosen-von-rev-uber-arduino-ansteuern/
 *
 * @param sGroup        Name of the switch group (A..D, resp. a..d)
 * @param nDevice       Number of the switch itself (1..3)
 * @param bStatus       Wether to switch on (true) or off (false)
 *
 * @return char[13]
 */
char* HomeControl::getCodeWordD(char sGroup, int nDevice, boolean bStatus){
    static char sReturn[13];
    int nReturnPos = 0;

    // Building 4 bits address
    // (Potential problem if dec2binWcharfill not returning correct string)
    char *sGroupCode;
    switch(sGroup){
        case 'a':
        case 'A':
            sGroupCode = dec2binWcharfill(8, 4, 'F'); break;
        case 'b':
        case 'B':
            sGroupCode = dec2binWcharfill(4, 4, 'F'); break;
        case 'c':
        case 'C':
            sGroupCode = dec2binWcharfill(2, 4, 'F'); break;
        case 'd':
        case 'D':
            sGroupCode = dec2binWcharfill(1, 4, 'F'); break;
        default:
            return '\0';
    }

    for (int i = 0; i<4; i++)
    {
        sReturn[nReturnPos++] = sGroupCode[i];
    }


    // Building 3 bits address
    // (Potential problem if dec2binWcharfill not returning correct string)
    char *sDevice;
    switch(nDevice) {
        case 1:
            sDevice = dec2binWcharfill(4, 3, 'F'); break;
        case 2:
            sDevice = dec2binWcharfill(2, 3, 'F'); break;
        case 3:
            sDevice = dec2binWcharfill(1, 3, 'F'); break;
        default:
            return '\0';
    }

    for (int i = 0; i<3; i++)
        sReturn[nReturnPos++] = sDevice[i];

    // fill up rest with zeros
    for (int i = 0; i<5; i++)
        sReturn[nReturnPos++] = '0';

    // encode on or off
    if (bStatus)
        sReturn[10] = '1';
    else
        sReturn[11] = '1';

    // last position terminate string
    sReturn[12] = '\0';
    return sReturn;

}

/**
 * @param sCodeWord   /^[10FS]*$/  -> see getCodeWord
 */
void HomeControl::sendTriState(char* sCodeWord) {
  //printf("HomeControl::sendTriState: %s\n", sCodeWord);

  for (int nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
      switch(sCodeWord[i]) {
        case '0':
          this->sendT0();
        break;
        case 'F':
          this->sendTF();
        break;
        case '1':
          this->sendT1();
        break;
      }
      i++;
    }
    this->sendSync();
      //printf("\n\n", sCodeWord);

  }
}

void HomeControl::send(unsigned long Code, unsigned int length) {
  //printf("HomeControl::send(unsigned long Code, unsigned int length)\n");
  this->send( this->dec2binWzerofill(Code, length) );
}

void HomeControl::send(char* sCodeWord) {
  //printf("HomeControl::send(char* sCodeWord)\n");

  for (int nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
      switch(sCodeWord[i]) {
        case '0':
          this->send0();
        break;
        case '1':
          this->send1();
        break;
      }
      i++;
    }
    this->sendSync();
  }
}

void HomeControl::transmit(int nHighPulses, int nLowPulses) {
  //printf("HomeControl::transmit()\n");

    boolean disabled_Receive = false;
    int nReceiverInterrupt_backup = nReceiverInterrupt;
    if (this->nTransmitterPin != -1) {
        if (this->nReceiverInterrupt != -1) {
            this->disableReceive();
            disabled_Receive = true;
        }

        //printf("this->nTransmitterPin %i\n", this->nTransmitterPin);
        
        digitalWrite(this->nTransmitterPin, HIGH);
        delayMicroseconds( this->nPulseLength * nHighPulses);
        digitalWrite(this->nTransmitterPin, LOW);
        delayMicroseconds( this->nPulseLength * nLowPulses);


        if(disabled_Receive){
            this->enableReceive(nReceiverInterrupt_backup);
        }
    }
}
/**
 * Sends a "0" Bit
 *                       _
 * Waveform Protocol 1: | |___
 *                       _
 * Waveform Protocol 2: | |__
 */
void HomeControl::send0() {
    //printf("HomeControl::send0()\n");

    if (this->nProtocol == 1){
        this->transmit(1,3);
    }
    else if (this->nProtocol == 2) {
        this->transmit(1,2);
    }
    else if (this->nProtocol == 3) {
        this->transmit(4,11);
    }
}

/**
 * Sends a "1" Bit
 *                       ___
 * Waveform Protocol 1: |   |_
 *                       __
 * Waveform Protocol 2: |  |_
 */
void HomeControl::send1() {
    //printf("HomeControl::send1()\n");

    if (this->nProtocol == 1){
        this->transmit(3,1);
    }
    else if (this->nProtocol == 2) {
        this->transmit(2,1);
    }
    else if (this->nProtocol == 3) {
        this->transmit(9,6);
    }
}


/**
 * Sends a Tri-State "0" Bit
 *            _     _
 * Waveform: | |___| |___
 */
void HomeControl::sendT0() {
  //printf("HomeControl::sendT0()\n");

  this->transmit(1,3);
  this->transmit(1,3);
}

/**
 * Sends a Tri-State "1" Bit
 *            ___   ___
 * Waveform: |   |_|   |_
 */
void HomeControl::sendT1() {
  //printf("HomeControl::sendT1()\n");

  this->transmit(3,1);
  this->transmit(3,1);
}

/**
 * Sends a Tri-State "F" Bit
 *            _     ___
 * Waveform: | |___|   |_
 */
void HomeControl::sendTF() {
  //printf("HomeControl::sendTF()\n");

  this->transmit(1,3);
  this->transmit(3,1);
}

/**
 * Sends a "Sync" Bit
 *                       _
 * Waveform Protocol 1: | |_______________________________
 *                       _
 * Waveform Protocol 2: | |__________
 */
void HomeControl::sendSync() {
    //printf("HomeControl::sendSync()\n");

    if (this->nProtocol == 1){
        //printf("HomeControl::sendSync() nProtocol 1\n");
        this->transmit(1,31);
    }
    else if (this->nProtocol == 2) {
        //printf("HomeControl::sendSync() nProtocol 2\n");
        this->transmit(1,10);
    }
    else if (this->nProtocol == 3) {
        //printf("HomeControl::sendSync() nProtocol 3\n");
        this->transmit(1,71);
    }
}

/**
 * Enable receiving data
 */
void HomeControl::enableReceive(int interrupt) {
  this->nReceiverInterrupt = interrupt;
  this->enableReceive();
}

void HomeControl::enableReceive() {
  if (this->nReceiverInterrupt != -1) {
    HomeControl::nReceivedValue = NULL;
    HomeControl::nReceivedBitlength = NULL;
  }
}

/**
 * Disable receiving data
 */
void HomeControl::disableReceive() {
  this->nReceiverInterrupt = -1;
}

bool HomeControl::available() {
  return HomeControl::nReceivedValue != NULL;
}

void HomeControl::resetAvailable() {
  HomeControl::nReceivedValue = NULL;
}

unsigned long HomeControl::getReceivedValue() {
    return HomeControl::nReceivedValue;
}

unsigned int HomeControl::getReceivedBitlength() {
  return HomeControl::nReceivedBitlength;
}

unsigned int HomeControl::getReceivedDelay() {
  return HomeControl::nReceivedDelay;
}

unsigned int HomeControl::getReceivedProtocol() {
  return HomeControl::nReceivedProtocol;
}

unsigned int* HomeControl::getReceivedRawdata() {
    return HomeControl::timings;
}

/**
 *
 */
bool HomeControl::receiveProtocol1(unsigned int changeCount){

    unsigned long code = 0;
      unsigned long delay = HomeControl::timings[0] / 31;
      unsigned long delayTolerance = delay * HomeControl::nReceiveTolerance * 0.01;

      for (int i = 1; i<changeCount ; i=i+2) {

          if (HomeControl::timings[i] > delay-delayTolerance && HomeControl::timings[i] < delay+delayTolerance && HomeControl::timings[i+1] > delay*3-delayTolerance && HomeControl::timings[i+1] < delay*3+delayTolerance) {
            code = code << 1;
          } else if (HomeControl::timings[i] > delay*3-delayTolerance && HomeControl::timings[i] < delay*3+delayTolerance && HomeControl::timings[i+1] > delay-delayTolerance && HomeControl::timings[i+1] < delay+delayTolerance) {
            code+=1;
            code = code << 1;
          } else {
            // Failed
            i = changeCount;
            code = 0;
          }
      }
      code = code >> 1;
    if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
      HomeControl::nReceivedValue = code;
      HomeControl::nReceivedBitlength = changeCount / 2;
      HomeControl::nReceivedDelay = delay;
    HomeControl::nReceivedProtocol = 1;
    }

  if (code == 0){
    return false;
  }else if (code != 0){
    return true;
  }


}

bool HomeControl::receiveProtocol2(unsigned int changeCount){

    unsigned long code = 0;
      unsigned long delay = HomeControl::timings[0] / 10;
      unsigned long delayTolerance = delay * HomeControl::nReceiveTolerance * 0.01;

      for (int i = 1; i<changeCount ; i=i+2) {

          if (HomeControl::timings[i] > delay-delayTolerance && HomeControl::timings[i] < delay+delayTolerance && HomeControl::timings[i+1] > delay*2-delayTolerance && HomeControl::timings[i+1] < delay*2+delayTolerance) {
            code = code << 1;
          } else if (HomeControl::timings[i] > delay*2-delayTolerance && HomeControl::timings[i] < delay*2+delayTolerance && HomeControl::timings[i+1] > delay-delayTolerance && HomeControl::timings[i+1] < delay+delayTolerance) {
            code+=1;
            code = code << 1;
          } else {
            // Failed
            i = changeCount;
            code = 0;
          }
      }
      code = code >> 1;
    if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
      HomeControl::nReceivedValue = code;
      HomeControl::nReceivedBitlength = changeCount / 2;
      HomeControl::nReceivedDelay = delay;
    HomeControl::nReceivedProtocol = 2;
    }

  if (code == 0){
    return false;
  }else if (code != 0){
    return true;
  }

}

/** Protocol 3 is used by BL35P02.
 *
 */
bool HomeControl::receiveProtocol3(unsigned int changeCount){

      unsigned long code = 0;
      unsigned long delay = HomeControl::timings[0] / PROTOCOL3_SYNC_FACTOR;
      unsigned long delayTolerance = delay * HomeControl::nReceiveTolerance * 0.01;

      for (int i = 1; i<changeCount ; i=i+2) {

          if  (HomeControl::timings[i]   > delay*PROTOCOL3_0_HIGH_CYCLES - delayTolerance
            && HomeControl::timings[i]   < delay*PROTOCOL3_0_HIGH_CYCLES + delayTolerance
            && HomeControl::timings[i+1] > delay*PROTOCOL3_0_LOW_CYCLES  - delayTolerance
            && HomeControl::timings[i+1] < delay*PROTOCOL3_0_LOW_CYCLES  + delayTolerance) {
            code = code << 1;
          } else if (HomeControl::timings[i]   > delay*PROTOCOL3_1_HIGH_CYCLES - delayTolerance
                  && HomeControl::timings[i]   < delay*PROTOCOL3_1_HIGH_CYCLES + delayTolerance
                  && HomeControl::timings[i+1] > delay*PROTOCOL3_1_LOW_CYCLES  - delayTolerance
                  && HomeControl::timings[i+1] < delay*PROTOCOL3_1_LOW_CYCLES  + delayTolerance) {
            code+=1;
            code = code << 1;
          } else {
            // Failed
            i = changeCount;
            code = 0;
          }
      }
      code = code >> 1;
      if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
        HomeControl::nReceivedValue = code;
        HomeControl::nReceivedBitlength = changeCount / 2;
        HomeControl::nReceivedDelay = delay;
        HomeControl::nReceivedProtocol = 3;
      }

      if (code == 0){
        return false;
      }else if (code != 0){
        return true;
      }
}

/**
  * Turns a decimal value to its binary representation
  */
char* HomeControl::dec2binWzerofill(unsigned long Dec, unsigned int bitLength){
    return dec2binWcharfill(Dec, bitLength, '0');
}

char* HomeControl::dec2binWcharfill(unsigned long Dec, unsigned int bitLength, char fill){
  static char bin[64];
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : fill;
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    }else {
      bin[j] = fill;
    }
  }
  bin[bitLength] = '\0';

  return bin;
}
