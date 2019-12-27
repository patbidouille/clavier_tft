/* Mod-aff
 * Afficheur d'un clavier 
 * avec possibilité de gérer un menu avec des flêches.
 */

/*
/*______Import Libraries_______*/
#include <stdint.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>   // Hardware-specific library
#include <TouchScreen.h> // touch control
MCUFRIEND_kbv tft;
/*______End of Libraries_______*/


#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#include <FreeDefaultFonts.h>

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

/*______Assign names to colors and pressure_______*/
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410
#define MINPRESSURE 10
#define MAXPRESSURE 1000
/*_______Assigned______*/

/*____Calibrate TFT LCD_____*/
#define TS_MINX 125
#define TS_MINY 85
#define TS_MAXX 965
#define TS_MAXY 905
/*______End of Calibration______*/

String symbol[4][4] = {
  { "7", "8", "9", "^" },
  { "4", "5", "6", "<" },
  { "1", "2", "3", ">" },
  { "C", "0", "OK", "v" }
};
 int X,Y;
 long Num1,Num2,Number;
 char action;
 boolean result = false;
 boolean alarme = false;

 uint8_t hh, mm, ss; //containers for current time
 uint8_t conv2d(const char* p)
 {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9') v = *p - '0';
    return 10 * v + *++p - '0';
 }
 int width, height;
 
 

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.println("Mod-Hall");
    Serial.print("found ID = 0x");
    Serial.println(ID, HEX);
    if (ID == 0xD3D3) ID = 0x9481; //force ID if write-only display
    //ID=0x9486;
    tft.begin(ID);
    tft.setRotation(0);
    tft.fillScreen(BLACK);

    width = tft.width();
    height = tft.height();  
    Serial.print(tft.width());
    Serial.print(F(" x "));
    Serial.println(tft.height());
    //IntroScreen();
    delay(1000);
    hh = conv2d(__TIME__);
    mm = conv2d(__TIME__ + 3);
    ss = conv2d(__TIME__ + 6);        
    
    draw_BoxNButtons();
}

void loop(void)
{
    
    //Aff_heure();
    TSPoint p = waitTouch();
    Y = p.y; X = p.x;   
    Serial.print("X="); Serial.print(X);
    Serial.print(", Y="); Serial.println(Y);    
    //if draw_BoxNButtons();
    DetectButtons();
}

TSPoint waitTouch() {
  TSPoint p;
  do {
    p = ts.getPoint(); 
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 240);;
  return p;
  /* ATTENTION !
   *  Bug dans la conf, elle est défini pour un écran 320x240
   *  le mien fait 480x320 mais j'avais la flemme de recalculer la détection
   */
}

void Aff_heure() {
  while (1) {
    if (++ss > 59) {
      ss = 0;
      mm++;
      if (mm > 59) {
        mm = 0;
        hh++;
        if (hh > 23) hh = 0;
      }
    }
    char buf[20];
    sprintf(buf, "%02d:%02d:%02d", hh, mm, ss);
    tft.fillRect(108, 10 * 18 + 3, 6 * 18, 18, GRAY);
    tft.setCursor(128, 187);
    tft.print(buf);
    delay(1000);
    }  
}

