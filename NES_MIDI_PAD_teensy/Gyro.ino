//#include "Gyro.h"
//GYRO:
#define MPU						  0x68  // I2C address of the MPU-6050

#define MPU_LOW -16384
#define MPU_HI 16384

#define AXIS_FIRST_MASK						B00000100
#define APPLY_GYRO_FILTER
#define FILTER_COEF 0.95//0.98

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
float smooth_ax, smooth_ay, smooth_az;

int midiAxis[AXIS_NUM];
int lastMidiAxis[AXIS_NUM];

int curr_axis_enable_configuration=0;
byte axisEnableMask[AXIS_ENABLE_MASK_NUM] = {
  B00000111,	//all enabled
  B00000100,
  B00000010,
  B00000001,
  B00000000	//all disabled
};

// ================================================================
// ===                CALIBRATION_ROUTINE                       ===
// ================================================================
// Simple calibration - just average first few readings to subtract
// from the later data
void calibrate_sensors() {
  int       num_readings = 10;

  // Read and average the raw values
  for (int i = 0; i < num_readings; i++) {
    //Read from gyro via I2C
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);  // request a total of 14 registers					//TODO try changing this to only 6 (x,y,z hi & lo)
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    /*
    base_x_gyro += GyX;
    base_y_gyro += GyY;
    base_z_gyro += GyZ;
    */
    smooth_ax += AcX;
    smooth_ay += AcY;
    smooth_az += AcZ;
  }
  
  /*
  base_x_gyro /= num_readings;
  base_y_gyro /= num_readings;
  base_z_gyro /= num_readings;
  */
  smooth_ax /= num_readings;
  smooth_ay /= num_readings;
  smooth_az /= num_readings;
}

void init_mpu6050(){
  //MPU init:
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  calibrate_sensors();
}


void gyroHandler(){
  byte mask=AXIS_FIRST_MASK;
  int i;

  //Read from gyro via I2C
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);  // request a total of 14 registers					//TODO try changing this to only 6 (x,y,z hi & lo)
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

#ifdef APPLY_GYRO_FILTER
  /* Apply low pass filter to smooth
   accelerometer values. */
   float oneMinusCoef = 1- FILTER_COEF;
  smooth_ax = FILTER_COEF * smooth_ax + oneMinusCoef * AcX;
  smooth_ay = FILTER_COEF * smooth_ay + oneMinusCoef * AcY;
  smooth_az = FILTER_COEF * smooth_az + oneMinusCoef * AcZ;
  //smooth_az = 0.95 * smooth_az + 0.05 * GyZ;
#else
  smooth_ax = AcX;
  smooth_ay = AcY;
  smooth_az = AcZ;
#endif

  if (mask&axisEnableMask[curr_axis_enable_configuration]){		//axis is enabled in current config
    midiAxis[0] = 127-map(smooth_ax,MPU_LOW,MPU_HI,MIDI_MIN_VALUE,MIDI_MAX_VALUE);  //reverse midi
  }
  mask>>=1;
  if (mask&axisEnableMask[curr_axis_enable_configuration]){		//axis is enabled in current config
    midiAxis[1] = map(smooth_ay,MPU_LOW,MPU_HI,MIDI_MIN_VALUE,MIDI_MAX_VALUE);  //reverse midi
  }
  mask>>=1;
  if (mask&axisEnableMask[curr_axis_enable_configuration]){		//axis is enabled in current config
    midiAxis[2] = map(smooth_az,MPU_LOW,MPU_HI,MIDI_MIN_VALUE,MIDI_MAX_VALUE);
  }
  //mask>>=1;
#ifdef DEBUG
  if (1){//(midiAxis[0]>80){
    Serial.print("smooth_ax=");
    Serial.print(smooth_ax);
    Serial.print("midiAxis[0]=");
    Serial.println(midiAxis[0]);  
  }
  /*
  Serial.print("midiAxis[2]=");
  Serial.println(midiAxis[2]);
  */
#endif
#if 0//def DEBUG
  Serial.print("smooth_ax=");
  Serial.print(smooth_ax);
  /*
  Serial.print("smooth_ay=");
  Serial.print(smooth_ay);
  Serial.print(" smooth_az=");
  Serial.println(smooth_az);
  */
#endif 
  for (i=0;i<AXIS_NUM;i++){
    midiAxis[i]=constrain(midiAxis[i],MIDI_MIN_VALUE,MIDI_MAX_VALUE);
    if (midiAxis[i]!=lastMidiAxis[i]){
      //send midi command
      midiSend(i, midiAxis[i], DEFAULT_MIDI_CHANNEL_OUT);
      lastMidiAxis[i]=midiAxis[i];
    }
  }
}

