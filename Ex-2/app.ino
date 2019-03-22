#include "event_driven.h"
#include "app.h"
#include "pindefs.h"

int t = 1000;
int ledState = LOW;

void appinit(void){
  Serial.begin(9600);
  pinMode(LED4, OUTPUT);
  timer_set(t);  
  button_listen(KEY1);
  button_listen(KEY2);
}

void button_changed(int p, int v){
  if(v == HIGH){
    Serial.println("Key pressed");
    Serial.println(p);
  }
}

void timer_expired(void){

  if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    Serial.println("Timer expired!");
    // set the LED with the ledState of the variable:
    digitalWrite(LED4, ledState);
    timer_set(t);
}
