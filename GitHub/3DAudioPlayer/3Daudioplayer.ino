//COMBINED

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=113,308
AudioPlaySdWav           playSdWav2;     //xy=125,363
AudioPlaySdWav           playSdWav3;     //xy=128,668
AudioEffectDelay         delay1;         //xy=358,220
AudioEffectDelay         delay3;         //xy=408,689
AudioEffectDelay         delay2;         //xy=435,472
AudioMixer4              mixer2;         //xy=612,374
AudioMixer4              mixer1;         //xy=614,222
AudioMixer4              mixer3;         //xy=656,576
AudioMixer4              mixer4;         //xy=675,691
AudioOutputI2S           i2s1;           //xy=893,361
AudioConnection          patchCord1(playSdWav1, 0, delay1, 0);
AudioConnection          patchCord2(playSdWav2, 0, delay2, 0);
AudioConnection          patchCord3(playSdWav3, 0, mixer3, 0);
AudioConnection          patchCord4(playSdWav3, 0, mixer4, 0);
AudioConnection          patchCord5(playSdWav3, 0, delay3, 0);
AudioConnection          patchCord6(delay1, 0, mixer1, 0);
AudioConnection          patchCord7(delay1, 1, mixer2, 0);
AudioConnection          patchCord8(delay3, 0, mixer3, 1);
AudioConnection          patchCord9(delay3, 1, mixer4, 1);
AudioConnection          patchCord10(delay2, 0, mixer1, 1);
AudioConnection          patchCord11(delay2, 1, mixer2, 1);
AudioConnection          patchCord12(mixer2, 0, i2s1, 1);
AudioConnection          patchCord13(mixer1, 0, i2s1, 0);
AudioConnection          patchCord14(mixer3, 0, mixer1, 2);
AudioConnection          patchCord15(mixer4, 0, mixer2, 2);
AudioControlSGTL5000     sgtl5000_1;     //xy=182,458
// GUItool: end automatically generated code


Bounce button0 = Bounce(2, 15);
Bounce button1 = Bounce(3, 15);// 15 = 15 ms debounce time

int state = 0;
int IMU;


void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);
  AudioMemory(160);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  SPI.setMOSI(7);
  SPI.setSCK(14);
  if (!(SD.begin(10))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  pinMode(13, OUTPUT); // LED on pin 13
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  //pinMode(21, INPUT);
  delay(1000);
}

elapsedMillis blinkTime;


