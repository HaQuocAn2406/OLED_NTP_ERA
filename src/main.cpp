/*
Giữ Nút 3s để chuyển qua lại giữ màn hình setting và màn hình chính
Xoay Để thay đổi giá trị
Nhấn để chuyển giữa các giá trị muốn thay đổi
5p sẽ lưu thời gian,setting hiện tại vào eeprom
Đã cmt eeprom do có hiện tượng tự reboot- Test lại 
*/
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AiEsp32RotaryEncoder.h>
#include <EEPROM.h>
#define ENCODER_CLK 25
#define ENCODER_DT  26
#define ENCODER_SW  27 
#define ENCODER_VCC -1
#define FLASH_MEMORY_SIZE 200
#define ENCODER_STEPS 4
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;

// hw_timer_t *timer0=NULL;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCODER_CLK, ENCODER_DT,ENCODER_SW);
static const unsigned char PROGMEM image_hand_pointer_bits[] = {0x08,0x00,0x14,0x00,0x14,0x00,0x14,0x00,
0x14,0x00,0x16,0x00,0x15,0x80,0x15,0x60,0xd5,0x50,0x90,0x50,0x40,0x10,0x40,0x10,0x20,0x10,0x20,0x20,0x10,0x20,0x08,0x40};

static const unsigned char PROGMEM image_network_www_bits[] = {0x03,0xc0,0x0d,0xb0,0x32,0x4c,0x24,0x24,0x44,0x22,0x7f,0xfe,
0x88,0x11,0x88,0x11,0x88,0x11,0x88,0x11,0x7f,0xfe,0x44,0x22,0x24,0x24,0x32,0x4c,0x0d,0xb0,0x03,0xc0};

static const unsigned char PROGMEM image_wifi_not_connected_bits[] = {0x21,0xf0,0x00,0x16,0x0c,0x00,0x08,0x03,0x00,0x25,0xf0,
0x80,0x42,0x0c,0x40,0x89,0x02,0x20,0x10,0xa1,0x00,0x23,0x58,0x80,0x04,0x24,0x00,0x08,0x52,0x00,0x01,
0xa8,0x00,0x02,0x04,0x00,0x00,0x42,0x00,0x00,0xa1,0x00,0x00,0x40,0x80,0x00,0x00,0x00};

static const unsigned char PROGMEM image_arrow_left_bits[] = {0x20,0x40,0xfe,0x40,0x20};


bool onsubmenu1=false;

int save=0;
int reset_sel= 0;
bool enable_reset= false;
bool auto_time_mode = false;
bool auto_region_mode = false;
int gio=0;
const int add_gio=0;
int phut;
const int add_phut=32;
int giay;
const int add_giay=64;
int utc;
const int add_utc=96;
float offset=0.00;
const int add_offset=128;
const int add_auto_time = 136;
const int add_auto_region = 144;
int startIndex;
int endIndex;
int maxVisibleItems =4;
int numMenus=6;
bool rotatingDown;
bool setting = false;
unsigned long lastRotaryChange;
static unsigned long lastTimeButtonDown = 0;
unsigned long shortPressAfterMiliseconds = 50;   
unsigned long longPressAfterMiliseconds = 2000;
unsigned long entersettingAfterMiliseconds = 3000;
unsigned long chagneModeAfterMiliseconds =1000;
unsigned long  currentMillis=millis();
unsigned long previousMillis= 0;
unsigned long interval =1000;
unsigned int changemode=0;
unsigned int changemode_auto_time=0;
unsigned int changemode_auto_region=0;

