
#include <stdio.h>
#include <stdlib.h>
#include <SoftwareSerial.h>

// Digitals
#define BTPower 2
#define ThrottleUP 3
#define ThrottleDOWN 4
#define ThrottleON 5
#define BTIndicator 6
#define ToggleIndicator 7

// Analogs
#define AnalogOut A1
#define Indicator1 A3
#define Indicator2 A4
#define Indicator3 A5
#define Indicator4 A6
#define Indicator5 A7

// Constants
#define BluetoothSerialSpeed 9600

// GLOBALS
char CONNECTION_UP = 0;
char BT_RESET = 1;
char DOWN_PRESSED = 0;
char DOWN_PRESSED_LIMIT = 30;
unsigned int THROTTLE = 0;
unsigned int TOGGLE = 0;
unsigned int ZERO = 0;
unsigned int GAS = 0;
int BUTTONSTATES_OLD[] = {0,0,0,0,0,0,0,0};
int BUTTONSTATES_NEW[] = {0,0,0,0,0,0,0,0};
int BUTTON_PINS[] = {3, 4, 5};
int LEN_BUTTON_PINS = 3;
int INDICATOR_PINS[] = {Indicator1,Indicator2,Indicator3,Indicator4,Indicator5};
int AUDIO_COMMANDS[6];
char MSG[5];

//Instanced
SoftwareSerial BTSerial(11, 12); //RX | TX



void setup() {
    //Digital pins - declare
    pinMode(BTPower, OUTPUT);
    pinMode(BTIndicator, OUTPUT);
    pinMode(ToggleIndicator, OUTPUT);
    pinMode(ThrottleUP, INPUT);
    pinMode(ThrottleDOWN, INPUT);
    pinMode(ThrottleON, INPUT);
    
    //Digital pins - setup
    digitalWrite(BTPower, HIGH);
    digitalWrite(BTIndicator, LOW);
    digitalWrite(ToggleIndicator, LOW);
    
    // internal pull-up
    digitalWrite(ThrottleUP, HIGH);
    digitalWrite(ThrottleDOWN, HIGH);
    digitalWrite(ThrottleON, HIGH);

    //Analog pins - declare
    pinMode(AnalogOut, OUTPUT);
    pinMode(Indicator1, OUTPUT);
    pinMode(Indicator2, OUTPUT);
    pinMode(Indicator3, OUTPUT);
    pinMode(Indicator4, OUTPUT);
    pinMode(Indicator5, OUTPUT);

    //Analog pins - setup
    analogWrite(AnalogOut, 0);

    BTSerial.begin(BluetoothSerialSpeed);
}


// MAINLOOP
void loop() {
    //digitalWrite(ToggleIndicator, HIGH);
    char cc;
    while (BTSerial.available()) {
      cc = BTSerial.read();
      if (cc == 'X'){
        errorStop();
      }
    }
    
    //poll buttons
    unsigned int i = 0;
    for(i=3; i<6; i++){
      BUTTONSTATES_OLD[i] = BUTTONSTATES_NEW[i];
      BUTTONSTATES_NEW[i] = getDigitalInput(i);
    }
    
    //parse press-events
    parseEvents();
    
    //parse & send Throttle level
    sendThrottle();


    if(TOGGLE){
      digitalWrite(ToggleIndicator, HIGH);
    }
    else{
      digitalWrite(ToggleIndicator, LOW);
    }
      
    delay(60);
}


//Send over Bluetooth
void sendThrottle(){
  unsigned char c = 0;
  unsigned char ic = 0;
  if (TOGGLE){
    c = THROTTLE;
  }
  ic = ~c;
  BTSerial.write('T');
  BTSerial.write('=');
  BTSerial.write(c);
  BTSerial.write('C');
  BTSerial.write(ic);
}


void parseEvents(){
  if(BUTTONSTATES_NEW[ThrottleUP] && (!BUTTONSTATES_OLD[ThrottleUP])){
    THROTTLE += 12;
    if(THROTTLE > 255){
      THROTTLE = 255;
    }
  }
  if(BUTTONSTATES_NEW[ThrottleDOWN] && (!BUTTONSTATES_OLD[ThrottleDOWN])){
    THROTTLE -= 12;
    if(THROTTLE > 255){
      THROTTLE = 0;
    }
  }
  if(BUTTONSTATES_NEW[ThrottleDOWN] && (BUTTONSTATES_OLD[ThrottleDOWN])){
    DOWN_PRESSED++;
    if(DOWN_PRESSED >= DOWN_PRESSED_LIMIT){
      THROTTLE = 0;
    }
  }
  else{
    DOWN_PRESSED = 0;
  }
  TOGGLE = 0;
  if(BUTTONSTATES_NEW[ThrottleON]){
    TOGGLE = 1;
  }
  
}


void pollButtons(){
  int i;
  int pin;
  for (i=0; i<LEN_BUTTON_PINS; i++){
    pin = BUTTON_PINS[i];
    BUTTONSTATES_OLD[pin] = BUTTONSTATES_NEW[pin];
    BUTTONSTATES_NEW[pin] = getDigitalInput(pin);
  }
}



//Note that the counterintuitive button groundstate is inversed by "!state"
int getDigitalInput(int inputName){
    int state = 0;
    state = digitalRead(inputName);
    if(!state){
      return 1;
    }
    return 0;
}


//Analog input to char
int getAnalogInput(int inputName) {
  int a = (((float)analogRead(inputName)/1024.0)*(float)(255));
  return a;
}