void loop() {
  
  // read the knob position (analog input A1)
  int knob = analogRead(A1);
  float vol = (float)knob / 1280.0;
  sgtl5000_1.volume(vol);
  //Serial.print("volume = ");
  //Serial.println(vol);

  // blink the LED without delays
  if (blinkTime < 250) {
    digitalWrite(13, LOW);
  } else if (blinkTime < 500) {
    digitalWrite(13, HIGH);
  } else {
    blinkTime = 0; // start blink cycle over again
  }
  
  int Switch = analogRead(A7);
  Serial.println(Switch);

  //SEL: Pass-Through
 int d = analogRead(A2); //HORZ: Left & Right
 int e = analogRead(A3); //VERT: Front & Back
 int c = analogRead(A6);
  
  if (Switch < 10) {
  mixer1.gain(0, 0); //MSS Left off
  mixer1.gain(1, 0); 
  mixer2.gain(0, 0); //MSS Right off
  mixer2.gain(1, 0);
  mixer1.gain(2, 1); //SSS Left on
  mixer2.gain(2, 1); //SSS Right on
  
  static int filenumber = 0;  // while file to play

static const char * filelist[4] = {
  "SDTEST1.WAV", "SDTEST2.WAV", "SDTEST3.WAV", "SDTEST4.WAV"
};
  
// send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                state = Serial.read();

                // say what you got:
                Serial.print("I received: ");
                Serial.println(state, DEC);
        }
  
  if (playSdWav3.isPlaying() == false) {
    const char *filename = filelist[filenumber];
    filenumber = filenumber + 1;
    if (filenumber >= 4) filenumber = 0;
    Serial.print("Start playing ");
    Serial.println(filename);
    playSdWav3.play(filename);
    delay(10); // wait for library to parse WAV info
  }

  
  // read pushbuttons
  button0.update();
  if (button0.fallingEdge()) {
    playSdWav3.stop();
  }
  button1.update();
  if (button1.fallingEdge()) {
    playSdWav3.stop();
    filenumber = filenumber - 2;
    if (filenumber < 0) filenumber = filenumber + 4;
  }
 
if (c < 1 ) {

    // Virtual Position
  if((600<d) && (d<850) && (600<e) && (e<850)){
     mixer3.gain(0,1);
     mixer4.gain(0,1);
     mixer3.gain(1,0);
     mixer4.gain(1,0);// when at static position mixer3:1.0 mixer4:1.0
     Serial.println("Center");
  }
  if((d<600) && (600<e) && (e<850)){
    mixer3.gain(0,1);
    delay3.delay(1, 1);
    mixer4.gain(0,0);
    mixer4.gain(1,0.9);// left mixer3:1.0 mixer4:0.9
    Serial.println("Left");
  }
  if ((d>850) && (600<e) && (e<850)){
    delay3.delay(0, 1);
    mixer3.gain(1,0.9);
    mixer3.gain(0,0);
    mixer4.gain(0,1);// right mixer3:0.9 mixer4:1.0
    Serial.println("Right");
  }
  
  if((600<d) && (d<850) && (e>850)){
    mixer3.gain(0,0.5);
    mixer4.gain(0,0.5);
    delay3.delay(0, 1);
    mixer3.gain(1,0.5);  
    delay3.delay(1, 1);
    mixer4.gain(1,0.5);// back mixer3:1.0 mixer4:1.0
    Serial.println("Back");
  }
  if((600<d) && (d<850) && (e<600)){
    mixer3.gain(0,0.5);
    mixer4.gain(0,0.5);
    delay3.delay(0, 0);
    mixer3.gain(1,0.5);
    delay3.delay(1, 0);
    mixer4.gain(1,0.5);// front mixer3:1 mixer4:1
    Serial.println("Front");
  }
  if((d<600) && (e>850)){
    mixer3.gain(0,0.7);
    mixer4.gain(0,0.5);
    delay3.delay(0,0.5);
    mixer3.gain(1,0.5);
    delay3.delay(1, 1);
    mixer4.gain(1,0.5);// left back mixer3:1.2 mixer4:1.0
    Serial.println("Left Back");
  }
  if((850<d) && (e>850)){
    mixer3.gain(0,0.5);
    mixer4.gain(0,0.7);
    delay3.delay(0,1);
    mixer3.gain(1,0.5);
    delay3.delay(1, 0.5);
    mixer4.gain(1,0.5);// right back mixer3:1.0 mixer4:1.2
    Serial.println("Right Back");
  }
  if((d<600) && (600>e)){
    mixer3.gain(0,0.7);
    mixer4.gain(0,0.5);
    delay3.delay(0,0);
    mixer3.gain(1,0.5);
    delay3.delay(1, 0.5);
    mixer4.gain(1,0.5);// left front mixer3:1.2 mixer4:1.0
    Serial.println("Left Front");
  }  
  if((850<d) && (600>e)){
    mixer3.gain(0,0.5);
    mixer4.gain(0,0.7);
    delay3.delay(0,0.5);
    mixer3.gain(1,0.5);
    delay3.delay(1, 0);
    mixer4.gain(1,0.5);// right front mixer3:1.0 mixer4:1.2
    Serial.println("Right Front");
  } 
  Serial.println(c);
}
else if (c > 0) {
  if(Serial1.available()>0){ // Checks whether data is comming from the serial port
        IMU =  (Serial1.parseFloat()); // Reads the data from the serial port
     Serial.println((IMU));
     }

    // Virtual Position
  if((IMU<=-68) && (IMU>=-112)){
    delay3.delay(0, 1);
    mixer3.gain(1,0.9);
    mixer3.gain(0,0);
    mixer4.gain(0,1);// right mixer3:0.9 mixer4:1.0
    Serial.println("Right");
  }
  if((IMU>=68) && (IMU<=112)){
    mixer3.gain(0,1);
    delay3.delay(1, 1);
    mixer4.gain(0,0);
    mixer4.gain(1,0.9);// left mixer3:1.0 mixer4:0.9
    Serial.println("Left");
  }
  
  if((IMU<=-158) || (158<=IMU)){
    mixer3.gain(0,0.5);
    mixer4.gain(0,0.5);
    delay3.delay(0, 1);
    mixer3.gain(1,0.5);  
    delay3.delay(1, 1);
    mixer4.gain(1,0.5);// back mixer3:1.0 mixer4:1.0
    Serial.println("Back");
  }
 if((IMU>=-22) && (IMU<0) || (IMU>0) && (IMU<=22)){
    mixer3.gain(0,0.5);
    mixer4.gain(0,0.5);
    delay3.delay(0, 0);
    mixer3.gain(1,0.5);
    delay3.delay(1, 0);
    mixer4.gain(1,0.5);// front mixer3:1 mixer4:1
    Serial.println("Front");
  }
  if((IMU>=-157) && (IMU<=-113)){
    mixer3.gain(0,0.5);
    mixer4.gain(0,0.7);
    delay3.delay(0,1);
    mixer3.gain(1,0.5);
    delay3.delay(1, 0.5);
    mixer4.gain(1,0.5);// right back mixer3:1.0 mixer4:1.2
    Serial.println("Right Back");
  }
  if((IMU<=157) && (IMU>=113)){
    mixer3.gain(0,0.7);
    mixer4.gain(0,0.5);
    delay3.delay(0,0.5);
    mixer3.gain(1,0.5);
    delay3.delay(1, 1);
    mixer4.gain(1,0.5);// left back mixer3:1.2 mixer4:1.0
    Serial.println("Left Back");
  }
  if((IMU<=-23) && (IMU>=-67)){
    mixer3.gain(0,0.5);
    mixer4.gain(0,0.7);
    delay3.delay(0,0.5);
    mixer3.gain(1,0.5);
    delay3.delay(1, 0);
    mixer4.gain(1,0.5);// right front mixer3:1.0 mixer4:1.2
    Serial.println("Right Front");
  }  
  if((IMU>=23) && (IMU<=67)){
    mixer3.gain(0,0.7);
    mixer4.gain(0,0.5);
    delay3.delay(0,0);
    mixer3.gain(1,0.5);
    delay3.delay(1, 0.5);
    mixer4.gain(1,0.5);// left front mixer3:1.2 mixer4:1.0
    Serial.println("Left Front");

  } 
}
}

