#include <virtuabotixRTC.h>               //Include library for clock module
#include <Adafruit_PWMServoDriver.h>      //Include library for servo driver

Adafruit_PWMServoDriver pwmH = Adafruit_PWMServoDriver(0x40);    //Create an object of Hour driver
Adafruit_PWMServoDriver pwmM = Adafruit_PWMServoDriver(0x41);    //Create an object of Minute driver (A0 Address Jumper)

int servoFrequency = 50;      //Set servo operating frequency

int segmentHOn[14] = {165,300,295,280,312,230,295,257,380,245,150,350,230,315};   //On positions for each Hour servo
int segmentMOn[14] = {250,305,300,300,285,300,250,250,500,325,295,290,270,305};   //On positions for each Minute servo
int segmentHOff[14] = {350,110,495,455,100,480,495,437,130,485,335,150,450,100};    //Off positions for each Hour servo
int segmentMOff[14] ={410,160,480,450,105,500,465,420,300,500,435,105,490,480};    //Off positions for each Minute servo
int digits[10][7] = {{1,1,1,1,1,1,0},{0,1,1,0,0,0,0},{1,1,0,1,1,0,1},{1,1,1,1,0,0,1},{0,1,1,0,0,1,1},
                     {1,0,1,1,0,1,1},{1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};    //Position values for each digit

virtuabotixRTC myRTC(6, 7, 8);    //Create a clock object attached to pins 6, 7, 8 - CLK, DAT, RST
int hourTens = 0;                 //Create variables to store each 7 segment display numeral
int hourUnits = 0;
int minuteTens = 0;
int minuteUnits = 0;

int prevHourTens = 8;           //Create variables to store the previous numeral displayed on each
int prevHourUnits = 8;          //This is required to move the segments adjacent to the middle ones out of the way when they move
int prevMinuteTens = 8;
int prevMinuteUnits = 8;

int midOffset = 100;            //Amount by which adjacent segments to the middle move away when required

void setup() 
{
  pwmH.begin();                             //Start each board
  pwmM.begin();
  pwmH.setOscillatorFrequency(27000000);    //Set the PWM oscillator frequency, used for fine calibration
  pwmM.setOscillatorFrequency(27000000);
  pwmH.setPWMFreq(servoFrequency);          //Set the servo operating frequency
  pwmM.setPWMFreq(servoFrequency);
  //myRTC.setDS1302Time(00 , 35, 12, 5, 2, 10, 2025);        //Only required once to reset the clock time
  for(int i=0 ; i<=13 ; i++)    //Set all of the servos to on or up (88:88 displayed)
  {
    pwmH.setPWM(i, 0, segmentHOn[i]);
    delay(10);
    pwmM.setPWM(i, 0, segmentMOn[i]);
    delay(10);
  }
  delay(2000);
  
}

 void loop()
{
  myRTC.updateTime();                 //Update the time
  int temp = myRTC.hours;             //Get the hours and save to variable temp
  hourTens = temp / 10;               //Split hours into two digits, tens and units
  hourUnits = temp % 10;
  temp = myRTC.minutes;               //Get the minutes and save to variable temp
  minuteTens = temp / 10;             //Split minutes into two digits, tens and units
  minuteUnits = temp % 10;

  if(minuteUnits != prevMinuteUnits)  //If minute units has changed, update display
    updateDisplay();

  prevHourTens = hourTens;            //Update previous displayed numerals
  prevHourUnits = hourUnits;
  prevMinuteTens = minuteTens;
  prevMinuteUnits = minuteUnits;

  delay(500);
}

void updateDisplay ()                               //Function to update the displayed time
{
  updateMid();                                      //Move the segments out of the way of the middle segment and then move the middle segments
  for (int i=0 ; i<=5 ; i++)                        //Move the remaining segments
  {
    if(digits[hourTens][i]==1)                      //Update the hour tens
      pwmH.setPWM(i+7, 0, segmentHOn[i+7]);
    else
      pwmH.setPWM(i+7, 0, segmentHOff[i+7]);
    delay(10);
    if(digits[hourUnits][i]==1)                     //Update the hour units
      pwmH.setPWM(i, 0, segmentHOn[i]);
    else
      pwmH.setPWM(i, 0, segmentHOff[i]);
    delay(10);
    if(digits[minuteTens][i]==1)                    //Update the minute tens
      pwmM.setPWM(i+7, 0, segmentMOn[i+7]);
    else
      pwmM.setPWM(i+7, 0, segmentMOff[i+7]);
    delay(10);
    if(digits[minuteUnits][i]==1)                   //Update the minute units
      pwmM.setPWM(i, 0, segmentMOn[i]);
    else
      pwmM.setPWM(i, 0, segmentMOff[i]);
    delay(10);
  }
}

void updateMid()                                              //Function to move the middle segements and adjacent ones out of the way
{
  if(digits[minuteUnits][6]!=digits[prevMinuteUnits][6])      //Move adjacent segments for Minute units
  {
    if(digits[prevMinuteUnits][1]==1)
      pwmM.setPWM(1, 0, segmentMOn[1]-midOffset);
    if(digits[prevMinuteUnits][6]==1)
      pwmM.setPWM(5, 0, segmentMOn[5]+midOffset);
  }
  delay(100);                                                 //Delay allows adjacent segments to move before moving middle
  if(digits[minuteUnits][6]==1)                               //Move Minute units middle segment if required
    pwmM.setPWM(6, 0, segmentMOn[6]);
  else
    pwmM.setPWM(6, 0, segmentMOff[6]);
  if(digits[minuteTens][6]!=digits[prevMinuteTens][6])        //Move adjacent segments for Minute tens
  {
    if(digits[prevMinuteTens][1]==1)
      pwmM.setPWM(8, 0, segmentMOn[8]-midOffset);
    if(digits[prevMinuteTens][6]==1)
      pwmM.setPWM(12, 0, segmentMOn[12]+midOffset);
  }
  delay(100);                                                 //Delay allows adjacent segments to move before moving middle
  if(digits[minuteTens][6]==1)                                //Move Minute tens middle segment if required
    pwmM.setPWM(13, 0, segmentMOn[13]);
  else
    pwmM.setPWM(13, 0, segmentMOff[13]);
  if(digits[hourUnits][6]!=digits[prevHourUnits][6])          //Move adjacent segments for Hour units
  {
    if(digits[prevHourUnits][1]==1)
      pwmH.setPWM(1, 0, segmentHOn[1]-midOffset);
    if(digits[prevHourUnits][6]==1)
      pwmH.setPWM(5, 0, segmentHOn[5]+midOffset);
  }
  delay(100);                                                 //Delay allows adjacent segments to move before moving middle
  if(digits[hourUnits][6]==1)                                 //Move Hour units middle segment if required
    pwmH.setPWM(6, 0, segmentHOn[6]);
  else
    pwmH.setPWM(6, 0, segmentHOff[6]);
  if(digits[hourTens][6]!=digits[prevHourTens][6])            //Move adjacent segments for Hour tens
  {
    if(digits[prevHourTens][1]==1)
      pwmH.setPWM(8, 0, segmentHOn[8]-midOffset);
    if(digits[prevHourTens][6]==1)
      pwmH.setPWM(12, 0, segmentHOn[12]+midOffset);
  }
  delay(100);                                                 //Delay allows adjacent segments to move before moving middle
  if(digits[hourTens][6]==1)                                  //Move Hour tens middle segment if required
    pwmH.setPWM(13, 0, segmentHOn[13]);
  else
    pwmH.setPWM(13, 0, segmentHOff[13]);
}
