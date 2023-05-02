#include <Wire.h>
#include <Arduino.h>
#include "HX711.h"

struct HallEffectReadings {
  signed long x;
  signed long y;
  signed long z;
};

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;

void setup() {
// put your setup code here, to run once:
  Wire.begin();

  Serial.begin(115200);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
            
  scale.set_scale(-727.598);  // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

  //Device Config 1 Register
  Wire.beginTransmission(53);
  Wire.write(byte(0x00));
  Wire.write(byte(0b00010101));
  Wire.endTransmission();

  //Device Config 2 Register
  Wire.beginTransmission(53);
  Wire.write(byte(0x01));
  Wire.write(byte(0b00000010));
  Wire.endTransmission();

  //Sensor Config 1 Register
  Wire.beginTransmission(53);
  Wire.write(byte(0x02));
  Wire.write(byte(0b01110000));
  Wire.endTransmission();
 
  //Sensor Config 2 Register
  Wire.beginTransmission(53);
  Wire.write(byte(0x03));
  Wire.write(byte(0b00000011));
  Wire.endTransmission();
}

int secondary = 0;
int Xreading = 0;
int Yreading = 0;
long Zreading = 0;
int CONV_Status = 0;
unsigned long time = 0;

HallEffectReadings read_average(byte times) {
	signed long sumX = 0;
  signed long sumY = 0;
  signed long sumZ = 0;

	for (byte i = 0; i < times; i++) {
    Wire.requestFrom(53, 7);
    if(7 <= Wire.available()){
      Xreading = Wire.read();
      Xreading = Xreading << 8;
      Xreading |= Wire.read();
      Xreading = Xreading*5;

      Yreading = Wire.read();
      Yreading = Yreading << 8;
      Yreading |= Wire.read();
      
      Zreading = Wire.read();
      Zreading = Zreading << 8;
      Zreading |= Wire.read();

      //Serial.println(String(time) + ", " + String(Xreading) + ", " + String(Yreading) + ", " + String(Zreading));    
      
      sumX += Xreading;
      sumY += Yreading;
      sumZ += Zreading;

      CONV_Status = Wire.read();
      delay(0);
    }
	}
  HallEffectReadings result;
  result.x = sumX/times;
  result.y = sumY/times;
  result.z = sumZ/times;
  return result;
}

HallEffectReadings avgs;

void loop() {
  while(time < 15000){
    avgs = read_average(200);
    Xreading = avgs.x;
    Yreading = avgs.y;
    Zreading = avgs.z;

    time = millis();
    Serial.println(String(time) + ", " + String(Xreading) + ", " + String(Yreading) + ", " + String(Zreading) + ", " + scale.get_units(1));
  }
}
