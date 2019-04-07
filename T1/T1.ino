/* Define shift register pins used for seven segment display */
#define LATCH_DIO 4
#define CLK_DIO 7
#define DATA_DIO 8
#include "pindefs.h"

/* Segment byte maps for numbers 0 to 9 */
const byte SEGMENT_MAP[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90};
/* Byte maps to select digit 1 to 4 */
const byte SEGMENT_SELECT[] = {0xF1,0xF2,0xF4,0xF8};

/* Variaveis com os valores de cada segmento do display */
int h = 0, hh = 0, m = 0, mm = 0;

/* Variaves que guardam a hora */
int minute = 0;
int hour = 0;
int alarmHour = 0;
int alarmMinute = 3;

bool blinkState = true;
bool alarmOn = false;

int minuteDuration = 60000; /* Duração do minuto em ms. Trocar para facilitar testes. */
int snoozeDuration = 10; /* Duração do snooze em minutos */

unsigned long previousMillis = 0;
unsigned long lastActivity = 0;
unsigned long lastFirstKeyClick = 0;
unsigned long lastThirdKeyClick = -1000;
unsigned long lastBlink = -1000;

int lastStates[3] = {0, 0, 0};

/* Variavel que representa o estado do relógio */
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
  pinMode(BUZZ, OUTPUT);
  pinMode(KEY1, INPUT_PULLUP);
  pinMode(KEY2, INPUT_PULLUP);
  pinMode(KEY3, INPUT_PULLUP);
  digitalWrite(BUZZ,HIGH);       
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

/* Função que checa se o relógio está inativo há 10 segundos nos modos de mudar horário*/

void checkActivity(){
   if((activeState == 3) || (activeState == 4) || (activeState == 5) || (activeState == 6)){
      unsigned long currentMillis = millis();
      if(currentMillis - lastActivity >= 10000){
          activeState = 0;
      }
   }  
}

/* Função que checa o alarme */

void checkAlarm(){
  if(activeState == 1){
     if ((hour == alarmHour) && (minute == alarmMinute)){
            digitalWrite(BUZZ,LOW);
            alarmOn = true;
     } 
  }  
}

/* Função para ligar os LEDs de acordo com o estado */
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

/* Função que checa e administra o clique */

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

  /* Verifica o clique simultaneo dos botões 1 e 3 */
  if(!digitalRead(KEY1) && !digitalRead(KEY3)){
     activeState = 0;
     delay(50);
     return;  
  }

  if (buttonState != lastStates[i]){
    if(buttonState == LOW){
       lastActivity = millis();

       /* Lida com clique no botão 1 */
       if(pin == KEY1){

          /* Para o alarme */
          lastFirstKeyClick = millis();
          alarmOn = false;
          digitalWrite(BUZZ, HIGH);
       }

       /* Lida com clique no botão 2 */
       if(pin == KEY2){
          if((activeState == 1) && alarmOn){

              /* Ativa a soneca */
              for(int i = 0; i < snoozeDuration; i++){
                  increaseMinute(true);
              }
              digitalWrite(BUZZ, HIGH);
          }
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

       /* Lida com o clique do botão 3 */
       if(pin == KEY3){
          activeState++;
          lastThirdKeyClick = millis();     
          if(activeState == 7){
             activeState = 0;
          }
       }
    }
    delay(50);
    lastStates[i] = buttonState;
  }
}

/* Aumenta em 1 a hora. Boolean diz se é do alarme ou não */
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

/* Aumenta em 1 o minuto. Boolean diz se é do alarme ou não */

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

/* Atualiza os segmentos do display */
void updateSegments(){

   unsigned long now = millis();
   if(activeState == 3 || activeState == 5){
      if (now - lastBlink >= 500){
         lastBlink = now;
         blinkState = !blinkState;
      }
      if(blinkState){
        WriteNumberToSegment(0 , h);
        WriteNumberToSegment(1 , hh);
      } 
      WriteNumberToSegment(2 , m);
      WriteNumberToSegment(3 , mm);
      
   }else if (activeState == 4 || activeState == 6){
      unsigned long now = millis();
      if (abs(now - lastBlink) >= 500){
         lastBlink = now;
         blinkState = !blinkState;
         Serial.println(blinkState);
      }
      if(blinkState){
        WriteNumberToSegment(2 , m);
        WriteNumberToSegment(3 , mm);
      } 
      WriteNumberToSegment(0 , h);
      WriteNumberToSegment(1 , hh);
   }else{
      WriteNumberToSegment(0 , h);
      WriteNumberToSegment(1 , hh);
      WriteNumberToSegment(2 , m);
      WriteNumberToSegment(3 , mm);
   }
   
  

   unsigned long currentMillis = millis();
    
  /* Registra o tempo passando */
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
