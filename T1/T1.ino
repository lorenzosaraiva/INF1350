/* Define shift register pins used for seven segment display */
#define LATCH_DIO 4
#define CLK_DIO 7
#define DATA_DIO 8
#include "pindefs.h"

/* Segment byte maps for numbers 0 to 9 */
const byte SEGMENT_MAP[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90};
/* Byte maps to select digit 1 to 4 */
const byte SEGMENT_SELECT[] = {0xF1,0xF2,0xF4,0xF8};

int h = 0, hh = 0, m = 0, mm = 0;

int minute = 0;
int hour = 0;
int alarmHour = 22;
int alarmMinute = 30;

int lastBlink = 0;

int minuteDuration = 1000; /* Duração do minuto em ms. Trocar para facilitar testes. */

unsigned long previousMillis = 0;
unsigned long lastActivity = 0;

int lastStates[3] = {0, 0, 0};

int activeState = 0;


void setup() {
    /* Set DIO pins to outputs */
  pinMode(LATCH_DIO,OUTPUT);
  pinMode(CLK_DIO,OUTPUT);
  pinMode(DATA_DIO,OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(KEY1, INPUT_PULLUP);
  pinMode(KEY2, INPUT_PULLUP);
  pinMode(KEY3, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
    
   updateLeds();
   updateSegments();
   checkButton(KEY1);   
   checkButton(KEY2);
   checkButton(KEY3);
   checkAlarm();
   checkActivity();
}

void checkActivity(){
   if((activeState == 3) || (activeState == 4) || (activeState == 5) || (activeState == 6)){
      unsigned long currentMillis = millis();
      if(currentMillis - lastActivity >= 10000){
          activeState = 0;
      }
   }  
}

void checkAlarm(){
  if(activeState == 1){
     if ((hour == alarmHour) && (minute == alarmMinute)){
        // sound alarm
     } 
  }  
}

void updateLeds(){
  switch(activeState){
      case 0:
        digitalWrite(LED4, LOW);
        digitalWrite(LED3, HIGH);
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, HIGH);
        break;
      case 1:
        digitalWrite(LED4, HIGH);
        digitalWrite(LED3, LOW);
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, HIGH);
        break;
      case 2:
        digitalWrite(LED4, HIGH);
        digitalWrite(LED3, HIGH);
        digitalWrite(LED2, LOW);
        digitalWrite(LED1, HIGH);
        break;
      case 3:
        digitalWrite(LED4, HIGH);
        digitalWrite(LED3, HIGH);
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, LOW);
        break;
      case 4:
        digitalWrite(LED4, HIGH);
        digitalWrite(LED3, HIGH);
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, LOW); 
        break;
      case 5:
        digitalWrite(LED4, HIGH);
        digitalWrite(LED3, HIGH);
        digitalWrite(LED2, LOW);
        digitalWrite(LED1, LOW); 
        break; 
      case 6:
        digitalWrite(LED4, HIGH);
        digitalWrite(LED3, HIGH);
        digitalWrite(LED2, LOW);
        digitalWrite(LED1, LOW); 
        break;  
   }
}

void checkButton(int pin){
  int buttonState = digitalRead(pin);
  int i = 0;

  switch(pin){
    case KEY1:
        i = 0;
        break;
    case KEY2:
        i = 1;
        break;
    case KEY3:
        i = 2;
        break;
  }

  if (buttonState != lastStates[i]){
    if(buttonState == LOW){
       lastActivity = millis();
       if(pin == KEY3){
          activeState++;
          if(activeState == 7){
             activeState = 0;
          }
       }

       if(pin == KEY2){
          if(activeState == 3){
              increaseHour(false);
          }
          if(activeState == 4){
              increaseMinute(false);
          }
          if(activeState == 5){
              increaseHour(true);
          }
          if(activeState == 6){
              increaseMinute(true);
          }
       }
    }
    delay(50);
    lastStates[i] = buttonState;
  }
}

void increaseHour(bool isAlarm){
  if(isAlarm){
    alarmHour++;
    if(alarmHour == 24){
      alarmHour = 0; 
    }
  }else{
    hour++;
    if(hour == 24){
      hour = 0;
    }  
  }
}

void increaseMinute(bool isAlarm){

  if(isAlarm){
    alarmMinute++;
    if(alarmMinute == 60){
      alarmMinute = 0;
      increaseHour(isAlarm);
    }
  }else{
    minute++;
    if(minute == 60){
      minute = 0;
      increaseHour(isAlarm);
    }
  }
}
void updateSegments(){

   WriteNumberToSegment(0 , h);
   WriteNumberToSegment(1 , hh);
   WriteNumberToSegment(2 , m);
   WriteNumberToSegment(3 , mm);
   
   unsigned long currentMillis = millis();
    

    if (currentMillis - previousMillis >= minuteDuration) {
      // One minute passed
      previousMillis = currentMillis;

      minute++;
      if (minute == 60){
        minute = 0;
        hour++;   
      }

      if (hour == 24){
        hour = 0;
      }

      int tempH = 0;
      int tempM = 0;
      if(activeState == 2 || activeState == 5 || activeState == 6){
          tempH = alarmHour;
          tempM = alarmMinute;
      }else{
          tempH = hour;
          tempM = minute;
      }

      float vhour = tempH/10;
      h = floor(vhour);
      hh = tempH % 10;

      float vminute = tempM/10;
      m = floor(vminute);
      mm = tempM % 10;
    }
}

void WriteNumberToSegment(byte Segment, byte Value) {

    digitalWrite(LATCH_DIO,LOW);
    shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_MAP[Value]);
    shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_SELECT[Segment] );
    digitalWrite(LATCH_DIO,HIGH);
}
