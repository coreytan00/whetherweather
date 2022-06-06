#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>

#include <WiFi.h>
#include <HttpClient.h>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include <TFT_eSPI.h>

#define I2C_SDA 21
#define I2C_SCL 22

#define GREEN 26
#define YELLOW 25
#define BLUE 33
#define RED 32
#define WHITE 12

/*
#define A {GREEN, YELLOW, RED}
#define B {GREEN, BLUE, YELLOW}
#define C {RED, YELLOW}
#define D {GREEN, YELLOW, BLUE, WHITE, RED}
#define E {BLUE, WHITE, RED}
*/

char ssid[] = "Patricks iPhone"; //"PDS-Resident"; // //    // your network SSID (name) 
char pass[] = "FriedChicken#0";// "AC86fm!6"; // // // your network password (use for WPA, or use as key for WEP)

// Name of the server we want to connect to
// const char kHostname[] = "worldtimeapi.org";
const char kHostname[] = "18.212.254.119";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath2[] = "/led_config";

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

Adafruit_AHTX0 aht;

TFT_eSPI tft = TFT_eSPI();

// Turn light on
void led_control(char color, int off = 0, int ms = 500, int ms2 = 500) {
  digitalWrite(color, HIGH);

  if (off) {
    delay(ms);
    digitalWrite(color, LOW);
    delay(ms2);
  }
}

// Turn multiple lights on at once
void led_control(int colors[], int len, int off = 0, int ms = 500, int ms2 = 500) {
  // Serial.println(sizeof(colors));
  // Serial.println(sizeof(colors[0]));
  
  //int len = sizeof(colors)/sizeof(colors[0])+1;
  // Serial.print(len);

  for (int i = 0; i < len; i++) {
    digitalWrite(colors[i], HIGH);
  }

  if (off) {
    delay(ms);
    for (int i = 0; i < len; i++) {
      digitalWrite(colors[i], LOW);
    }
    delay(ms2);
  }
}

sensors_event_t humidity, temp;

void setup() {
  Serial.begin(9600);

  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

// Check aht is connected
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  } 
  Serial.println("AHT20 found!");

  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(WHITE, OUTPUT);

  int err =0;

  // Start the OLED
  tft.init();
  tft.setRotation(1);

  WiFiClient c;
  HttpClient http(c);

  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  //Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  // Ready the OLED
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(100, 50, 1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  
  tft.setTextSize(7);

  // Print out the temperature
  tft.println(temp.temperature);
  tft.setCursor(100, 50, 1);

  char kPath[256] ;
  char tmp[8];
  dtostrf(temp.temperature,4,2,tmp);
  strcpy(kPath, "/?var=");
  strcat(kPath, tmp);

  delay(500);
  
  err = http.get(kHostname, 5000, kPath);

  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                Serial.print(c);
               
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("GET Connect failed: ");
    Serial.println(err);
  }
}

int temparr[5];
int temparr2[5];
int temparr3[5];

