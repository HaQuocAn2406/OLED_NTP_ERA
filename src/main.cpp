/*
Giữ Nút 3s để chuyển qua lại giữ màn hình setting và màn hình chính
Nhấn để chọn các chế độ
Giữ để lưu và quay lại màn hình setting
*/
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

hw_timer_t *timer0=NULL;

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
int shortpress;
bool setting = false;
unsigned long lastRotaryChange;
static unsigned long lastTimeButtonDown = 0;
unsigned long shortPressAfterMiliseconds = 50;   
unsigned long longPressAfterMiliseconds = 2000;
unsigned long entersettingAfterMiliseconds = 3000;
unsigned long chagneModeAfterMiliseconds =1000;

unsigned long previousMillis= 0;
unsigned long interval =1000;
int changemode=0;
bool onsubmenu=false;
int dem=0;
int dem_region=0;
int currentRotaryValue;
int previousRotaryValue;
int giay=0;
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
// void IRAM_ATTR timer0_ISR()
// {
//   giay++;
//   Serial1.print(giay);
//   if(giay>=60)
//   {
//     giay=0;
//     phut++;
//     if(phut>=60)
//     {
//       phut=0;
//       gio++;
//       if(gio>=24)
//       {
//         gio=0;
//       }
//     }
//   }
// }
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
  // timer0=timerBegin(0,80,true);
  // timerAttachInterrupt(timer0,&timer0_ISR,true);
  // timerAlarmWrite(timer0,409600,true);
  // timerAlarmEnable(timer0);
  display.display();
  delay(2000);
  display.clearDisplay();

  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(-99999, 99999, true);
  rotaryEncoder.disableAcceleration();
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Number of core: ");
  Serial.println(ESP.getChipCores());
}

void loop() {
  // put your main code here, to run repeatedly:
  rotary_loop();
  unsigned long currentMillis = millis();
  if(currentMillis-previousMillis >=interval)
  {
    previousMillis=currentMillis;
    giay++;
    Serial.println(giay);
    if(giay>=60)
    {
      giay=0;
      phut++;
      if(phut>=60)
      {
        phut=0;
        gio++;
        if(gio>=24)
        {
          gio=0;
        }
      }
    }
  }

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
  if(!setting )
  {
    handle_rotary_button();
    maindisplay();
    onsubmenu=false;
    shortpress=0;
    dem=0;
    
  }
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
  }
  display.display();
}
void maindisplay()
{
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(2);
  display.setCursor(19, 26);
  display.print(gio);
  display.setCursor(39, 26);
  display.print(":");
  display.setCursor(57, 26);
  display.print(phut);
  display.setCursor(76, 26);
  display.print(":");
  display.setCursor(93, 26);
  display.print(giay);
  display.setCursor(20, 4);
  display.print("ERA-IOT");
  display.setTextSize(1);
  display.setCursor(5, 52);
  display.print("Region: ");
  display.setCursor(50, 52);
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
      if(dem==1)//Chinh Region
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
      if(dem==2)// Chỉnh Calib
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

  // Serial.println(dem);
  previousRotaryValue=currentRotaryValue;
	}
  handle_rotary_button();
}
void handle_rotary_button() {
  static unsigned long lastTimeButtonDown = 0;
  unsigned long time_clicked=0;
  static bool isLongpress = false;
  static bool isSettingpress = false;
  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
  if (isEncoderButtonDown)//
  {    
    if (!lastTimeButtonDown) {
      //start measuring 
      lastTimeButtonDown = millis();
    }
    if(!isLongpress && (millis() - lastTimeButtonDown >= longPressAfterMiliseconds)&& onsubmenu)
    {
      Serial.print("button LONG press ");
      isLongpress =true;
      shortpress=0;
      display.clearDisplay();
      display.setTextColor(1);
      display.setTextSize(3);
      display.setCursor(24, 18);
      display.print("SAVED");
      display.display();
      delay(1000);
      onsubmenu=false;
    }
    else 
    {
      if(!isSettingpress && onsubmenu==false && (millis()-lastTimeButtonDown>= entersettingAfterMiliseconds))
      {
        Serial.print("Setting Press");
        setting=!setting;
        isSettingpress = true;
        
      }
    }
  }
  else
  {
    if(lastTimeButtonDown && !isSettingpress && !isLongpress) {
      Serial.print("button SHORT press ");
      onsubmenu=true;
      shortpress=1;
      changemode=!changemode;
    }
    isLongpress = false;
    isSettingpress = false;
    lastTimeButtonDown = 0;
  }
}
void time_setting()
{
  rotary_loop();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30,0);
  display.print("Chinh Gio");
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