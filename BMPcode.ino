#include"BMP.h"


void setup() {
  // put your setup code here, to run once:
  Initialize();
}

void loop() {
  Compute();
  Serial.println(Temperature()/10.0);
  Serial.println(Pressure()/100.0);
  delay(2000);
  // put your main code here, to run repeatedly:

}