void DetectButtons()
{
  Serial.println ("Detection bouton");
   Serial.print("X="); Serial.print(X);
   Serial.print(", Y="); Serial.println(Y);
  
  if (Y<243 && Y>184) //Detecting Buttons on Column 1
  {
    Serial.println("colonne 1");
    if (X>253 && X<308) //If cancel Button is pressed
    {Serial.println ("Button Cancel"); action="c"; result=false;}
    
     if (X>196 && X<253) //If Button 1 is pressed
    {Serial.println ("Button 1"); Number=1; }
    
     if (X>137 && X<196) //If Button 4 is pressed
    {Serial.println ("Button 4"); Number=4; }
    
     if (X>87 && X<138) //If Button 7 is pressed
    {Serial.println ("Button 7"); Number=7; } 
  }

    if (Y<184 && Y>123) //Detecting Buttons on Column 2
  {
    if (X>253 && X<308) //If cancel Button is pressed
    {Serial.println ("Button 0"); Number=0; }
    
     if (X>196 && X<253) //If Button 1 is pressed
    {Serial.println ("Button 2"); Number=2; }
    
     if (X>137 && X<196) //If Button 4 is pressed
    {Serial.println ("Button 5"); Number=5; }
    
     if (X>87 && X<137) //If Button 7 is pressed
    {Serial.println ("Button 8"); Number=8; } 
  }

    if (Y<123 && Y>64) //Detecting Buttons on Column 3
  {
    if (X>253 && X<308) //If cancel Button is pressed
    {Serial.println ("Button OK"); action="o"; result=true;}
    
     if (X>196 && X<253) //If Button 1 is pressed
    {Serial.println ("Button 3"); Number=3; }
    
     if (X>137 && X<196) //If Button 4 is pressed
    {Serial.println ("Button 6"); Number=6; }
    
     if (X>87 && X<137) //If Button 7 is pressed
    {Serial.println ("Button 9"); Number=9; }   
    
  }

    if (Y<64 && Y>0) //Detecting Buttons on Column 4
  {
    Num1 = Number;    
    Number =0;
    tft.setCursor(200, 20);
    tft.setTextColor(RED);
    if (X>253 && X<308)
    {Serial.println ("Bas"); action = "b"; tft.println('v');}
     if (X>196 && X<253)
    {Serial.println ("Droite"); action = "d"; tft.println('>');}
     if (X>137 && X<196)
    {Serial.println ("Gauche"); action = "g"; tft.println('<');}
     if (X>87 && X<137)
    {Serial.println ("Haut"); action = "h"; tft.println('^');}  
  } 
  delay(300); 
}

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.drawFastHLine(0, y, tft.width(), WHITE);
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(GREEN);
    tft.setTextSize(sz);
    tft.print(msg);
    delay(1000);
}


void IntroScreen()
{
  tft.setCursor (65, 120);
  tft.setTextSize (3);
  tft.setTextColor(RED);
  tft.println("Clavier numérique");
  tft.setCursor (40, 160);
  tft.println("Bienvenue");
  tft.setCursor (40, 220);
  tft.setTextSize (2);
  tft.setTextColor(BLUE);
  tft.println("Patbidouille");
  delay(1800);
}

void draw_BoxNButtons()
{
  // On met tous en BLACK
  tft.fillScreen(BLACK);
  
  //Draw the Result Box
  tft.fillRect(0,0,width,160,GRAY);

 //Draw First Column
  tft.fillRect  (0,400,80,80,RED);
  /*tft.fillRect  (0,400,80,80,BLACK);
  tft.fillRect  (0,320,80,80,BLACK);
  tft.fillRect  (0,240,80,80,BLACK);*/

 //Draw Third Column  
  tft.fillRect  (160,400,80,80,GREEN);
  /*tft.fillRect  (160,400,80,80,RED);
  tft.fillRect  (160,320,80,80,GRAY);
  tft.fillRect  (160,240,80,80,BLACK);*/

  //Draw Secound & Fourth Column  
  for (int b=480; b>=160; b-=80)
 { tft.fillRect  (240,b,80,80,BLUE); 
   tft.fillRect  (80,b,80,80,BLACK);}

  //Draw Horizontal Lines
  for (int h=160; h<=400; h+=80)
  tft.drawFastHLine(0, h, 320, WHITE);
  tft.drawFastVLine(0, 479, 320, WHITE);

  //Draw Vertical Lines
  for (int v=0; v<=240; v+=80)
  tft.drawFastVLine(v, 160, 320, WHITE);
  tft.drawFastVLine(239, 160, 320, WHITE);

  //Display keypad lables 
  for (int j=0;j<4;j++) {
    for (int i=0;i<4;i++) {
      tft.setCursor(25 + (80*i), 185 + (80*j));
      tft.setTextSize(3);
      tft.setTextColor(WHITE);
      tft.println(symbol[j][i]);
    }
  }
}
