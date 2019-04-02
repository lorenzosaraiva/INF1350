#include "pindefs.h"
#include <avr/sleep.h>
#include <avr/power.h>

byte state = HIGH;
volatile int counter = 0;

unsigned long lastK1 = 0;
unsigned long lastK2 = -1000;

int lastK1Stare = 0;
int lastK2State = 0;

void timerSetup () {
   TIMSK2 = (TIMSK2 & B11111110) | 0x01;
   TCCR2B = (TCCR2B & B11111000) | 0x07;
}

void setup() {
   pinMode(LED1, OUTPUT); digitalWrite(LED1, state);
   pinMode(LED2, OUTPUT); digitalWrite(LED2, state);
   pinMode(LED3, OUTPUT); digitalWrite(LED3, state);
   pinMode(LED4, OUTPUT); digitalWrite(LED4, state);
   pinMode(KEY1, INPUT_PULLUP);
   pinMode(KEY2, INPUT_PULLUP);
   timerSetup();
}
 
void loop() {
  enterSleep();
  if (counter>50) {
    state = !state;
    digitalWrite(LED1, state);
    counter = 0;
  }

  int k1 = digitalRead(KEY1);
  int k2 = digitalRead(KEY2);

  if(!k1){
     lastK1 = millis();  
  }

  if(!k2){
     lastK2 = millis();  
  }

  if(abs(lastK1 - lastK2) <= 500){
      while(1);  
  }
}

void enterSleep(void){     
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);     
  sleep_enable();     
  sleep_mode();     /* The program  will continue  from here. */  
/* First thing to do is disable  sleep.  */  
  sleep_disable(); 
}

ISR(TIMER2_OVF_vect){
   counter++;
}

