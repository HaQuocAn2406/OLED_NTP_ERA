
#define ERA_LOCATION_VN
#define ERA_AUTH_TOKEN "2a377e27-cf9a-4061-ba71-bdcedde02e64"
#define ERA_DEBUG
#define ERA_SERIAL Serial
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <AiEsp32RotaryEncoder.h>
#include <EEPROM.h>
#include <ERa.hpp>
#include <Time/ERaEspTime.hpp>
#include <string.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <DHTesp.h>
#include <time.h>
#include <PNGdec.h>
#include "nhietke.h"
#include "FreeSans10pt7b.h"
#define ENCODER_CLK 25
#define ENCODER_DT 26
#define ENCODER_SW 27
#define ENCODER_VCC -1

#define FLASH_MEMORY_SIZE 200
#define ERA_DEBUG
#define ERA_SERIAL Serial

#define ENCODER_STEPS 4
#define DHTPIN 5

// #define SH110X

#ifdef SH110X
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;
#define i2c_Address 0x3c // initialize with the I2C addr 0x3C Typically eBay OLED's
// #define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    //
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#else
TFT_eSPI display = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&display);
byte xcolon = 0;
byte x2colon = 0;
#endif
DHTesp dht;
ERaEspTime syncTime;
TimeElement_t ntpTime;


AiEsp32RotaryEncoder rotaryEncoder(ENCODER_CLK, ENCODER_DT,ENCODER_SW);

static const unsigned char PROGMEM image_hand_pointer_bits[] = {0x08, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00,
                                                                0x14, 0x00, 0x16, 0x00, 0x15, 0x80, 0x15, 0x60, 0xd5, 0x50, 0x90, 0x50, 0x40, 0x10, 0x40, 0x10, 0x20, 0x10, 0x20, 0x20, 0x10, 0x20, 0x08, 0x40};

static const unsigned char PROGMEM image_network_www_bits[] = {0x03, 0xc0, 0x0d, 0xb0, 0x32, 0x4c, 0x24, 0x24, 0x44, 0x22, 0x7f, 0xfe,
                                                               0x88, 0x11, 0x88, 0x11, 0x88, 0x11, 0x88, 0x11, 0x7f, 0xfe, 0x44, 0x22, 0x24, 0x24, 0x32, 0x4c, 0x0d, 0xb0, 0x03, 0xc0};

static const unsigned char PROGMEM image_wifi_not_connected_bits[] = {0x21, 0xf0, 0x00, 0x16, 0x0c, 0x00, 0x08, 0x03, 0x00, 0x25, 0xf0,
                                                                      0x80, 0x42, 0x0c, 0x40, 0x89, 0x02, 0x20, 0x10, 0xa1, 0x00, 0x23, 0x58, 0x80, 0x04, 0x24, 0x00, 0x08, 0x52, 0x00, 0x01,
                                                                      0xa8, 0x00, 0x02, 0x04, 0x00, 0x00, 0x42, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0x00};
static const unsigned char PROGMEM image_wifi_1_bits[] = {0x01, 0xf0, 0x00, 0x06, 0x0c, 0x00, 0x18, 0x03, 0x00, 0x21, 0xf0, 0x80, 0x46, 0x0c,
                                                          0x40, 0x88, 0x02, 0x20, 0x10, 0xe1, 0x00, 0x23, 0x18, 0x80, 0x04, 0x04, 0x00, 0x08, 0x42, 0x00, 0x01, 0xb0, 0x00, 0x02, 0x08, 0x00, 0x00, 0x40, 0x00, 0x00,
                                                          0xa0, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00};
static const unsigned char PROGMEM image_arrow_left_bits[] = {0x20, 0x40, 0xfe, 0x40, 0x20};

static const unsigned char PROGMEM cham_thang[] = {0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0xc0, 0xc0};

