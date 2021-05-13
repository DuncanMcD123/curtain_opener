// This #include statement was automatically added by the Particle IDE.
#include <RCSwitch.h>
//Transmitter codes. Must match codes flashed onto curtain opener.
#define open_code 6969
#define close_code 9696
#define lights_off_code 9542
#define lights_on_code 2459

RCSwitch mySwitch = RCSwitch();

void setup() {
    mySwitch.enableTransmit(D1);
  Particle.subscribe("close_curtains", close_curtains); 
      //turning off function declaration
  Particle.subscribe("open_curtains", open_curtains); 
  Particle.subscribe("adjust_lights", adjust_lights);
}


// loop() runs continuously, it's our infinite loop.

void loop() {
}

//our events are called when IFTTT applets are triggered

void close_curtains(const char *event, const char *data){
    mySwitch.send(close_code, 24);
}
void open_curtains(const char *event, const char *data){
     mySwitch.send(open_code, 24);
}
void adjust_lights(const char *event, const char *data){
    int brightness = atoi(data);
    brightness = round(brightness*2.55);
    if (brightness>0 && brightness<=255){
        mySwitch.send(lights_on_code, 24);
        delay(200);
        mySwitch.send(brightness, 24);
    }
    if(brightness==0){
       mySwitch.send(lights_off_code, 24);
    }

}
