#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin();

  Serial.begin(115200);
  
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

  Wire.beginTransmission(53);
  Wire.write(byte(0x03));
  Wire.write(byte(0b00000011));
  Wire.endTransmission();
}

int secondary = 0;
int Xreading = 0;
int Yreading = 0;
int Zreading = 0;
int CONV_Status = 0;
void loop() {

  Wire.requestFrom(53, 7);
  if(7 <= Wire.available()){
    //secondary = Wire.read();
    
    Xreading = Wire.read();
    Xreading = Xreading << 8;
    Xreading |= Wire.read();
    //Serial.print(Xreading);
    //Serial.print(", ");
    
    Yreading = Wire.read();
    Yreading = Yreading << 8;
    Yreading |= Wire.read();
    //Serial.print(Yreading);
    //Serial.print(", ");
    
    Zreading = Wire.read();
    Zreading = Zreading << 8;
    Zreading |= Wire.read();
    //Serial.println(Zreading);
    Serial.println(String(Xreading) + ", " + String(Yreading) + ", " + String(Zreading));
    CONV_Status = Wire.read();
  }
}