void loop() {

  char leds[3];
  int err2 =0;
  
  WiFiClient c;
  HttpClient http(c);

  // POST request start
  err2 = http.get(kHostname, 5000, kPath2);


  if (err2 == 0)
  {
    Serial.println("startedRequest ok");

    err2 = http.responseStatusCode();
    if (err2 >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err2);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err2 = http.skipResponseHeaders();
      if (err2 >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        int count = 0;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                //Serial.print(c);
                //if terminate
                if (c == ','){
                  //skip
                }
                else{
                  leds[count++] = c;
                }
               
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err2);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err2);
    }
  }
  else
  {
    Serial.print("POST Connect failed: ");
    Serial.println(err2);
  }

  /*
  led_control(GREEN, 1, 250, 250);
  led_control(YELLOW, 1, 250, 250);
  led_control(BLUE, 1, 250, 250);
  led_control(RED, 1, 250, 250);
  led_control(WHITE, 1);
  */

  Serial.print(leds[0]);
  Serial.print(leds[1]);
  Serial.print(leds[2]);

  //TEMPARR
  //easter
  if (leds[0]=='0') {
    temparr[0] = YELLOW;
    temparr[1] = GREEN;
    temparr[2] = 0;
    temparr[3] = 0;
    temparr[4] = 0;
  }
  //bingbong
  else if (leds[0]=='1') {
    temparr[0] = YELLOW;
    temparr[1] = BLUE;
    temparr[2] = 0;
    temparr[3] = 0;
    temparr[4] = 0;
  }
  //fire
  else if (leds[0]=='2') {
    temparr[0] = RED;
    temparr[1] = 0;
    temparr[2] = 0;
    temparr[3] = 0;
    temparr[4] = 0;
  }
  //party
  else if (leds[0]=='3') {
    temparr[0] = YELLOW;
    temparr[1] = GREEN;
    temparr[2] = BLUE;
    temparr[3] = WHITE;
    temparr[4] = RED;
  }
  //america
  else if (leds[0]=='4') {
    temparr[0] = WHITE;
    temparr[1] = BLUE;
    temparr[2] = RED;
    temparr[3] = 0;
    temparr[4] = 0;
  }

  //TEMPARR2
  if (leds[1]=='0') {
    temparr2[0] = YELLOW;
    temparr2[1] = GREEN;
    temparr2[2] = 0;
    temparr2[3] = 0;
    temparr2[4] = 0;
  }
  else if (leds[1]=='1') {
    temparr2[0] = YELLOW;
    temparr2[1] = BLUE;
    temparr2[2] = 0;
    temparr2[3] = 0;
    temparr2[4] = 0;
  }
  else if (leds[1]=='2') {
    temparr2[0] = RED;
    temparr2[1] = 0;
    temparr2[2] = 0;
    temparr2[3] = 0;
    temparr2[4] = 0;
  }
  else if (leds[1]=='3') {
    temparr2[0] = YELLOW;
    temparr2[1] = GREEN;
    temparr2[2] = BLUE;
    temparr2[3] = WHITE;
    temparr2[4] = RED;
  }
  else if (leds[1]=='4') {
    temparr2[0] = WHITE;
    temparr2[1] = BLUE;
    temparr2[2] = RED;
    temparr2[3] = 0;
    temparr2[4] = 0;
  }

  //TEMPARR3
  if (leds[2]=='0') {
    temparr3[0] = YELLOW;
    temparr3[1] = GREEN;
    temparr3[2] = 0;
    temparr3[3] = 0;
    temparr3[4] = 0;
  }
  else if (leds[2]=='1') {
    temparr3[0] = YELLOW;
    temparr3[1] = BLUE;
    temparr3[2] = 0;
    temparr3[3] = 0;
    temparr3[4] = 0;
  }
  else if (leds[2]=='2') {
    temparr3[0] = RED;
    temparr3[1] = 0;
    temparr3[2] = 0;
    temparr3[3] = 0;
    temparr3[4] = 0;
  }
  else if (leds[2]=='3') {
    temparr3[0] = YELLOW;
    temparr3[1] = GREEN;
    temparr3[2] = BLUE;
    temparr3[3] = WHITE;
    temparr3[4] = RED;
  }
  else if (leds[2]=='4') {
    temparr3[0] = WHITE;
    temparr3[1] = BLUE;
    temparr3[2] = RED;
    temparr3[3] = 0;
    temparr3[4] = 0;
  }
 
  Serial.println();
  Serial.println(temp.temperature);

  if (temp.temperature < 18) 
  {
    Serial.println("less than 18");
    led_control(temparr, sizeof(temparr)/sizeof(temparr[0]), 1);
  }
  else if (temp.temperature > 18 && temp.temperature < 25)
  {
    Serial.println("in between 18 and 25");
    led_control(temparr2, sizeof(temparr2)/sizeof(temparr2[0]), 1);
  }
  else if (temp.temperature > 25)
  {
    Serial.println("greater than 25");
    led_control(temparr3, sizeof(temparr3)/sizeof(temparr3[0]), 1);
  }

  // Set the colors
  // Set the alarm
  // Front end
}


