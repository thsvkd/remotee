unsigned int pinIR = 6;         // IR sensor pin
unsigned int currentPulse = 0;  // Pulse index
unsigned long pulses[100][2];   // Array of high and low pulse
unsigned long maxPulse = 50000; // Maximum listen duration (65 msec)

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
}

void loop(void)
{
  unsigned long highPulse = 0; // Pulse length
  unsigned long lowPulse = 0;
  unsigned long currTime = micros();

  while (digitalRead(pinIR))
  {
    highPulse = micros() - currTime;
    if ((highPulse >= maxPulse) && (currentPulse != 0))
    {
      printPulses();
      currentPulse = 0;
    }
  }
  pulses[currentPulse][0] = highPulse;
  currTime = micros();

  while (!digitalRead(pinIR))
  {
    lowPulse = micros() - currTime;
  }
  pulses[currentPulse][1] = lowPulse;
  currentPulse++;
}

void printPulses(void)
{
  for (int i = 0; i < currentPulse - 1; i++)
  {
    Serial.print(pulses[i][1], DEC);
    Serial.print(",-");
    Serial.print(pulses[i + 1][0], DEC);
    Serial.print(",");
  }
  Serial.println(pulses[currentPulse - 1][1], DEC);
}
