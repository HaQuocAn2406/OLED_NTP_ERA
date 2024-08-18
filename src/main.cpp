
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
#include <JPEGDecoder.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ElegantOTA.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "bitmap.h"
#include "Free_Fonts.h"
#include "icon.h"
#include "tahoma10pt7b.h"
// #define GFXFF 1
#define TAHOMA &tahoma10pt7b
#define ENCODER_CLK 25
#define ENCODER_DT 26
#define ENCODER_SW 27
#define ENCODER_VCC -1
#define FLASH_MEMORY_SIZE 200
#define ENCODER_STEPS 4
#define DHTPIN 5
#define minimum(a, b) (((a) < (b)) ? (a) : (b))
#define sun image_weather_sun_bits
// #define SH110X
bool screen = false;
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
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
byte xcolon = 0;
byte x2colon = 0;
#endif
DHTesp dht;
ERaEspTime syncTime;
TimeElement_t ntpTime;
AiEsp32RotaryEncoder rotaryEncoder(ENCODER_CLK, ENCODER_DT, ENCODER_SW);
WebServer server(80);
String URL = "http://api.openweathermap.org/data/2.5/weather?";
/// @brief eb1d2c68ab206e3e4ecf26becc7ddc9c
String API_KEY = "eb1d2c68ab206e3e4ecf26becc7ddc9c";
WiFiClient client;
// THU DUC //
String lat = "10.8201506";
String lon = "106.7074488";

const char *daysOfTheWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *monthOfTheYear[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};
char SSID_AP[] = {"ERA"};
String menus[] = {"Date/Time", "WiFi", "Alarm", "Calib DHT", "Hard Reset", "Back"};
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
String location = "Hello";
int16_t DoW;
int16_t months = 0;
uint16_t years = 1970;
uint16_t days = 1;
uint16_t hours, _hours;
uint16_t minutes, _minutes;
uint16_t seconds;
uint16_t oldsecond;
int batt = 100;
int clicked = 0;
bool enable_reset = false;
bool auto_time_mode = false;
bool auto_region_mode = false;
bool sub_menu_flag = false;
bool rotatingDown;
bool setting_menu_flag = false;
bool ERa_CONNECTED = false;
bool set_time_flag = false;
bool region_flag = false;
bool ontime_setting = false;
bool onhard_reset = false;
int startWifiIndex, endWifiIndex;
int startTimeIndex, endTimeIndex, startsetTimeIndex, endsetTimeIndex;
int second_count = 0;
int reset_sel = 0;
int offset_utc;
int utc = 19;
int startIndex;
int endIndex;
int maxVisibleItems = 6;
int numMenus = 6;
int setting_menuIndex = 0;
int time_menuIndex = 0;
int settime_menuIndex = 0;
int currentRotaryValue;
int previousRotaryValue;
int wifiMenu_choose = 0;
int id;
float offset = 0.00;
float _offset;
float nhietdo;
float humi_room;
float temp_room;
float temp_outside, humi_outside;
const int add_hours = 0;
const int add_minutes = 4;
const int add_seconds = 8;
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
long prevMillis = 0, prevMillis_client = 0;
String result;
String weather;
String icon_id;
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
void Alarm();
void wifi();
void hienthi();
void weather_screen();
void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos);
void renderJPEG(int xpos, int ypos);
void connect_succes();
void get_openweather();
void time_calculate(unsigned long current_time);
int batt_get();
void updating();
void updating_done(bool success);
void batt_get(int *batt)
{
  int lower = 1;
  int upper = 100;
  *batt = (rand() % (upper - lower + 1)) + lower;
  // Serial.println(batt);
}
void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}

ERA_CONNECTED()
{
  ERA_LOG("ERa", "ERa connected!");
  Serial.println("ERa connected!");
  ElegantOTA.begin(&server);
  server.begin();
  // ElegantOTA.onStart(updating);
  // ElegantOTA.onEnd(updating_done);
  ERa_CONNECTED = true;
  auto_time_mode = true;
}

ERA_DISCONNECTED()
{
  ERA_LOG("ERa", "ERa disconnected!");
  Serial.println("ERa disconnected!");
  ERa_CONNECTED = false;
}
// void updating(){
//   spr.fillSprite(TFT_BLACK);
//   spr.setTextSize(2);
//   spr.drawString("Updating....",20,50);
// }
// void updating_done(bool success){
//   spr.fillSprite(TFT_BLACK);
//   spr.setTextSize(2);
//   spr.drawString("Done",20,50);
// }
void timerEvent()
{
  ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}
