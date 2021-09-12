#include <Arduino.h>
#include "FastTrig.h"
#include "LinearAlgebra.h"
#include "SinFit60Hz.h"

#define TESTING

void setup() {
  Serial.begin(9600);
  FastTrig::Test();
  //LinearAlgebra::TestCholesky();
  SinFit60Hz::TestFit();
}

void loop() {
  // put your main code here, to run repeatedly:
}
