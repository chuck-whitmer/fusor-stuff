void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial3.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial3.available())
  {
    int byte = Serial3.read();
    Serial.write(byte);
  }
  if (Serial.available())
  {
    int byte = Serial.read();
    Serial3.write(byte);
  }
}
