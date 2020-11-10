void setup()
{
  Serial.begin(9600);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
}

void loop()
{
  int pin3 = digitalRead(3);
  int pin4 = digitalRead(4);
  int pin5 = digitalRead(5);

  Serial.print(pin3);
  Serial.print(pin4);
  Serial.println(pin5);
}