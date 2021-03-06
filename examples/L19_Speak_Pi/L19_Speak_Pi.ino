/************************************************* ************************************************** ******
* OPEN-SMART Rich UNO R3 Lesson 19: Speak pi
NOTE!!! First of all you should download the voice resources from our google drive:
https://drive.google.com/drive/folders/0B6uNNXJ2z4CxaXFpakMxR0p1Unc?usp=sharing

Then unzip it and find the 01 and 02 folder and put them into your TF card (should not larger than 32GB). 

* You can learn how to play a number according to the value and the filename of digit.
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

#define MP3_RX 7
#define MP3_TX 8
MP3 mp3(MP3_RX, MP3_TX);
int8_t volume = 0x1a;//0~0x1e (30 adjustable level)
int8_t folderName = 2;//folder name must be 01 02 03 04 ...
int8_t fileName = 1; // prefix of file name must be 001xxx 002xxx 003xxx 004xxx ...
#define NUM_OFFSET 2//offset between number and file name, such as file name of 0 is 002, 1 is 003

const char pi[] = "3.14159265";
void setup()
{

}

void loop()
{
  uint8_t num = sizeof(pi);
  mp3.playWithFileName(folderName,39);//039 pi is
  for(uint8_t i = 0; i < num; i++)
  {
    speaknum(pi[i]);
  }
  while(1);
  
}

void speaknum(char c) {
  
  if ('0' <= c && c <= '9') {
    fileName = c - 0x30 + NUM_OFFSET;
  } 
  else if(c == '.')
  {
    fileName = 1;//001 point
  }
  else if (c != '.') {
    // error if not period
    return;
  }
 while(mp3.getStatus()!=STATUS_STOP)delay(50);
  mp3.playWithFileName(folderName,fileName);
 delay(100);
}

/*********************************************************************************************************
The end of file
*********************************************************************************************************/
