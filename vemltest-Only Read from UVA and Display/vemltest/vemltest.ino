
/*Code used to read the digital counts from the uva sensor, and display it on the Arduino Serial monitor window. To open the
Serial monitor, click on Tools and Serial monitor. The baud rate should be 38400.
Code is provided by Adafruit for the VEML6070 sensor.
 */


#include <Wire.h>
#include "Adafruit_VEML6070.h"

Adafruit_VEML6070 uv = Adafruit_VEML6070();

word uv_data;

void setup() {
  Serial.begin(38400);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("VEML6070 Test");
  uv.begin(VEML6070_1_T);  // pass in the integration time constant
  Serial.println("Reading");
}


void loop() {

  uv_data = uv.readUV();
 
  Serial.print("UV light level: "); 
  Serial.println(uv_data);
  
  delay(10000);
}
