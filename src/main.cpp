
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
#include <otadrive_esp.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "bitmap.h"
#include "Free_Fonts.h"
#include "icon.h"
#include "tahoma10pt7b.h"

#define APIKEY "b182dce6-64e4-4650-972c-588489ec0fcc"
// this app version
#define FW_VER "v@1.2.22"

#define MAX_DAYS_FEB 28
#define MAX_DAYS_OTHER 31
#define MAX_MONTHS 12
#define MAX_HOURS 24
#define MAX_MINUTES 60
#define MAX_WEEKDAYS 7

#define ENCODER_CLK 25
#define ENCODER_DT 26
#define ENCODER_SW 27
#define ENCODER_VCC -1
#define FLASH_MEMORY_SIZE 200
#define ENCODER_STEPS 4
#define DHTPIN 5
#define VBAT_PIN 35
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

updateInfo inf;

// WebServer server(80);
String URL = "http://api.openweathermap.org/data/2.5/weather?";
String URL_2 = "https://api.openweathermap.org/data/2.5/forecast?";
/// @brief eb1d2c68ab206e3e4ecf26becc7ddc9c
String API_KEY = "eb1d2c68ab206e3e4ecf26becc7ddc9c";
WiFiClient client;
WebServer server(80);
// THU DUC //
String lat = "10.8201506";
String lon = "106.7074488";

const char *daysOfTheWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *monthOfTheYear[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};
char SSID_AP[] = {"ERA"};
String menus[] = {"Date/Time", "WiFi", "Update", "Calib DHT", "Hard Reset", "Back"};
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
String location = "Viet Nam";
int16_t DoW;
int16_t months = 0;
uint16_t years = 1970;
uint16_t days = 1;
int8_t hours, _hours;
int8_t minutes, _minutes;
uint8_t seconds;
int batt = 100;
int clicked = 0;
bool enable_reset = false;
bool sync_time_mode = false;
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
int wifiMenu_Index = 0;
int id;
float offset = 0.00;
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
void Update_Screen();
void wifi();
void hienthi();
#ifndef SH110X
void weather_screen();
void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos);
void renderJPEG(int xpos, int ypos);
#endif
void connect_succes();
void get_Current_Weather();
void get_3DayWeather();
void time_calculate(unsigned long current_time);
int batt_get();
void updating();
void updating_done(bool success);
void onUpdateProgress(int progress, int totalt);
void handleWeekdays(int direction);
void handleDays(int direction);
void handleMonths(int direction);
void handleYears(int direction);
void handleHours(int direction);
void handleMinutes(int direction);
void handleSetTime_Menu();
void handleWiFi_Menu();
void handleCalib_Menu();
// Handle weekdays
void handleWeekdays(int direction)
{
  DoW = (DoW + direction) % MAX_WEEKDAYS;
  if (DoW < 0)
  {
    DoW += MAX_WEEKDAYS;
  }
}

// Handle days
void handleDays(int direction)
{
  int maxDays = (months == 2) ? MAX_DAYS_FEB : MAX_DAYS_OTHER;
  days = (days + direction) % maxDays;
  if (days < 1)
  {
    days += maxDays;
  }
}

// Handle months
void handleMonths(int direction)
{
  months = (months + direction) % MAX_MONTHS;
  if (months < 0)
  {
    months += MAX_MONTHS;
  }
}

// Handle years
void handleYears(int direction)
{
  years += direction;
}

