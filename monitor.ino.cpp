# 1 "C:\\Users\\Dominik\\AppData\\Local\\Temp\\tmp3ejxecyk"
#include <Arduino.h>
# 1 "C:/Users/Dominik/projects/cyd-monitor/monitor.ino"
#include <Arduino.h>
#include <WiFi.h>
#include <string.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "esp_sntp.h"
#include <XPT2046_Touchscreen.h>
#include "credentials.h"
#include <TFT_eSPI.h>

#define TOUCH_CS
void Debug(String label, int val);
void printLocalTime();
String timeString();
void timeavailable(struct timeval *t);
void SetupCYD();
void SetupWiFi();
String decodeHtmlEntities(const String &str);
void request_data();
void setup();
void loop();
#line 15 "C:/Users/Dominik/projects/cyd-monitor/monitor.ino"
void Debug(String label, int val)
{
  Serial.print(label);
  Serial.print("=");
  Serial.println(val);
}


String url[4] = {"https://fahrplan.oebb.at/bin/stboard.exe/dn?L=vs_scotty.vs_liveticker&tickerID=dep&start=yes&eqstops=false&evaId=1292101&dirInput=1292001&showJourneys=11&maxJourneys=11&additionalTime=0&productsFilter=1011110000001&boardType=dep&outputMode=tickerDataOnly",
                 "https://fahrplan.oebb.at/bin/stboard.exe/dn?L=vs_scotty.vs_liveticker&tickerID=dep&start=yes&eqstops=false&evaId=1292101&dirInput=&showJourneys=11&maxJourneys=11&additionalTime=0&productsFilter=0000000010000&boardType=dep&outputMode=tickerDataOnly",
                 "https://fahrplan.oebb.at/bin/stboard.exe/dn?L=vs_scotty.vs_liveticker&tickerID=dep&start=yes&eqstops=false&evaId=1292101&dirInput=1292105&showJourneys=11&maxJourneys=11&additionalTime=0&productsFilter=1011110000001&boardType=dep&outputMode=tickerDataOnly",
                 "https://fahrplan.oebb.at/bin/stboard.exe/dn?L=vs_scotty.vs_liveticker&tickerID=dep&start=yes&eqstops=false&evaId=1392169&dirInput=&showJourneys=11&maxJourneys=11&additionalTime=0&productsFilter=1111111111111&boardType=dep&outputMode=tickerDataOnly"};

u_int16_t title_colors[4] = {TFT_RED, TFT_GREEN, TFT_SKYBLUE, TFT_YELLOW};
unsigned long timer_delay = 30000;




const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 0;

const char *time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";


const char *test_root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
    "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
    "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
    "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
    "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
    "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
    "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
    "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
    "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
    "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
    "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
    "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
    "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
    "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
    "MrY=\n"
    "-----END CERTIFICATE-----\n";


#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();
# 77 "C:/Users/Dominik/projects/cyd-monitor/monitor.ino"
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

String timeString()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("No time available (yet)");
    return "NoT";
  }
  char timestamp[7];
  strftime(timestamp, 7, "%H:%M", &timeinfo);
  return String(timestamp);
}


void timeavailable(struct timeval *t)
{
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void SetupCYD()
{
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(mySpi);
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.setRotation(1);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);
}

void SetupWiFi()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    int numSSIDs = sizeof(credentials) / sizeof(credentials[0]);
    for (size_t i = 0; (i < numSSIDs); i++)
    {
      String ssid = credentials[i][0];
      String pass = credentials[i][1];

      Serial.print("Connecting to ");
      Serial.println();
      Serial.print("pass: ");
      Serial.println(credentials[i][1]);

      tft.fillScreen(TFT_BLACK);
      tft.setTextFont(2);

      tft.setTextSize(2);
      tft.setTextDatum(MC_DATUM);
      tft.drawString("ÄConnecting to", 320 / 2, 240 / 2 - 40);
      tft.setTextSize(1);
      tft.drawString(ssid, 320 / 2, 240 / 2);

      WiFi.begin(ssid.c_str(), pass.c_str());
# 164 "C:/Users/Dominik/projects/cyd-monitor/monitor.ino"
      esp_sntp_servermode_dhcp(1);

      int tries = 5 * 2;
      String dots = "";
      while (WiFi.status() != WL_CONNECTED && tries-- > 0)
      {
        delay(500);
        Serial.print(".");
        dots = dots + ".";
        tft.drawString(dots, 320 / 2, 240 / 2 + 20);
      }

      if (WiFi.status() == WL_CONNECTED)
      {

        sntp_set_time_sync_notification_cb(timeavailable);






        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);







        break;
      }
    }
  }
}

