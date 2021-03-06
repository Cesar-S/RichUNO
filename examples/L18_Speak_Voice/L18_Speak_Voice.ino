/************************************************* ************************************************** ******
* OPEN-SMART Rich UNO R3 Lesson 18: Speak voice
NOTE!!! First of all you should download the voice resources from our google drive:
https://drive.google.com/drive/folders/0B6uNNXJ2z4CxaXFpakMxR0p1Unc?usp=sharing

Then unzip it and find the 01 and 02 folder and put them into your TF card (should not larger than 32GB). 

* You can learn know all the voice sources in the 02 folder so you can make your voice broadcast project.

*
* The following functions are available:
*
/--------basic operations---------------/
mp3.play();
mp3.pause();
mp3.nextSong();
mp3.previousSong();
mp3.volumeUp();
mp3.volumeDown();
mp3.forward();    //fast forward
mp3.rewind();     //fast rewind
mp3.stopPlay();  
mp3.stopInject(); //when you inject a song, this operation can stop it and come back to the song befor you inject
mp3.singleCycle();//it can be set to cycle play the currently playing song 
mp3.allCycle();   //to cycle play all the songs in the TF card
/--------------------------------/

mp3.playWithIndex(int8_t index);//play the song according to the physical index of song in the TF card

mp3.injectWithIndex(int8_t index);//inject a song according to the physical index of song in the TF card when it is playing song.

mp3.setVolume(int8_t vol);//vol is 0~0x1e, 30 adjustable level

mp3.playWithFileName(int8_t directory, int8_t file);//play a song according to the folder name and prefix of its file name
                                                            //foler name must be 01 02 03...09 10...99
                                                            //prefix of file name must be 001...009 010...099

mp3.playWithVolume(int8_t index, int8_t volume);//play the song according to the physical index of song in the TF card and the volume set

mp3.cyclePlay(int16_t index);//single cycle play a song according to the physical index of song in the TF

mp3.playCombine(int16_t folderAndIndex[], int8_t number);//play combination of the songs with its folder name and physical index
      //folderAndIndex: high 8bit is folder name(01 02 ...09 10 11...99) , low 8bit is index of the song
      //number is how many songs you want to play combination

************************************************** **************************************************/

#include <Wire.h>
#include <SoftwareSerial.h>
#include "RichUNOMP3.h"
#include "RichUNOTouchSensor.h"
#include "RichUNOTM1637.h"
#define CLK 10//CLK of the TM1637 IC connect to D10 of Arduino
#define DIO 11//DIO of the TM1637 IC connect to D11 of Arduino
TM1637 disp(CLK,DIO);

#define TOUCH_OUT1 3//out1-TCH1 area corresponds to the number increase
#define TOUCH_OUT2 4//out2-TCH2 area corresponds to the number decrease
#define TOUCH_OUT3 5//out3-TCH3 area corresponds to play again
#define TOUCH_OUT4 6//out4-TCH4 area not use now
TouchSensor touch(TOUCH_OUT1,TOUCH_OUT2, TOUCH_OUT3,TOUCH_OUT4);

#define MP3_RX 7
#define MP3_TX 8
MP3 mp3(MP3_RX, MP3_TX);

int8_t volume = 0x1a;//0~0x1e (30 adjustable level)
int8_t folderName = 2;//folder name must be 01 02 03 04 ...
int8_t fileName = 1; // prefix of file name must be 001xxx 002xxx 003xxx 004xxx ...
int8_t maxNum = 40;//we just supply 49 number of voice on our google drive
int8_t minNum = 1;//minimum number should be 1 but not 0
void setup()
{
  mp3.setVolume(volume);
  disp.init();//The initialization of the display
  delay(50);//you should wait for >=50ms between two commands
}

void loop()
{
  int button;
  uint8_t flag_speak = 0;//0 = not speak, 1 = to speak
  button = touch.get();
  if(button != -1) //if touch the area
  {
    delay(10);//delay for 10ms
    if(button == TOUCH_OUT1) //if touch the TCH1 area
    {
      if(fileName < maxNum)
      {
        flag_speak = 1;
        fileName++;
      }
    }
    else if(button == TOUCH_OUT2) //if touch the TCH2 area
    {
      if(fileName > minNum)
      {
        flag_speak = 1;
        fileName--;
      }
    }
    else if(button == TOUCH_OUT3) //if touch the TCH3 area
    {
      flag_speak = 1;
    }
    else if(button == TOUCH_OUT4) //if touch the TCH4 area
    {
    }
	while(button == touch.get());//Wait for the button to be released
  }
  if(button!=-1)disp.display(fileName);

  if(flag_speak)
  {
    mp3.playWithFileName(folderName,fileName);
	flag_speak = 0;
  }
}

/*********************************************************************************************************
The end of file
*********************************************************************************************************/