// Handle hours
void handleHours(int direction)
{
  _hours = (_hours + direction);
  if (_hours < 0)
  {
    _hours = MAX_HOURS;
  }
  else if (_hours > MAX_HOURS)
  {
    _hours = 0;
  }
}
// Handle minutes
void handleMinutes(int direction)
{
  _minutes = (_minutes + direction);
  if (_minutes < 0)
  {
    _minutes = MAX_MINUTES;
  }
  else if (_minutes > MAX_MINUTES)
  {
    _minutes = 0;
  }
}
void handleSetTime_Menu()
{
  if (currentRotaryValue > previousRotaryValue)
  {
    if (set_time_flag)
    {
      if (clicked == 1)
      {
        switch (settime_menuIndex)
        {
        case 0:
          handleWeekdays(1);
          break;
        case 1:
          handleDays(1);
          break;
        case 2:
          handleMonths(1);
          break;
        case 3:
          handleYears(1);
          break;
        case 4:
          handleHours(1);
          break;
        case 5:
          handleMinutes(1);
          break;
        default:
          break;
        }
      }
      else
      {
        settime_menuIndex = (settime_menuIndex + 1) % 7;
      }
    }
    else if (region_flag)
    {
      utc = (utc + 1) % 27;
    }
    else
    {
      rotatingDown = true;
      time_menuIndex = (time_menuIndex + 1) % 5;
    }
  }
  else
  {
    if (set_time_flag)
    {
      if (clicked == 1)
      {
        switch (settime_menuIndex)
        {
        case 0:
          handleWeekdays(-1);
          break;
        case 1:
          handleDays(-1);
          break;
        case 2:
          handleMonths(-1);
          break;
        case 3:
          handleYears(-1);
          break;
        case 4:
          handleHours(-1);
          break;
        case 5:
          handleMinutes(-1);
          break;
        default:
          // clicked == 0;
          break;
        }
      }
      else
      {
        settime_menuIndex = (settime_menuIndex - 1) % 7;
        if (settime_menuIndex < 0)
          settime_menuIndex = 6;
      }
    }
    else if (region_flag)
    {
      utc = (utc - 1) % 27;
      if (utc < 0)
        utc = 26;
    }
    else
    {
      rotatingDown = false;
      time_menuIndex = (time_menuIndex - 1) % 5;
      if (time_menuIndex < 0)
        time_menuIndex = 4;
    }
  }
}

void handleWiFi_Menu()
{
  if (currentRotaryValue > previousRotaryValue)
  {
    rotatingDown = true;
    wifiMenu_Index++;
    if (wifiMenu_Index > 4)
    {
      wifiMenu_Index = 0;
    }
  }
  else
  {
    rotatingDown = false;
    wifiMenu_Index--;
    if (wifiMenu_Index < 0)
    {
      wifiMenu_Index = 4;
    }
  }
}

void handleCalib_Menu()
{
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
}

void handleHardReset_Menu()
{
  if (currentRotaryValue > previousRotaryValue)
  {
    reset_sel = (reset_sel + 1) % 2;
  }
  else
  {
    reset_sel = (reset_sel - 1 + 2) % 2;
  }
}
void batt_get(int *batt)
{
  int lower = 1;
  int upper = 100;
  float battv;
  battv = ((float)analogRead(VBAT_PIN) / 4095) * 3.3;
  *batt = (uint8_t)((battv / 4.2) * 100);
  Serial.println(*batt);
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
  sync_time_mode = true;
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
  Serial.println("E-Ra Task Started");
  ERa.setScanWiFi(true);
  ERa.setPersistent(true);
  ERa.begin();
  // ERa.addInterval(1000L, timerEvent);
  for (;;)
  {
    ERa.run();
    if (WiFi.status() == WL_CONNECTED && (millis() - prevMillis_client >= 10000))
    {
      prevMillis_client = millis();
      get_Current_Weather();
    }
    syncTime.setTimeZone(list[utc]);
    syncTime.getTime(ntpTime);
  }
}

