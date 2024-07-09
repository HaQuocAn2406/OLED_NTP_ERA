/*
Giữ Nút 3s để chuyển qua lại giữ màn hình setting và màn hình chính
Xoay Để thay đổi giá trị
Nhấn để chuyển giữa các giá trị muốn thay đổi
5p sẽ lưu thời gian,setting hiện tại vào eeprom
*/
#define ERA_LOCATION_VN
#define ERA_AUTH_TOKEN "2a377e27-cf9a-4061-ba71-bdcedde02e64"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <AiEsp32RotaryEncoder.h>
#include <EEPROM.h>
#include <ERa.hpp>
#include <Time/ERaEspTime.hpp>
#include <WiFiManager.h> 
#include <string.h>
#include<iostream>
#include <qrcode.h>
#define ENCODER_CLK 25
#define ENCODER_DT  26
#define ENCODER_SW  27 
#define ENCODER_VCC -1
#define FLASH_MEMORY_SIZE 200
#define ERA_DEBUG
#define ERA_SERIAL Serial
#define ENCODER_STEPS 4


const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;

WiFiManager wm;

ERaEspTime syncTime;
TimeElement_t ntpTime;

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   // 
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AiEsp32RotaryEncoder rotaryEncoder(ENCODER_CLK, ENCODER_DT,ENCODER_SW);
static const unsigned char PROGMEM image_hand_pointer_bits[] = {0x08,0x00,0x14,0x00,0x14,0x00,0x14,0x00,
0x14,0x00,0x16,0x00,0x15,0x80,0x15,0x60,0xd5,0x50,0x90,0x50,0x40,0x10,0x40,0x10,0x20,0x10,0x20,0x20,0x10,0x20,0x08,0x40};

static const unsigned char PROGMEM image_network_www_bits[] = {0x03,0xc0,0x0d,0xb0,0x32,0x4c,0x24,0x24,0x44,0x22,0x7f,0xfe,
0x88,0x11,0x88,0x11,0x88,0x11,0x88,0x11,0x7f,0xfe,0x44,0x22,0x24,0x24,0x32,0x4c,0x0d,0xb0,0x03,0xc0};

static const unsigned char PROGMEM image_wifi_not_connected_bits[] = {0x21,0xf0,0x00,0x16,0x0c,0x00,0x08,0x03,0x00,0x25,0xf0,
0x80,0x42,0x0c,0x40,0x89,0x02,0x20,0x10,0xa1,0x00,0x23,0x58,0x80,0x04,0x24,0x00,0x08,0x52,0x00,0x01,
0xa8,0x00,0x02,0x04,0x00,0x00,0x42,0x00,0x00,0xa1,0x00,0x00,0x40,0x80,0x00,0x00,0x00};
static const unsigned char PROGMEM image_wifi_1_bits[] = {0x01,0xf0,0x00,0x06,0x0c,0x00,0x18,0x03,0x00,0x21,0xf0,0x80,0x46,0x0c,
0x40,0x88,0x02,0x20,0x10,0xe1,0x00,0x23,0x18,0x80,0x04,0x04,0x00,0x08,0x42,0x00,0x01,0xb0,0x00,0x02,0x08,0x00,0x00,0x40,0x00,0x00,
0xa0,0x00,0x00,0x40,0x00,0x00,0x00,0x00};
static const unsigned char PROGMEM image_arrow_left_bits[] = {0x20,0x40,0xfe,0x40,0x20};