String decodeHtmlEntities(const String &str)
{
  String decoded = str;
  decoded.replace("&#246;", "ö");
  decoded.replace("&#228;", "ä");
  decoded.replace("&#252;", "ü");
  decoded.replace("&#223;", "ß");
  decoded.replace("&#196;", "Ä");
  decoded.replace("&#214;", "Ö");
  decoded.replace("&#220;", "Ü");

  return decoded;
}

int selection = 0;

void request_data()
{
  Serial.println("Requesting data");
  if (WiFi.status() == WL_CONNECTED)
  {

    WiFiClientSecure client;
    client.setCACert(test_root_ca);
    HTTPClient http;
    String serverPath = url[selection];
    http.begin(client, serverPath.c_str());
    http.addHeader("Content-Type", "application/json; charset=utf-8");
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
      String payload = http.getString();
      payload = String(payload.c_str());
      payload = decodeHtmlEntities(payload);

      StaticJsonDocument<0> filter;
      filter.set(true);
      DynamicJsonDocument doc(4096 * 3);
      DeserializationError error = deserializeJson(doc, payload.substring(14), DeserializationOption::Filter(filter), DeserializationOption::NestingLimit(15));
      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      String station_name = doc["stationName"];
      Serial.println(station_name);
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(title_colors[selection], TFT_BLACK);
      tft.setTextSize(1);

      tft.setTextDatum(TL_DATUM);
      tft.drawString(station_name, 0, 0);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      String clock = timeString();
      tft.drawString(clock, 280, 0);
      int row = 0;
      for (JsonObject journey : doc["journey"].as<JsonArray>())
      {


        String time = journey["ti"];
        String train = journey["pr"];
        String destination = journey["st"];
        String track = journey["tr"];
        String line = journey["pr"];


        Serial.println(time + " " + train + " " + destination + " " + track);

        int y = 20 + row * 20;
        bool isCanceled = false;
        if (journey["rt"])
        {
          time = journey["rt"]["dlt"].as<String>();
          Serial.println(journey["rt"]["dlt"].as<String>());
          tft.setTextColor(TFT_RED, TFT_BLACK);
          if (journey["rt"]["status"] == "Ausfall")
          {
            isCanceled = true;
            time = "Cncld";
          }
        }
        if (!isCanceled)
        {
          int dep_min = (time.substring(0, 2).toInt() - clock.substring(0, 2).toInt()) * 60 + time.substring(3, 5).toInt() - clock.substring(3, 5).toInt();
          tft.drawString(String(dep_min), 0, y);
        }

        tft.drawString(time, 30, y);
        if (!isCanceled)
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(train, 70, y);
        if (destination.length() > 25)
        {
          destination = destination.substring(0, 25);
        }
        tft.drawString(destination, 120, y);
        tft.drawString(track, 300, y);
        row++;
      }
    }
  }
}


void setup()
{
  Serial.begin(115200);
  SetupCYD();
  SetupWiFi();
  request_data();
}

unsigned int last_time = 0;

void loop()
{
  if (millis() - last_time > timer_delay)
  {

    request_data();
    last_time = millis();
  }

  if (ts.tirqTouched() && ts.touched())
  {
    TS_Point p = ts.getPoint();
    Serial.print("Pressure = ");
    Serial.print(p.z);
    Serial.print(", x = ");
    Serial.print(p.x);
    Serial.print(", y = ");
    Serial.print(p.y);
    Serial.println(" Touched");
    if (p.y < 1000)
    {
      request_data();
    }
    else if (p.x > 2000)
    {
      selection = (selection + 1) % 4;
      request_data();
    }
    else if (p.x < 1000)
    {
      selection = (selection + 3) % 4;
      request_data();
    }

    delay(500);
  }
}