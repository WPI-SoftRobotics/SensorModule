#include <Wire.h>
#include <Arduino.h>
#include "HX711.h"

struct HallEffectReadings {
  signed long x;
  signed long y;
  signed long z;
};

struct ForceReadings {
  signed long Fx;
  signed long Fy;
  signed long Fz;
};

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;

int secondary = 0;
int Xreading = 0;
int Yreading = 0;
long Zreading = 0;
int CONV_Status = 0;
unsigned long time = 0;
signed long x_offset = 0;
signed long y_offset = 0;
signed long z_offset = 0;

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

  HallEffectReadings Offset = read_average(1000);

  x_offset = Offset.x;
  y_offset = Offset.y;
  z_offset = Offset.z;
}

int Cxy = .015;
int Cxz = -.071;
int Cyx = .016;
int Cyz = -.154;
int Czx = .283;
int Czy = -.210;
int Cfx = 4.226;
int Cfy = -15.422;
int Cfz = 9.264;

signed long Fx = 0;
signed long Fy = 0;
signed long Fz = 0;
signed long F = 0;

ForceReadings get_3Dforce(signed long x,signed long y,signed long z){
  x = x - x_offset;
  y = y - y_offset;
  z = z - z_offset;

  x = x - Cxy*y - Cxz*z;
  y = y - Cyx*x - Cyz*z;
  z = z - Czx*x - Czy*y;

  Fx = x*Cfx;
  Fy = y*Cfy;
  Fz = z*Cfz;

  if(abs(Fx) < 40){
    Fx = 0;
  } 

  if(abs(Fy) < 30){
    Fy = 0;
  } 

  if(abs(Fz) < 30){
    Fz = 0;
  } 

  ForceReadings forces;
  forces.Fx = Fx;
  forces.Fy = Fy;
  forces.Fz = Fz;
  return forces;
}

signed long get_force(signed long x,signed long y,signed long z){

  //Serial.println(String(x) + ", " + String(y) + ", " + String(z));

  x = x - x_offset;
  y = y - y_offset;
  z = z - z_offset;

  //Serial.println(String(x) + ", " + String(y) + ", " + String(z));

  x = x - Cxy*y - Cxz*z;
  y = y - Cyx*x - Cyz*z;
  z = z - Czx*x - Czy*y;

  //Serial.println(String(x) + ", " + String(y) + ", " + String(z));

  Fx = x*Cfx;
  Fy = y*Cfy;
  Fz = z*Cfz;

  if(abs(Fx) < 40){
    Fx = 0;
  } 

  if(abs(Fy) < 30){
    Fy = 0;
  } 

  if(abs(Fz) < 30){
    Fz = 0;
  } 

  //Serial.println(String(Fx) + ", " + String(Fy) + ", " + String(Fz));

  F = sqrt((Fx*Fx) + (Fy*Fy) + (Fz*Fz));

  return F;
}

HallEffectReadings avgs;
ForceReadings Forces;

void loop() {
  while(time < 30000){
    avgs = read_average(200);
    Xreading = avgs.x;
    Yreading = avgs.y;
    Zreading = avgs.z;

    F = get_force(Xreading,Yreading,Zreading);
    
    Forces = get_3Dforce(Xreading,Yreading,Zreading);

    Fx = Forces.Fx;
    Fy = Forces.Fy;
    Fz = Forces.Fz;

    time = millis();
    Serial.println(String(time) + ", " + String(Fx) + ", " + String(Fy) + ", " + String(Fz) + ", " + String(F) + ", " + scale.get_units(1));
  }
}