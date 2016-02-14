#include <NESpad.h>
/* 
 this example from the NESpad Arduino library
 displays the buttons on the joystick as bits
 on the serial port - rahji@rahji.com
 
 Version: 1.3 (11/12/2010) - got rid of shortcut constructor - seems to be broken
 NES   | Arduino  |Color
 5v    | 5v       |Orange
 GND   | GND      |Purple
 STROBE| D2       |Green
 CLOCK | D3       |Blue
 DATA  | D4       |Grey
 */
 
//BUTTONS:
#define TOTAL_MIDI_BUTTONS        6
#define TOTAL_BUTTONS			  8
#define SCALE_BUTTON_NUM		  6
#define AXIS_BUTTON_NUM			  7			//isolation of axis is needed for midi-learn
#define MIDI_BUTTONS_FIRST_INDEX          3    //midi buttons start from this index so they don't overlap with axis commands



// put your own strobe/clock/data pin numbers here -- see the pinout in readme.txt
#ifdef TEENSY
NESpad nintendo = NESpad(2,1,0);
#else
NESpad nintendo = NESpad(2,3,4);
#endif //TEENSY
extern int curr_axis_enable_configuration;
extern byte axisEnableMask[AXIS_ENABLE_MASK_NUM];
byte state = 0;		//current button state
byte lastState =0;	//last button state
byte upperState=LOW;
byte upperLastState=LOW;

//keep internal control buttons after midi buttons!!
byte buttonMask[TOTAL_BUTTONS] = {
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  B00000010,
  B00000001,
  B00001000,		//internal control button
  B00000100		//internal control button
};

void buttonPress(int button_num){
  int cc_or_pitch_index=curr_scale*TOTAL_MIDI_BUTTONS+button_num+MIDI_BUTTONS_FIRST_INDEX;
  
  midiSend(cc_or_pitch_index,MIDI_MAX_VALUE,midi_chnl_out); //send CC on
}

void buttonRelease(int button_num){
  int cc_or_pitch_index=curr_scale*TOTAL_MIDI_BUTTONS+button_num+MIDI_BUTTONS_FIRST_INDEX;

  midiSend(cc_or_pitch_index,MIDI_MIN_VALUE,midi_chnl_out); //send CC on
}

//release all currently pressed buttons (send a midi off command)
void releaseAllButtons(){
  //Release all currently pressed buttons here when changing scale
  int i;
  for (i=0;i<TOTAL_MIDI_BUTTONS;i++){
    if (buttonMask[i]&state){	//if button i is presses - release it
      buttonRelease(i);
    }
  }
  //state=0;	//if this is on buttons will be repressed... if not empty note offs will be sent..
}
void updateSideLed(){
#ifdef TEENSY
  ledsUpdateReset(((curr_scale==0) && (curr_axis_enable_configuration==0)));
#else
  if ((curr_scale==0) && (curr_axis_enable_configuration==0)){
    digitalWrite(SIDE_LED_PIN, HIGH);
  }else{
     digitalWrite(SIDE_LED_PIN, LOW);
  }
#endif
}
void upperButtonHandler(){
  upperState=digitalRead(UPPER_BUTTON_PIN);
#ifdef TEENSY
  upperState=(upperState==HIGH)?LOW:HIGH;
#endif
  
  if ((upperState==HIGH) && (upperLastState==LOW)){  //button pressed
    curr_scale=0;
    curr_axis_enable_configuration=0;
    releaseAllButtons();
    upperLastState=HIGH;
  }else if ((upperState==LOW) && (upperLastState==HIGH)){  //button pressed
    upperLastState=LOW;
  }
  updateSideLed();
  ledsUpdateScale(curr_scale);
  ledsUpdateAxis(axisEnableMask[curr_axis_enable_configuration]);
}

//inc cyclic scale setting
int changeScale(){
  releaseAllButtons();
  curr_scale = (curr_scale + 1) % SCALES;
  //update led state
  updateSideLed();
  ledsUpdateScale(curr_scale);
  return curr_scale;
}

//inc cyclic axis configuration
void axisEnableToggle(){
  curr_axis_enable_configuration = (curr_axis_enable_configuration + 1) % AXIS_ENABLE_MASK_NUM;
  ledsUpdateAxis(axisEnableMask[curr_axis_enable_configuration]);
  //update led state
  updateSideLed();
}

//handle buttons
void buttonHandler(){
  //get buttons state:
  state = nintendo.buttons();
  upperButtonHandler();

  if (state!=lastState){	//button pressed/released
#if 0//def DEBUG
  Serial.println(state,BIN);
#endif //DEBUG
    // shows the shifted bits from the joystick
    // buttons are high (1) when up 
    // and low (0) when pressed
    //Serial.println(~state, BIN);	//print to serial
    int i;
    //checking arrows, A, B buttons:
    for (i=0;i<TOTAL_MIDI_BUTTONS;i++){
      if ((buttonMask[i]&state) && !(buttonMask[i]&lastState)){
        buttonPress(i);
      }
      else if (!(buttonMask[i]&state) && (buttonMask[i]&lastState)){
        buttonRelease(i);
      }
    }

    //checking control buttons:
    if ((buttonMask[SCALE_BUTTON_NUM]&state) && !(buttonMask[SCALE_BUTTON_NUM]&lastState)){
      changeScale();
    }
    if ((buttonMask[AXIS_BUTTON_NUM]&state) && !(buttonMask[AXIS_BUTTON_NUM]&lastState)){
      axisEnableToggle();
    }

    lastState=state;
  }
}
