#define LED1       10
#define LED2       11
#define LED3       12
#define LED4       13
#define BUZZ        3
#define KEY1       A1
#define KEY2       A2
#define KEY3       A3
#define POT        A0
#define LATCH_DIO 4
#define CLK_DIO 7
#define DATA_DIO 8

/* Segment byte maps for numbers 0 to 9 */
const byte SEGMENT_MAP[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90};
/* Byte maps to select digit 1 to 4 */
const byte SEGMENT_SELECT[] = {0xF1,0xF2,0xF4,0xF8};

/* Variaveis com os valores de cada segmento do display */
int h = 0, hh = 0, m = 0, mm = 0;

/* Variaves que guardam a hora */
int minute = 0;
int hour = 0;
int minuteDuration = 1000;

const byte numLEDs = 4;
byte ledPin[numLEDs] = {10, 11, 12, 13};
unsigned long LEDinterval[numLEDs] = {200, 400, 600, 800};
unsigned long prevLEDmillis[numLEDs] = {0, 0, 0, 0};

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize] = {0};
int newFlashInterval = 0;


unsigned long curMillis;

unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 1000;


//MEU

boolean check = false;

const int leftButtonPin = A1;
const int midButtonPin = A2;
const int rightButtonPin = A3;




 
// Variables will change:
int leftButtonState = 0;         // current state of the button
int lastLeftButtonState = 0;     // previous state of the button

int midButtonState = 0;         // current state of the button
int lastMidButtonState = 0;     // previous state of the button

int rightButtonState = 0;         // current state of the button
int lastRightButtonState = 0;     // previous state of the button

int value = -1;

unsigned long previousMillis = 0;

//=============

void setup() {
  Serial.begin(9600);
  pinMode(LATCH_DIO,OUTPUT);
  pinMode(CLK_DIO,OUTPUT);
  pinMode(DATA_DIO,OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(midButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  digitalWrite(BUZZ,HIGH);       



  
    // flash LEDs so we know we are alive
  for (byte n = 0; n < numLEDs; n++) {
     pinMode(ledPin[n], OUTPUT);
     digitalWrite(ledPin[n], HIGH);
  }
  delay(500); // delay() is OK in setup as it only happens once
  
  for (byte n = 0; n < numLEDs; n++) {
     digitalWrite(ledPin[n], LOW);
  }
  
  
    // tell the PC we are ready
  Serial.println("<Arduino is ready>");
}

//=============

void loop() {
  curMillis = millis();
  getDataFromPC();
  updateFlashInterval();
  replyToPC();
  flashLEDs();
  updateSegments();



  //LEFT BUTTON            
  leftButtonState = digitalRead(leftButtonPin);
  
  if (leftButtonState != lastLeftButtonState) {
    if (leftButtonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      value = 0;     
    }

    // Delay a little bit to avoid bouncing
    delay(50);
  }
  
  lastLeftButtonState = leftButtonState;

  //END LEFT

  //MID BUTTONr

    midButtonState = digitalRead(midButtonPin);
  
  if (midButtonState != lastMidButtonState) {
    if (midButtonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      value = 1;      
    }

    // Delay a little bit to avoid bouncing
    delay(50);
  }
  
  lastMidButtonState = midButtonState;

  //END MID

  //RIGHT BUTTON
  rightButtonState = digitalRead(rightButtonPin);
  
  if (rightButtonState != lastRightButtonState) {
    if (rightButtonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      value = 2;      
    }

    // Delay a little bit to avoid bouncing
    delay(50);
  }
  
  lastRightButtonState = rightButtonState;
}

//=============

void getDataFromPC() {

    // receive data from PC and save it into inputBuffer
    
  if(Serial.available() > 0) {

    char x = Serial.read();

      // the order of these IF clauses is significant
      
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  }
}

//=============
 
void parseData() {

    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
  strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
  
  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  newFlashInterval = atoi(strtokIndx);     // convert this part to an integer
  
  strtokIndx = strtok(NULL, ","); 
}

//=============

void replyToPC() {

  boolean isShot = false;

  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<");

    if(check){
        Serial.print("timeout");
        check = false;
    }else{
      if(value == 0){
        Serial.print("left");
      }
      if(value == 1){
        Serial.print("tiro");
        isShot = true;
      }
      if(value == 2){
        Serial.print("right");
      }
      if(value == -1){
        Serial.print("nothing");
      }
    }
    value = -1;
    Serial.println(">");
    if (isShot){
      digitalWrite(BUZZ,LOW);
      delay(50);
      digitalWrite(BUZZ,HIGH);
    }
  }
}

//============

void updateFlashInterval() {

   // this illustrates using different inputs to call different functions
  if (strcmp(messageFromPC, "LED1") == 0) {
     updateLED1();
  }
  
  if (strcmp(messageFromPC, "LED2") == 0) {
     updateLED2();
  }
}

//=============

void updateLED1() {

  if (newFlashInterval > 100) {
    LEDinterval[0] = newFlashInterval;
  }
}

//=============

void updateLED2() {

  if (newFlashInterval > 100) {
    LEDinterval[1] = newFlashInterval;
  }
}

//=============

void flashLEDs() {

  for (byte n = 0; n < numLEDs; n++) {
    if (curMillis - prevLEDmillis[n] >= LEDinterval[n]) {
       prevLEDmillis[n] += LEDinterval[n];
       digitalWrite( ledPin[n], ! digitalRead( ledPin[n]) );
    }
  }
}

//=============

void updateSegments(){
      

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

      float vhour = hour/10;
      h = floor(vhour);
      hh = hour % 10;

      float vminute = minute/10;
      m = floor(vminute);
      mm = minute % 10;

      if(hh == 1){
        check = true;
        minute = 0;
        hour = 0;  
      }

    }

    WriteNumberToSegment(0 , h);
    WriteNumberToSegment(1 , hh);
    WriteNumberToSegment(2 , m);
    WriteNumberToSegment(3 , mm);
}

//==============

void WriteNumberToSegment(byte Segment, byte Value) {
      digitalWrite(LATCH_DIO,LOW);
      shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_MAP[Value]);
      shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_SELECT[Segment] );
      digitalWrite(LATCH_DIO,HIGH); 
}