bool onsubmenu=false;
int dem=0;
int dem_region=0;
bool auto_time_sel;
int currentRotaryValue;
int previousRotaryValue;
String menus[] = {"Set Time", "Region", "Calib", "Auto Time", "DHT", "Hard Reset"};
void displayMenu();
void maindisplay();
void rotary_loop();
void handle_rotary_button();
void on_button_short_click();
void on_button_long_click();
void time_setting();
void region();
void calib();
void auto_time();
void kiem_tra_nut_nhan();
// void IRAM_ATTR timer0_ISR()// Timer Cho Đồng Hồ Offline
// {
//   // if(millis()-previousMillis>=300000){
//     // previousMillis=millis();
//     // EEPROM.writeInt(add_giay,giay);
//     // EEPROM.writeInt(add_gio,gio);
//     // EEPROM.writeInt(add_phut,phut);
//     // EEPROM.writeInt(add_utc,utc);
//     // EEPROM.writeFloat(add_offset,offset);
//     // EEPROM.writeBool(add_auto_time,auto_time_mode);
//     // EEPROM.writeBool(add_auto_region,auto_region_mode);
//     // EEPROM.commit();
//   // }
//   // giay++;
//   // Serial.println(giay);
//   // if(giay>=60)
//   // {
//   //   giay=0;
//   //   phut++;
//   //   if(phut>=60)
//   //   {
//   //     phut=0;
//   //     gio++;
//   //     if(gio>=24)
//   //     {
//   //       gio=0;
//   //     }
//   //   }
//   // }
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
  display.display();
  delay(2000);
  EEPROM.begin(FLASH_MEMORY_SIZE);
  phut=EEPROM.read(add_phut);
  gio=EEPROM.read(add_gio);
  utc=EEPROM.read(add_utc);
  offset=EEPROM.read(add_offset);
  auto_time_mode=EEPROM.read(add_auto_time);
  auto_region_mode=EEPROM.read(add_auto_region);
  // timer0=timerBegin(0,80,true);
  // timerAttachInterrupt(timer0,&timer0_ISR,true);
  // timerAlarmWrite(timer0,60000000,true);
  // timerAlarmEnable(timer0);
  display.clearDisplay();
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(-99999, 99999, true);
  rotaryEncoder.disableAcceleration();
}
void loop() {
  rotary_loop();
  // kiem_tra_nut_nhan();
  currentMillis=millis();
  if(millis()-previousMillis >=1000)
  {
    previousMillis=millis();
    save++;
    giay++;
    Serial.println(save);
    if(save==300)
    {
      save=0;
      EEPROM.writeInt(add_giay,giay);
      EEPROM.writeInt(add_gio,gio);
      EEPROM.writeInt(add_phut,phut);
      EEPROM.writeInt(add_utc,utc);
      EEPROM.writeFloat(add_offset,offset);
      EEPROM.writeBool(add_auto_time,auto_time_mode);
      EEPROM.writeBool(add_auto_region,auto_region_mode);
      EEPROM.commit();
    }
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
      display.drawRoundRect(0, (i-startIndex) * 12 + 10, 120, 12, 3, SSD1306_WHITE);
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
  display.setCursor(18, 5);
  display.print("ERa-IoT");
  display.drawBitmap(4, 44, image_network_www_bits, 16, 16, 1);
  if(gio>=10){display.setCursor(11, 26);}else{
    display.setCursor(16, 26);
  }
  display.print(gio);
  display.setCursor(38, 26);
  display.print(":");
  if(phut>=10){display.setCursor(53, 26);}else{
    display.setCursor(57, 26);
  }
  display.print(phut);
  display.setCursor(76, 26);
  display.print(":");
  display.setCursor(92, 26);
  display.print(giay);
  display.setTextSize(1);
  display.setCursor(23, 50);
  display.print(":");
  display.drawBitmap(104, 47, image_wifi_not_connected_bits, 19, 16, 1);
  display.setCursor(29, 50);
  display.print("UTC");
  if(utc>0){display.setCursor(48, 50);display.print("+");}
  display.setCursor(53, 50);
  display.print(utc);
  display.display();
}
void rotary_loop()
{
  kiem_tra_nut_nhan();
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
    else
    {
      switch (dem)
      {
      case 0:
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
            if(gio>24){gio=0;}
          }
          else{
            phut++;
            if(phut>=60){phut=0;}
          }
        }
        break;
      case 1:
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
        break;
      case 2:
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
        break;
      case 3:
        if(currentRotaryValue>previousRotaryValue)
        {
          if(auto_time_sel){
            auto_time_mode=!auto_time_mode;
          }else{
            auto_region_mode=!auto_region_mode;
          }
        }else{
          if(auto_time_sel){
            auto_time_mode=!auto_time_mode;
          }else{
            auto_region_mode=!auto_region_mode;
          }
        }
        break;
      case 5:
        if(currentRotaryValue>previousRotaryValue)
        {
          reset_sel++;
          if(reset_sel>2){reset_sel=1;}
        }else{
          reset_sel--;
          if(reset_sel<1){reset_sel=2;}
        }
        Serial.print(reset_sel);
        break;
      default:
        break;
      }
    }
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
      int x,y;
      for(x=0;x<20;x++)
      {
        display.clearDisplay();
        display.setTextColor(1);
        display.setTextSize(3);
        display.setCursor(24, 18);
        display.print("SAVED");
        display.display();
      }
      EEPROM.writeInt(add_giay,giay);
      EEPROM.writeInt(add_gio,gio);
      EEPROM.writeInt(add_phut,phut);
      EEPROM.writeInt(add_utc,utc);
      EEPROM.writeFloat(add_offset,offset);
      EEPROM.writeBool(add_auto_time,auto_time_mode);
      EEPROM.writeBool(add_auto_region,auto_region_mode);
      EEPROM.commit();
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
    if(lastTimeButtonDown && !isSettingpress && !isLongpress && setting) {
      Serial.print("button SHORT press ");
      onsubmenu=true;
      changemode=!changemode;
      auto_time_sel=!auto_time_sel;

      if(reset_sel==2&& dem == 5)
      {
        enable_reset = true;
      }else if(reset_sel==1&& dem==5){
        onsubmenu = false;
        reset_sel=0;
        displayMenu();
      }
    }
      isLongpress = false;
      isSettingpress = false;
      lastTimeButtonDown = 0;
  }
}
void time_setting()
{
  delay(50);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30,0);
  display.print("Set Time");
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
  onsubmenu1 =true;
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
void auto_time()
{
  String mode[]={"OFF","ON"};
  String auto_time[]={"Auto Time: ","Auto Region: "};
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(13, 4);
  display.print("Auto Time Setting");
  display.setCursor(4,16);
  display.print(auto_time[0]);
  display.setCursor(4,28);
  display.print(auto_time[1]);
  display.setCursor(64, 16);
  display.print(mode[auto_time_mode]);
  display.setCursor(93, 28);
  display.print(mode[auto_region_mode]);
  if(auto_time_sel){
    display.drawBitmap(116, 18 , image_arrow_left_bits, 7, 5, 1);
  }else{
    display.drawBitmap(116, 30, image_arrow_left_bits, 7, 5, 1);
  }
  display.display();
}
void hard_reset(){
  String choose[]={"","No","Yes"};
  display.clearDisplay();
  display.setTextColor(1);
  display.setCursor(16, 8);
  display.print("Hard Resetting...");
  display.setCursor(4, 28);
  display.print("Are You Sure?");
  display.setCursor(90, 28);
  display.print(choose[reset_sel]);
  if(enable_reset){
    display.clearDisplay();
    display.setTextColor(1);
    display.setCursor(31, 23);
    display.print("Resetting..");
    display.display();
    for(int i=0;i<FLASH_MEMORY_SIZE;i++)
    {
      EEPROM.write(i,0);
      EEPROM.commit();
    }
    delay(2000);
    ESP.restart();
  }
  display.display();
  }
void kiem_tra_nut_nhan()
{
  if(setting)
  {
    if(onsubmenu)
    {
      switch (dem)
      {
        case 0:
          time_setting();
          break;
        case 1:
          region();
          break;
        case 2:
          calib();
          break;
        case 3:
          auto_time();
          break;
        case 5:
          hard_reset();
          break;
        default:
          onsubmenu=false;
          break;
      }
    }
    else{displayMenu();}
  }
  else
  {
    handle_rotary_button();
    maindisplay();
    onsubmenu=false;
    dem=0; 
  }
}