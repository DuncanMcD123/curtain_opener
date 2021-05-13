//Libraries
#include <Stepper.h>
#include <RCSwitch.h>

//Definitions 
#define STEPS 200
//#define steps_to_open -2000 //for bed  window
#define steps_to_open -2500 //for front window
//#define steps_to_close 2032 //for bed  window
#define steps_to_close 2516 //for front window
#define opener_speed 120 
#define rx_pin 2
#define led_pin 5
#define led_dim_rate 15 //controls the speed which lights dim. It's the number of ms between each change in the pwm value. Lower is faster. 
#define led_brighten_rate 30 

//Transmitter codes
#define open_code 6969
#define close_code 9696
#define lights_off_code 9542
#define lights_on_code 2459

//Operating Variables
int is_open = 2;
int current_brightness = 0;

// Create an instance of the library
RCSwitch mySwitch = RCSwitch();
//define pins 9, 10, 11, 12 for stepper motor
Stepper stepper(STEPS, 9, 10, 11, 12);

void setup() {
  Serial.begin(9600);
  stepper.setSpeed(opener_speed);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void adjust_led(int brightness){
  int difference = abs(brightness-current_brightness);
  if (brightness>current_brightness){
    for(int i=1;i<=difference;i++){
      current_brightness = current_brightness + 1;
      analogWrite(led_pin, current_brightness);
      delay(led_brighten_rate);
    }
  }
  if (brightness<current_brightness){
    for (int i=1;i<=difference;i++){
      current_brightness = current_brightness - 1;
      analogWrite(led_pin, current_brightness);
      delay(led_dim_rate);
    }
  }
}

void loop() {
  int val = mySwitch.getReceivedValue();
  if (val==open_code && is_open != 1){ //opens the curtains
    mySwitch.resetAvailable(); //resets the value of the switch once the code has been received
    Serial.println("Opening curtain");
    stepper.step(steps_to_open);
    digitalWrite(9, LOW); //these four lines are CRITICAL, otherwise the dual H-bridge IC will always be actively holding the stepper position. It's not needed and will cook the IC.
    digitalWrite(10, LOW); //they're responsible for turning off the driver output when not in use
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    is_open = 1; //used to set the curtain state and prevent the curtain from opening when already open
  }
  else if (val==close_code && is_open != 0){ //closes the curtains
    mySwitch.resetAvailable(); 
    Serial.println("Closing curtain");
    stepper.step(steps_to_close);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    is_open = 0; //used to set the curtain state and prevent the curtain from closing when already closed
  }
  else if (val==lights_on_code){
    mySwitch.resetAvailable();
    for (int i=0;i<=100;i++){ //waits for 100 cycles to receive a valid brightness value ( 0-255 accepted). If no valid brightness value is received, it goes back to default listening state. 
      delay(10);
      Serial.println("Waiting for brightness value" + i);
      int brightness = mySwitch.getReceivedValue();
      if (brightness > 0 && brightness <= 255){
        adjust_led(brightness);
        Serial.println("This is the brightness: ");
        Serial.print(current_brightness);
        delay(10);
        brightness = 256;
      }
    }
  }
  else if (val==lights_off_code){ //a separate code for turning the lights off, used because the transmitter doesn't like to transmit a 0 value. 
    mySwitch.resetAvailable();
    adjust_led(0);
    Serial.println("Lights off! Here is the current brightness" + current_brightness);
    delay(10);
  }
}
