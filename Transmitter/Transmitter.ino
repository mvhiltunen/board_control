/*
 * Transmitter for an electronic longboard.
 * Inteded for Arduino Nano (v3.0).
 *
 * Bluetooth:       Serial         pins RX0, TX1, 5V, (GND)
 * Digital out:     2, 12
 * Digital in:      3, 4, 5, 6, 7, 8, 9, 10, 11
 * Analog out:      A0, A3, A4, A5, A6, A7
 * Analog in:       A1
 */
#include <stdio.h>
#include <stdlib.h>

// Digitals
#define BTPower 2
#define ThrottleUP 3
#define ThrottleDOWN 4
#define ThrottleON 5
#define APlayPause 6
#define AStop 7
#define ANext 8
#define APrev 9
#define AVolUP 10
#define AVolDOWN 11
#define BTIndicator 12
#define ToggleIndicator 13

// Analogs
#define ConstantOut A0
#define AnalogIn A1

#define Indicator1 A3
#define Indicator2 A4
#define Indicator3 A5
#define Indicator4 A6
#define Indicator5 A7


// Bluetooth pins 0, 1, 3V3
#define Bluetooth Serial
#define BluetoothSerialSpeed 9600
// serial config is default: 8, 1, n


// Bluetooth-sending on/off
#define SendBluetooth 1


/* declarations
void setupBluetooth();
void errorStop();
int getDigitalInput(int inputName);
int getAnalogInput(int inputName);
*/