static const unsigned char PROGMEM cham_thang [] = {0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0xc0,0xc0};
static const unsigned char PROGMEM image_qr_1_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0x06,0x18,0xff,0xfe,0x00,0x00,0x7f,0xff,0x06,0x18,
0xff,0xfe,0x00,0x00,0x60,0x03,0x1e,0x60,0xc0,0x06,0x00,0x00,0x60,0x03,0x1e,0x60,0xc0,0x06,0x00,0x00,0x63,0xf3,0x0e,0x18,
0xcf,0xc6,0x00,0x00,0x67,0xf3,0x06,0x1c,0xcf,0xe6,0x00,0x00,0x67,0xf3,0x1e,0x00,0xcf,0xe6,0x00,0x00,0x67,0xf3,0x3e,0x00,
0xcf,0xe6,0x00,0x00,0x67,0xf3,0x1e,0x00,0xcf,0xe6,0x00,0x00,0x67,0xf3,0x01,0xfc,0xcf,0xe6,0x00,0x00,0x67,0xf3,0x01,0xf8,
0xcf,0xe6,0x00,0x00,0x60,0x03,0x1f,0xe0,0xc0,0x06,0x00,0x00,0x60,0x03,0x3f,0xe0,0xc0,0x06,0x00,0x00,0x7f,0xff,0x39,0x98,
0xff,0xfe,0x00,0x00,0x7f,0xff,0x39,0x9c,0xff,0xfe,0x00,0x00,0x00,0x00,0x01,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xfc,
0x00,0x00,0x00,0x00,0x3f,0xe1,0xde,0xf2,0x11,0x00,0x00,0x00,0x7f,0xf3,0xfe,0x63,0x33,0x98,0x00,0x00,0x7f,0xf3,0xfc,0x62,
0x33,0xb8,0x00,0x00,0x66,0x30,0x00,0x1c,0x0f,0xfe,0x00,0x00,0x66,0x30,0x00,0x18,0x0f,0xfe,0x00,0x00,0x7e,0x03,0xc6,0x60,
0xcc,0xf8,0x00,0x00,0x7e,0x03,0xe6,0x60,0xcc,0xf8,0x00,0x00,0x00,0x3c,0xfe,0x00,0x0f,0xe0,0x00,0x00,0x00,0x3c,0xfe,0x00,
0x0f,0xe0,0x00,0x00,0x1f,0xf3,0xf9,0xe0,0x03,0x86,0x00,0x00,0x1f,0xf3,0xf9,0xe0,0x03,0x86,0x00,0x00,0x0e,0xe1,0xff,0xc0,
0x03,0xc6,0x00,0x00,0x00,0x00,0x3f,0x83,0x3f,0xe6,0x00,0x00,0x00,0x00,0x3f,0x83,0x3f,0xe6,0x00,0x00,0x7f,0xff,0x3e,0x7c,
0xcc,0x78,0x00,0x00,0x7f,0xff,0x1e,0x7c,0xcc,0xf8,0x00,0x00,0x60,0x03,0x06,0x7f,0x0f,0xe0,0x00,0x00,0x60,0x03,0x06,0x7f,
0x0f,0xe0,0x00,0x00,0x67,0xf3,0x18,0x1f,0x03,0x98,0x00,0x00,0x67,0xf3,0x38,0x1f,0x03,0x98,0x00,0x00,0x67,0xf3,0x18,0x7f,
0xc4,0x60,0x00,0x00,0x67,0xf3,0x38,0x7f,0xcc,0x60,0x00,0x00,0x67,0xf3,0x38,0x3f,0xc4,0x60,0x00,0x00,0x67,0xf3,0x38,0x03,
0xc0,0x60,0x00,0x00,0x63,0xf3,0x38,0x03,0xc0,0x60,0x00,0x00,0x60,0x03,0x38,0x78,0xf0,0x60,0x00,0x00,0x60,0x03,0x38,0x7c,
0xf0,0x60,0x00,0x00,0x7f,0xff,0x3f,0x9f,0xf0,0x18,0x00,0x00,0x7f,0xff,0x1f,0x9f,0xf0,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static const unsigned char PROGMEM image_check_contour_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x50,0x20,
0x88,0x51,0x10,0x8a,0x20,0x44,0x40,0x20,0x80,0x11,0x00,0x0a,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