void setup()
{
  Serial.begin(115200);
  syncTime.begin();
  dht.setup(DHTPIN, DHTesp::DHT11);
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);
  OTADRIVE.setInfo(APIKEY, FW_VER);
  OTADRIVE.onUpdateFirmwareProgress(onUpdateProgress);

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
  sync_time_mode = EEPROM.read(add_auto_time);
  current_time = (hours * 3600 + minutes * 60 + seconds) - (list[utc] * 3600);
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(-99999, 99999, true);
  rotaryEncoder.disableAcceleration();
  xTaskCreatePinnedToCore(TaskEra, "Task Era NTP", 10000, NULL, 1, NULL, 1);
}
void get_Current_Weather()
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
void get_3DayWeather()
{

  HTTPClient http;
  // Set HTTP Request Final URL with Location and API key information
  http.begin(URL_2 + "lat=" + lat + "&lon=" + lon + "&appid=" + API_KEY);

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
    String dt;
    dt = obj["list"][0]["dt_txt"][0].as<String>();
    Serial.println(dt);
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
  if (ERa_CONNECTED == true && sync_time_mode == true)
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
    temp_room = dht.getTemperature() + offset;
    humi_room = dht.getHumidity() + offset;
    if (ERa_CONNECTED == false || sync_time_mode == false)
      time_calculate(current_time);
    if (second_count == 300)
    {
      second_count = 0;
      EEPROM.writeInt(add_seconds, seconds);
      EEPROM.writeInt(add_hours, hours);
      EEPROM.writeInt(add_minutes, minutes);
      EEPROM.writeInt(add_utc, utc);
      EEPROM.writeFloat(add_offset, offset);
      EEPROM.writeBool(add_auto_time, sync_time_mode);
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
        Update_Screen();
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
  spr.setTextSize(1);
  byte xpos = 10;
  byte ypos = 23;
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
  spr.drawString(location, 3, 194);
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
  spr.setTextSize(1);
  spr.drawNumber(batt, 79, 6);
  spr.pushSprite(0, 0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void rotary_loop()
{
  if (rotaryEncoder.encoderChanged())
  {
    currentRotaryValue = rotaryEncoder.readEncoder();
    if (sub_menu_flag == false)
    {
      if (currentRotaryValue > previousRotaryValue)
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
      switch (setting_menuIndex)
      {
      case 0:
        handleSetTime_Menu();
        break;
      case 1: // wifi sel
        handleWiFi_Menu();
        break;
      case 3: // Change offset value
        handleCalib_Menu();
        break;
      case 4: // Choose Reset or Not
        handleHardReset_Menu();
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
  static bool isLongpress = false;
  static bool isSettingpress = false;
  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
  if (isEncoderButtonDown)
  {
    if (!lastTimeButtonDown)
    {
      lastTimeButtonDown = millis();
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
        clicked = !clicked;
      }
      ///////////////////////////////////////////////////////////////////////////////

      ////////////////////Datet/Time setting_menu_flag Action /////////////////////////////////
      if (setting_menuIndex == 0 && ontime_setting == true)
      { // control submenu 1
        byte xpos;
        switch (time_menuIndex) // Date/time setting_menu_flag
        {
        case 0: // Set time setting_menu_flag

          if (settime_menuIndex == 6)
          { // Back from Date/time Seting
            set_time_flag = false;
            settime_menuIndex = 0;
            clicked = 0;
            time_setting();
          }
          else
          {
            set_time_flag = true;
          }
          break;
        case 1:
          sync_time_mode = !sync_time_mode;
          break;
        case 2: // Region setting_menu_flag
          region_flag = true;
          break;
        case 3: // Save
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
          EEPROM.writeInt(add_hours, hours);
          EEPROM.writeInt(add_minutes, minutes);
          EEPROM.writeInt(add_seconds, seconds);
          EEPROM.writeInt(add_utc, utc);
          EEPROM.writeBool(add_auto_time, sync_time_mode);
          EEPROM.commit();
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
      if (wifiMenu_Index == 3 && setting_menuIndex == 1 && WiFi.status() == WL_CONNECTED) // Disconnect
      {
        spr.fillScreen(TFT_BLACK);
        spr.setTextColor(0xFFFF);
        spr.setTextSize(2);
        spr.drawString("Disconnecting", 22, 88);
        spr.pushSprite(0, 0);
        byte xpos = 22;
        for (int i = 0; i < 5; i++)
        {
          xpos += 20;
          spr.setTextSize(3);
          spr.setTextColor(0xFFFF);
          spr.drawString(".", xpos, 110);
          spr.pushSprite(0, 0);
          delay(300);
        }
        sync_time_mode = false;
        current_time = (hours * 3600 + minutes * 60 + seconds) - (list[utc] * 3600);
        wifi();
        ERa.switchToConfig(true);
        delay(100);
      }
      else if (wifiMenu_Index == 4 && setting_menuIndex == 1 && WiFi.status() == WL_CONNECTED) // Back
      {
        sub_menu_flag = false;
        wifiMenu_Index = -1;
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
    spr.fillScreen(TFT_BLACK);
    spr.setTextColor(0xFFFF);
    spr.setTextSize(2);
    spr.drawString("Date/Time Setting", 20, 11);
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
    }
    spr.pushSprite(0, 0);
  }
}
void time_setting()
{
  spr.fillSprite(TFT_BLACK);
  if (set_time_flag == true)
  {
    if (sync_time_mode == true && ERa_CONNECTED == true)
    {
      spr.fillScreen(TFT_BLACK);
      spr.setTextColor(0xFFFF);
      spr.setTextSize(2);
      spr.drawString("Auto Time Is ON", 30, 50);
      spr.drawString("Turn OFF To Use This", 0, 97);
      spr.drawBitmap(97, 67, image_Warning_bits, 30, 23, 0xFFEA);
      spr.pushSprite(0, 0);
      delay(2000);
      set_time_flag = false;
      clicked == 0;
      time_setting();
      return;
    }
    else
    {
      set_time();
    }
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
      spr.drawRoundRect(10, (1 - startTimeIndex) * 33 + 30, spr.textWidth(time_menu[1]) + spr.textWidth(mode[sync_time_mode]) + 5, 25, 4, TFT_WHITE);
      spr.setTextColor(TFT_WHITE);
      spr.drawString(time_menu[1], 14, (1 - startTimeIndex) * 33 + 35);
      spr.drawString(mode[sync_time_mode], spr.textWidth(time_menu[1]) + 5, (1 - startTimeIndex) * 33 + 35);
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
      spr.drawString(mode[sync_time_mode], spr.textWidth(time_menu[1]) + 3, (1 - startTimeIndex) * 33 + 35);
    }
    spr.drawString(time_menu[i], 14, (i - startTimeIndex) * 33 + 35);
  }
  spr.pushSprite(0, 0);
}
void region()
{
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
}
void calib()
{
  spr.fillScreen(TFT_BLACK);
  spr.setTextColor(TFT_WHITE);
  spr.setTextColor(0xFFEA);
  spr.setTextSize(2);
  spr.drawString("OFFSET Setting", 36, 34);
  spr.setTextColor(0x57FF);
  spr.drawString("OFFSET:", 37, 108);
  spr.drawFloat(offset, 2, 140, 108);
  spr.pushSprite(0, 0);
}
void hard_reset()
{
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
}
void onUpdateProgress(int progress, int totalt)
{
  spr.fillSprite(TFT_BLACK);
  static int last = 0;
  int progressPercent = (100 * progress) / totalt;
  spr.setTextSize(2);
  spr.setTextColor(0xFFFF);
  spr.drawString("Update Availble", 6, 72);
  spr.drawString("On Process....", 6, 99);
  last = progressPercent;
  spr.drawNumber(progressPercent, 90, 127);
  spr.drawString("%", spr.textWidth((String)progressPercent) + 2, 127);
  spr.pushSprite(0, 0);
}
void Update_Screen()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    spr.fillSprite(TFT_BLACK);
    spr.setTextSize(2);
    spr.setTextColor(0xFFFF);
    spr.drawString("Checking Update", 7, 47);
    int xpos = 10;
    for (int i = 0; i < 10; i++)
    {
      xpos += 25;
      spr.drawString(".", xpos, 71);
      delay(500);
      spr.pushSprite(0, 0);
    }
    // if (OTADRIVE.timeTick(5))
    // {
    inf = OTADRIVE.updateFirmwareInfo();
    if (inf.available)
    {
      log_d("\nNew version available, %dBytes, %s\n", inf.size, inf.version.c_str());
      while (inf.code == update_result::Success)
      {
        OTADRIVE.updateFirmware();
        Serial.println("Newer version");
      }
    }
    else
    {
      log_d("\nNo newer version\n");
      Serial.println("No newer version");
      spr.fillSprite(TFT_BLACK);
      spr.setTextColor(0xFFFF);
      spr.setTextSize(3);
      spr.drawString("Nothing New", 23, 44);
      spr.drawBitmap(92, 83, image_Cry_dolph_bits, 55, 52, 0x57FF);
      spr.pushSprite(0, 0);
      delay(3000);
      sub_menu_flag = false;
      displayMenu();
      return;
    }
    // }
  }
  else
  {
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(0xFFFF);
    spr.setTextColor(0xFFFF);
    spr.setTextSize(2);
    spr.drawString("Connect WiFi ", 40, 57);
    spr.drawString("To Check For Update", 8, 139);
    spr.drawBitmap(56, 79, image_Scanning_bits, 123, 52, 0xFFFF);
    spr.pushSprite(0, 0);
    delay(3000);
    sub_menu_flag = false;
    displayMenu();
    return;
  }
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
      startWifiIndex = wifiMenu_Index - 3;
      endWifiIndex = wifiMenu_Index + 2;
    }
    else
    {
      startWifiIndex = wifiMenu_Index - 3;
      endWifiIndex = wifiMenu_Index + 2;
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
      if (i == wifiMenu_Index)
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
}