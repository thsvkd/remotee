#include <IRremote.h>

int RECV_PIN = 5;

unsigned int ON[] = {0xA, 0x2,0x5, 0xD};
unsigned int OFF[]= {0xE, 0x2,0x1, 0xD};

IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results;

void send() 
{
  if (Serial.read() != -1) {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(ON, 1000, 65000); // Sony TV power code
      delay(40);
    }
  }
}

void read()
{
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
}

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  read();
  //send();
}
