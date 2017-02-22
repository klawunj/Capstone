#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

int msg[1];
RF24 radio(8,53);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int LED1 = 3;
int SW1 = 7;
int i = 0;

void setup() {
  Serial.begin(9600);
  radio.begin();
  //if receiving
//  radio.openReadingPipe(1,pipe);
//  radio.startListening();

  //if transmitting

  radio.openWritingPipe(pipe);
  
  pinMode(LED1,OUTPUT);

}

void loop() {
  transmit();
  //receive();

}


static inline void receive(){
  if (radio.available()){

    bool done = false;

    while (radio.available()){

      radio.read(msg, 1);

      Serial.println(msg[0]);

      if (msg[0] == 111){
        delay(10);
        digitalWrite(LED1, HIGH);
       }

      else {
        digitalWrite(LED1, LOW);
      }

      delay(10);

      msg[0] == 9;
     }
   }

  else{
    Serial.println("No radio available");

    digitalWrite(LED1, LOW);
  }
}


static inline void transmit(){
 if(Serial.available()){ 
   if(i < 10){
    Serial.println("Sending");
    msg[0] = 111;
    radio.write(msg, 1);
    i = i+1;
   }
 }
}
