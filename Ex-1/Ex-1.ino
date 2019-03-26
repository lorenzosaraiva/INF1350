/*
  State change detection (edge detection)

  Often, you don't need to know the state of a digital input all the time, but
  you just need to know when the input changes from one state to another.
  For example, you want to know when a button goes from OFF to ON. This is called
  state change detection, or edge detection.

  This example shows how to detect when a button or button changes from off to on
  and on to off.

  The circuit:
  - pushbutton attached to pin 2 from +5V
  - 10 kilohm resistor attached to pin 2 from ground
  - LED attached from pin 13 to ground (or use the built-in LED on most
    Arduino boards)

  created  27 Sep 2005
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/fastButtonStateChange
*/

// this constant won't change:
const int  fastButtonPin = A1;    // the pin that the pushbutton is attached to
const int  slowButtonPin = A2;

const int ledPin = 13;       // the pin that the LED is attached to

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int fastButtonState = 0;         // current state of the button
int lastFastButtonState = 0;     // previous state of the button

int slowButtonState = 0;         // current state of the button
int lastSlowButtonState = 0;     // previous state of the button

void setup() {
  // initialize the button pin as a input:
  pinMode(fastButtonPin, INPUT_PULLUP);
  pinMode(slowButtonPin, INPUT_PULLUP);

  // initialize the LED as an output:
  pinMode(ledPin, OUTPUT);
  // initialize serial communication:
  Serial.begin(9600);
}

int t = 1000;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long lastFastButtonClick = 0;
unsigned long lastSlowButtonClick = 0;
int ledState = LOW;             // ledState used to set the LED

void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= t) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
  
  // read the pushbutton input pin:
  fastButtonState = digitalRead(fastButtonPin);
  

  // compare the fastButtonState to its previous state
  if (fastButtonState != lastFastButtonState) {
    // if the state has changed, increment the counter
    if (fastButtonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      lastFastButtonClick = millis();
      t = t - 100;
      if (t <= 0){
        t = 0;
      }
      buttonPushCounter++;
      Serial.println("Delay:");
      Serial.println(t);
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }

    
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  
  lastFastButtonState = fastButtonState;

  slowButtonState = digitalRead(slowButtonPin);
  

  // compare the fastButtonState to its previous state
  if (slowButtonState != lastSlowButtonState) {
    // if the state has changed, increment the counter
    if (slowButtonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      lastSlowButtonClick = millis();
      t = t + 100;
      if (t >= 1000){
        t = 1000;
      }
      buttonPushCounter++;
      Serial.println("Delay:");
      Serial.println(t);
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    delay(50);
  }
  lastSlowButtonState = slowButtonState;
  
  if (abs(lastFastButtonClick - lastSlowButtonClick) < 500){
      digitalWrite(ledPin, LOW); // Turn off the LED
      Serial.println("LIGHTS STOP");
      while(true);
    
  }

  /*if ((digitalRead (fastButtonPin)==LOW) && (digitalRead(slowButtonPin)==LOW)){
      digitalWrite(ledPin, LOW); // Turn off the LED
      Serial.println("LIGHTS OFF");
      while(true);
  }*/
}
