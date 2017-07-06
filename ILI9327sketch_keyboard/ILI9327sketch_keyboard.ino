//Работает очень медленно!

// Тест совместной работы UTFT и тача TFTLCD
// Arduino Uno R3 3.5 inch TFTLCD 8bit
// Правка в файле TouchScreen.cpp:
/////////////////////////////////////////////////////////////////////////////
// Point TouchScreen::getPoint(void) {
//    byte c = DDRC; // <a href="http://arduino.ru/forum/apparatnye-voprosy/tft-35-i-arduino?page=3#comment-73960" title="http://arduino.ru/forum/apparatnye-voprosy/tft-35-i-arduino?page=3#comment-73960" rel="nofollow">http://arduino.ru/forum/apparatnye-voprosy/tft-35-i-arduino?page=3#comme...</a>
// ...........
//    DDRC = c; //restore port c directional settings
//   return Point(x, y, z);
//}
/////////////////////////////////////////////////////////////////////////////
// UTouch_ButtonTest (C)2010-2012 Henning Karlsen
// web: <a href="<a href="http://www.henningkarlsen.com/electronics" rel="nofollow">http://www.henningkarlsen.com/electronics</a>" title="<a href="http://www.henningkarlsen.com/electronics" rel="nofollow">http://www.henningkarlsen.com/electronics</a>" rel="nofollow"><a href="http://www.henningkarlsen.com/electronics" rel="nofollow">http://www.henningkarlsen.com/electronics</a></a>
//
// This program is a quick demo of how create and use buttons.
//
// This program requires the UTFT library.
//
// It is assumed that the display module is connected to an
// appropriate shield or that you know how to change the pin 
// numbers in the setup.
//

#include <UTFT.h>
#include <stdint.h>
#include "TouchScreen.h"

bool Debug = false; //режим отладки

// These are the pins for the shield!
#define YP 15  // must be an analog pin, use "An" notation!
#define XM 10  // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin
#define XP 9   // can be a digital pin

#define MINPRESSURE 2
#define MAXPRESSURE 1000

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


// Declare which fonts we will be using
extern uint8_t BigFont[];

// Uncomment the next two lines for the Arduino 2009/UNO
//UTFT        myGLCD(ITDB24D,19,18,17,16);   // Remember to change the model parameter to suit your display module!
//UTouch      myTouch(15,10,14,9,8);

// Uncomment the next two lines for the Arduino Mega
//UTFT        myGLCD(ITDB32S, 38,39,40,41);   // Remember to change the model parameter to suit your display module!
UTFT myGLCD(ILI9327, A2,A1,A3,A4);

int x, y;
char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";

/*************************
**   Custom functions   **
*************************/

void drawButtons()
{
// Draw the upper row of buttons
  for (x=0; x<5; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*60), 10, 60+(x*60), 60);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*60), 10, 60+(x*60), 60);
    myGLCD.printNumI(x+1, 27+(x*60), 27);
  }
// Draw the center row of buttons
  for (x=0; x<5; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*60), 70, 60+(x*60), 120);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*60), 70, 60+(x*60), 120);
    if (x<4)
      myGLCD.printNumI(x+6, 27+(x*60), 87);
  }
  myGLCD.print("0", 267, 87);
// Draw the lower row of buttons
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (10, 130, 150, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 130, 150, 180);
  myGLCD.print("Clear", 40, 147);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (160, 130, 300, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (160, 130, 300, 180);
  myGLCD.print("Enter", 190, 147);
  myGLCD.setBackColor (0, 0, 0);
}

void updateStr(int val)
{
  if (stCurrentLen<20)
  {
    stCurrent[stCurrentLen]=val;
    stCurrent[stCurrentLen+1]='\0';
    stCurrentLen++;
    myGLCD.setColor(0, 255, 0);
    myGLCD.print(stCurrent, LEFT, 224);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.print("BUFFER FULL!", CENTER, 192);
    delay(500);
    myGLCD.print("            ", CENTER, 192);
    delay(500);
    myGLCD.print("BUFFER FULL!", CENTER, 192);
    delay(500);
    myGLCD.print("            ", CENTER, 192);
    myGLCD.setColor(0, 255, 0);
  }
}

// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
if(Debug)
 { Serial.print("\tx1:");  Serial.print(x1);
  Serial.print("\ty1");Serial.print(y1);
  Serial.print("\tx2");Serial.print(x2);
  Serial.print("\ty2");Serial.println(y2);}

myGLCD.setColor(255, 255, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  delay(500);
  TSPoint p = ts.getPoint();
  while      (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    TSPoint p = ts.getPoint();

  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  }

/*************************
**  Required functions  **
*************************/
void setup()
{
 if (Debug) { Serial.begin(9600);}
// Initial setup
  myGLCD.InitLCD();
  myGLCD.clrScr();

//  myTouch.InitTouch();
//  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 255);
  drawButtons();  
}

void loop()
{
  while (true)
  {
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
//    if (myTouch.dataAvailable())
    {
/*
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
*/
      x=(927-p.x)>>1;
if (Debug)  {  Serial.print("p,x:");Serial.print(x);}
      y=int((p.y-150)/3); // подгоняем
if (Debug)  {  Serial.print("\ty:");Serial.println(y);}

      if ((y>=10) && (y<=60))  // Upper row
      {
        if ((x>=10) && (x<=60))  // Button: 1
        {
          waitForIt(10, 10, 60, 60);
          updateStr('1');
        }
        if ((x>=70) && (x<=120))  // Button: 2
        {
          waitForIt(70, 10, 120, 60);
          updateStr('2');
        }
        if ((x>=130) && (x<=180))  // Button: 3
        {
          waitForIt(130, 10, 180, 60);
          updateStr('3');
        }
        if ((x>=190) && (x<=240))  // Button: 4
        {
          waitForIt(190, 10, 240, 60);
          updateStr('4');
        }
        if ((x>=250) && (x<=300))  // Button: 5
        {
          waitForIt(250, 10, 300, 60);
          updateStr('5');
        }
      }

      if ((y>=70) && (y<=120))  // Center row
      {
        if ((x>=10) && (x<=60))  // Button: 6
        {
          waitForIt(10, 70, 60, 120);
          updateStr('6');
        }
        if ((x>=70) && (x<=120))  // Button: 7
        {
          waitForIt(70, 70, 120, 120);
          updateStr('7');
        }
        if ((x>=130) && (x<=180))  // Button: 8
        {
          waitForIt(130, 70, 180, 120);
          updateStr('8');
        }
        if ((x>=190) && (x<=240))  // Button: 9
        {
          waitForIt(190, 70, 240, 120);
          updateStr('9');
        }
        if ((x>=250) && (x<=300))  // Button: 0
        {
          waitForIt(250, 70, 300, 120);
          updateStr('0');
        }
      }

      if ((y>=130) && (y<=180))  // Upper row
      {
        if ((x>=10) && (x<=150))  // Button: Clear
        {
          waitForIt(10, 130, 150, 180);
          stCurrent[0]='\0';
          stCurrentLen=0;
          myGLCD.setColor(0, 0, 0);
          myGLCD.fillRect(0, 224, 319, 239);
        }
        if ((x>=160) && (x<=300))  // Button: Enter
        {
          waitForIt(160, 130, 300, 180);
          if (stCurrentLen>0)
          {
            for (x=0; x<stCurrentLen+1; x++)
            {
              stLast[x]=stCurrent[x];
            }
            stCurrent[0]='\0';
            stCurrentLen=0;
            myGLCD.setColor(0, 0, 0);
            myGLCD.fillRect(0, 208, 319, 239);
            myGLCD.setColor(0, 255, 0);
            myGLCD.print(stLast, LEFT, 208);
          }
          else
          {
            myGLCD.setColor(255, 0, 0);
            myGLCD.print("BUFFER EMPTY", CENTER, 192);
            delay(500);
            myGLCD.print("            ", CENTER, 192);
            delay(500);
            myGLCD.print("BUFFER EMPTY", CENTER, 192);
            delay(500);
            myGLCD.print("            ", CENTER, 192);
            myGLCD.setColor(0, 255, 0);
          }
        }
      }
    }
  }
}