static const unsigned char PROGMEM image_qr_1_bits[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x06, 0x18, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0x06, 0x18,
                                                        0xff, 0xfe, 0x00, 0x00, 0x60, 0x03, 0x1e, 0x60, 0xc0, 0x06, 0x00, 0x00, 0x60, 0x03, 0x1e, 0x60, 0xc0, 0x06, 0x00, 0x00, 0x63, 0xf3, 0x0e, 0x18,
                                                        0xcf, 0xc6, 0x00, 0x00, 0x67, 0xf3, 0x06, 0x1c, 0xcf, 0xe6, 0x00, 0x00, 0x67, 0xf3, 0x1e, 0x00, 0xcf, 0xe6, 0x00, 0x00, 0x67, 0xf3, 0x3e, 0x00,
                                                        0xcf, 0xe6, 0x00, 0x00, 0x67, 0xf3, 0x1e, 0x00, 0xcf, 0xe6, 0x00, 0x00, 0x67, 0xf3, 0x01, 0xfc, 0xcf, 0xe6, 0x00, 0x00, 0x67, 0xf3, 0x01, 0xf8,
                                                        0xcf, 0xe6, 0x00, 0x00, 0x60, 0x03, 0x1f, 0xe0, 0xc0, 0x06, 0x00, 0x00, 0x60, 0x03, 0x3f, 0xe0, 0xc0, 0x06, 0x00, 0x00, 0x7f, 0xff, 0x39, 0x98,
                                                        0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0x39, 0x9c, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc,
                                                        0x00, 0x00, 0x00, 0x00, 0x3f, 0xe1, 0xde, 0xf2, 0x11, 0x00, 0x00, 0x00, 0x7f, 0xf3, 0xfe, 0x63, 0x33, 0x98, 0x00, 0x00, 0x7f, 0xf3, 0xfc, 0x62,
                                                        0x33, 0xb8, 0x00, 0x00, 0x66, 0x30, 0x00, 0x1c, 0x0f, 0xfe, 0x00, 0x00, 0x66, 0x30, 0x00, 0x18, 0x0f, 0xfe, 0x00, 0x00, 0x7e, 0x03, 0xc6, 0x60,
                                                        0xcc, 0xf8, 0x00, 0x00, 0x7e, 0x03, 0xe6, 0x60, 0xcc, 0xf8, 0x00, 0x00, 0x00, 0x3c, 0xfe, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x3c, 0xfe, 0x00,
                                                        0x0f, 0xe0, 0x00, 0x00, 0x1f, 0xf3, 0xf9, 0xe0, 0x03, 0x86, 0x00, 0x00, 0x1f, 0xf3, 0xf9, 0xe0, 0x03, 0x86, 0x00, 0x00, 0x0e, 0xe1, 0xff, 0xc0,
                                                        0x03, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x83, 0x3f, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x83, 0x3f, 0xe6, 0x00, 0x00, 0x7f, 0xff, 0x3e, 0x7c,
                                                        0xcc, 0x78, 0x00, 0x00, 0x7f, 0xff, 0x1e, 0x7c, 0xcc, 0xf8, 0x00, 0x00, 0x60, 0x03, 0x06, 0x7f, 0x0f, 0xe0, 0x00, 0x00, 0x60, 0x03, 0x06, 0x7f,
                                                        0x0f, 0xe0, 0x00, 0x00, 0x67, 0xf3, 0x18, 0x1f, 0x03, 0x98, 0x00, 0x00, 0x67, 0xf3, 0x38, 0x1f, 0x03, 0x98, 0x00, 0x00, 0x67, 0xf3, 0x18, 0x7f,
                                                        0xc4, 0x60, 0x00, 0x00, 0x67, 0xf3, 0x38, 0x7f, 0xcc, 0x60, 0x00, 0x00, 0x67, 0xf3, 0x38, 0x3f, 0xc4, 0x60, 0x00, 0x00, 0x67, 0xf3, 0x38, 0x03,
                                                        0xc0, 0x60, 0x00, 0x00, 0x63, 0xf3, 0x38, 0x03, 0xc0, 0x60, 0x00, 0x00, 0x60, 0x03, 0x38, 0x78, 0xf0, 0x60, 0x00, 0x00, 0x60, 0x03, 0x38, 0x7c,
                                                        0xf0, 0x60, 0x00, 0x00, 0x7f, 0xff, 0x3f, 0x9f, 0xf0, 0x18, 0x00, 0x00, 0x7f, 0xff, 0x1f, 0x9f, 0xf0, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const unsigned char PROGMEM image_check_contour_bits[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x50, 0x20,
                                                                 0x88, 0x51, 0x10, 0x8a, 0x20, 0x44, 0x40, 0x20, 0x80, 0x11, 0x00, 0x0a, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const char *daysOfTheWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *monthOfTheYear[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};

char SSID_AP[] = {"ERA"};

String menus[] = {"Date/Time", "WiFi", "Calib", "Chip Temperature", "Hard Reset", "Back"};
// float list[]={-12,-11,-10,-9.5,-9,-8,-7,-6,-5,-4,-3,-2.5,-2,-1,0,1,2,3,3.5,4,
// 4.5,5,5.5,5.75,6,6.5,7,8,8.75,9,9.5,10,10.5,11,12,12.75,13,14};
int list[] = {-12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
// String region_list[]={"UTC-12","UTC-11","UTC-10","UTC-9:30","UTC-9","UTC-8","UTC-7","UTC-6","UTC-5",
// "UTC-4","UTC-3","UTC-2:30","UTC-2","UTC-1","UTC","UTC+1","UTC+2","UTC+3","UTC+3:30","UTC+4","UTC+4:30",
// "UTC+5","UTC+5:30","UTC+5:45","UTC+6","UTC+6:30","UTC+7","UTC+8","UTC+8:45","UTC+9","UTC+9:30","UTC+10",
// "UTC+10:30","UTC+11","UTC+12","UTC+12:45","UTC+13","UTC+14"};
String region_list[] = {"UTC-12", "UTC-11", "UTC-10", "UTC-9", "UTC-8", "UTC-7", "UTC-6", "UTC-5",
                        "UTC-4", "UTC-3", "UTC-2", "UTC-1", "UTC", "UTC+1", "UTC+2", "UTC+3", "UTC+4",
                        "UTC+5", "UTC+6", "UTC+7", "UTC+8", "UTC+9", "UTC+10", "UTC+11", "UTC+12", "UTC+13", "UTC+14"};

String rssi;
int16_t thu;
int16_t thang = 0;
uint16_t nam = 1970;
uint16_t ngay = 1;
uint16_t hours;
uint16_t minutes;
uint16_t seconds;
uint16_t oldsecond;
int clicked = 0;
bool enable_reset = false;
bool auto_time_mode = false;
bool auto_region_mode = false;
bool onsubmenu = false;
bool rotatingDown;
bool setting = false;
bool ERa_CONNECTED = false;
bool nothing_changed = true;
bool onsubmenu1 = false;
bool onsubmenu1a = false;
bool ontime_setting = false;
bool onhard_reset = false;
int startWifiIndex, endWifiIndex;
int startTimeIndex, endTimeIndex, startsetTimeIndex, endsetTimeIndex;
int save = 0;
int reset_sel = 0;
int offset_utc;

int _gio, _phut;
int gio;
int phut;
int giay;
int utc = 19;
int _utc;
int startIndex;
int endIndex;
int maxVisibleItems = 6;
int numMenus = 6;
int dem = 0;
int dem_region = 0;
int time_menuIndex = 0;
int settime_menuIndex = 0;
int currentRotaryValue;
int previousRotaryValue;
int wifiMenu_choose = 0;
float offset = 0.00;
float _offset;
float nhietdo;
int humidity;
int temperature;
const int add_gio = 0;
const int add_phut = 4;
const int add_giay = 8;
const int add_utc = 16;
const int add_offset = 20;
const int add_auto_time = 24;
unsigned long lastRotaryChange;
long preRSSI = 0;
unsigned long lastTimeButtonDown = 0;
unsigned long shortPressAfterMiliseconds = 50;
unsigned long local_time;
unsigned long longPressAfterMiliseconds = 2000;
unsigned long entersettingAfterMiliseconds = 3000;
unsigned long chagneModeAfterMiliseconds = 1000;
unsigned long currentMillis = millis();
unsigned long previousMillis = 0;
unsigned long interval = 1000;
unsigned int changemode = 0;
unsigned int changemode_auto_time = 0;
unsigned long current_time = -25200;

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
void hard_reset();
void Chip_Temp();
void wifi();
void hienthi();
void connect_succes();
void time_calculate(unsigned long current_time);
void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}

ERA_CONNECTED()
{
  ERA_LOG("ERa", "ERa connected!");
  Serial.println("ERa connected!");
  ERa_CONNECTED = true;
  auto_time_mode = true;
}

ERA_DISCONNECTED()
{
  ERA_LOG("ERa", "ERa disconnected!");
  Serial.println("ERa disconnected!");
  ERa_CONNECTED = false;
}
void timerEvent()
{
  ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}
void TaskEra(void *parameters)
{
  Serial.println("ERa Started");
  ERa.setScanWiFi(true);

  /* Initializing the ERa library. */
  ERa.begin();

  /* Setup timer called function every second */
  ERa.addInterval(1000L, timerEvent);
  for (;;)
  {
    ERa.run();
    syncTime.setTimeZone(list[utc]);
    syncTime.getTime(ntpTime);
  }
}
class thoigian
{
private:
  int hours, minutes, seconds;
  String Weekdays;
  int days, years;
  String Months;

public:
  thoigian(int hours, int minutes, int seconds, int days, int years, String Weekdays = "Sun", String Months = "Jan");
  void setHours(int hours)
  {
    this->hours = hours;
  }
  void setMinutes(int minutes)
  {
    this->minutes = minutes;
  }
  void setSeconds(int seconds)
  {
    this->seconds = seconds;
  }
  void setDay(int days)
  {
    this->days = days;
  }
  void setWeekdays(String Weekdays)
  {
    this->Weekdays = Weekdays;
  }
  void setMonths(String Months)
  {
    this->Months = Months;
  }
  void setYears(int years)
  {
    this->years = years;
  }
  int getHours()
  {
    return this->hours;
  }
  int getMinutes()
  {
    return this->minutes;
  }
  int getSeconds()
  {
    return this->seconds;
  }
  String getWeekDays()
  {
    return this->Weekdays;
  }
};

void setup()
{
  // put your setup code here, to run once:
  // SPIFFS.begin(true);
  Serial.begin(115200);
  syncTime.begin();
  dht.setup(DHTPIN, DHTesp::DHT11);
  /////////////////////////////////////////////////////////
#ifdef SH110X
  display.begin(i2c_Address, true); // Address 0x3C default
  display.display();
  delay(2000);
  display.clearDisplay();
#else
  display.init();
  display.setRotation(1);
  spr.createSprite(240, 230);
  spr.fillScreen(TFT_BLACK);
  display.fillScreen(TFT_BLACK);
#endif
  EEPROM.begin(FLASH_MEMORY_SIZE);
  minutes = EEPROM.read(add_phut);
  hours = EEPROM.read(add_gio);
  utc = EEPROM.read(add_utc);
  offset = EEPROM.read(add_offset);
  auto_time_mode = EEPROM.read(add_auto_time);
  current_time = (hours * 3600 + minutes * 60 + seconds) - (list[utc] * 3600);
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(-99999, 99999, true);
  rotaryEncoder.disableAcceleration();
  xTaskCreatePinnedToCore(TaskEra, "Task Era NTP", 10000, NULL, 1, NULL, 1);
}
void loop()
{
  rotary_loop();
  hienthi();
  if (dht.getStatusString() == "OK")
  {
    temperature = dht.getTemperature() + offset;
    humidity = dht.getHumidity() + offset;
  }
  else
  {
    temperature = 0;
    humidity = 0;
  }
  if (ERa_CONNECTED == true && auto_time_mode == true)
  {
    hours = ntpTime.hour;
    minutes = ntpTime.minute;
    seconds = ntpTime.second;
    thu = ntpTime.wDay - 1;
    ngay = ntpTime.day;
    thang = ntpTime.month - 1;
    nam = ntpTime.year + 1970;
  }
  else
  {
    time_calculate(current_time);
  }
  if (millis() - previousMillis >= 1000)
  {
    previousMillis = millis();
    nhietdo = temperatureRead();
    save++;
    if (save == 300)
    {
      save = 0;
      EEPROM.writeInt(add_giay, seconds);
      EEPROM.writeInt(add_gio, hours);
      EEPROM.writeInt(add_phut, minutes);
      EEPROM.writeInt(add_utc, utc);
      EEPROM.writeFloat(add_offset, offset);
      EEPROM.writeBool(add_auto_time, auto_time_mode);
      EEPROM.commit();
    }
  }
}
///////////////// Extract Time From Timestamp /////////////////////////
void time_calculate(unsigned long current_time)
{
  unsigned long utc_time = current_time + millis() / 1000;
  int time_offset = list[utc] * 3600; //
  unsigned long local_time = utc_time + time_offset;
  hours = (local_time / 3600) % 24;
  minutes = (local_time / 60) % 60;
  seconds = local_time % 60;
  // Print local time
  // Serial.print("Local time in VietNam: ");
  // Serial.print(hours);
  // Serial.print(":");
  // if (minutes < 10) Serial.print("0"); // Add leading zero if needed
  // Serial.print(minutes);
  // Serial.print(":");
  // if (seconds < 10) Serial.print("0"); // Add leading zero if needed
  // Serial.println(seconds);
}
///////////////////////////////////////////////////////////////////////////////

//////////////////////  Setting Screen //////////////////////////////////////////////////
void displayMenu()
{
#ifdef SH110X
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 0);
  display.print("Setting");
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(4, 10);
  if (rotatingDown)
  {
    startIndex = dem - 2;
    endIndex = dem + 1;
  }
  else
  {
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
  for (int i = startIndex; i <= endIndex; i++)
  {
    if (i == dem)
    {
      display.drawRoundRect(0, (i - startIndex) * 12 + 10, 120, 12, 3, SH110X_WHITE);
    }
    display.setCursor(4, (i - startIndex) * 12 + 12);
    display.print(menus[i]);
  }
  display.display();
#else
  spr.fillSprite(TFT_BLACK);
  spr.setTextColor(0xFFFF);
  spr.setTextSize(2);
  spr.drawString("Setting", 66, 13);
  spr.setTextSize(2);
  if (rotatingDown)
  {
    startIndex = dem - 3;
    endIndex = dem + 2;
  }
  else
  {
    startIndex = dem - 3;
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
  for (int i = startIndex; i <= endIndex; i++)
  {
    if (i == dem)
    {
      spr.drawRoundRect(10, (i - startIndex) * 33 + 30, 200, 25, 4, TFT_WHITE);
    }
    spr.setTextSize(2);
    spr.drawString(menus[i], 14, (i - startIndex) * 33 + 33);
    spr.setCursor(14, (i - startIndex) * 33 + 33);
  }
  spr.pushSprite(0, 0);
#endif
}
///////////////////////////////////////////////////////////////////////////////////////////////
void hienthi()
{
  if (setting == true)
  {
    if (onsubmenu)
    {
      switch (dem)
      {
      case 0:
        time_setting();
        ontime_setting = true;
        break;
      case 1:
        wifi();
        break;
      case 2:
        calib();
        break;
      case 3:
        Chip_Temp();
        break;
      case 4:
        hard_reset();
        onhard_reset = true;
        break;
      default:
        onsubmenu = false;
        break;
      }
    }
    else
    {
      displayMenu();
    }
  }
  else
  {
    maindisplay();
    onsubmenu = false;
    dem = 0;
  }
}
///////////////////// Main Screen ///////////////////////////////////////////////
void maindisplay()
{
#ifdef SH110X
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(1);
  static const unsigned char PROGMEM image_weather_humidity_white_bits[] = {0x04, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x0a, 0x00, 0x12, 0x00, 0x11, 0x00, 0x20, 0x80, 0x20, 0x80, 0x41, 0x40, 0x40, 0xc0, 0x80, 0xa0, 0x80, 0x20, 0x40, 0x40, 0x40, 0x40, 0x30, 0x80, 0x0f, 0x00};
  static const unsigned char PROGMEM image_weather_temperature_bits[] = {0x1c, 0x00, 0x22, 0x02, 0x2b, 0x05, 0x2a, 0x02, 0x2b, 0x38, 0x2a, 0x60, 0x2b, 0x40, 0x2a, 0x40, 0x2a, 0x60, 0x49, 0x38, 0x9c, 0x80, 0xae, 0x80, 0xbe, 0x80, 0x9c, 0x80, 0x41, 0x00, 0x3e, 0x00};
  static const unsigned char PROGMEM image_wifi_full_bits[] = {0x01, 0xf0, 0x00, 0x07, 0xfc, 0x00, 0x1e, 0x0f, 0x00, 0x39, 0xf3, 0x80, 0x77, 0xfd, 0xc0, 0xef, 0x1e, 0xe0, 0x5c, 0xe7, 0x40, 0x3b, 0xfb, 0x80, 0x17, 0x1d, 0x00, 0x0e, 0xee, 0x00, 0x05, 0xf4, 0x00, 0x03, 0xb8, 0x00, 0x01, 0x50, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00};
  if (WiFi.status() != WL_CONNECTED)
  {
    display.drawBitmap(109, 47, image_wifi_not_connected_bits, 19, 16, 1);
  }
  else
  {
    if (!ERa_CONNECTED)
    {
      display.drawBitmap(105, 47, cham_thang, 2, 13, 1);
    }
    display.drawBitmap(109, 47, image_wifi_full_bits, 19, 16, 1);
  }
  display.drawLine(0, 45, 127, 45, 1);
  display.setCursor(2, 51);
  display.print(daysOfTheWeek[thu]);
  display.setCursor(44, 51);
  display.print(monthOfTheYear[thang]);
  display.setCursor(26, 51);
  display.print(ngay);
  display.setCursor(67, 51);
  display.print(nam);
  display.drawBitmap(83, 2, image_weather_humidity_white_bits, 11, 16, 1);
  display.drawBitmap(2, 2, image_weather_temperature_bits, 16, 16, 1);
  display.setCursor(24, 26);
  display.print(hours);
  display.setCursor(41, 26);
  display.print(":");
  display.setCursor(53, 26);
  display.print(minutes);
  display.setCursor(72, 26);
  display.print(":");
  display.setCursor(85, 26);
  display.print(seconds);
  display.setCursor(97, 8);
  display.print(humidity);
  display.setCursor(109, 8);
  display.print("%");
  display.setCursor(19, 8);
  display.print(temperature);
  display.display();
  // }
#else
  spr.fillScreen(TFT_BLACK);
  oldsecond = seconds;
  spr.setTextSize(1);
  byte xpos = 10;
  byte ypos = 17;
  static const unsigned char PROGMEM image_weather_temperature_bits[] = {0x1c, 0x00, 0x22, 0x02, 0x2b, 0x05, 0x2a, 0x02, 0x2b, 0x38, 0x2a, 0x60, 0x2b, 0x40, 0x2a, 0x40, 0x2a, 0x60, 0x49, 0x38, 0x9c, 0x80, 0xae, 0x80, 0xbe, 0x80, 0x9c, 0x80, 0x41, 0x00, 0x3e, 0x00};
  static const unsigned char PROGMEM image_weather_humidity_bits[] = {0x04, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x0e, 0x00, 0x1e, 0x00, 0x1f, 0x00, 0x3f, 0x80, 0x3f, 0x80,
                                                                      0x7e, 0xc0, 0x7f, 0x40, 0xff, 0x60, 0xff, 0xe0, 0x7f, 0xc0, 0x7f, 0xc0, 0x3f, 0x80, 0x0f, 0x00};
  static const unsigned char PROGMEM image_operation_warning_bits[] = {0x00, 0x00, 0x01, 0x80, 0x02, 0x40, 0x02, 0x40, 0x04, 0x20, 0x09,
                                                                       0x90, 0x09, 0x90, 0x11, 0x88, 0x11, 0x88, 0x21, 0x84, 0x40, 0x02, 0x41, 0x82, 0x81, 0x81, 0x80, 0x01, 0x7f, 0xfe, 0x00, 0x00};
  // spr.setTextColor(0x3186, TFT_BLACK);  // Leave a 7 segment ghost image, comment out next line!
  // spr.drawString("88:88:88",xpos,ypos,7); // Overwrite the text to clear it
  spr.setTextColor(0xB7C0);
  if (hours < 10)
    xpos += spr.drawChar('0', xpos, ypos, 7);
  xpos += spr.drawNumber(hours, xpos, ypos, 7);
  xcolon = xpos;
  xpos += spr.drawChar(':', xpos, ypos, 7);
  if (minutes < 10)
    xpos += spr.drawChar('0', xpos, ypos, 7);
  xpos += spr.drawNumber(minutes, xpos, ypos, 7);
  x2colon = xpos;
  xpos += spr.drawChar(':', xpos, ypos, 7);
  if (seconds < 10)
    xpos += spr.drawChar('0', xpos, ypos, 7);
  spr.drawNumber(seconds, xpos, ypos, 7);
  if (seconds % 2)
  { // Flash the colon
    spr.setTextColor(0x39C4, TFT_BLACK);
    xpos += spr.drawChar(':', xcolon, ypos, 7);
    xpos += spr.drawChar(':', x2colon, ypos, 7);
    spr.setTextColor(0xFBE0, TFT_BLACK);
  }
  ////////////////////////////////////////////////////////
  spr.setTextColor(0xFFFF, TFT_BLACK);
  spr.setTextSize(2);
  spr.drawNumber(humidity, 31, 148);
  spr.setTextColor(0xFFFF, TFT_BLACK);
  spr.drawNumber(temperature, 32, 175);
  spr.drawLine(0, 200, 239, 200, 0xFFFF);
  spr.drawBitmap(6, 148, image_weather_humidity_bits, 11, 16, 0x57FF);
  spr.drawBitmap(7, 174, image_weather_temperature_bits, 16, 16, 0xFAAA);
  spr.drawString(daysOfTheWeek[thu], 28, 95);
  spr.drawNumber(ngay, 79, 95);
  spr.drawString(monthOfTheYear[thang], 112, 95);
  spr.drawString("Viet Nam", 12, 215);
  spr.drawNumber(nam, 162, 95);
  static const unsigned char PROGMEM image_wifi_full_bits[] = {0x01, 0xf0, 0x00, 0x07, 0xfc, 0x00, 0x1e, 0x0f, 0x00, 0x39, 0xf3, 0x80, 0x77, 0xfd, 0xc0, 0xef, 0x1e, 0xe0, 0x5c, 0xe7, 0x40, 0x3b, 0xfb, 0x80, 0x17, 0x1d, 0x00, 0x0e, 0xee, 0x00, 0x05, 0xf4, 0x00, 0x03, 0xb8, 0x00, 0x01, 0x50, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00};
  if (WiFi.status() != WL_CONNECTED)
  {
    spr.drawBitmap(216, 210, image_wifi_not_connected_bits, 19, 16, TFT_WHITE, TFT_BLACK);
  }
  else
  {
    if (!ERa_CONNECTED)
    {

      spr.drawBitmap(199, 210, image_operation_warning_bits, 16, 16, 0xFFEA, TFT_BLACK);
      spr.drawBitmap(216, 210, image_wifi_full_bits, 19, 16, TFT_WHITE, TFT_BLACK);
    }
    else
    {
      spr.fillRect(199, 219, 16, 16, TFT_BLACK);
      spr.drawBitmap(216, 210, image_wifi_full_bits, 19, 16, TFT_WHITE, TFT_BLACK);
    }
  }
  spr.pushSprite(0, 0);
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void rotary_loop()
{ 
  if (rotaryEncoder.encoderChanged()) // if rotary change
  {
    // spr.fillScreen(TFT_BLACK);
    currentRotaryValue = rotaryEncoder.readEncoder(); // save currrent value
    lastRotaryChange = millis();
    if (!onsubmenu) // if user not in any sub menu(setting screen)
    {
      if (currentRotaryValue > previousRotaryValue) // same clockwise
      {
        rotatingDown = true; // change startIndex
        dem++;
        if (dem > 5)
        {
          dem = 0;
        }
      }
      else
      {
        rotatingDown = false;
        dem--;
        if (dem < 0)
        {
          dem = 5;
        }
      }
    }
    else // on sub menu
    {
      switch (dem) // Check what sub menu is sel and change value of that submenu
      {
      case 0:                                         // Set time sel
        if (currentRotaryValue > previousRotaryValue) // increase value
        {
          if (onsubmenu1)
          { // if date/time setting was sel
            if (clicked == 1)
            { // check what value user want to change
              switch (settime_menuIndex)
              {
              case 0: // WeekDays
                ++thu;
                if (thu > 6)
                {
                  thu = 0;
                }
                break;
              case 1: // Days
                ngay++;
                if (thang == 1 && ngay > 28)
                {
                  ngay = 1;
                }
                else if (ngay > 31 && (thang == 0 || thang == 2 || thang == 4 || thang == 6 || thang == 7 || thang == 9 || thang == 11))
                {
                  ngay = 1;
                }
                else if (ngay > 30)
                {
                  ngay = 1;
                }
                break;
              case 2: // Months
                thang++;
                if (thang > 11)
                {
                  thang = 0;
                }
                break;
              case 3: // Years
                nam += 1;
                break;
              case 4: // Hours
                gio++;
                if (gio > 24)
                {
                  gio = 0;
                }
                break;
              case 5: // Minutes
                phut++;
                if (phut > 60)
                {
                  phut = 0;
                }
                break;
              default:
                break;
              }
            }
            else
            {
              settime_menuIndex++;
              if (settime_menuIndex > 6)
              {
                settime_menuIndex = 0;
              }
            }
          }
          else if (onsubmenu1a == true)
          { // if region sel
            utc++;
            if (utc > 26)
            {
              utc = 0;
            }
          }
          else
          {
            rotatingDown = true;
            time_menuIndex++;
            if (time_menuIndex > 4)
              time_menuIndex = 0;
          }
        }
        else // decrease value
        {
          if (onsubmenu1)
          {
            if (clicked == 1)
            {
              switch (settime_menuIndex)
              {
              case 0:
                --thu;
                if (thu < 0)
                {
                  thu = 6;
                }
                break;
              case 1:
                ngay--;
                if (thang == 1 && ngay < 1)
                {
                  ngay = 28;
                }
                else if (ngay < 1 && (thang == 0 || thang == 2 || thang == 4 || thang == 6 || thang == 7 || thang == 9 || thang == 11))
                {
                  ngay = 31;
                }
                else if (ngay < 1)
                {
                  ngay = 30;
                }
                break;
              case 2:
                thang--;
                if (thang < 0)
                {
                  thang = 11;
                }
                break;
              case 3:
                nam -= 1;
                break;
              case 4:
                gio--;
                if (gio < 0)
                {
                  gio = 24;
                }
                break;
              case 5:
                phut--;
                if (phut < 0)
                {
                  phut = 60;
                }
                break;
              default:
                break;
              }
            }
            else
            {
              settime_menuIndex--;
              if (settime_menuIndex < 0)
              {
                settime_menuIndex = 6;
              }
            }
          }
          else if (onsubmenu1a == true)
          {
            utc--;
            if (utc < 0)
            {
              utc = 26;
            }
          }
          else
          {
            rotatingDown = true;
            time_menuIndex--;
            if (time_menuIndex < 0)
              time_menuIndex = 4;
          }
        }
        break;
      case 1: // wifi sel
        if (currentRotaryValue > previousRotaryValue)
        {
          rotatingDown = true;
          wifiMenu_choose++;
          if (wifiMenu_choose > 5)
          {
            wifiMenu_choose = 0;
          }
        }
        else
        {
          rotatingDown = false;
          wifiMenu_choose--;
          if (wifiMenu_choose < 0)
          {
            wifiMenu_choose = 5;
          }
        }
        break;
      case 2: // Change offset value
        if (currentRotaryValue > previousRotaryValue)
        {
          offset = offset - 0.1;
          if (offset < -5)
          {
            offset = 5;
          }
        }
        else
        {
          offset = offset + 0.1;
          if (offset > 5)
          {
            offset = -5;
          }
        }
        if (_utc != utc)
        {
          nothing_changed = false;
        }
        else
        {
          nothing_changed = true;
        }
        break;
      case 4: // Choose Reset or Not
        if (currentRotaryValue > previousRotaryValue)
        {
          reset_sel++;
          if (reset_sel > 1)
          {
            reset_sel = 0;
          }
        }
        else
        {
          reset_sel--;
          if (reset_sel < 0)
          {
            reset_sel = 1;
          }
        }
        break;
      default:
        break;
      }
    }
    previousRotaryValue = currentRotaryValue;
  }
  handle_rotary_button();
}
void handle_rotary_button()
{
  static unsigned long lastTimeButtonDown = 0;
  unsigned long time_clicked = 0;
  static bool isLongpress = false;
  static bool isSettingpress = false;
  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
  if (isEncoderButtonDown)
  {
    if (!lastTimeButtonDown)
    {
      lastTimeButtonDown = millis();
    }
    if (!isLongpress && (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) && onsubmenu)
    {
      Serial.println("button LONG press ");
      isLongpress = true;
      if (onsubmenu1a == true)
      { // back from region setting ( hold 2s)
        onsubmenu1a = false;
        time_menuIndex = 2;
        time_setting();
        return;
      }
      if (dem == 1 && WiFi.status() != WL_CONNECTED)
      { // back from disconnect screen
        onsubmenu = false;
        displayMenu();
        return;
      }
      if (dem == 2 || dem == 3)
      { // back from cablib screen
        onsubmenu = false;
        displayMenu();
        return;
      }
    }
    else // đè 3s để vào setting menu
    {
      if (!isSettingpress && onsubmenu == false && (millis() - lastTimeButtonDown >= entersettingAfterMiliseconds))
      {
        Serial.println("Setting Press");
        setting = !setting;
        isSettingpress = true;
        display.fillScreen(TFT_BLACK);
      }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  }
  else
  {
    if (lastTimeButtonDown && !isSettingpress && !isLongpress && setting)
    {
      Serial.println("button SHORT press ");
      onsubmenu = true;
      /////////////////////// Button control on Set Time submenu( Date/Time Setting -> Set Time) /////////////////
      if (rotaryEncoder.readButtonState() == BUT_DOWN && onsubmenu1 == true)
      {
        clicked++;
      }
      if (clicked > 1)
      {
        clicked = 0;
      }
      ///////////////////////////////////////////////////////////////////////////////

      ////////////////////datet/time Setting Action /////////////////////////////////
      if (dem == 0 && ontime_setting == true)
      {                         // control submenu 1
        switch (time_menuIndex) // Date/time Setting
        {
        case 0: // Set time setting
          onsubmenu1 = true;
          if (settime_menuIndex == 6)
          { // Back from Date/time Seting
            onsubmenu1 = false;
            settime_menuIndex = 0;
            clicked = 0;
            time_setting();
          }
          break;
        case 1:
          auto_time_mode = !auto_time_mode;
          break;
        case 2: // Region setting
          onsubmenu1a = true;
          break;
        case 3: // Save
          for (int x = 0; x < 20; x++)
          {
            // display.clearDisplay();
            display.fillScreen(TFT_BLACK);
            display.setTextColor(TFT_WHITE);
            display.setTextSize(3);
            display.setCursor(24, 18);
            display.print("SAVED");
            // display.display();
          }
          // EEPROM.writeInt(add_gio,hours);
          // EEPROM.writeInt(add_phut,minutes);
          // EEPROM.writeInt(add_giay,seconds);
          // EEPROM.writeInt(add_utc,utc);
          // EEPROM.writeBool(add_auto_time,auto_time_mode);
          // EEPROM.commit();
          current_time = (gio * 3600 + phut * 60) - (list[utc] * 3600);
          time_setting();
          break;
        case 4: // Back
          onsubmenu = false;
          ontime_setting = false;
          time_menuIndex = 0;
          displayMenu();
          break;
        default:
          break;
        }
      }
      //////////////////////////////////////////////////////////////////////

      /////// Hard reset Action ////////////////////////////////////////////
      if (reset_sel == 1 && dem == 4 && onhard_reset == true)
      {
        enable_reset = true;
      }
      else if (reset_sel == 0 && dem == 4 && onhard_reset == true)
      {
        onsubmenu = false;
        onhard_reset = false;
        reset_sel = 0;
        displayMenu();
      }
      //////////////////////////////////////////////////////////////////////

      ///////////////// WiFi Setting Action /////////////////////////////////
      if (wifiMenu_choose == 4 && dem == 1 && WiFi.status() == WL_CONNECTED)
      {
        // display.clearDisplay();
        display.fillScreen(TFT_BLACK);
        display.setTextColor(TFT_WHITE);
        display.setCursor(16, 13);
        display.print("Disconnecting");
        // display.display();
        display.setCursor(95, 13);
        for (int i = 0; i < 5; i++)
        {
          display.print(".");
          // display.display();
          delay(300);
        }
        auto_time_mode = false;
        current_time = (hours * 3600 + minutes * 60 + seconds) - (list[utc] * 3600);
        wifi();
        ERa.switchToConfig(true);
        delay(100);
      }
      else if (wifiMenu_choose == 5 && dem == 1 && WiFi.status() == WL_CONNECTED)
      {
        onsubmenu = false;
        wifiMenu_choose = -1;
        displayMenu();
      }
      //////////////////////////////////////////////////////////////////////////

      ///////////// Back Sel Action ( displayMenu() ) //////////////////////////
      if (dem == 5 && setting == true)
      {
        setting = false;
        onsubmenu = false;
        dem = 0;
        maindisplay();
      }
      /////////////////////////////////////////////////////////////////////////
    }
    isLongpress = false;
    isSettingpress = false;
    lastTimeButtonDown = 0;
  }
}
void set_time()
{
#ifdef SH110X
  const char *settime_menu[] = {"WeekDays:", "Days:", "Months:", "Years:", "Hours:", "Minutes:", "Back"};
  char str_ngay[4]; // Make sure the buffer is large enough
  char str_nam[5];
  char str_hours[3];
  char str_minutes[3];
  sprintf(str_ngay, "%u", ngay);
  sprintf(str_nam, "%u", nam);
  sprintf(str_hours, "%u", gio);
  sprintf(str_minutes, "%u", phut);
  const char *rs[] = {daysOfTheWeek[thu], str_ngay, monthOfTheYear[thang], str_nam, str_hours, str_minutes};
  display.clearDisplay(); 
  display.fillScreen(TFT_BLACK);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(11, 3);
  display.print("Date/Time Setting");
  if (auto_time_mode == true && ERa_CONNECTED == true)
  {
    display.clearDisplay();
    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE);
    display.setCursor(17, 14);
    display.print("AuTo Time Is ON");
    display.setCursor(6, 33);
    display.print("Turn OFF To Use This");
    display.display();  
    delay(2000);
    onsubmenu1 = false;
    time_setting();
    return;
  }

  if (rotatingDown)
  {
    startsetTimeIndex = settime_menuIndex - 2;
    endsetTimeIndex = settime_menuIndex + 1;
  }
  else
  {
    startsetTimeIndex = settime_menuIndex - 1;
    endsetTimeIndex = settime_menuIndex + 2;
  }
  if (startsetTimeIndex < 0)
  {
    startsetTimeIndex = 0;
    endsetTimeIndex = 3;
  }
  else if (endsetTimeIndex >= 7)
  {
    endsetTimeIndex = 6;
    startsetTimeIndex = endsetTimeIndex - 3;
  }
  for (int i = startsetTimeIndex; i <= endsetTimeIndex; i++)
  {
    display.setCursor(6, (i - startsetTimeIndex) * 12 + 13);
    if (i == settime_menuIndex)
    {
      if (clicked == 1 && i <= 5)
      {
        display.drawRoundRect(strlen(settime_menu[i]) * 5 + 12, (i - startsetTimeIndex) * 12 + 11, strlen(rs[i]) * 5 + 10, 12, 3, SH110X_WHITE);
      }
      else
      {
        display.drawRoundRect(0, (i - startsetTimeIndex) * 12 + 11, 120, 12, 3, SH110X_WHITE);
      }
    }
    display.print(settime_menu[i]);
    if (i > 5)
    {
      continue;
    }
    display.setCursor(strlen(settime_menu[i]) * 5 + 15, (i - startsetTimeIndex) * 12 + 13);
    display.print(rs[i]);
  }
  display.display();
#else
   
#endif
}
void time_setting()
{
#ifdef SH110X
  if (onsubmenu1 == true)
  {
    set_time();
    return;
  }
  if (onsubmenu1a == true)
  {
    region();
    return;
  }
  String time_menu[] = {"Set Time", "Sync Time: ", "Region", "Save", "Back"};
  String mode[] = {"OFF", "ON"};
  display.clearDisplay();
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.print("Time Setting");
  if (rotatingDown)
  {
    startTimeIndex = time_menuIndex - 2;
    endTimeIndex = time_menuIndex + 1;
  }
  else
  {
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
    endTimeIndex = 4;
    startTimeIndex = endTimeIndex - 3;
  }
  for (int i = startTimeIndex; i <= endTimeIndex; i++)
  {
    display.setCursor(6, (i - startTimeIndex) * 12 + 13);
    if (i == time_menuIndex)
    {
      display.drawRoundRect(0, (i - startTimeIndex) * 12 + 11, 120, 12, 3, SH110X_WHITE);
    }
    display.print(time_menu[i]);
  }
  display.setCursor(75, (1 - startTimeIndex) * 13 + 13);
  display.print(mode[auto_time_mode]);
  display.display();
#else
  spr.fillSprite(TFT_BLACK);
  if (onsubmenu1 == true)
  {
    set_time();
    return;
  }
  if (onsubmenu1a == true)
  {
    region();
    return;
  }
  String time_menu[] = {"Set Time", "Sync Time: ", "Region", "Save", "Back"};
  String mode[] = {"OFF", "ON"};
  spr.setTextColor(TFT_WHITE);
  spr.setTextSize(2);
  spr.drawString("Time Setting", 46, 12);
  if (rotatingDown)
  {
    startTimeIndex = time_menuIndex - 3;
    endTimeIndex = time_menuIndex + 2;
  }
  else
  {
    startTimeIndex = time_menuIndex - 3;
    endTimeIndex = time_menuIndex + 2;
  }
  if (startTimeIndex < 0)
  {
    startTimeIndex = 0;
    endTimeIndex = 4; // maxVisibleItems - 1
  }
  else if (endTimeIndex >= 5) // numMenus
  {
    endTimeIndex = 4;                  // numMenus-1
    startTimeIndex = endTimeIndex - 4; // maxVisibleItems - 1
  }
  for (int i = startTimeIndex; i <= endTimeIndex; i++)
  {
    spr.setCursor(14, (i - startTimeIndex) * 33 + 33);
    if (i == time_menuIndex)
    {
      spr.drawRoundRect(10, (i - startTimeIndex) * 33 + 30, 200, 25, 4, TFT_WHITE);
    }
    if (i == 1)
    {
      byte x;
      x += spr.drawString(time_menu[i], 14, (i - startTimeIndex) * 33 + 33);
      spr.setCursor(x, (1 - startTimeIndex) * 33 + 33);
      spr.print(mode[auto_time_mode]);
      continue;
    }
    spr.print(time_menu[i]);
  }
  spr.pushSprite(0, 0);
#endif
}
void region()
{
#ifdef SH110X
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(25, 4);
  display.print("Region Setting");
  display.setCursor(34, 19);
  display.print("UTC:");
  display.setCursor(68, 19);
  display.print(region_list[utc]);
  display.setTextSize(2);
  display.setCursor(15, 43);
  display.print(hours);
  display.setTextSize(1);
  display.setCursor(42, 47);
  display.print(":");
  display.setTextSize(2);
  display.setCursor(53, 43);
  display.print(minutes);
  display.setTextSize(1);
  display.setCursor(81, 47);
  display.print(":");
  display.setTextSize(2);
  display.setCursor(92, 43);
  display.print(seconds);
  display.display();
#else
  static const unsigned char PROGMEM image_earth_bits[] = {0x07, 0xc0, 0x1e, 0x70, 0x27, 0xf8, 0x61, 0xe4, 0x43, 0xe4, 0x87, 0xca, 0x9f, 0xf6, 0xdf, 0x82, 0xdf, 0x82, 0xe3, 0xc2, 0x61, 0xf4, 0x70, 0xf4, 0x31, 0xf8, 0x1b, 0xf0, 0x07, 0xc0, 0x00, 0x00};
  spr.fillScreen(TFT_BLACK);
  spr.setTextColor(TFT_WHITE);
  spr.setTextColor(0xFFFF);
  spr.setTextSize(2);
  spr.drawString("Region Setting", 38, 45);
  spr.setTextColor(0x57FF);
  spr.drawString("UTC:", 59, 85);
  spr.drawBitmap(37, 85, image_earth_bits, 15, 16, 0x57FF);
  spr.setTextColor(0xFFEA);
  spr.drawString(region_list[utc], 119, 84);
  byte xpos = 15;
  byte ypos = 143;
  spr.setTextColor(0xB7C0);
  spr.setTextSize(1);
  if (hours < 10)
    xpos += spr.drawChar('0', xpos, ypos, 7);
  xpos += spr.drawNumber(hours, xpos, ypos, 7);
  xcolon = xpos;
  xpos += spr.drawChar(':', xpos, ypos, 7);
  if (minutes < 10)
    xpos += spr.drawChar('0', xpos, ypos, 7);
  xpos += spr.drawNumber(minutes, xpos, ypos, 7);
  x2colon = xpos;
  xpos += spr.drawChar(':', xpos, ypos, 7);
  if (seconds < 10)
    xpos += spr.drawChar('0', xpos, ypos, 7);
  spr.drawNumber(seconds, xpos, ypos, 7);
  if (seconds % 2)
  { // Flash the colon
    spr.setTextColor(0x39C4, TFT_BLACK);
    xpos += spr.drawChar(':', xcolon, ypos, 7);
    xpos += spr.drawChar(':', x2colon, ypos, 7);
    spr.setTextColor(0xFBE0, TFT_BLACK);
  }
  spr.pushSprite(0,0);
#endif
}
void calib()
{
#ifdef SH110X
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(10, 0);
  display.print("OFF Setting");
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20, 20);
  display.setTextSize(1);
  display.print("OFFSET: ");
  display.setCursor(70, 20);
  display.print(offset);
  display.display();
#else
  spr.fillScreen(TFT_BLACK);
  spr.setTextColor(TFT_WHITE);
  spr.setTextColor(0xFFEA);
  spr.setTextSize(2);
  spr.drawString("OFFSET Setting", 36, 34);
  spr.setTextColor(0x57FF);
  spr.drawString("OFFSET:", 37, 108);
  spr.drawFloat(offset, 2, 140, 108);
  spr.pushSprite(0, 0);
#endif
}
void hard_reset()
{
#ifdef SH110X
  String choose[] = {"No", "Yes"};
  // display.clearDisplay();
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE);
  display.setCursor(16, 8);
  display.print("Hard Resetting...");
  display.setCursor(4, 28);
  display.print("Are You Sure?");
  display.setCursor(90, 28);
  if (reset_sel > 0)
    display.print(choose[reset_sel]);
  else
  {
    display.print(choose[0]);
    reset_sel = 0;
  }
  if (enable_reset)
  {
    display.clearDisplay();
    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE);
    display.setCursor(31, 23);
    display.print("Resetting..");
    display.display();
    for (int i = 0; i < FLASH_MEMORY_SIZE; i++)
    {
      EEPROM.write(i, 0);
      EEPROM.commit();
    }
    WiFi.disconnect();
    delay(2000);
    ESP.restart();
  }
  display.display();
#else
  static const unsigned char PROGMEM image_Warning_bits[] = {0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00,
                                                             0x0f, 0xc0, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x7c, 0xf8, 0x00, 0x00, 0xfc, 0xfc, 0x00,
                                                             0x00, 0xfc, 0xfc, 0x00, 0x01, 0xfc, 0xfe, 0x00, 0x03, 0xfc, 0xff, 0x00, 0x03, 0xfc, 0xff, 0x00, 0x07, 0xfc, 0xff, 0x80, 0x0f, 0xfc, 0xff,
                                                             0xc0, 0x0f, 0xfc, 0xff, 0xc0, 0x1f, 0xfc, 0xff, 0xe0, 0x3f, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xf0, 0x7f, 0xfc, 0xff, 0xf8, 0xff, 0xfc,
                                                             0xff, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xf8};
  static const unsigned char PROGMEM image_Restoring_bits[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe,
                                                               0x00, 0x00, 0x00, 0x07, 0x01, 0x80, 0x00, 0x00, 0x18, 0x00, 0x40, 0x00, 0x00, 0x20, 0x7e, 0x40, 0x00, 0x00, 0x43, 0xff, 0xc0, 0x00, 0x00,
                                                               0x87, 0x81, 0x80, 0x00, 0x00, 0x8c, 0x00, 0x02, 0x00, 0x01, 0x10, 0x00, 0x05, 0x00, 0x01, 0x10, 0x00, 0x08, 0x80, 0x02, 0x20, 0x00, 0x10,
                                                               0x40, 0x02, 0x20, 0x00, 0x20, 0x20, 0x04, 0x40, 0x00, 0x40, 0x10, 0x04, 0x40, 0x00, 0x80, 0x08, 0x04, 0x40, 0x00, 0xf8, 0xf8, 0x7c, 0x7c,
                                                               0x00, 0xf8, 0xf8, 0x40, 0x04, 0x00, 0x08, 0x80, 0x60, 0x0c, 0x00, 0x08, 0x80, 0x70, 0x1c, 0x00, 0x11, 0x00, 0x38, 0x38, 0x00, 0x11, 0x00,
                                                               0x1c, 0x70, 0x00, 0x22, 0x00, 0x0e, 0xe0, 0x00, 0x22, 0x00, 0x07, 0xc0, 0x00, 0xc4, 0x00, 0x03, 0x80, 0x00, 0x84, 0x00, 0x00, 0x06, 0x07,
                                                               0x08, 0x00, 0x00, 0x09, 0xf8, 0x10, 0x00, 0x00, 0x08, 0x00, 0x70, 0x00, 0x00, 0x0e, 0x03, 0xe0, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 0x00,
                                                               0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  String choose[] = {"No", "Yes"};
  // display.clearDisplay();
  spr.fillScreen(TFT_BLACK);
  spr.setTextColor(TFT_WHITE);
  spr.drawBitmap(22, 15, image_Warning_bits, 30, 23, 0xFFEA);
  spr.drawBitmap(178, 13, image_Warning_bits, 30, 23, 0xFFEA);
  spr.setTextColor(0xFFFF);
  spr.setTextSize(2);
  spr.drawString("Hard Resetting...", 22, 58);
  spr.drawString("Are You Sure?", 12, 135);
  if (reset_sel > 0)
  {
    spr.setCursor(182, 135);
    spr.print(choose[reset_sel]);
  }
  else
  {
    spr.setCursor(182, 135);
    spr.print(choose[0]);
    reset_sel = 0;
  }
  if (enable_reset)
  {
    spr.fillScreen(TFT_BLACK);
    spr.setTextColor(TFT_WHITE);
    spr.setTextColor(0xFFFF);
    spr.setTextSize(2);
    spr.drawString("Resetting", 53, 66);
    spr.drawBitmap(91, 108, image_Restoring_bits, 38, 32, 0xFFFF);
    spr.drawBitmap(18, 58, image_Warning_bits, 30, 23, 0xFFEA);
    for (int i = 0; i < FLASH_MEMORY_SIZE; i++)
    {
      EEPROM.write(i, 0);
      EEPROM.commit();
    }
    int xpos = 145;
    for (int i = 0; i < 5; i++)
    {
      spr.setTextSize(2);
      xpos += 15;
      spr.drawString(".", xpos, 68);
      spr.pushSprite(0, 0);
      delay(500);
    }
    WiFi.disconnect();
    static const unsigned char PROGMEM image_checked_bits[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x1c, 0x80, 0x38, 0xc0, 0x70, 0xe0, 0xe0, 0x71, 0xc0, 0x3b, 0x80, 0x1f, 0x00, 0x0e, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
    spr.fillScreen(TFT_BLACK);
    spr.setTextColor(TFT_WHITE);
    spr.setTextColor(0x57EA);
    spr.setTextSize(3);
    spr.drawString("Done", 68, 49);
    spr.setTextColor(0xFFEA);
    spr.setTextSize(2);
    spr.drawString("Clock Will Restart", 13, 114);
    spr.drawBitmap(146, 53, image_checked_bits, 14, 16, 0x57EA);
    spr.pushSprite(0, 0);
    delay(1000);
    ESP.restart();
  }
  spr.pushSprite(0, 0);
#endif
}
void Chip_Temp()
{
#ifdef SH110X
  static const unsigned char PROGMEM image_weather_temperature_bits[] = {0x1c, 0x00, 0x22, 0x02, 0x2b, 0x05, 0x2a, 0x02, 0x2b, 0x38, 0x2a,
                                                                         0x60, 0x2b, 0x40, 0x2a, 0x40, 0x2a, 0x60, 0x49, 0x38, 0x9c, 0x80, 0xae, 0x80, 0xbe, 0x80, 0x9c, 0x80, 0x41, 0x00, 0x3e, 0x00};
  display.clearDisplay();
  display.setTextColor(TFT_WHITE);
  display.setTextSize(1);
  display.setCursor(21, 15);
  display.print("Chip Temperature");
  display.setTextSize(2);
  display.setCursor(36, 34);
  display.print(nhietdo);
  display.drawBitmap(2, 11, image_weather_temperature_bits, 16, 16, 1);
  display.display();
#else
    static const unsigned char PROGMEM image_weather_temperature_bits[] = {0x1c,0x00,0x22,0x02,0x2b,0x05,0x2a,0x02,0x2b,0x38,0x2a,
    0x60,0x2b,0x40,0x2a,0x40,0x2a,0x60,0x49,0x38,0x9c,0x80,0xae,0x80,0xbe,0x80,0x9c,0x80,0x41,0x00,0x3e,0x00};
    display.clearDisplay();
    display.setTextColor(TFT_WHITE);
    display.setTextSize(1);
    display.setCursor(21, 15);
    display.print("Chip Temperature");
    display.setTextSize(2);
    display.setCursor(36, 34);
    display.print(nhietdo);
    display.drawBitmap(2, 11, image_weather_temperature_bits, 16, 16, 1);
    display.display();
#endif
}
String ConverIpToString(IPAddress ip)
{
  String res = "";
  for (int i = 0; i < 3; i++)
  {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
String RSSIasQuality()
{
  int res;
  String rssi;
  preRSSI = millis();
  res = WiFi.RSSI();
  Serial.println(res);
  if (res >= -30)
  {
    rssi = "Amazing";
  }
  else if (res >= -55 && res < -30)
  {
    rssi = "Very Good";
  }
  else if (res >= -67 && res < -55)
  {
    rssi = "Fairly Good";
  }
  else if (res >= -70 && res < -67)
  {
    rssi = "Ok";
  }
  else if (res >= -80 && res < -70)
  {
    rssi = "Low";
  }
  else if (res > -90 && res < -80)
  {
    rssi = "Very Low";
  }
  else
  {
    rssi = "No Signal";
  }
  return rssi;
}
void wifi()
{
#ifdef SH110X
  if (WiFi.status() != WL_CONNECTED)
  {
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
  }
  else if (WiFi.status() == WL_CONNECTED)
  {
    IPAddress ip;
    ip = WiFi.localIP();

    String mac;
    if (millis() - preRSSI >= 5000)
    {
      rssi = RSSIasQuality();
    }
    mac = WiFi.macAddress();
    const char *wifi_menu[] = {"SSID: ", "IP:", "MAC:", "RSSI: ", "Disconnect", "Back"};
    String infor[] = {WiFi.SSID(), ConverIpToString(ip), mac, rssi};
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextSize(1);
    display.setCursor(10, 0);
    display.print("WiFi Setting");
    if (rotatingDown)
    {
      startWifiIndex = wifiMenu_choose - 2;
      endWifiIndex = wifiMenu_choose + 1;
    }
    else
    {
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
      endWifiIndex = 5;
      startWifiIndex = endWifiIndex - 3;
    }
    for (int i = startWifiIndex; i <= endWifiIndex; i++)
    {
      if (i == wifiMenu_choose)
      {
        display.drawRoundRect(0, (i - startWifiIndex) * 12 + 10, 128, 12, 3, SH110X_WHITE);
      }
      display.setCursor(2, (i - startWifiIndex) * 12 + 12);
      display.print(wifi_menu[i]);
      if (i > 3)
      {
        continue;
      }
      display.setCursor(strlen(wifi_menu[i]) * 3 + 12, (i - startWifiIndex) * 12 + 12);
      display.print(infor[i]);
    }
    display.display();
  }
#else
  if (WiFi.status() != WL_CONNECTED)
  {
    spr.fillScreen(TFT_BLACK);
    static const unsigned char PROGMEM image_frame_removebg_preview_bits[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x3c, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x3c, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x3c, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0x3c, 0xfe, 0x7f, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x03, 0xcf, 0x3c, 0x1e, 0x7f, 0x3c, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x03, 0xcf, 0x3c, 0x1e, 0x7f, 0x3c, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0x3c, 0xff, 0xff, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xc0, 0x00, 0xf3, 0xc0, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xc0, 0x00, 0xf3, 0xc0, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xfc, 0xff, 0xf8, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xfc, 0xff, 0xf8, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xfc, 0xff, 0xf8, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xfc, 0xff, 0xff, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xc1, 0xe0, 0x03, 0xcf, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xc1, 0xe0, 0x03, 0xcf, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xe7, 0xff, 0xcf, 0x3c, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x03, 0xcf, 0xe7, 0xfe, 0x00, 0x3c, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x03, 0xcf, 0xe7, 0xfe, 0x00, 0x3c, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xcf, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0x3c, 0xf3, 0xcf, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0x3c, 0xf3, 0xcf, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xcf, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf3, 0xff, 0xff, 0x9f, 0xf9, 0xe7, 0x9e, 0x78, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf3, 0xff, 0xe7, 0x9f, 0xf9, 0xe7, 0x9e, 0x78, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf3, 0xff, 0xe7, 0x9f, 0xf9, 0xe7, 0x9e, 0x78, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0x9e, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0xfe, 0x0f, 0x3c, 0xf0, 0x79, 0xfc, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0xfe, 0x0f, 0x3c, 0xf0, 0x79, 0xfc, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0x3c, 0xfe, 0x7f, 0xff, 0xf3, 0xcf, 0x00, 0x00, 0x00, 0x01, 0xff, 0x83, 0xc0, 0x3c, 0x1e, 0x0f, 0x3f, 0xf3, 0xc0, 0x00, 0x00, 0x00, 0x01, 0xff, 0x83, 0xc0, 0x3c, 0x1e, 0x0f, 0x3f, 0xf3, 0xc0, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x9f, 0xc0, 0x3c, 0xff, 0xcf, 0xff, 0xf3, 0xc0, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x1e, 0x00, 0x00, 0xf3, 0xc1, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x1e, 0x00, 0x00, 0xf3, 0xc1, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0xff, 0xf9, 0xfc, 0xf3, 0xf9, 0xe7, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x3c, 0xff, 0xf9, 0xfc, 0xf0, 0x78, 0x07, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x3c, 0xff, 0xf9, 0xfc, 0xf0, 0x78, 0x07, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x3c, 0xff, 0xf9, 0xfc, 0xf3, 0xff, 0xff, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x1e, 0x79, 0xe0, 0x03, 0xcf, 0xff, 0xf0, 0x78, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x1e, 0x79, 0xe0, 0x03, 0xcf, 0xff, 0xf0, 0x78, 0x00, 0x00, 0x00, 0x0f, 0x00, 0xff, 0xf9, 0xe7, 0xf3, 0xff, 0xff, 0xf3, 0xff, 0x00, 0x00, 0x00, 0x0f, 0x00, 0xf3, 0xf8, 0x07, 0xf0, 0x7f, 0x3c, 0xf3, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x00, 0xf3, 0xf8, 0x07, 0xf0, 0x7f, 0x3c, 0xf3, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x00, 0xff, 0xf8, 0x07, 0xf0, 0x7f, 0xfc, 0xf3, 0xff, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x1e, 0x78, 0x07, 0x80, 0x01, 0xe0, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x1e, 0x78, 0x07, 0x80, 0x01, 0xe0, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x3c, 0xff, 0xff, 0xe7, 0x80, 0x7f, 0xff, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x3c, 0xff, 0xff, 0xe7, 0x80, 0x7f, 0xff, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x3c, 0xff, 0xff, 0xe7, 0x80, 0x7f, 0xff, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x3c, 0xff, 0xff, 0xe7, 0xfe, 0x7f, 0xff, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0xfe, 0x0f, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0xfe, 0x0f, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0xfc, 0xff, 0xff, 0x3c, 0xf3, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0xfc, 0x1f, 0xff, 0x3c, 0xf3, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0xfc, 0x1f, 0xff, 0x3c, 0xf3, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0xfc, 0xff, 0xff, 0x3c, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x03, 0xc1, 0xe0, 0xfe, 0x7f, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x03, 0xc1, 0xe0, 0xfe, 0x7f, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xe0, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0x00, 0xf3, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0x00, 0xf3, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xe0, 0xf3, 0xff, 0xff, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xe0, 0x00, 0x0f, 0x3f, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xe0, 0x00, 0x0f, 0x3f, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xff, 0xf0, 0x7f, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0x3f, 0xf0, 0x78, 0x07, 0xf3, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0x3f, 0xf0, 0x78, 0x07, 0xf3, 0xcf, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0xf3, 0xcf, 0xff, 0xf3, 0xff, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x03, 0xcf, 0xe7, 0x83, 0xff, 0x3c, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x03, 0xcf, 0xe7, 0x83, 0xff, 0x3c, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0xff, 0x83, 0xff, 0x3c, 0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0x3f, 0x80, 0x0f, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0x3f, 0x80, 0x0f, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xcf, 0x3f, 0x80, 0x0f, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    spr.setTextSize(2);
    spr.setTextColor(0xFFFF);
    spr.drawString("Connect To: ", 5, 24);
    spr.setTextColor(0xFFFF);
    spr.drawString("To Config WiFi", 32, 55);
    spr.drawString("Your Token Is:", 5, 81);
    spr.setTextColor(0xFFEA);
    spr.setTextSize(1);
    spr.drawString(ERA_AUTH_TOKEN, 15, 107);
    uint16_t colour = random(0x10000);
    spr.setTextColor(colour);
    spr.setTextSize(2);
    spr.drawString(SSID_AP, 148, 24);
    spr.drawBitmap(69, 127, image_frame_removebg_preview_bits, 100, 100, 0xFFFF);
    spr.pushSprite(0, 0);
  }
  else if (WiFi.status() == WL_CONNECTED)
  {
    IPAddress ip;
    ip = WiFi.localIP();
    String mac;
    if (millis() - preRSSI >= 5000)
    {
      rssi = RSSIasQuality();
    }
    mac = WiFi.macAddress();
    const char *wifi_menu[] = {"SSID: ", "IP:", "MAC:", "RSSI: ", "Disconnect", "Back"};
    String infor[] = {WiFi.SSID(), ConverIpToString(ip), mac, rssi};
    spr.fillScreen(TFT_BLACK);
    spr.setTextColor(TFT_WHITE);
    spr.setTextSize(2);
    spr.drawString("WiFi Infomation", 42, 15);
    if (rotatingDown)
    {
      startWifiIndex = wifiMenu_choose - 3;
      endWifiIndex = wifiMenu_choose + 2;
    }
    else
    {
      startWifiIndex = wifiMenu_choose - 3;
      endWifiIndex = wifiMenu_choose + 2;
    }
    if (startWifiIndex < 0)
    {
      startWifiIndex = 0;
      endWifiIndex = 5;
    }
    else if (endWifiIndex >= 6)
    {
      endWifiIndex = 5;
      startWifiIndex = endWifiIndex - 5;
    }
    for (int i = startWifiIndex; i <= endWifiIndex; i++)
    {
      spr.setTextSize(1);
      if (i == wifiMenu_choose)
      {
        spr.drawRoundRect(2, (i - startWifiIndex) * 34 + 40, 230, 12, 3, TFT_WHITE);
      }
      spr.drawString(wifi_menu[i], 7, (i - startWifiIndex) * 34 + 42);
      if (i > 3)
      {
        continue;
      }
      spr.setCursor(strlen(wifi_menu[i]) * 3 + 43, (i - startWifiIndex) * 34 + 42);
      spr.print(infor[i]);
    }
    spr.pushSprite(0, 0);
  }
#endif
}