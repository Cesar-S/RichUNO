/************************************************* ************************************************** ******
* OPEN-SMART Rich UNO R3 Lesson 22: Speak time and temperature
NOTE!!! First of all you should download the voice resources from our google drive:
https://drive.google.com/drive/folders/0B6uNNXJ2z4CxaXFpakMxR0p1Unc?usp=sharing

Then unzip it and find the 01 and 02 folder and put them into your TF card (should not larger than 32GB). 

* This is integrated project. 
*  You can learn how to speak the time and temperature you get from the RTC module
   according to the value and the filename of digit /beep tone.
* Each time you touch the buzzer beep a time.
* The knob contorls the volume of the speak tone before you touch.
*
* buzzer.on();//turn on the buzzer
* buzzer.off();//turn off the buzzer

* The following functions are available:

* clock.begin(); // The clock is on and the function must be called first
* clock.getTime(void); / / read the clock and time, will be saved in the clock class variable

* touch.get(); / / return is the touch area corresponding Arduino pin number, if not then return -1

* knob.getAngle();//read the angle of the knob, the range of angle is 0~280 degrees

* timer1.initialize(unsigned long microseconds); // set the timed length, the unit is subtle
* timer1.attachInterrupt (TimingISR); // set interrupt routine function name, is the timing interrupt entry
* disp.init(); // initialization
* disp.display(int Decimal);   // display range: -999 ~ 9999
* disp.point (0); // Turn off the display colon and the next display takes effect
* disp.point (1); // Turn on the display colon and the next display takes effect

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
#include <TimerOne.h>

#include "RichUNOMP3.h"
#include "RichUNOTouchSensor.h"
#include "RichUNOTM1637.h"
#include "RichUNODS1307.h"
#include "RichUNOBuzzer.h"
#include "RichUNOLM75.h"
#include "RichUNOKnob.h"

#define KNOB_PIN A0
Knob knob(KNOB_PIN);

#define BUZZER_PIN 9          //buzzer connect to D9 of Arduino
Buzzer buzzer(BUZZER_PIN);

//note: LM75 object should be defined befor DS1307 object
LM75 temper;  // initialize an LM75 object "temper" for temperature 
DS1307 clock;//define a object of DS1307 class

#define CLK 10//CLK of the TM1637 IC connect to D10 of Arduino
#define DIO 11//DIO of the TM1637 IC connect to D11 of Arduino
TM1637 disp(CLK,DIO);

#define ON 1
#define OFF 0

int8_t Time[] = {0,0,0,0};
unsigned char ClockPoint = 1;
unsigned char Update;

#define TOUCH_OUT1 3//out1-TCH1 area corresponds to start speaker 
#define TOUCH_OUT2 4//out2-TCH2 area  not use now
#define TOUCH_OUT3 5//out3-TCH3 area not use now
#define TOUCH_OUT4 6//out4-TCH4 area not use now
TouchSensor touch(TOUCH_OUT1,TOUCH_OUT2, TOUCH_OUT3,TOUCH_OUT4);


#define MP3_RX 7
#define MP3_TX 8
MP3 mp3(MP3_RX, MP3_TX);
int8_t volume = 0x1a;//0~0x1e (30 adjustable level)
int8_t oldVolume;
int8_t newVolume;  //0~0x1e (30 adjustable level)

int8_t folderName = 2;//folder name must be 01 02 03 04 ...
int8_t fileName = 1; // prefix of file name must be 001xxx 002xxx 003xxx 004xxx ...099xxx
#define NUM_OFFSET 2//lonly for 0~19, offset between number and file name, such as file name of 0 is 002, 1 is 003
#define TEN_OFFSET 20//only for whole ten, offset between whole ten digit and file name, such as file name of 20 is 22, 30 is 23, 40 is 24

void setup()
{
  Wire.begin();
  disp.init();//The initialization of the display
  clock.begin();
  Timer1.initialize(500000);//timing for 500ms
  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR 
  int angle;
  angle = knob.getAngle();
  oldVolume = map(angle, 0, FULL_ANGLE, 0, 30);
  mp3.setVolume(oldVolume);
}

void loop()
{
  
  int button;
  uint8_t flag_speak_time = 0;//0 = not speak, 1 = to speak
  uint8_t flag_speak_temp = 0;//0 = not speak, 1 = to speak
  button = touch.get();
  if(button != -1) //if touch the area
  {
    delay(10);//delay for 10ms
    buzzer.on();
    if(button == TOUCH_OUT1) //if touch the TCH1 area
    {
      flag_speak_temp = 1;
    }
    else if(button == TOUCH_OUT2) //if touch the TCH2 area
    {
      flag_speak_time = 1;
    }
	delay(50);
	buzzer.off();
	while(button == touch.get());//Wait for the button to be released
  }
  if(Update == ON)//Update the display time, is to flash the clock porint
  {
    TimeUpdate();
    disp.display(Time);
  }
  updateVolume();
  if(flag_speak_time)
  {
    SpeakTime(Time);
	flag_speak_time = 0;
  }
  
  if(flag_speak_temp)
  {
    float celsius;
    celsius = temper.getTemperatue();//get temperature
    displayTemperature((int8_t)celsius);//
    SpeakTemp(celsius);
	flag_speak_temp = 0;
  }
    
}
void updateVolume()
{
  int angle;
  angle = knob.getAngle();
  newVolume = map(angle, 0, FULL_ANGLE, 0, 30); 
  if(newVolume != oldVolume) 
  {
  	mp3.setVolume(newVolume);
	oldVolume = newVolume;
  }
}
void SpeakTime(int8_t time[])
{
  uint8_t addr[10] = {0};
  uint8_t next = 0;
  addr[next++] = 31;//031 before speak time
  addr[next++] = 32;//032 opensmart time
  if(Time[0] < 2)
  {
    addr[next++] = Time[0]*10 + Time[1]+NUM_OFFSET;
  }
  else
  {
    addr[next++] = 22;//022 twenty
    if(Time[1] != 0) addr[next++] = Time[1] + NUM_OFFSET;
  }
  if((Time[2] == 0) && (Time[3] == 0)) addr[next++] = 33;//033 e clock
  else{
    if(Time[2] < 2){
  	  addr[next++] = Time[2]*10 + Time[3]+NUM_OFFSET;
  	}
	else
	{
	  addr[next++] = Time[2] + TEN_OFFSET;
	  if(Time[3] != 0)addr[next++] = Time[3] + NUM_OFFSET;
	}
  }
  SpeakGroup(addr, next);
}
void SpeakTemp(float temp)
{
  if(temp >= 1000)return;
  else if(temp <= -1000)return;
  
  uint8_t addr[10] = {0};
  uint8_t next = 0;
  addr[next++] = 31;//031 before speak time
  addr[next++] = 35;//035 opensmart temperature
  if(temp < 0)
  {
    temp = abs(temp);
	addr[next++] = 40;//040 minus
  }
  int t = temp;//Get the integer part of the temperature
  uint8_t flag_hundred;
  if(t >= 100)
  	{
  	  flag_hundred = 1;
  	  addr[next++] = t / 100 + NUM_OFFSET;//digit befor hundred
	  addr[next++] = 30;//030 hundred
	  t %= 100;
  	}
  else flag_hundred = 0;
  if(t != 0) 
  {
  	if(flag_hundred)addr[next++] = 38;//038 and
  	if(t < 20)
  	{
  	  addr[next++] = t + 2;
  	}
    else
  	{
  	  addr[next++] = t / 10 + 20;
	  t %= 10;
	  if(t != 0)addr[next++] = t + 2;
  	}
  }
 
  addr[next++] = 1;//001 point
 
  uint8_t subbit;
  subbit = ((int)(temp*10))%10;// 
  addr[next++] = subbit + 2; 
  addr[next++] = 36;//036 Degrees Celsius
  SpeakGroup(addr, next);
}
void SpeakGroup(uint8_t addr[], uint8_t size)//
{
  
  for(uint8_t i=0; i < size; i ++)
  {
    while(mp3.getStatus()!=STATUS_STOP)delay(50);
	mp3.playWithFileName(folderName,addr[i]);
  }
  while(mp3.getStatus()!=STATUS_STOP)delay(50);
}
/************************************************* *********************/
/* Function: Display temperature on 4-digit digital tube */
/* Parameter: -int8_t temperature, temperature range is -40 ~ 125 degrees celsius */
/* Return Value: void */

void displayTemperature(int8_t temperature)
{
  int8_t temp[4];
  if(temperature < 0)
	{
		temp[0] = INDEX_NEGATIVE_SIGN;
		temperature = abs(temperature);
	}
	else if(temperature < 100)temp[0] = INDEX_BLANK;
	else temp[0] = temperature/100;
	temperature %= 100;
	temp[1] = temperature / 10;
	temp[2] = temperature % 10;
	temp[3] = 12;	          //index of 'C' for celsius degree symbol.
	disp.point(0);
	disp.display(temp);
}

/**********Timer 1 interrupt routine*********/
void TimingISR()
{
  Update = ON;
  ClockPoint = !ClockPoint;
}

void TimeUpdate(void)
{
  if(ClockPoint)disp.point(POINT_ON);
  else disp.point(POINT_OFF);
  clock.getTime();
  Time[0] = clock.hour / 10;
  Time[1] = clock.hour % 10;
  Time[2] = clock.minute / 10;
  Time[3] = clock.minute % 10;
  Update = OFF;
}

/*********************************************************************************************************
The end of file
*********************************************************************************************************/

