#include <ctype.h>
#include <SoftwareSerial.h>
#include <Servo.h>


#define MOTOR_PIN 9
#define TEST_PIN 2

#define DEBUG_MODE 1

Servo MotorControl;
SoftwareSerial BTSerial(11, 12); // RX | TX

//GLOBALS
unsigned int BAUDRATE = 9600;
unsigned long SLEEPTIME = 50;
unsigned char MUTE_CYCLES = 20;
unsigned int CONNECTION_THRESHOLD = 8;

unsigned int CONNECTION_COUNTER = 0;
unsigned char CONNECTION_BROKEN = 1;
unsigned char CYCLES_AFTER_CONNECTION = 0;
unsigned char THROTTLE_LOCK = 1;
unsigned int THROTTLE = 0;
unsigned char RECV_VECTOR[5];
unsigned int RECV_I = 0;
int TESTVAL = 0;

char c = ' ';

void setup() 
{
    BTSerial.begin(BAUDRATE);
    pinMode(TEST_PIN, OUTPUT);
    //MotorControl.attach(MOTOR_PIN);
    //MotorControl.writeMicroseconds(1500);
    
    // HC-06 default baud rate is 9600
    
    Serial.begin(BAUDRATE);
    Serial.println("Arduino with HC-06 is ready");
    Serial.println("BTserial started at 9600");
}

void loop()
{
  CONNECTION_COUNTER++;
  RECV_I = 0;
  memset(RECV_VECTOR, 0, 5);
  char overflow_bin;
  while (BTSerial.available()){
    if (RECV_I<5){
      RECV_VECTOR[RECV_I] = BTSerial.read();
    }
    else{
      overflow_bin = BTSerial.read();
    }
    RECV_I++;
  }
  if (RECV_I==5){
    if (coherent_message()){
      
      CONNECTION_BROKEN = 0;
      CONNECTION_COUNTER = 0;
      unsigned int throttle = decode_message();
      
      if (throttle != THROTTLE){
        THROTTLE = throttle;
        update_duty_cycle();
      }
      if (CYCLES_AFTER_CONNECTION < MUTE_CYCLES){
        CYCLES_AFTER_CONNECTION++;
      }
      if (CYCLES_AFTER_CONNECTION >= MUTE_CYCLES){
        if(THROTTLE_LOCK){
          THROTTLE_LOCK = 0;
          MotorControl.attach(MOTOR_PIN);
          MotorControl.writeMicroseconds(1500);
        }
      }
    }
  }

  if (CONNECTION_COUNTER >= CONNECTION_THRESHOLD){
    THROTTLE = 0;
    update_duty_cycle();
    CONNECTION_COUNTER = CONNECTION_THRESHOLD+1;
    CONNECTION_BROKEN = 1;
    CYCLES_AFTER_CONNECTION = 0;
    if(!THROTTLE_LOCK){
      THROTTLE_LOCK = 1;
      MotorControl.writeMicroseconds(1500);
      MotorControl.detach();
    }
  }
  update_duty_cycle();

  if (DEBUG_MODE){
    Serial.print(TESTVAL);
    Serial.print(":");
    Serial.print(THROTTLE);
    Serial.println(":");
    TESTVAL++;
    if (TESTVAL > 1000){
      TESTVAL = 0;
    }
    if(THROTTLE){
      digitalWrite(TEST_PIN, HIGH);
    }
    else{
      digitalWrite(TEST_PIN, LOW);
    }
  }


  delay(SLEEPTIME);
}


void update_duty_cycle(){
  float ff = 0;
  if(!THROTTLE_LOCK){
    ff = THROTTLE;
    unsigned int us = 500 * (ff/255);
    MotorControl.writeMicroseconds(1500+us);
  }
}


unsigned char decode_message(){
  unsigned char cc = RECV_VECTOR[2];
  return cc;
}


unsigned char coherent_message(){
  if (RECV_VECTOR[0] == 'T'){
    if (RECV_VECTOR[1] == '='){
      if (RECV_VECTOR[3] == 'C'){
        unsigned char c = ~RECV_VECTOR[2];
        unsigned char c2 = RECV_VECTOR[4];
        if (c == c2){
          return 1;
        }
      }
    }
  }
  return 0;
}


char to_char_length(unsigned int T){
  unsigned char c;
  if (T > 255){
    c = 255;
  }
  else{
    c = T;
  }
  return c;
}