else if (Switch > 10) {
  mixer1.gain(2, 0); //SSS Left off
  mixer2.gain(2, 0); //SSS Right off
  
  if (playSdWav1.isPlaying() == false) {
    Serial.println("Start playing 1");
    playSdWav1.play("BIRD.WAV");
    delay(10); // wait for library to parse WAV info
  }
  if (playSdWav2.isPlaying() == false) {
    Serial.println("Start playing 2");
    playSdWav2.play("RIVER.WAV");
    delay(10); // wait for library to parse WAV info
  }

    // read pushbuttons
  button0.update();
  if (button0.fallingEdge()) {
    playSdWav1.stop();
    delay(3000);
  }
  button1.update();
  if (button1.fallingEdge()) {
    playSdWav2.stop();
    delay(3000);
  }

  //Default position: Bird in Front, River Behind
 
if (c < 1 ) {

    // Virtual Position
  if((600<d) && (d<850) && (600<e) && (e<850)){
  mixer1.gain(0, 1.4); //440 Hz Left
  mixer2.gain(0, 1.4); //440 Hz Right
  mixer1.gain(1, 0.065); //660 Hz Left
  mixer2.gain(1, 0.065); //660 Hz Right
  delay1.delay(0, 0); //440 Hz Left 0ms delay
  delay2.delay(0, 0); //440 Hz Right 0ms delay 
  delay1.delay(1, 1); //660 Hz Left 0ms delay
  delay2.delay(1, 1); //660 Hz Right 0ms delay
    Serial.println("Front");
  }
  if((d<600) && (600<e) && (e<850)){
  mixer1.gain(0, 0.1); //440 Hz Left
  mixer2.gain(0, 0.9); //440 Hz Right
  mixer1.gain(1, 0.155); //660 Hz Left 0.9
  mixer2.gain(1, 0.07); //660 Hz Right 0.1
  delay1.delay(0, 1); //440 Hz Left 1ms delay
  delay2.delay(0, 0); //440 Hz Right 0ms delay 
  delay1.delay(1, 0); //660 Hz Left 0ms delay
  delay2.delay(1, 1); //660 Hz Right 1ms delay
    Serial.println("Left");
  }
  if ((d>850) && (600<e) && (e<850)){
  mixer1.gain(0, 0.9); //440 Hz Left
  mixer2.gain(0, 0.1); //440 Hz Right
  mixer1.gain(1, 0.07); //660 Hz Left 0.1
  mixer2.gain(1, 0.155); //660 Hz Right 0.9
  delay1.delay(0, 0); //440 Hz Left 1ms delay
  delay2.delay(0, 1); //440 Hz Right 0ms delay 
  delay1.delay(1, 1); //660 Hz Left 0ms delay
  delay2.delay(1, 0); //660 Hz Right 1ms delay
    Serial.println("Right");
  }
  
  if((600<d) && (d<850) && (e>850)){
  mixer1.gain(0, 0.25); //440 Hz Left
  mixer2.gain(0, 0.25); //440 Hz Right
  mixer1.gain(1, 0.13); //660 Hz Left 0.5
  mixer2.gain(1, 0.13); //660 Hz Right 0.5
  delay1.delay(0, 1.5); //440 Hz Left 0ms delay
  delay2.delay(0, 1.5); //440 Hz Right 0ms delay 
  delay1.delay(1, 0); //660 Hz Left 0ms delay
  delay2.delay(1, 0); //660 Hz Right 0ms delay
    Serial.println("Back");
  }
  if((d<600) && (e>850)){
  mixer1.gain(0, 0.3); //440 Hz Left
  mixer2.gain(0, 0.7); //440 Hz Right
  mixer1.gain(1, 0.13); //660 Hz Left 0.7
  mixer2.gain(1, 0.09); //660 Hz Right 0.3
  delay1.delay(0, 1); //440 Hz Left 1ms delay
  delay2.delay(0, 0.5); //440 Hz Right 0ms delay 
  delay1.delay(1, 0.5); //660 Hz Left 0ms delay
  delay2.delay(1, 1); //660 Hz Right 1ms delay
    Serial.println("Left Back");
  }
  if((850<d) && (e>850)){
  mixer1.gain(0, 0.7); //440 Hz Left
  mixer2.gain(0, 0.3); //440 Hz Right
  mixer1.gain(1, 0.09); //660 Hz Left 0.3
  mixer2.gain(1, 0.13); //660 Hz Right 0.7
  delay1.delay(0, 0.5); //440 Hz Left 1ms delay
  delay2.delay(0, 1); //440 Hz Right 0ms delay 
  delay1.delay(1, 1); //660 Hz Left 0ms delay
  delay2.delay(1, 0.5); //660 Hz Right 1ms delay
    Serial.println("Right Back");
  }
  if((d<600) && (600>e)){
  mixer1.gain(0, 0.4); //440 Hz Left
  mixer2.gain(0, 0.6); //440 Hz Right
  mixer1.gain(1, 0.1); //660 Hz Left 0.7
  mixer2.gain(1, 0.085); //660 Hz Right 0.5
  delay1.delay(0, 0.5); //440 Hz Left 1ms delay
  delay2.delay(0, 0); //440 Hz Right 0ms delay 
  delay1.delay(1, 0); //660 Hz Left 0ms delay
  delay2.delay(1, 0.5); //660 Hz Right 1ms delay
    Serial.println("Left Front");
  }  
  if((850<d) && (600>e)){
  mixer1.gain(0, 0.6); //440 Hz Left
  mixer2.gain(0, 0.4); //440 Hz Right
  mixer1.gain(1, 0.085); //660 Hz Left 0.5
  mixer2.gain(1, 0.1); //660 Hz Right 0.7
  delay1.delay(0, 0); //440 Hz Left 1ms delay
  delay2.delay(0, 0.5); //440 Hz Right 0ms delay 
  delay1.delay(1, 0.5); //660 Hz Left 0ms delay
  delay2.delay(1, 0); //660 Hz Right 1ms delay
    Serial.println("Right Front");
  } 
  Serial.println(c);
}
else if (c > 0) {
  if(Serial1.available()>0){ // Checks whether data is comming from the serial port
        IMU =  (Serial1.parseFloat()); // Reads the data from the serial port
     Serial.println((IMU));
     }

    // Virtual Position

  if((IMU<=-68) && (IMU>=-112)){
  mixer1.gain(0, 0.1); //440 Hz Left
  mixer2.gain(0, 0.9); //440 Hz Right
  mixer1.gain(1, 0.155); //660 Hz Left 0.9
  mixer2.gain(1, 0.07); //660 Hz Right 0.1
  delay1.delay(0, 1); //440 Hz Left 1ms delay
  delay2.delay(0, 0); //440 Hz Right 0ms delay 
  delay1.delay(1, 0); //660 Hz Left 0ms delay
  delay2.delay(1, 1); //660 Hz Right 1ms delay
    Serial.println("Left");
  }
  if((IMU>=68) && (IMU<=112)){
  mixer1.gain(0, 0.9); //440 Hz Left
  mixer2.gain(0, 0.1); //440 Hz Right
  mixer1.gain(1, 0.07); //660 Hz Left 0.1
  mixer2.gain(1, 0.155); //660 Hz Right 0.9
  delay1.delay(0, 0); //440 Hz Left 1ms delay
  delay2.delay(0, 1); //440 Hz Right 0ms delay 
  delay1.delay(1, 1); //660 Hz Left 0ms delay
  delay2.delay(1, 0); //660 Hz Right 1ms delay
    Serial.println("Right");
  }
  
  if((IMU<=-158) || (158<=IMU)){
  mixer1.gain(0, 0.25); //440 Hz Left
  mixer2.gain(0, 0.25); //440 Hz Right
  mixer1.gain(1, 0.13); //660 Hz Left 0.5
  mixer2.gain(1, 0.13); //660 Hz Right 0.5
  delay1.delay(0, 1.5); //440 Hz Left 0ms delay
  delay2.delay(0, 1.5); //440 Hz Right 0ms delay 
  delay1.delay(1, 0); //660 Hz Left 0ms delay
  delay2.delay(1, 0); //660 Hz Right 0ms delay
    Serial.println("Back");
  }
 if((IMU>=-22) && (IMU<0) || (IMU>0) && (IMU<=22)){
  mixer1.gain(0, 1.4); //440 Hz Left
  mixer2.gain(0, 1.4); //440 Hz Right
  mixer1.gain(1, 0.065); //660 Hz Left
  mixer2.gain(1, 0.065); //660 Hz Right
  delay1.delay(0, 0); //440 Hz Left 0ms delay
  delay2.delay(0, 0); //440 Hz Right 0ms delay 
  delay1.delay(1, 1); //660 Hz Left 0ms delay
  delay2.delay(1, 1); //660 Hz Right 0ms delay
    Serial.println("Front");
  }
  if((IMU>=-157) && (IMU<=-113)){
  mixer1.gain(0, 0.3); //440 Hz Left
  mixer2.gain(0, 0.7); //440 Hz Right
  mixer1.gain(1, 0.13); //660 Hz Left 0.7
  mixer2.gain(1, 0.09); //660 Hz Right 0.3
  delay1.delay(0, 1); //440 Hz Left 1ms delay
  delay2.delay(0, 0.5); //440 Hz Right 0ms delay 
  delay1.delay(1, 0.5); //660 Hz Left 0ms delay
  delay2.delay(1, 1); //660 Hz Right 1ms delay
    Serial.println("Left Back");
  }
  if((IMU<=157) && (IMU>=113)){
  mixer1.gain(0, 0.7); //440 Hz Left
  mixer2.gain(0, 0.3); //440 Hz Right
  mixer1.gain(1, 0.09); //660 Hz Left 0.3
  mixer2.gain(1, 0.13); //660 Hz Right 0.7
  delay1.delay(0, 0.5); //440 Hz Left 1ms delay
  delay2.delay(0, 1); //440 Hz Right 0ms delay 
  delay1.delay(1, 1); //660 Hz Left 0ms delay
  delay2.delay(1, 0.5); //660 Hz Right 1ms delay
    Serial.println("Right Back");
  }
  if((IMU<=-23) && (IMU>=-67)){
  mixer1.gain(0, 0.4); //440 Hz Left
  mixer2.gain(0, 0.6); //440 Hz Right
  mixer1.gain(1, 0.1); //660 Hz Left 0.7
  mixer2.gain(1, 0.085); //660 Hz Right 0.5
  delay1.delay(0, 0.5); //440 Hz Left 1ms delay
  delay2.delay(0, 0); //440 Hz Right 0ms delay 
  delay1.delay(1, 0); //660 Hz Left 0ms delay
  delay2.delay(1, 0.5); //660 Hz Right 1ms delay
    Serial.println("Left Front");
  }  
  if((IMU>=23) && (IMU<=67)){
  mixer1.gain(0, 0.6); //440 Hz Left
  mixer2.gain(0, 0.4); //440 Hz Right
  mixer1.gain(1, 0.085); //660 Hz Left 0.5
  mixer2.gain(1, 0.1); //660 Hz Right 0.7
  delay1.delay(0, 0); //440 Hz Left 1ms delay
  delay2.delay(0, 0.5); //440 Hz Right 0ms delay 
  delay1.delay(1, 0.5); //660 Hz Left 0ms delay
  delay2.delay(1, 0); //660 Hz Right 1ms delay
    Serial.println("Right Front");
  } 
}
}

}


