#include "event_driven.h"
#include "app.h"
#include "pindefs.h"

//Button Variables
int keyArray[3];
int lastStates[3] = {0, 0, 0};
int addedKeys = 0;

//Timer Variables
int timeLeft = 0;
int timerSet = 0;
int startTime = 0;
int lastCheck = 0;

void button_listen(int pin){
  
  keyArray[addedKeys] = pin;
  addedKeys++;
  pinMode(pin, INPUT_PULLUP);

}

void timer_set(int ms){
  
  startTime = millis();
  timerSet = 1;
  timeLeft = ms;
  lastCheck = startTime;
  
}

void button_changed(int pin, int v);
void timer_expired(void);

void setup() {
  // put your setup code here, to run once:
  appinit();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < addedKeys; i++){
    
      int buttonState = digitalRead(keyArray[i]);

      if (buttonState != lastStates[i]){
        
          button_changed(keyArray[i], buttonState);
          lastStates[i] = buttonState;
          delay(50);
      }
  }

  if (timerSet){
      int currentTime = millis();
      int timeElapsed = currentTime - lastCheck;
      lastCheck = currentTime;
      timeLeft = timeLeft - timeElapsed;

      if (timeLeft <= 0){
          timerSet = 0;
          timer_expired();
      }
  }
}