void TaskEra(void *parameters)
{
  // long prev_time = 0;
  Serial.println("E-Ra Task Started");
  // WiFi.mode(WIFI_STA);
  // wm.setConfigPortalBlocking(false);
  // wm.setConfigPortalTimeout(60);

  // if (wm.autoConnect("AutoConnectAP"))
  // {
  //   Serial.println("connected...yeey :)");
  // }
  // else
  // {
  //   Serial.println("Configportal running");
  // }
  // wifi_ap_record_t in4;
  // wifi_config_t conf_pass;
  // ssid = reinterpret_cast<char *>(in4.ssid);
  // password = reinterpret_cast<char *>(conf_pass.sta.password);
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  ERa.setScanWiFi(true);
  ERa.setPersistent(true);
  ERa.begin();
  // ERa.addInterval(1000L, timerEvent);
  for (;;)
  {

    ERa.run();
    syncTime.setTimeZone(list[utc]);
    syncTime.getTime(ntpTime);
  }
}
// class thoigian
// {
// private:
//   int hours, minutes, seconds;
//   String Weekdays;
//   int days, years;
//   String Months;

// public:
//   thoigian(int hours, int minutes, int seconds, int days, int years, String Weekdays = "Sun", String Months = "Jan");
//   void setHours(int hours)
//   {
//     this->hours = hours;
//   }
//   void setMinutes(int minutes)
//   {
//     this->minutes = minutes;
//   }
//   void setSeconds(int seconds)
//   {
//     this->seconds = seconds;
//   }
//   void setDay(int days)
//   {
//     this->days = days;
//   }
//   void setWeekdays(String Weekdays)
//   {
//     this->Weekdays = Weekdays;
//   }
//   void setMonths(String Months)
//   {
//     this->Months = Months;
//   }
//   void setYears(int years)
//   {
//     this->years = years;
//   }
//   int getHours()
//   {
//     return this->hours;
//   }
//   int getMinutes()
//   {
//     return this->minutes;
//   }
//   int getSeconds()
//   {
//     return this->seconds;
//   }
//   String getWeekDays()
//   {
//     return this->Weekdays;
//   }
// };

