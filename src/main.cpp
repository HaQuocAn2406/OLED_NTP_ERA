#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AiEsp32RotaryEncoder.h>

#define ENCODER_CLK 25
#define ENCODER_DT  26
#define ENCODER_SW  27 
#define ENCODER_VCC -1

#define ENCODER_STEPS 4
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCODER_CLK, ENCODER_DT,ENCODER_SW);
static const unsigned char PROGMEM image_hand_pointer_bits[] = {0x08,0x00,0x14,0x00,0x14,0x00,0x14,0x00,
0x14,0x00,0x16,0x00,0x15,0x80,0x15,0x60,0xd5,0x50,0x90,0x50,0x40,0x10,0x40,0x10,0x20,0x10,0x20,0x20,0x10,0x20,0x08,0x40};

int gio=0;
int phut=0;
int utc=0;
float offset;
int startIndex;
int endIndex;
int maxVisibleItems =4;
int numMenus=6;
bool rotatingDown;
float nhietdo;
int longpress;
int shortpress;
bool setting = false;
unsigned long lastRotaryChange;
static unsigned long lastTimeButtonDown = 0;
unsigned long shortPressAfterMiliseconds = 50;   
unsigned long longPressAfterMiliseconds = 2000;
unsigned long entersettingAfterMiliseconds = 3000;
unsigned long chagneModeAfterMiliseconds =1000;
int changemode=0;
static bool wasButtonDown = false;
bool onsubmenu=false;
int dem=0;
int dem_region=0;
int currentRotaryValue;
int previousRotaryValue;
String menus[] = {"Time Setting", "Region", "Calib", "Other", "Menu 5", "Menu 6"};
void displayMenu();
void maindisplay();
void rotary_loop();
void handle_rotary_button();
void on_button_short_click();
void on_button_long_click();
void time_setting();
void region();
void calib();

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  delay(2000);
  display.clearDisplay();

  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(-99999, 99999, true);
  rotaryEncoder.disableAcceleration();
}

void loop() {
  // put your main code here, to run repeatedly:
  rotary_loop();
  if(dem==0 && shortpress==1)
  {
    time_setting();
  }
  else if(dem==1 && shortpress==1)
  {
    region();
  }
  else if(dem==2 && shortpress==1)
  {
    calib();
  }
  else if (setting)
  {
    displayMenu();
  }
  while(!setting )
  {
    handle_rotary_button();
    maindisplay();
    onsubmenu=false;
    longpress=0;
    shortpress=0;
    dem=0;
    
  }
  Serial.print("Setting: ");
  Serial.println(setting);
}
void displayMenu()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,0);
  display.print("Menu");
  display.setCursor(60,0);
  display.print("utc: ");
  display.print(utc);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4,10);
  if (rotatingDown) {
    startIndex = dem - 2;
    endIndex = dem + 1;
  } else {
    startIndex = dem - 1;
    endIndex = dem + 2;
  }
  if (startIndex < 0) 
  {
      startIndex = 0;
      endIndex = (maxVisibleItems - 1);
  } 
  else if (endIndex >= numMenus) 
  {
      endIndex = numMenus - 1;
      startIndex = endIndex - (maxVisibleItems - 1);
  }
  for (int i = startIndex; i <= endIndex; i++) {
    if (i == dem) {
      display.drawRoundRect(0, (i-startIndex) * 12 + 10, 120, 12, 0, SSD1306_WHITE);
    }
    // dem_menu=menuIndex;
    display.setCursor(4, (i - startIndex) * 12 + 12);
    display.print(menus[i]);
    Serial.print("Dem: ");
    Serial.println(dem);
  }
  Serial.print("Start Index: ");
  Serial.println(startIndex);
  Serial.print("End Index: ");
  Serial.println(endIndex);
  display.display();
}
void maindisplay()
{
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(2);
  display.setCursor(28, 29);
  display.print(gio);
  display.setTextSize(1);
  display.setCursor(37, 6);
  display.print("EoH IoT");
  display.setTextSize(2);
  display.setCursor(51, 28);
  display.print(":");
  display.setCursor(70, 28);
  display.print(phut);
  display.setTextSize(1);
  display.setCursor(4, 51);
  display.print("Region: ");
  display.setCursor(51, 51);
  display.print(utc);
  display.display();
}

