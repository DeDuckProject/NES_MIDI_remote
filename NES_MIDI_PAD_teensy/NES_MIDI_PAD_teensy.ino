#include "Arduino.h"

#include<Wire.h>	//I2C for MPU-6050
#include <MIDI.h>
//..#include "Buttons.h"
//#include <NESpad.h>
//#include "Gyro.h"
//#include "Common.h"

//Common:
#define TEENSY 1
#define ENABLE_GYRO
//#define SERIAL_DEBUG
//#define DEBUG
#define US_DELAY_TIME			  10//1000
#define SCALES                    4

#define AXIS_ENABLE_MASK_NUM	  5
#define AXIS_NUM		3
#define SIDE_LED_PIN            6
#define UPPER_BUTTON_PIN                  13//5


//MIDI:
//#define BUTTONS_SEND_MIDI_NOTES	//if disabled buttons send CC

MIDI_CREATE_DEFAULT_INSTANCE();
#define DEFAULT_MIDI_CHANNEL_OUT  2
#define DEFAULT_VELOCITY          127
#define MIDI_MIN_VALUE			  0
#define MIDI_MAX_VALUE			  127

int midi_chnl_out = DEFAULT_MIDI_CHANNEL_OUT;
int curr_scale=0;

void setup() {
  initMidi();
  init_leds();
  Serial.begin(115200);		//midi serial freq
  //Serial.begin(57600);  	//regular serial freq
#ifdef ENABLE_GYRO
  init_mpu6050();
#endif  //ENABLE_GYRO
  //configure button and led
#ifdef TEENSY
  pinMode(UPPER_BUTTON_PIN, INPUT_PULLUP);
#else
  pinMode(UPPER_BUTTON_PIN, INPUT);
  pinMode(SIDE_LED_PIN, OUTPUT);
  digitalWrite(SIDE_LED_PIN, HIGH);
#endif //TEENSY
}
void loop() {

  buttonHandler();
#ifdef ENABLE_GYRO
  gyroHandler();
#endif  //ENABLE_GYRO
  delayMicroseconds(US_DELAY_TIME);
}