void setup()
{
  Serial.begin(115200);
  syncTime.begin();
  dht.setup(DHTPIN, DHTesp::DHT11);
#ifdef SH110X
  display.begin(i2c_Address, true); // Address 0x3C default
  display.display();
  delay(2000);
  display.clearDisplay();
#else
  spr.init();
  // tft.setRotation(1);
  spr.createSprite(240, 230);
  spr.fillScreen(TFT_RED);
  tft.fillScreen(TFT_BLACK);
  spr.pushSprite(0, 0);
#endif
  EEPROM.begin(FLASH_MEMORY_SIZE);
  minutes = EEPROM.read(add_minutes);
  hours = EEPROM.read(add_hours);
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
void get_openweather()
{

  HTTPClient http;
  // Set HTTP Request Final URL with Location and API key information
  http.begin(URL + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + API_KEY + "&lang=en");

  // start connection and send HTTP Request
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    // Read Data as a JSON string
    String JSON_Data = http.getString();
    Serial.println(JSON_Data);
    // Retrieve some information about the weather from the JSON format
    JsonDocument doc;
    deserializeJson(doc, JSON_Data);
    JsonObject obj = doc.as<JsonObject>();
    // Display the Current Weather Info
    temp_outside = obj["main"]["temp"].as<float>();
    humi_outside = obj["main"]["humidity"].as<float>();
    weather = obj["weather"][0]["description"].as<String>();
    location = obj["name"].as<String>();
    icon_id = obj["weather"][0]["icon"].as<String>();
    id = obj["weather"][0]["id"].as<int>();
    Serial.println(weather);
  }
  else
  {
    Serial.println("Error!");
  }
  http.end();
}
void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    server.handleClient();
    ElegantOTA.loop();
  }
  rotary_loop();
  hienthi();
  temp_room = dht.getTemperature() + offset;
  humi_room = dht.getHumidity() + offset;
  if (WiFi.status() == WL_CONNECTED && (millis() - prevMillis_client >= 10000))
  {
    prevMillis_client = millis();
    get_openweather();
  }
  if (ERa_CONNECTED == true && auto_time_mode == true)
  {
    hours = ntpTime.hour;
    minutes = ntpTime.minute;
    seconds = ntpTime.second;
    DoW = ntpTime.wDay - 1;
    days = ntpTime.day;
    months = ntpTime.month - 1;
    years = ntpTime.year + 1970;
  }
  if (millis() - previousMillis >= 1000)
  {
    previousMillis = millis();
    nhietdo = temperatureRead();
    second_count++;
    batt_get(&batt);
    if (ERa_CONNECTED == false || auto_time_mode == false)
      time_calculate(current_time);
    if (second_count == 300)
    {
      second_count = 0;
      EEPROM.writeInt(add_seconds, seconds);
      EEPROM.writeInt(add_hours, hours);
      EEPROM.writeInt(add_minutes, minutes);
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
  unsigned long utc_time = current_time + (millis() / 1000);
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

//////////////////////  setting_menu_flag Screen //////////////////////////////////////////////////
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
    startIndex = setting_menuIndex - 2;
    endIndex = setting_menuIndex + 1;
  }
  else
  {
    startIndex = setting_menuIndex - 1;
    endIndex = setting_menuIndex + 2;
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
    if (i == setting_menuIndex)
    {
      display.drawRoundRect(0, (i - startIndex) * 12 + 10, 120, 12, 3, SH110X_WHITE);
    }
    display.setCursor(4, (i - startIndex) * 12 + 12);
    display.print(menus[i]);
  }
  display.display();
#else
  spr.fillSprite(TFT_BLACK);
  spr.setTextColor(TFT_WHITE);
  spr.setTextSize(2);
  spr.drawString("Setting", 66, 13);
  spr.setTextColor(0x4208);
  spr.setTextSize(2);
  if (rotatingDown)
  {
    startIndex = setting_menuIndex - 3;
    endIndex = setting_menuIndex + 2;
  }
  else
  {
    startIndex = setting_menuIndex - 3;
    endIndex = setting_menuIndex + 2;
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
    if (i == setting_menuIndex)
    {
      spr.drawRoundRect(10, (i - startIndex) * 33 + 30, spr.textWidth(menus[i]) + 5, 25, 4, TFT_WHITE);
      spr.setTextColor(TFT_WHITE);
    }
    else
    {
      spr.setTextColor(0x4208);
    }
    spr.setTextSize(2);
    spr.drawString(menus[i], 14, (i - startIndex) * 33 + 35);
  }
  spr.pushSprite(0, 0);
#endif
}
///////////////////////////////////////////////////////////////////////////////////////////////
void hienthi()
{
  if (setting_menu_flag == true) // Vào setting menu
  {
    if (sub_menu_flag) // vào sub menu
    {
      switch (setting_menuIndex)
      {
      case 0:
        time_setting();
        ontime_setting = true;
        break;
      case 1:
        wifi();
        break;
      case 3:
        calib();
        break;
      case 2:
        Alarm();
        break;
      case 4:
        hard_reset();
        onhard_reset = true;
        break;
      default:
        sub_menu_flag = false;
        break;
      }
    }
    else // Hiển thị setting menu
    {
      displayMenu();
    }
  }
  else // Hiển thị màn hình chính
  {

    maindisplay();
    sub_menu_flag = false;
    setting_menuIndex = 0;
    //   if (WiFi.status() == WL_CONNECTED)
    //   {
    //     if (millis() - prevMillis >= 3000)
    //     {
    //       prevMillis = millis();
    //       screen = !screen;
    //     }
    //     if (screen)
    //     {
    //       maindisplay();
    //       sub_menu_flag = false;
    //       setting_menuIndex = 0;
    //     }
    //     else
    //     {
    //       spr.fillScreen(TFT_BLACK);
    //       weather_screen();
    //       spr.pushSprite(0, 0);
    //     }
    //   }
    //   else
    //   {
    //     maindisplay();
    //     sub_menu_flag = false;
    //     setting_menuIndex = 0;
    //   }
    // }
  }
}
void weather_screen()
{
  if (icon_id == "01d")
  {

    drawArrayJpeg(_01d, sizeof(_01d), 50, 50);
  }
  else if (icon_id == "01n")
  {
    drawArrayJpeg(_01n, sizeof(_01n), 50, 50);
  }
  else if (icon_id == "02d")
  {
    drawArrayJpeg(_02d, sizeof(_02d), 50, 50);
  }
  else if (icon_id == "02n")
  {
    drawArrayJpeg(_02n, sizeof(_02n), 50, 50);
  }
  else if (icon_id == "03d")
  {
    drawArrayJpeg(_03d, sizeof(_03d), 50, 50);
  }
  else if (icon_id == "03n")
  {
    drawArrayJpeg(_03n, sizeof(_03n), 50, 50);
  }
  else if (icon_id == "04n")
  {
    drawArrayJpeg(_04n, sizeof(_04n), 50, 50);
  }
  else if (icon_id == "04d")
  {
    drawArrayJpeg(_04d, sizeof(_04d), 50, 50);
  }
  else if (icon_id == "09n")
  {
    drawArrayJpeg(_09n, sizeof(_09n), 50, 50);
  }
  else if (icon_id == "09d")
  {
    drawArrayJpeg(_09n, sizeof(_09n), 50, 50);
  }
  else if (icon_id == "10n")
  {
    drawArrayJpeg(_10n, sizeof(_10n), 50, 50);
  }
  else if (icon_id == "10d")
  {
    drawArrayJpeg(_10d, sizeof(_10d), 50, 50);
  }
  else if (icon_id == "11d")
  {
    drawArrayJpeg(_11d, sizeof(_11d), 50, 50);
  }
  else if (icon_id == "11n")
  {
    drawArrayJpeg(_11n, sizeof(_11n), 50, 50);
  }
  else if (icon_id == "13d")
  {
    drawArrayJpeg(_13d, sizeof(_13d), 50, 50);
  }
  else if (icon_id == "13n")
  {
    drawArrayJpeg(_13n, sizeof(_13n), 50, 50);
  }
  else if (icon_id == "50n")
  {
    drawArrayJpeg(_50n, sizeof(_50n), 50, 50);
  }
  else if (icon_id == "50d")
  {
    drawArrayJpeg(_50d, sizeof(_50d), 50, 50);
  }
}
void renderJPEG(int xpos, int ypos)
{
  // retrieve information about the image
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;
  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);
  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;
  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;
  // read each MCU block until there are no more
  while (JpegDec.readSwappedBytes())
  {

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos; // Calculate coordinates of top left corner of current MCU
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x)
      win_w = mcu_w;
    else
      win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y)
      win_h = mcu_h;
    else
      win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }

    // draw image MCU block only if it will fit on the screen
    if ((mcu_x + win_w) <= spr.width() && (mcu_y + win_h) <= spr.height())
    {
      spr.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
    }
    else if ((mcu_y + win_h) >= spr.height())
    {
      JpegDec.abort(); // Image has run off bottom of screen so abort decoding
    }
  }
}
void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos)
{
  int x = xpos;
  int y = ypos;
  JpegDec.decodeArray(arrayname, array_size);
  renderJPEG(x, y);
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
  display.print(daysOfTheWeek[DoW]);
  display.setCursor(44, 51);
  display.print(monthOfTheYear[months]);
  display.setCursor(26, 51);
  display.print(days);
  display.setCursor(67, 51);
  display.print(years);
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
  display.print(humi_room);
  display.setCursor(109, 8);
  display.print("%");
  display.setCursor(19, 8);
  display.print(temp_room);
  display.display();
  // }
#else
  spr.fillScreen(TFT_BLACK);
  if (batt > 83)
  {
    spr.drawBitmap(0, 0, image_battery_full_bits, 24, 16, 0x1FE0);
  }
  else if (batt > 67 && batt <= 83)
  {
    spr.drawBitmap(0, 0, image_battery_83_bits, 24, 16, 0x7F8F);
  }
  else if (batt > 50 && batt <= 67)
  {
    spr.drawBitmap(0, 0, image_battery_67_bits, 24, 16, 0xF7A0);
  }
  else if (batt > 33 && batt <= 50)
  {
    spr.drawBitmap(0, 0, image_battery_50_bits, 24, 16, 0xFD60);
  }
  else if (batt > 17 && batt <= 33)
  {
    spr.drawBitmap(0, 0, image_battery_33_bits, 24, 16, 0xF1E2);
  }
  else
  {
    spr.drawBitmap(0, 0, image_battery_17_bits, 24, 16, 0xF800);
  }

  oldsecond = seconds;
  spr.setTextSize(1);
  byte xpos = 10;
  byte ypos = 23;
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
  spr.drawString("INDOOR", 8, 114);
  spr.drawBitmap(5, 135, image_weather_humidity_bits, 11, 16, 0x57FF);
  spr.drawBitmap(5, 161, image_weather_temperature_bits, 16, 16, 0xFAAA);
  spr.drawNumber(humi_room, 27, 137);
  spr.drawString("%", 27 + spr.textWidth((String)humi_room) - 38 + 6, 138);
  spr.drawFloat(temp_room, 1, 27, 160);
  spr.drawString("OUTDOOR", 137, 113);
  spr.drawBitmap(125, 135, image_weather_humidity_bits, 11, 16, 0x57FF);
  spr.drawNumber(humi_outside, 146, 136);
  spr.drawString("%", 146 + spr.textWidth((String)humi_outside) - 38 + 6, 138);
  spr.drawBitmap(125, 161, image_weather_temperature_bits, 16, 16, 0xFAAA);
  spr.drawFloat(temp_outside, 1, 146, 163);
  if (id >= 200 && id <= 232)
  {
    spr.drawBitmap(3, 211, image_weather_cloud_lightning_bolt_bits, 17, 16, 0x37E);
  }
  else if (id >= 300 && id <= 521)
  {
    spr.drawBitmap(3, 211, image_weather_cloud_rain_bits, 17, 16, 0x57FF);
  }
  else if (id == 800)
  {
    if (icon_id == "01d")
    {
      spr.drawBitmap(3, 211, image_weather_sun_bits, 15, 16, 0xFFEA);
    }
    else if (icon_id == "01n")
    {
      spr.drawBitmap(3, 211, image_moon_bits, 16, 16, 0xFFFF);
    }
  }
  else if (id == 801)
  {
    if (icon_id == "02d")
    {
      spr.drawBitmap(3, 211, image_weather_cloud_sunny_bits, 17, 16, 0xFFEA);
    }
    else if (icon_id == "02n")
    {
      spr.drawBitmap(3, 211, image_cloud_3_bits, 17, 16, 0xFFEA);
    }
  }
  else if (id >= 802 && id <= 804)
  {
    spr.drawBitmap(3, 211, image_cloud_3_bits, 17, 16, 0xFFEA);
  }
  spr.drawString(weather, 22, 209);
  spr.drawString(location, 24, 194);
  spr.drawString(daysOfTheWeek[DoW], 28, 80);
  spr.drawNumber(days, 79, 80);
  spr.drawString(monthOfTheYear[months], 112, 80);
  spr.drawNumber(years, 162, 80);
  spr.drawLine(0, 104, 239, 104, 0xFFFF);
  spr.drawLine(114, 104, 114, 189, 0xFFFF);
  spr.drawLine(0, 190, 239, 190, 0x57FF);
  spr.drawLine(0, 229, 238, 229, 0x57FF);
  if (WiFi.status() != WL_CONNECTED)
  {
    spr.drawBitmap(29, 1, image_wifi_not_connected_bits, 19, 16, TFT_WHITE, TFT_BLACK);
  }
  else
  {
    if (!ERa_CONNECTED)
    {

      spr.drawBitmap(51, 1, image_operation_warning_bits, 16, 16, 0xFFEA, TFT_BLACK);
      spr.drawBitmap(29, 1, image_wifi_full_bits, 19, 16, TFT_WHITE, TFT_BLACK);
      spr.pushSprite(0, 0);
    }
    else
    {
      spr.fillRect(25, 0, 16, 16, TFT_BLACK);
      spr.drawBitmap(29, 1, image_wifi_full_bits, 19, 16, TFT_WHITE, TFT_BLACK);
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
    currentRotaryValue = rotaryEncoder.readEncoder(); // save currrent value
    lastRotaryChange = millis();
    if (!sub_menu_flag) // if user not in any sub menu(setting_menu_flag screen)
    {
      if (currentRotaryValue > previousRotaryValue) // same clockwise
      {
        rotatingDown = true;
        setting_menuIndex++;
        if (setting_menuIndex > 5)
        {
          setting_menuIndex = 0;
        }
      }
      else
      {
        rotatingDown = false;
        setting_menuIndex--;
        if (setting_menuIndex < 0)
        {
          setting_menuIndex = 5;
        }
      }
    }
    else // on sub menu
    {
      switch (setting_menuIndex) // Check what sub menu is sel and change value of that submenu
      {
      case 0:                                         // Set time sel
        if (currentRotaryValue > previousRotaryValue) // increase value
        {
          if (set_time_flag)
          { // if date/time setting_menu_flag was sel
            if (clicked == 1)
            { // check what value user want to change
              switch (settime_menuIndex)
              {
              case 0: // WeekDays
                ++DoW;
                if (DoW > 6)
                {
                  DoW = 0;
                }
                break;
              case 1: // Days
                days++;
                if (months == 1 && days > 28)
                {
                  days = 1;
                }
                else if (days > 31 && (months == 0 || months == 2 || months == 4 || months == 6 || months == 7 || months == 9 || months == 11))
                {
                  days = 1;
                }
                else if (days > 30)
                {
                  days = 1;
                }
                break;
              case 2: // Months
                months++;
                if (months > 11)
                {
                  months = 0;
                }
                break;
              case 3: // Years
                years += 1;
                break;
              case 4: // Hours
                _hours++;
                if (_hours > 23)
                {
                  _hours = 0;
                }
                break;
              case 5: // Minutes
                _minutes++;
                if (_minutes > 59)
                {
                  _minutes = 0;
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
          else if (region_flag == true)
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
          if (set_time_flag)
          {
            if (clicked == 1)
            {
              switch (settime_menuIndex)
              {
              case 0:
                --DoW;
                if (DoW < 0)
                {
                  DoW = 6;
                }
                break;
              case 1:
                days--;
                if (months == 1 && days < 1)
                {
                  days = 28;
                }
                else if (days < 1 && (months == 0 || months == 2 || months == 4 || months == 6 || months == 7 || months == 9 || months == 11))
                {
                  days = 31;
                }
                else if (days < 1)
                {
                  days = 30;
                }
                break;
              case 2:
                months--;
                if (months < 0)
                {
                  months = 11;
                }
                break;
              case 3:
                years -= 1;
                break;
              case 4:
                _hours--;
                if (_hours < 0)
                {
                  _hours = 24;
                }
                break;
              case 5:
                _minutes--;
                if (_minutes < 0)
                {
                  _minutes = 60;
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
          else if (region_flag == true)
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
          if (wifiMenu_choose > 4)
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
            wifiMenu_choose = 4;
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
      // click_time++;
      // if (millis() - doubleClickMillis <= 500)
      // {
      //   doubleClickMillis = millis();
      //   Serial.println("DoubleClick");
      //   click_time = 0;
      // }
      // if (click_time > 1)
      //   click_time = 0;
    }
    if (!isLongpress && (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) && sub_menu_flag)
    {
      Serial.println("button LONG press ");
      isLongpress = true;
      if (region_flag == true)
      { // back from region setting_menu_flag ( hold 2s)
        region_flag = false;
        time_menuIndex = 2;
        time_setting();
        return;
      }
      if (setting_menuIndex == 1 && WiFi.status() != WL_CONNECTED)
      { // back from disconnect screen
        sub_menu_flag = false;
        displayMenu();
        return;
      }
      if (setting_menuIndex == 2 || setting_menuIndex == 3)
      { // back from cablib screen
        sub_menu_flag = false;
        displayMenu();
        return;
      }
    }
    else // đè 3s để vào setting_menu_flag menu
    {
      if (!isSettingpress && sub_menu_flag == false && (millis() - lastTimeButtonDown >= entersettingAfterMiliseconds))
      {
        Serial.println("Setting Press");
        setting_menu_flag = !setting_menu_flag;
        isSettingpress = true;
        spr.fillScreen(TFT_BLACK);
      }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  }
  else
  {
    if (lastTimeButtonDown && !isSettingpress && !isLongpress && setting_menu_flag)
    {
      Serial.println("button SHORT press ");
      sub_menu_flag = true;
      /////////////////////// Button control on Set Time submenu( Date/Time setting_menu_flag -> Set Time) /////////////////
      if (rotaryEncoder.readButtonState() == BUT_DOWN && set_time_flag == true)
      {
        clicked++;
      }
      if (clicked > 1)
      {
        clicked = 0;
      }
      ///////////////////////////////////////////////////////////////////////////////

      ////////////////////datet/time setting_menu_flag Action /////////////////////////////////
      if (setting_menuIndex == 0 && ontime_setting == true)
      { // control submenu 1
        byte xpos;
        switch (time_menuIndex) // Date/time setting_menu_flag
        {
        case 0: // Set time setting_menu_flag
          set_time_flag = true;
          if (settime_menuIndex == 6)
          { // Back from Date/time Seting
            set_time_flag = false;
            settime_menuIndex = 0;
            clicked = 0;
            time_setting();
          }
          break;
        case 1:
          auto_time_mode = !auto_time_mode;
          break;
        case 2: // Region setting_menu_flag
          region_flag = true;
          break;
        case 3: // Save
#ifdef SH110X
          for (int x = 0; x < 20; x++)
          {

            display.clearDisplay();
            display.fillScreen(TFT_BLACK);
            display.setTextColor(TFT_WHITE);
            display.setTextSize(3);
            display.setCursor(24, 18);
            display.print("SAVED");
            display.display();
          }
#else
          spr.fillScreen(TFT_BLACK);
          spr.setTextColor(0xFFFF);
          spr.setTextSize(3);
          spr.drawString("Saving", 18, 92);
          spr.pushSprite(0, 0);
          xpos = 110;
          for (int i = 0; i <= 4; i++)
          {
            xpos += 20;
            spr.drawString(".", xpos, 92);
            spr.pushSprite(0, 0);
            delay(200);
          }
#endif
          // EEPROM.writeInt(add_hours,hours);
          // EEPROM.writeInt(add_minutes,minutes);
          // EEPROM.writeInt(add_seconds,seconds);
          // EEPROM.writeInt(add_utc,utc);
          // EEPROM.writeBool(add_auto_time,auto_time_mode);
          // EEPROM.commit();
          current_time = (_hours * 3600 + _minutes * 60) - (list[utc] * 3600);
          time_setting();
          break;
        case 4: // Back
          sub_menu_flag = false;
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
      if (reset_sel == 1 && setting_menuIndex == 4 && onhard_reset == true)
      {
        enable_reset = true;
      }
      else if (reset_sel == 0 && setting_menuIndex == 4 && onhard_reset == true)
      {
        sub_menu_flag = false;
        onhard_reset = false;
        reset_sel = 0;
        displayMenu();
      }
      //////////////////////////////////////////////////////////////////////

      ///////////////// WiFi setting_menu_flag Action /////////////////////////////////
      if (wifiMenu_choose == 3 && setting_menuIndex == 1 && WiFi.status() == WL_CONNECTED) // Disconnect
      {
#ifdef SH110X
        display.clearDisplay();
        display.fillScreen(TFT_BLACK);
        display.setTextColor(TFT_WHITE);
        display.setCursor(16, 13);
        display.print("Disconnecting");
        display.display();
        display.setCursor(95, 13);
#else
        spr.fillScreen(TFT_BLACK);
        spr.setTextColor(0xFFFF);
        spr.setTextSize(2);
        spr.drawString("Disconnecting", 22, 88);
        spr.pushSprite(0, 0);
#endif
        byte xpos = 22;
        for (int i = 0; i < 5; i++)
        {
#ifdef SH110X
          display.print(".");
          display.display();
#else
          xpos += 20;
          spr.setTextSize(3);
          spr.setTextColor(0xFFFF);
          spr.drawString(".", xpos, 110);
          spr.pushSprite(0, 0);
#endif
          delay(300);
        }
        auto_time_mode = false;
        current_time = (hours * 3600 + minutes * 60 + seconds) - (list[utc] * 3600);
        wifi();
        ERa.switchToConfig(true);
        delay(100);
      }
      else if (wifiMenu_choose == 4 && setting_menuIndex == 1 && WiFi.status() == WL_CONNECTED) // Back
      {
        sub_menu_flag = false;
        wifiMenu_choose = -1;
        displayMenu();
      }
      ////////////////// END WiFi setting_menu_flag Action//////////////////////////////////

      ///////////// Back Sel Action ( displayMenu() ) //////////////////////////
      if (setting_menuIndex == 5 && setting_menu_flag == true)
      {
        setting_menu_flag = false;
        sub_menu_flag = false;
        setting_menuIndex = 0;
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
  _hours = hours;
  _minutes = minutes;
  while (set_time_flag)
  {
    rotary_loop();
    const char *settime_menu[] = {"WeekDays:", "Days:", "Months:", "Years:", "Hours:", "Minutes:", "Back"};
    char str_ngay[4]; // Make sure the buffer is large enough
    char str_nam[5];
    char str_hours[3];
    char str_minutes[3];
    sprintf(str_ngay, "%u", days);
    sprintf(str_nam, "%u", years);
    sprintf(str_hours, "%u", _hours);
    sprintf(str_minutes, "%u", _minutes);
    const char *rs[] = {daysOfTheWeek[DoW], str_ngay, monthOfTheYear[months], str_nam, str_hours, str_minutes};
#ifdef SH110X
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(11, 3);
    display.print("Date/Time Setting");
#else
    spr.fillScreen(TFT_BLACK);
    spr.setTextColor(0xFFFF);
    spr.setTextSize(2);
    spr.drawString("Date/Time Setting", 20, 11);
#endif
    if (auto_time_mode == true && ERa_CONNECTED == true)
    {
#ifdef SH110X
      display.clearDisplay();
      display.fillScreen(TFT_BLACK);
      display.setTextColor(TFT_WHITE);
      display.setCursor(17, 14);
      display.print("AuTo Time Is ON");
      display.setCursor(6, 33);
      display.print("Turn OFF To Use This");
      display.display();
#else
      spr.fillScreen(TFT_BLACK);
      spr.setTextColor(0xFFFF);
      spr.setTextSize(2);
      spr.drawString("Auto Time Is ON", 30, 50);
      spr.drawString("Turn OFF To Use This", 0, 97);
      spr.drawBitmap(97, 67, image_Warning_bits, 30, 23, 0xFFEA);
      spr.pushSprite(0, 0);
#endif
      delay(2000);
      set_time_flag = false;
      clicked == 0;
      time_setting();
      return;
    }
    if (rotatingDown)
    {
      startsetTimeIndex = settime_menuIndex - 4;
      endsetTimeIndex = settime_menuIndex + 3;
    }
    else
    {
      startsetTimeIndex = settime_menuIndex - 4;
      endsetTimeIndex = settime_menuIndex + 3;
    }
    if (startsetTimeIndex < 0)
    {
      startsetTimeIndex = 0;
      endsetTimeIndex = (7 - 1);
    }
    else if (endsetTimeIndex >= 7)
    {
      endsetTimeIndex = 7 - 1;
      startsetTimeIndex = endsetTimeIndex - (7 - 1);
    }
    for (int i = startsetTimeIndex; i <= endsetTimeIndex; i++)
    {
      spr.setTextColor(0xFFFF);
      spr.setTextSize(2);
      spr.drawString(settime_menu[i], 10, (i - startsetTimeIndex) * 28 + 37);
      if (i == settime_menuIndex)
      {
        if (clicked == 1 && i <= 5)
        {
          spr.drawRoundRect(spr.textWidth(settime_menu[i]) + 8, (i - startsetTimeIndex) * 28 + 35, spr.textWidth(rs[i]) + 3, 19, 3, TFT_WHITE);
        }
        else
        {
          spr.drawRoundRect(5, (i - startsetTimeIndex) * 28 + 35, spr.textWidth(settime_menu[i]) + spr.textWidth(rs[i]) + 10, 19, 3, TFT_WHITE);
        }
      }
      if (i > 5)
      {
        continue;
      }
      spr.drawString(rs[i], spr.textWidth(settime_menu[i]) + 10, (i - startsetTimeIndex) * 28 + 37);
      // spr.pushSprite(0, 0);
    }
    spr.pushSprite(0, 0);
  }
}
void time_setting()
{
#ifdef SH110X
  if (set_time_flag == true)
  {
    set_time();
    return;
  }
  if (region_flag == true)
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
  if (set_time_flag == true)
  {
    set_time();
    return;
  }
  if (region_flag == true)
  {
    region();
    return;
  }
  String time_menu[] = {"Set Time", "Sync Time: ", "Region", "Save", "Back"};
  String mode[] = {"OFF", "ON"};
  spr.setTextColor(TFT_WHITE);
  spr.setTextSize(2);
  spr.drawString("Time Setting", 46, 12);
  spr.setTextColor(0x4208);
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
    if (time_menuIndex == 1)
    {
      spr.drawRoundRect(10, (1 - startTimeIndex) * 33 + 30, spr.textWidth(time_menu[1]) + spr.textWidth(mode[auto_time_mode]) + 5, 25, 4, TFT_WHITE);
      spr.setTextColor(TFT_WHITE);
      spr.drawString(time_menu[1], 14, (1 - startTimeIndex) * 33 + 35);
      spr.drawString(mode[auto_time_mode], spr.textWidth(time_menu[1]) + 5, (1 - startTimeIndex) * 33 + 35);
      spr.setTextColor(0x4208);
    }
    else if (i == time_menuIndex)
    {
      spr.drawRoundRect(10, (i - startTimeIndex) * 33 + 30, spr.textWidth(time_menu[i]) + 5, 25, 4, TFT_WHITE);
      spr.setTextColor(TFT_WHITE);
    }
    else
    {
      spr.setTextColor(0x4208);
      spr.drawString(mode[auto_time_mode], spr.textWidth(time_menu[1]) + 3, (1 - startTimeIndex) * 33 + 35);
    }
    spr.drawString(time_menu[i], 14, (i - startTimeIndex) * 33 + 35);
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
  spr.pushSprite(0, 0);
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
void Alarm()
{
#ifdef SH110X
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
  spr.fillScreen(TFT_BLACK);
  spr.setTextColor(TFT_WHITE);
  spr.setTextSize(1);
  spr.drawString("Developing.....", 50, 50);
  spr.pushSprite(0, 0);
  delay(3000);
  sub_menu_flag = false;
  displayMenu();
  return;
  // static const unsigned char PROGMEM image_weather_temperature_bits[] = {0x1c,0x00,0x22,0x02,0x2b,0x05,0x2a,0x02,0x2b,0x38,0x2a,
  // 0x60,0x2b,0x40,0x2a,0x40,0x2a,0x60,0x49,0x38,0x9c,0x80,0xae,0x80,0xbe,0x80,0x9c,0x80,0x41,0x00,0x3e,0x00};
  // display.clearDisplay();
  // display.setTextColor(TFT_WHITE);
  // display.setTextSize(1);
  // display.setCursor(21, 15);
  // display.print("Chip Temperature");
  // display.setTextSize(2);
  // display.setCursor(36, 34);
  // display.print(nhietdo);
  // display.drawBitmap(2, 11, image_weather_temperature_bits, 16, 16, 1);
  // display.display();
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
    const char *wifi_menu[] = {"SSID: ", "IP:", "RSSI: ", "Disconnect", "Back"};
    String infor[] = {WiFi.SSID(), ConverIpToString(ip), rssi};
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
      endWifiIndex = 4;
    }
    else if (endWifiIndex >= 4)
    {
      endWifiIndex = 4;
      startWifiIndex = endWifiIndex - 4;
    }

    for (int i = startWifiIndex; i <= endWifiIndex; i++)
    {
      if (i == wifiMenu_choose)
      {
        if (i > 2)
        {
          spr.drawRoundRect(10, (i - startWifiIndex) * 33 + 30, spr.textWidth(wifi_menu[i]) + 5, 25, 4, TFT_WHITE);
        }
        else
        {
          spr.drawRoundRect(10, (i - startWifiIndex) * 33 + 30, spr.textWidth(wifi_menu[i]) + spr.textWidth(infor[i]) + 5, 25, 4, TFT_WHITE);
        }
      }
      spr.drawString(wifi_menu[i], 14, (i - startWifiIndex) * 33 + 35);
      if (i > 2)
      {
        continue;
      }

      spr.drawString(infor[i], spr.textWidth(wifi_menu[i]) + 5, (i - startWifiIndex) * 33 + 35);
    }
    spr.pushSprite(0, 0);
  }
#endif
}