void rotary_loop()
{
  if (rotaryEncoder.encoderChanged())
	{
    currentRotaryValue = rotaryEncoder.readEncoder();
    lastRotaryChange=millis();
    if(!onsubmenu)
    {
      if(currentRotaryValue>previousRotaryValue)
      {
        rotatingDown=true;
        dem++;
        if(dem>5){dem=0;}
      }
      else
      {
        rotatingDown= false ;
        dem--;
        if(dem<0){dem=5;}
      }
    }
    if(onsubmenu)
    {
      if(dem==0)//Chinh gio
      {
        if(currentRotaryValue>previousRotaryValue)
        {
          if(changemode==0)
          {
            gio--;
            if(gio<0){gio=24;}
          }
          else{
            phut--;
            if(phut<0){phut=59;}
          }
        }   
        else
        {
          if(changemode==0)
          {
            gio++;
            if(gio>24){gio=00;}
          }
          else{
            phut++;
            if(phut>=60){phut=00;}
          }
        }
        
      }
      if(dem==1)
      {
        if(currentRotaryValue>previousRotaryValue)
        {
          rotatingDown=true;
          utc--;
          if(utc<-12){utc=12;}
        }
        else
        {
          rotatingDown= false ;
          utc++;
          if(utc>12){utc=-12;}
        }
      }
      if(dem==2)
      {
        if(currentRotaryValue>previousRotaryValue)
        {
          rotatingDown=true;
          offset=offset - 0.1;
          if(offset<-5){offset=5;}
        }
        else
        {
          rotatingDown= false ;
          offset=offset + 0.1;
          if(offset>5){offset=-5;}
        }
      }
    }

  Serial.println(dem);
  previousRotaryValue=currentRotaryValue;
	}
  handle_rotary_button();
}
void on_button_short_click() {
  Serial.print("button SHORT press ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
}

void on_button_long_click() {
  Serial.print("button LONG press ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
}
void on_button_changemode_click()
{
  Serial.print("button_changemode_click");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
}
void handle_rotary_button() {
  static unsigned long lastTimeButtonDown = 0;
  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
  //isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed
  if (isEncoderButtonDown) {
    Serial.print("+");  //REMOVE THIS LINE IF YOU DONT WANT TO SEE    
    if (!wasButtonDown) {
      //start measuring
      lastTimeButtonDown = millis();
    }
    //else we wait since button is still down
    wasButtonDown = true;
    return;
  }
  // button is up
  if (wasButtonDown) {
    //click happened, lets see if it was short click, long click or just too short
    if(millis()-lastTimeButtonDown>= entersettingAfterMiliseconds && onsubmenu==false)
    {
      setting=!setting;
    }
    else if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
      on_button_long_click();
      shortpress=0;
      onsubmenu=false;
      // displayMenu();
    }
    else if (millis() - lastTimeButtonDown >= chagneModeAfterMiliseconds) {
      on_button_changemode_click();
      changemode++;
      if(changemode>1){changemode=0;}

    }
     else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
      on_button_short_click();
      onsubmenu=true;
      shortpress=1;
    }
  }
  wasButtonDown = false;
}
void time_setting()
{
  rotary_loop();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30,0);
  display.print("Chinh Gio");
  nhietdo=temperatureRead();
  display.setCursor(20,10);
  if(changemode==0)
  {
    // display.drawLine(28, 35, 42, 35,SSD1306_WHITE);
    // display.setCursor(30,20);
    display.drawBitmap(30, 37, image_hand_pointer_bits, 12, 16, 1);
  }
  else
  {
    display.drawBitmap(80, 37, image_hand_pointer_bits, 12, 16, 1);
  }
  display.setTextSize(2);
  display.setCursor(30,20);
  display.print(gio);
  display.setCursor(55,20);
  display.print(":");
  display.setCursor(80,20);
  display.print(phut);
  display.display();
}
void region()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,0);
  display.print("Region");
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,20);
  display.setTextSize(1);
  display.print("UTC: ");
  display.setCursor(60,20);
  display.print(utc);
  display.display();
}
void calib()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,0);
  display.print("Calib");
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,20);
  display.setTextSize(1);
  display.print("OFFSET: ");
  display.setCursor(70,20);
  display.print(offset);
  display.display();
}