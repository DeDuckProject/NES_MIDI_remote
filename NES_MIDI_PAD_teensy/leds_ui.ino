#define LED_Z_AXIS  21
#define LED_Y_AXIS  20
#define LED_X_AXIS  19
#define LED_RESET  18
#define LED_SCALE_4  17
#define LED_SCALE_3  16
#define LED_SCALE_2  15
#define LED_SCALE_1  14

#define NUM_OF_SCALE_LEDS  4
#define NUM_OF_AXIS_LEDS  3

int led_scale[NUM_OF_SCALE_LEDS];
int led_axis[NUM_OF_AXIS_LEDS];

void init_leds(){
  int i=0;
  led_scale[0]=LED_SCALE_1;
  led_scale[1]=LED_SCALE_2;
  led_scale[2]=LED_SCALE_3;
  led_scale[3]=LED_SCALE_4;
  
  led_axis[0]=LED_X_AXIS;
  led_axis[1]=LED_Y_AXIS;
  led_axis[2]=LED_Z_AXIS;

  //turn off leds:
  for (;i<NUM_OF_AXIS_LEDS;i++){
    digitalWrite(led_axis[i],LOW);
  }
  for (;i<NUM_OF_SCALE_LEDS;i++){
    digitalWrite(led_scale[i],LOW);
  }
  digitalWrite(LED_RESET,LOW);
  ledsUpdateReset(1);
  ledsUpdateAxis(B00000111);
  ledsUpdateScale(0);
}

void ledsUpdateReset(int on){
 if (on)
   digitalWrite(LED_RESET,HIGH);
 else
   digitalWrite(LED_RESET,LOW);
}

void ledsUpdateAxis(int mask){
  /*
  byte axisEnableMask[AXIS_ENABLE_MASK_NUM] = {
  B00000111,	//all enabled
  B00000100,
  B00000010,
  B00000001,
  B00000000	//all disabled
};
*/
  int i;
  int temp;
  for (i=0;i<NUM_OF_AXIS_LEDS;i++){
    temp=mask>>(NUM_OF_AXIS_LEDS-i-1);
    temp&=1;
    digitalWrite(led_axis[i],temp);
  }
}

void ledsUpdateScale(int scale){
  int i;
  for (i=0;i<NUM_OF_SCALE_LEDS;i++){
      digitalWrite(led_scale[i],(i==scale)?HIGH:LOW);
  }
}
