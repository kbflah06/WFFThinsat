

/*Code to read the sensor data in digital counts, 
and convert it to uva intensity (mW/m^2), 
and then serially packet the data and serially send it to the NSL bus
The EM (Engineering model and realterm is used to view data.
Data is two bytes in mW/m^2 appended with zeroes to make the packet to be 35 bytes for 
Serial data format. 

KBF:
5/2/18: added unconverted uva_data from sensor readout to thinsat serial packet (byte #5, infoTx[5])
        Remaining code untouched from Negeen's latest version (UVA_Final_Code)
*/


#include <Wire.h>
#include "Adafruit_VEML6070.h"

#define SB 4        // Serial Busy line


byte TotDat=38;           //Payload Data 35 bytes size
byte infoTx[38]={0x00};      //Payload packet 50 50 50 Payload(35 bytes)
int dely=0;               //Delay (ms)
int CommStatus=0;     // Comm status on busy line

byte RxByte=0;       // Incoming byte from Serial port
int dly=10;           //delay count
int ACK=0;        //ACK response from ThinSat
int tries=0;          //Tries to resend a package to the ThinsSat
int wait=0;            //Counter to wait for response or leave loop is is to longer
word i;

word uva_data;          //digital counts from the sensor
word temp_h;          //temperoray buffer for uva most significant bit
word temp_l;         //temperoray buffer for uva least significant bit
byte uva_data_h;      //uva most significant bit 
byte uva_data_l;      //uva least significant bit
float intensity_step = 25;     //intesity step count in mW/m^2


word uva_intensity_mapping_table[4] = {561, 1121, 1494, 2055};    //range of digital uva data from the sensor
 
float intensity [5] = {1.5*intensity_step, 4*intensity_step, 7*intensity_step, 9.5*intensity_step, 11.5*intensity_step};     //conversion of uva data to intensity of uva


Adafruit_VEML6070 uv = Adafruit_VEML6070();      //declaring the uva by using the function provided from the Adafruit library for the VEML6070 sensor


void setup() {
  
  Serial.begin(38400);      //Serial baud rate
  pinMode(SB, INPUT);       // Serial Busy Line  
  
  uv.begin(VEML6070_1_T);  // pass in the integration time constant for the uva sensor     
  
  infoTx[0]= 0x50;          //Preamble for serial data packet
  infoTx[1]= 0x50;          //Preamble for serial data packet
  infoTx[2]= 0x50;          //Preamble for serial data packet

  infoTx[3]= 0x01;          //NSL Reserved
  infoTx[4]= 0x01;          //Seq. Count

  Serial.flush();         // Clear the Serial buffer
    
}


void loop() {

  CommStatus=digitalRead(SB);     //serial line status
    while(CommStatus == 1){
      CommStatus=digitalRead(SB);
    }
  
   uva_data = uv.readUV();    //reading uva data from the sensor, function provided by Adafruit lib for VEML6070
   infoTx[5] = uva_data;
  
  
  //converting the digital data to uva intensity
  
  for (i = 0; i < 4; i++)
  {
    if (uva_data <= uva_intensity_mapping_table[i])
    { 
      break;
    }
  }

  word inten = word(intensity[i]);
  
  temp_h = inten;
  temp_l = inten;
  uva_data_h = temp_h >>= 8;
  uva_data_l = temp_l;
  infoTx[6] =  uva_data_h;
  infoTx[7] = uva_data_l;
  
  ClearSerial();
    
  tx();         //Transmit packet
    
  while (Serial.available() == 0){      //if serial line is not available wait
      wait=wait+1;
      delay(10);
      if (wait == 50){ 
        break;
        }   //To avoid infinite loop
  }
  
  wait=0;
    
  ReadACK();    //read the acknowledgement signal from NSL for the serial data transmission
  
  int comp=ACK;
  
  tries=0;      //if there was problem with the serial communication, trying for a few more times
    while ( tries < 3 && comp == 0){
      tx();         //Transmit packet
      tries = tries +1;
    }
   
  delay(500);

}             //end of the loop


//********Function for reading the acknowledgement signal

void ReadACK(){
  ACK=0;
  if (Serial.available() > 0){
   RxByte = Serial.read();
    if(RxByte == 0xAA){
     delay (dly);
     RxByte = Serial.read();
     if(RxByte == 0x05){
       delay (dly);
       RxByte = Serial.read();
       if(RxByte == 0x00){
       ACK=1;
       }
     }
   }  
  }
}


//*************function for serial data transmission

void tx(){
  
  Serial.write(infoTx,TotDat);   //transmite el vector de informacion de totDatos bytes.
  
}



//********************ClearSerial
void ClearSerial() {
  
 while(Serial.available()){  //is there anything to read?
  char getData = Serial.read();  //if yes, read it     
 }
}


