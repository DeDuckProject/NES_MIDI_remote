//#include "Common.h"

void initMidi(){
  MIDI.begin(4);          // Launch MIDI and listen to channel 4
  
  int i;
  for (i=0;i<AXIS_NUM;i++){
    midiAxis[i]=0;
    lastMidiAxis[i]=0;
  }
}

void midiSend(int cc, int value,int chnl){
#ifdef SERIAL_DEBUG
#if 0//def DEBUG
      Serial.print("midi: cc=");
      Serial.print(cc);
      Serial.print(" value=");
      Serial.print(value);
      Serial.print(" Channel=");
      Serial.println(chnl);
#endif //DEBUG
#else
#ifdef TEENSY
#ifndef BUTTONS_SEND_MIDI_NOTES
      usbMIDI.sendControlChange(cc, value, chnl);
#else
      usbMIDI.sendNoteOff(cc, value, chnl);
#endif //BUTTONS_SEND_MIDI_NOTES
#else
#ifndef BUTTONS_SEND_MIDI_NOTES
      MIDI.sendControlChange(cc, value, chnl);
#else
      MIDI.sendNoteOff(cc, value, chnl);
#endif //BUTTONS_SEND_MIDI_NOTES
#endif //TEENSY
#endif //SERIAL_DEBUG
}
