#define SEND_PIN 7

int khz = 38;
unsigned long pulse_term = 1000 / 38;
unsigned int TV_ON[] = {9071, 4432, 625, 501, 624, 504, 623, 1613, 623, 504, 623, 503, 627, 501, 625, 501, 628, 498, 627, 1608, 626, 1609, 627, 502, 624, 1612, 622, 1636, 600, 1635, 599, 1637, 597, 1639, 595, 529, 597, 530, 598, 529, 596, 1640, 593, 531, 596, 532, 595, 532, 594, 530, 597, 1640, 596, 1637, 597, 1640, 595, 532, 594, 1640, 596, 1639, 595, 1641, 594, 1643, 595};
unsigned int Air_ON[] = {9022, 4435, 596, 1617, 620, 501, 621, 502, 620, 500, 620, 500, 620, 500, 619, 503, 618, 1620, 619, 1617, 620, 1616, 619, 1618, 618, 504, 616, 507, 613, 504, 594, 1646, 590, 529, 592, 1644, 590, 533, 592, 530, 592, 529, 591, 530, 589, 532, 589, 532, 590, 1646, 591, 529, 592, 1645, 591, 1646, 589, 1645, 592, 1646, 616, 1622, 616, 1618, 620, 504, 616, 41679, 8994, 2207, 592};
unsigned int Air_Strength[] = {9025, 4406, 622, 1620, 619, 501, 618, 506, 617, 500, 620, 503, 620, 501, 620, 501, 619, 1620, 620, 1618, 616, 1620, 620, 1618, 618, 503, 616, 504, 619, 504, 615, 1621, 617, 504, 593, 1643, 591, 531, 592, 530, 592, 1645, 592, 1645, 592, 529, 590, 530, 591, 1646, 592, 529, 600, 1637, 593, 1644, 592, 530, 591, 530, 592, 1645, 618, 1621, 614, 506, 617, 41685, 8994, 2209, 592};
unsigned int Drive_Mode[] = {9016, 4404, 612, 1640, 584, 496, 620, 488, 620, 496, 612, 496, 608, 500, 608, 496, 612, 1608, 616, 1608, 612, 1616, 608, 1608, 620, 492, 620, 492, 620, 500, 612, 1608, 612, 496, 612, 1616, 612, 1608, 612, 488, 616, 1608, 608, 1620, 604, 500, 608, 504, 584, 1640, 580, 532, 584, 524, 588, 1644, 584, 524, 588, 520, 580, 1640, 588, 1644, 584, 520, 588, 41684, 8988, 2204, 608};
unsigned int CirculatorPan_Strength[] = {9016, 4404, 620, 1608, 612, 500, 612, 500, 612, 500, 612, 500, 612, 500, 612, 500, 612, 1616, 608, 1616, 604, 1620, 612, 1612, 612, 496, 588, 520, 592, 520, 588, 1644, 580, 520, 588, 524, 580, 1644, 580, 1640, 592, 520, 588, 1640, 580, 520, 588, 524, 580, 1640, 588, 1632, 588, 524, 588, 520, 592, 1640, 580, 524, 588, 1632, 588, 1644, 580, 524, 588, 41684, 8988, 2204, 580};
unsigned int CirculatorPan_Rotate[] = {9008, 4400, 612, 1608, 620, 488, 620, 496, 604, 500, 608, 488, 616, 496, 608, 500, 612, 1608, 604, 1640, 592, 1608, 612, 1616, 612, 500, 612, 500, 612, 500, 612, 1616, 604, 504, 580, 1640, 584, 528, 580, 1644, 584, 524, 588, 1644, 580, 520, 588, 520, 592, 1640, 580, 532, 580, 1640, 580, 524, 592, 1632, 588, 520, 588, 1640, 584, 1640, 584, 532, 580, 41684, 8988, 2204, 592};

void custom_delay_usec(unsigned long uSecs)
{
  unsigned long Start = micros();
  unsigned long endMicros = Start + uSecs;
  if (endMicros < Start)
  {
    while (micros() > Start)
    {
    }
  }
  while (micros() < endMicros)
  {
  }
}

void space(unsigned int Time)
{
  digitalWrite(SEND_PIN, LOW);
  if (Time > 0)
  {
    unsigned long Start = micros();
    unsigned long endMicros = Start + Time - 4;
    custom_delay_usec(Time);
  }
}

void mark(unsigned int Time)
{
  unsigned long Start = micros();
  unsigned long endMicros = Start + Time;
  int count = 0;

  while (micros() < endMicros)
  {
    digitalWrite(SEND_PIN, HIGH);
    custom_delay_usec(10);
    digitalWrite(SEND_PIN, LOW);
    custom_delay_usec(9);
    count++;
  }
}

void send_ir(unsigned int *Signal, int Length)
{
  digitalWrite(SEND_PIN, LOW);

  for (unsigned int i = 0; i < Length; i++)
  {
    unsigned long Start = micros();
    if (i & 1)
      space(Signal[i]);
    else
      mark(Signal[i]);
  }

  digitalWrite(SEND_PIN, LOW);

  delay(200);
  digitalWrite(1, LOW);
}

void setup()
{
  pinMode(SEND_PIN, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
}

void loop()
{
  if (digitalRead(3) == 0)
  {
    digitalWrite(1, HIGH);
    int Length = sizeof(TV_ON) / sizeof(TV_ON[0]);
    send_ir(TV_ON, Length);
    digitalWrite(1, LOW);
  }
  else if (digitalRead(4) == 0)
  {
    digitalWrite(2, HIGH);
    int Length = sizeof(Air_ON) / sizeof(Air_ON[0]);
    send_ir(Air_ON, Length);
    digitalWrite(2, LOW);
  }
  else if (digitalRead(5) == 0)
  {
    digitalWrite(1, HIGH);
    digitalWrite(2, HIGH);
    int Length = sizeof(Air_Strength) / sizeof(Air_Strength[0]);
    send_ir(Air_Strength, Length);
    digitalWrite(1, LOW);
    digitalWrite(2, LOW);
  }
}