bool enable_reset= false;
bool auto_time_mode = false;
bool auto_region_mode = false;
bool onsubmenu=false;
bool rotatingDown;
bool setting = false;
bool CONNECTED=false;
bool auto_time_sel;
bool nothing_changed=true;
bool onsubmenu1=false;
bool onsubmenu1a=false;
bool ontime_setting =false;
bool onhard_reset= false;
int startWifiIndex, endWifiIndex;
int startTimeIndex,endTimeIndex;
int save=0;
int reset_sel= 0;
int _gio,_phut;
int gio=0;
int phut;
int giay;
int utc=26;
int _utc;
int startIndex;
int endIndex;
int maxVisibleItems =4;
int numMenus=5;
int dem=0;
int dem_region=0;
int time_menuIndex=0;
int currentRotaryValue;
int previousRotaryValue;
int wifiMenu_choose=0;
int count_click=0;
float offset=0.00;
float _offset;
float nhietdo;
const int add_gio=0;
const int add_phut=32;
const int add_giay=64;
const int add_utc=96;
const int add_offset=128;
const int add_auto_time = 136;
const int add_auto_region = 144;
unsigned long lastRotaryChange;
long preRSSI=0;
static unsigned long lastTimeButtonDown = 0;
unsigned long shortPressAfterMiliseconds = 50;   
unsigned long longPressAfterMiliseconds = 2000;
unsigned long entersettingAfterMiliseconds = 3000;
unsigned long chagneModeAfterMiliseconds =1000;
unsigned long  currentMillis=millis();
unsigned long previousMillis= 0;
unsigned long interval =1000;
static unsigned int changemode=0;
static unsigned int changemode_auto_time=0;
static unsigned int changemode_auto_region=0;
String rssi;
char SSID_AP[]={"ERa-Clock"};
String menus[] = {"Time","WiFi", "Calib","DHT","Hard Reset"};
float list[]={-12,-11,-10,-9.5,-9,-8,-7,-6,-5,-4,-3,-2.5,-2,-1,0,1,2,3,3.5,4,
4.5,5,5.5,5.75,6,6.5,7,8,8.75,9,9.5,10,10.5,11,12,12.75,13,14};
String region_list[]={"UTC-12","UTC-11","UTC-10","UTC-9:30","UTC-9","UTC-8","UTC-7","UTC-6","UTC-5",
"UTC-4","UTC-3","UTC-2:30","UTC-2","UTC-1","UTC","UTC+1","UTC+2","UTC+3","UTC+3:30","UTC+4","UTC+4:30",
"UTC+5","UTC+5:30","UTC+5:45","UTC+6","UTC+6:30","UTC+7","UTC+8","UTC+8:45","UTC+9","UTC+9:30","UTC+10",
"UTC+10:30","UTC+11","UTC+12","UTC+12:45","UTC+13","UTC+14"};

void displayMenu();
void maindisplay();
void rotary_loop();
void handle_rotary_button();
void on_button_short_click();
void on_button_long_click();
void time_setting();
void set_time();
void region();
void calib();
// void auto_time();
void hard_reset();
void DHT();
void wifi();
void hienthi();
void connect_succes();
// String RSSIasQuality();
void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

ERA_CONNECTED() {
  ERA_LOG("ERa", "ERa connected!");
  Serial.println("ERa connected!");
  CONNECTED =true;
  // connect_succes();
}

ERA_DISCONNECTED() {
    ERA_LOG("ERa", "ERa disconnected!");
    Serial.println("ERa disconnected!");
}