// GLOBALS
char SWITCH = 0;
char CONNECTION_UP = 0;
int PLACEHOLDER = 0;
char BT_RESET = 1;
unsigned int THROTTLE = 0;
unsigned int TOGGLE = 0;
unsigned int ZERO = 0;
unsigned int GAS = 0;
int BUTTONSTATES_OLD[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
int BUTTONSTATES_NEW[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
int INDICATOR_PINS[] = {Indicator1,Indicator2,Indicator3,Indicator4,Indicator5};
int AUDIO_COMMANDS[6];
char MSG[5];





void setup() {
    //Digital pins - declare
    pinMode(BTPower, OUTPUT);
    pinMode(BTIndicator, OUTPUT);
    pinMode(ToggleIndicator, OUTPUT);
    pinMode(ThrottleUP, INPUT);
    pinMode(ThrottleDOWN, INPUT);
    pinMode(ThrottleON, INPUT);
    pinMode(APlayPause, INPUT);
    pinMode(AStop, INPUT);
    pinMode(ANext, INPUT);
    pinMode(APrev, INPUT);
    pinMode(AVolUP, INPUT);
    pinMode(AVolDOWN, INPUT);
    
    //Digital pins - setup
    digitalWrite(BTPower, HIGH);
    digitalWrite(BTIndicator, LOW);
    digitalWrite(ToggleIndicator, LOW);
    // internal pull-up
    digitalWrite(ThrottleUP, HIGH);
    digitalWrite(ThrottleDOWN, HIGH);
    digitalWrite(ThrottleON, HIGH);
    digitalWrite(APlayPause, HIGH);
    digitalWrite(AStop, HIGH);
    digitalWrite(ANext, HIGH);
    digitalWrite(APrev, HIGH);
    digitalWrite(AVolUP, HIGH);
    digitalWrite(AVolDOWN, HIGH);


    //Analog pins - declare
    pinMode(AnalogIn, INPUT);
    pinMode(ConstantOut, OUTPUT);
    pinMode(Indicator1, OUTPUT);
    pinMode(Indicator2, OUTPUT);
    pinMode(Indicator3, OUTPUT);
    pinMode(Indicator4, OUTPUT);
    pinMode(Indicator5, OUTPUT);

    //Analog pins - setup
    analogWrite(ConstantOut, 255);
    analogWrite(AnalogIn, HIGH);
    
    // Connect bluetooth serial
    setupBluetooth();
}

// MAINLOOP
void loop() {
    CONNECTION_UP = max(CONNECTION_UP - 1, 0);
    while (Bluetooth.available()) {
        switch (Bluetooth.read()) {
            case 'X':
                errorStop();
                break;
            case 'U':
                CONNECTION_UP = 70;
                BT_RESET = 0;
                break;
            default:
                break;
        }
        if (Bluetooth.available() && Bluetooth.read() == '\n') continue;
    }
    int i = 0;
    
    //poll buttons
    for(i=3; i<12; i++){
      BUTTONSTATES_OLD[i] = BUTTONSTATES_NEW[i];
      BUTTONSTATES_NEW[i] = getDigitalInput(i);
    }
    
    //parse press-events
    parseEvents();
    

    //parse & send Throttle level
    sendThrottle();


    //sendAudioCommands
    sendAudioCommands();


    //setIndicatorLEDs
    setIndicatorLeds();

    if(TOGGLE){
      digitalWrite(ToggleIndicator, HIGH);
    }
    else{
      digitalWrite(ToggleIndicator, LOW);
    }


    if(CONNECTION_UP){
      digitalWrite(BTIndicator, HIGH);}
    else{
      digitalWrite(BTIndicator, LOW);
      if(!BT_RESET){
        digitalWrite(BTPower, LOW);
        delay(200);
        digitalWrite(BTPower, HIGH);
        BT_RESET = 1;
      }
    }
      
    delay(10);
}



void sendAudioCommands(){
  int i;
  for(i=0;i<6;i++){
    if(AUDIO_COMMANDS[i]){
      sprintf(MSG, "A00%d", i);
      MSG[4] = '=';
      sendMSG();
    }
  }
}


void sendThrottle(){
  MSG[0] = 'T';
  GAS = ZERO + TOGGLE*THROTTLE;
  int printi = 1;
  if(GAS < 100){
    MSG[1] = '0';
    printi = 2;
    if(GAS < 10){
      MSG[2] = '0';
      printi = 3;
    }
  }
  sprintf(MSG+printi, "%d", GAS);
  MSG[4] = '=';
  sendMSG();
}


void sendMSG(){
  Bluetooth.print(MSG);
  Bluetooth.println();
}



void parseEvents(){
/*
ThrottleUP 3
ThrottleDOWN 4
ThrottleON 5
APlayPause 6
AStop 7
ANext 8
APrev 9
AVolUP 10
AVolDOWN 11
*/
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
  TOGGLE = 0;
  if(BUTTONSTATES_NEW[ThrottleON]){
    TOGGLE = 1;
  }
  memset(AUDIO_COMMANDS, 0, 12); //12 = 6*2 where 2 is integer size in this hardware
  if(BUTTONSTATES_NEW[APlayPause] && (!BUTTONSTATES_OLD[APlayPause])){
    AUDIO_COMMANDS[0] = 1;
  }
  if(BUTTONSTATES_NEW[AStop] && (!BUTTONSTATES_OLD[AStop])){
    AUDIO_COMMANDS[1] = 1;
  }
  if(BUTTONSTATES_NEW[ANext] && (!BUTTONSTATES_OLD[ANext])){
    AUDIO_COMMANDS[2] = 1;
  }
  if(BUTTONSTATES_NEW[APrev] && (!BUTTONSTATES_OLD[APrev])){
    AUDIO_COMMANDS[3] = 1;
  }
  if(BUTTONSTATES_NEW[AVolUP] && (!BUTTONSTATES_OLD[AVolUP])){
    AUDIO_COMMANDS[4] = 1;
  }
  if(BUTTONSTATES_NEW[AVolDOWN] && (!BUTTONSTATES_OLD[AVolDOWN])){
    AUDIO_COMMANDS[5] = 1;
  }
}


void setIndicatorLeds(){
  unsigned int i;
  for(i=0;i<5;i++){
    if(THROTTLE > i*60 ){
      digitalWrite(INDICATOR_PINS[i], HIGH);
    }
    else{
      digitalWrite(INDICATOR_PINS[i], LOW);
    }
  }
}




//Note that the illogical button groundstate is inversed by "!state"
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


// Stop and blink when error occurs
void errorStop() {
    while (1) {
        digitalWrite(BTIndicator, LOW);
        delay(700);
        digitalWrite(BTIndicator, HIGH);
        delay(700);
    }
}


void setupBluetooth() {
    Bluetooth.begin(BluetoothSerialSpeed);
    Bluetooth.println("DBluetooth module active.");
}



