#include <Arduino.h>
#include "FastTrig.h"
#include "LinearAlgebra.h"

void setup() {
  Serial.begin(9600);
  FastTrig::Test();
  LinearAlgebra::TestCholesky();
}

void loop() {
  // put your main code here, to run repeatedly:

}