void TaskEra(void * parameters){
  for(;;){
    if(WiFi.status() != WL_CONNECTED)
    {
      // wm.resetSettings();
      wm.setTitle("ERa-WiFi Config");
      bool res;
      res = wm.autoConnect(SSID_AP,""); // password protected ap
      if(!res) {
          Serial.println("Failed to connect");
      } 
      else 
      {
          String _ssid = wm.getWiFiSSID();
          String _pass = wm.getWiFiPass();
          char *ssid;
          char *pass;
          ssid=&_ssid[0];
          pass=&_pass[0];
          ERa.begin(ssid, pass);
          Serial.print("Trạng Thái Kết Nối: ");
          Serial.println(wm.getWLStatusString());
      }
    }
      ERa.run();
      syncTime.setTimeZone(list[utc]);
      syncTime.getTime(ntpTime);
    }
}
// void connect_succes(){
//   display.clearDisplay();
//   display.setTextColor(SH110X_WHITE);
//   display.setTextSize(1);
//   display.setCursor(17, 11);
//   display.print("Connect Success");
//   display.setCursor(40, 33);
//   display.print(wm.getWiFiSSID());
//   display.drawBitmap(110, 6, image_check_contour_bits, 13, 16, 1);
//   display.drawBitmap(1, 6, image_check_contour_bits, 13, 16, 1);
//   display.display();
//   delay(3000);
// }
void setup() {
  // put your setup code here, to run once:
  // SPIFFS.begin(true);
  Serial.begin(115200); 
  syncTime.begin(); 
  /////////////////////////////////////////////////////////
  display.begin(i2c_Address, true); // Address 0x3C default
  display.display();
  delay(2000);
  EEPROM.begin(FLASH_MEMORY_SIZE);
  // phut=EEPROM.read(add_phut);
  // gio=EEPROM.read(add_gio);
  // utc=EEPROM.read(add_utc);
  // offset=EEPROM.read(add_offset);
  // auto_time_mode=EEPROM.read(add_auto_time);
  // auto_region_mode=EEPROM.read(add_auto_region);
  display.clearDisplay();
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(-99999, 99999, true);
  rotaryEncoder.disableAcceleration();
  xTaskCreatePinnedToCore(TaskEra,"Task Era NTP",100000,NULL,1,NULL,1);
}
void loop() 
{
  rotary_loop();
  hienthi();
  currentMillis=millis();
  if(millis()-previousMillis >=1000)
  {
    nhietdo=temperatureRead();
    save++;
    if(save==300)
    {
      save=0;
      // EEPROM.writeInt(add_giay,giay);
      // EEPROM.writeInt(add_gio,gio);
      // EEPROM.writeInt(add_phut,phut);
      // EEPROM.writeInt(add_utc,utc);
      // EEPROM.writeFloat(add_offset,offset);
      // EEPROM.writeBool(add_auto_time,auto_time_mode);
      // EEPROM.writeBool(add_auto_region,auto_region_mode);
      // EEPROM.commit();
    }
  }
  if(auto_time_mode && CONNECTED ){
    gio=ntpTime.hour;
    phut=ntpTime.minute;
    giay=ntpTime.second;
  }
  else

  {
      if(millis()-previousMillis >=1000)
      {
        previousMillis=millis();
        giay++;
        if(giay>=60)
        {
          giay=0;
          phut++;
          if(phut>=60)
          {
            phut=0;
            gio++;
            if(gio>=24){gio=0;}
          }
        }
      }
  }
}
void displayMenu()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30,0);
  display.print("Setting");
  // display.setCursor(70,0);
  // display.print(region_list[utc]);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
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
      display.drawRoundRect(0, (i-startIndex) * 12 + 10, 120, 12, 3, SH110X_WHITE);
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
  if(WiFi.status() != WL_CONNECTED)
  {
    display.drawBitmap(104, 47, image_wifi_not_connected_bits, 19, 16, 1);
  }else{
    if(!CONNECTED)
    {
      display.drawBitmap(100, 49, cham_thang , 2, 13, 1);
    }
    display.drawBitmap(104, 47, image_wifi_1_bits, 19, 16, 1);
  }
  
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
  display.setCursor(29, 50);
  display.print(region_list[utc]);
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
        if(dem>4){dem=0;}
      }
      else
      {
        rotatingDown= false ;
        dem--;
        if(dem<0){dem=4;}
      }
    }
    else
    {
      switch (dem)
      {
      case 0:// Set time
          if(currentRotaryValue>previousRotaryValue)
          {
            if(onsubmenu1){
              if(changemode==0)
              {
                gio--;
                if(gio<0){gio=24;}
              }
              else{
                phut--;
                if(phut<0){phut=59;}
              }
            }else if(onsubmenu1a==true){
              utc--;
              if(utc<0){utc=37;}
            }else{
              rotatingDown=true;
              time_menuIndex++;
              if(time_menuIndex>4) time_menuIndex=0;
            }
          }   
          else
          {
            if(onsubmenu1){
              if(changemode==0)
              {
                gio++;
                if(gio>24){gio=0;}
              }
              else{
                phut++;
                if(phut>60){phut=0;}
              }
            }else if(onsubmenu1a==true){
              utc++;
              if(utc>37){utc=0;}
            }else{
              rotatingDown=false;
              time_menuIndex--;
              if(time_menuIndex<0) time_menuIndex=4;
            }
          }
          // if(_gio != gio || _phut!=phut){
          //   nothing_changed = false;
          // }else{
          //   nothing_changed = true;
          // }
        break;
      case 1://wifi
          if(currentRotaryValue>previousRotaryValue)
          {
            rotatingDown=true;
            wifiMenu_choose++;
            if(wifiMenu_choose>5){wifiMenu_choose=0;}
          }   
          else
          {
            rotatingDown=false;
            wifiMenu_choose--;
            if(wifiMenu_choose<0){wifiMenu_choose=5;}
          }
        break;
      // case 2://Chỉnh UTC trong region menu

      //   break;
      case 2:// Chỉnh offset trong menu calib
        if(currentRotaryValue>previousRotaryValue)
        {
          offset=offset - 0.1;
          if(offset<-5){offset=5;}
        }
        else
        {
          offset=offset + 0.1;
          if(offset>5){offset=-5;}
        }
        if(_utc!=utc){
          nothing_changed=false;
        }else{
          nothing_changed=true;
        }
        break;
      case 4://Chọn reset trong menu  hard_reset
        if(currentRotaryValue>previousRotaryValue)
        {
          reset_sel++;
          if(reset_sel>1){reset_sel=0;}
        }else{
          reset_sel--;
          if(reset_sel<0){reset_sel=1;}
        }
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
  if (isEncoderButtonDown)
  {    
    if (!lastTimeButtonDown) {
      //start measuring 
      lastTimeButtonDown = millis();
    }
    if(!isLongpress && (millis() - lastTimeButtonDown >= longPressAfterMiliseconds)&& onsubmenu)// đè 2s để lưu setting
    {
      Serial.print("button LONG press ");
      isLongpress =true;
      if(onsubmenu1==true){
        onsubmenu1=false;
        time_menuIndex=0;
        time_setting();
        return;
      }
      if(onsubmenu1a==true){
        onsubmenu1a=false;
        time_menuIndex=2;
        time_setting();
        return;
      }
      if(dem==1 && CONNECTED==false){
        onsubmenu=false;
        displayMenu();
        return;
      }else{
        onsubmenu=false;
        displayMenu();
      }
      // else if(nothing_changed==true){
      //   onsubmenu=false;
      //   displayMenu();
      // }
      // else// thoát submenu và lưu setting
      // {
        // for(int x=0;x<20;x++)
        // {
        //   display.clearDisplay();
        //   display.setTextColor(1);
        //   display.setTextSize(3);
        //   display.setCursor(24, 18);
        //   display.print("SAVED");
        //   display.display();
        // }
        // EEPROM.writeInt(add_giay,giay);
        // EEPROM.writeInt(add_gio,gio);
        // EEPROM.writeInt(add_phut,phut);
        // EEPROM.writeInt(add_utc,utc);
        // EEPROM.writeFloat(add_offset,offset);
        // EEPROM.writeBool(add_auto_time,auto_time_mode);
        // EEPROM.writeBool(add_auto_region,auto_region_mode);
        // EEPROM.commit();
        // nothing_changed=true;
      // }
    }
    else// đè 3s để vào setting menu
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
    if(lastTimeButtonDown && !isSettingpress && !isLongpress && setting){
      // _gio=gio;
      // _phut=phut;
      // _utc=utc;
      // _offset=offset;
      time_clicked=millis();
      Serial.print("button SHORT press ");
      onsubmenu=true;
      if(dem==0 && ontime_setting==true){// control ubmenu 1
        switch (time_menuIndex)
        {
          case 0:
            onsubmenu1=true;
            changemode=!changemode;
          break;
          case 1:
            auto_time_mode=!auto_time_mode;
            break;
          case 2:
            onsubmenu1a=true;
            break;
          case 3:
            for(int x=0;x<20;x++)
            {
              display.clearDisplay();
              display.setTextColor(1);
              display.setTextSize(3);
              display.setCursor(24, 18);
              display.print("SAVED");
              display.display();
            }
            // EEPROM.writeInt(add_giay,giay);
            // EEPROM.writeInt(add_gio,gio);
            // EEPROM.writeBool(add_auto_time,auto_time_mode);
            // EEPROM.commit();
            time_setting();
            break;
          case 4:
            onsubmenu=false;
            ontime_setting = false;
            time_menuIndex = 0;
            displayMenu();
            break;
        default:
          break;
        }
      }
      if(reset_sel==1&& dem == 4 && onhard_reset==true)
      {
        enable_reset = true;
      }else if(reset_sel==0 && dem==4&& onhard_reset==true){
        onsubmenu = false;
        onhard_reset= false;
        reset_sel=0;
        displayMenu();
      }
      if(wifiMenu_choose==4 && dem==1 && CONNECTED==true){
        display.clearDisplay();
        display.setTextColor(1);
        display.setCursor(16, 13);
        display.print("Disconnecting");
        display.display();
        display.setCursor(95, 13);
        for(int i;i<5;i++){
          display.print(".");
          display.display();
          delay(300);
        }
        wm.resetSettings();
        CONNECTED=false;
        wifi();
      }else if(wifiMenu_choose==5 && dem==1 && CONNECTED==true){
        onsubmenu=false;
        wifiMenu_choose=-1;
        displayMenu();
     }
    }
      isLongpress = false;
      isSettingpress = false;
      lastTimeButtonDown = 0;
  }
}
void set_time(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30,0);
  display.print("Set Time");
  if(auto_time_mode){
    display.clearDisplay();
    display.setTextColor(1);
    display.setCursor(17, 14);
    display.print("AuTo Time Is ON");
    display.setCursor(6, 33);
    display.print("Turn OFF To Use This");
    display.display();
    delay(2000);
    onsubmenu1=false;
    time_setting();
    return;
  }
  display.setCursor(20,10);
  if(changemode==0)
  {
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
void time_setting()
{
  if(onsubmenu1==true){
    set_time();
    return;
  }
  if(onsubmenu1a==true){
    region();
    return;
  }
  String time_menu[]={"Set Time","Sync Time: ","Region","Save","Back"};
  String mode[]={"OFF","ON"};
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(1);
  display.setCursor(10,0);
  display.print("Time Setting");
  if (rotatingDown) {
    startTimeIndex = time_menuIndex - 2;
    endTimeIndex = time_menuIndex + 1;
  } else {
    startTimeIndex = time_menuIndex - 1;
    endTimeIndex = time_menuIndex + 2;
  }
  if (startTimeIndex < 0) 
  {
      startTimeIndex = 0;
      endTimeIndex = 3;
  } 
  else if (endTimeIndex >= 5) 
  {
      endTimeIndex =4;
      startTimeIndex = endTimeIndex - 3;
  }
  for(int i=startTimeIndex;i<=endTimeIndex;i++){
    display.setCursor(6,(i-startTimeIndex)*12+13);
    if(i==time_menuIndex){
      display.drawRoundRect(0, (i-startTimeIndex) * 12 + 11, 120, 12, 3, SH110X_WHITE);
    }
    display.print(time_menu[i]);
  }
  display.setCursor(75,(1-startTimeIndex)*13+13);
  display.print(mode[auto_time_mode]);
  display.display();
}
void region()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20,0);
  display.print("Region");
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20,20);
  display.setTextSize(1);
  display.print("UTC: ");
  display.setCursor(60,20);
  display.print(region_list[utc]);
  display.display();
}
void calib()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(10,0);
  display.print("OFF Setting");
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20,20);
  display.setTextSize(1);
  display.print("OFFSET: ");
  display.setCursor(70,20);
  display.print(offset);
  display.display();
}
// void auto_time()
// {
//   String mode[]={"OFF","ON"};
//   String auto_time[]={"Auto Time: ","Auto Region: "};
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SH110X_WHITE);
//   display.setCursor(13, 4);
//   display.print("Auto Time Setting");
//   display.setCursor(4,16);
//   display.print(auto_time[0]);
//   display.setCursor(4,28);
//   display.print(auto_time[1]);
//   display.setCursor(64, 16);
//   display.print(mode[auto_time_mode]);
//   display.setCursor(93, 28);
//   display.print(mode[auto_region_mode]);
//   if(auto_time_sel){
//     display.drawBitmap(116, 18 , image_arrow_left_bits, 7, 5, 1);
//   }else{
//     display.drawBitmap(116, 30, image_arrow_left_bits, 7, 5, 1);
//   }
//   display.display();
// }
void hard_reset(){
  String choose[]={"No","Yes"};
  display.clearDisplay();
  display.setTextColor(1);
  display.setCursor(16, 8);
  display.print("Hard Resetting...");
  display.setCursor(4, 28);
  display.print("Are You Sure?");
  display.setCursor(90, 28);
  if(reset_sel>0)
    display.print(choose[reset_sel]);
  else{
    display.print(choose[0]);
    reset_sel=0;
  }
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
    WiFiManager wm;
    wm.resetSettings();
    delay(2000);
    ESP.restart();
  }
  display.display();
  }
void DHT()
{
    static const unsigned char PROGMEM image_weather_temperature_bits[] = {0x1c,0x00,0x22,0x02,0x2b,0x05,0x2a,0x02,0x2b,0x38,0x2a,
    0x60,0x2b,0x40,0x2a,0x40,0x2a,0x60,0x49,0x38,0x9c,0x80,0xae,0x80,0xbe,0x80,0x9c,0x80,0x41,0x00,0x3e,0x00};
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextSize(1);
    display.setCursor(21, 15);
    display.print("Chip Temperature");
    display.setTextSize(2);
    display.setCursor(36, 34);
    display.print(nhietdo);
    display.drawBitmap(2, 11, image_weather_temperature_bits, 16, 16, 1);
    display.display();
}
String ConverIpToString(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

String RSSIasQuality(){
  int res;
  String rssi;
    preRSSI=millis();
    res=WiFi.RSSI();
    Serial.println(res);
    if(res >= -30){
      rssi="Amazing";
    }else if(res >= -55 && res < -30 ){
      rssi="Very Good";
    }else if(res >= -67 && res < -55){
      rssi="Fairly Good";
    }else if(res >= -70 && res < -67){
      rssi="Ok";
    }else if(res >= -80 && res < -70){
      rssi="Low";
    }else if(res > -90 && res < -80){
      rssi="Very Low";
    }else{
      rssi ="No Signal";
    }
  return rssi;
}
void wifi()
{ 
  if(CONNECTED==false){
    display.clearDisplay();
    display.setTextColor(1);
    display.setCursor(4, 3);
    display.print("Connect to");                                                 
    display.setCursor(7, 23);
    display.print(SSID_AP);
    display.setCursor(2, 46);
    display.print("And Scan -->");
    display.drawBitmap(69, 0, image_qr_1_bits, 64, 64, 1);
    display.display();
    return;
  }else if(CONNECTED==true){
  IPAddress ip;
  ip=WiFi.localIP();
  String mac;
  if(millis()- preRSSI>=5000){
    rssi = RSSIasQuality();
  }
  mac=WiFi.macAddress();
  const char* wifi_menu[]={"SSID: ","IP:","MAC:","RSSI: ","Disconnect","Back"};
  String infor[]={wm.getWiFiSSID(),ConverIpToString(ip),mac,rssi};
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(1);
  display.setCursor(10,0);
  display.print("WiFi Setting");
  if (rotatingDown) {
    startWifiIndex = wifiMenu_choose - 2;
    endWifiIndex = wifiMenu_choose + 1;
  } else {
    startWifiIndex = wifiMenu_choose - 1;
    endWifiIndex = wifiMenu_choose + 2;
  }
  if (startWifiIndex < 0) 
  {
      startWifiIndex = 0;
      endWifiIndex = 3;
  } 
  else if (endWifiIndex >= 6) 
  {
      endWifiIndex =5;
      startWifiIndex = endWifiIndex - 3;
  }
  for(int i=startWifiIndex ; i <= endWifiIndex ;i++){
    if(i== wifiMenu_choose){
      display.drawRoundRect(0,(i-startWifiIndex)*12+10,128,12,3,SH110X_WHITE);
    }
    display.setCursor(2, (i-startWifiIndex)*12+12);
    display.print(wifi_menu[i]);
    if(i>3){
      continue;
    }
    display.setCursor(strlen(wifi_menu[i])*3+12, (i-startWifiIndex)*12+12);
    display.print(infor[i]);
  }
    display.display();
  }
}
void hienthi()
{
  if(setting)
  {
    if(onsubmenu)
    {
      switch (dem)
      {
        case 0:
          time_setting();
          ontime_setting=true;
          break;
        case 1:
          wifi();
          break;
        // case 2:
        //   region();
        //   break;
        case 2:
          calib();
          break;
        // case 4:
        //   auto_time();
        //   break;
        case 3:
          DHT();
          break;
        case 4:
          hard_reset();
          onhard_reset=true;
          break;
        default:
          onsubmenu=false;
          break;
      }
    }
    else{displayMenu();}
  }else{
    // handle_rotary_button();
    maindisplay();
    onsubmenu=false;
    dem=0;
  }
}