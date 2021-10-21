

#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xD69A      /* 211, 211, 211 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFE19      /* 255, 192, 203 */
#define TFT_BROWN       0x9A60      /* 150,  75,   0 */
#define TFT_GOLD        0xFEA0      /* 255, 215,   0 */
#define TFT_SILVER      0xC618      /* 192, 192, 192 */
#define TFT_SKYBLUE     0x867D      /* 135, 206, 235 */
#define TFT_VIOLET      0x915C      /* 180,  46, 226 */

#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#define _TASK_TIMECRITICAL


#define WIFI_AP ""
#define WIFI_PASSWORD ""



#include <Adafruit_MLX90614.h>


#include <WiFi.h>

#include <DNSServer.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <TaskScheduler.h>
#include <PubSubClient.h>


#include <ArduinoOTA.h>


#include "Alert.h" // Out of range alert icon
#include "wifi1.h" // Signal Streng WiFi
#include "wifi2.h" // Signal Streng WiFi
#include "wifi3.h" // Signal Streng WiFi
#include "wifi4.h" // Signal Streng WiFi

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>


#include "Free_Fonts.h" // Include the header file attached to this sketch





#include "RTClib.h"
#include "Free_Fonts.h"
#include "EEPROM.h"

#define HOSTNAME "TAT"
#define PASSWORD "7650"

TaskHandle_t Task1;
String deviceToken = "";
uint64_t chipId = 0;
String weight = "";
String rawWeight = "";
String wifiName = "@WC";
float DP = 0.0;
unsigned long ms;
unsigned long diffMillis = 0;
TaskHandle_t Task2;

struct Settings
{
  char TOKEN[40] = "";
  char SERVER[40] = "mqttservice.smartfarmpro.com";
  int PORT = 1883;
  char MODE[60] = "Farm/Cloud/Device/Mode2";
  uint32_t ip;
} sett;

#define trigWDTPin    32
#define ledHeartPIN   0
HardwareSerial hwSerial(1);
#define SERIAL1_RXPIN 27
#define SERIAL1_TXPIN 14

//unsigned long drawTime = 0;
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library with default width and height
TFT_eSprite weightSprite = TFT_eSprite(&tft); // Invoke custom library with default width and height

TFT_eSprite headerSprite = TFT_eSprite(&tft); // Invoke custom library with default width and height
TFT_eSprite wifiSprite = TFT_eSprite(&tft); // Invoke custom library with default width and height
TFT_eSprite timeSprite = TFT_eSprite(&tft); // Invoke custom library with default width and height
TFT_eSprite updateSprite = TFT_eSprite(&tft); // Invoke custom library with default width and height




Scheduler runner;

float value = 0.0;
String json = "";


WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


int rssi = 0; ;

// # Add On
#include <TimeLib.h>
#include <ArduinoJson.h>
#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 7;

const int   daylightOffset_sec = 3600;

bool connectWifi = false;
StaticJsonDocument<400> doc;
bool validEpoc = false;
String dataJson = "";
unsigned long _epoch = 0;
unsigned long _messageId = 0;
WiFiManager wifiManager;
unsigned long time_s = 0;

struct tm timeinfo;




int reading = 0; // Value to be displayed
int d = 0; // Variable used for the sinewave test waveform
boolean range_error = 0;
int8_t ramp = 1;



#define HEADER_WIDTH  160
#define HEADER_HEIGHT 30
#define WEIGHT_WIDTH 160
#define WEIGHT_HEIGHT 50
#define TIME_WIDTH  100
#define TIME_HEIGHT 35

#define WiFi_WIDTH  30
#define WiFi_HEIGHT 30


// Pause in milliseconds between screens, change to 0 to time font rendering


// Callback methods prototypes
void tCallback();

void t2CallShowEnv();
void t3CallConnector();
void t4CallWatchDog();
void t5CallSendAttribute();
void t7showTime();
// Tasks

Task t2(60000, TASK_FOREVER, &t2CallShowEnv);
Task t3(360000, TASK_FOREVER, &t3CallConnector);
//
Task t4(15000, TASK_FOREVER, &t4CallWatchDog);  //adding task to the chain on creation
Task t5(10400000, TASK_FOREVER, &t5CallSendAttribute);  //adding task to the chain on creation
Task t7(60000, TASK_FOREVER, &t7showTime);

//flag for saving data
bool shouldSaveConfig = false;


  void drawWiFi( )
  {

    //  wifiSprite.fillScreen(TFT_GREEN);
 
    if (rssi <= 1)
      wifiSprite.pushImage(0 , 0, wifi1Width, wifi1Height, wifi1);
    if (rssi == 2)
      wifiSprite.pushImage(0 , 0, wifi1Width, wifi1Height, wifi2);
    if (rssi == 3)
      wifiSprite.pushImage(0 , 0, wifi1Width, wifi1Height, wifi3);
    if (rssi >= 4)
      wifiSprite.pushImage(0 , 0, wifi1Width, wifi1Height, wifi4);

    wifiSprite.pushSprite(140, 0);
  }



void Task2code( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    //    delay(2000);
    
    ms = millis();
    if (ms % 1000 == 0)
    {
      
      rssi = map(WiFi.RSSI(), -90, -20, 1, 4);
      
    }
  }
}  
  class IPAddressParameter : public WiFiManagerParameter
  {
    public:
      IPAddressParameter(const char *id, const char *placeholder, IPAddress address)
        : WiFiManagerParameter("")
      {
        init(id, placeholder, address.toString().c_str(), 16, "", WFM_LABEL_BEFORE);
      }

      bool getValue(IPAddress &ip)
      {
        return ip.fromString(WiFiManagerParameter::getValue());
      }
  };

  class IntParameter : public WiFiManagerParameter
  {
    public:
      IntParameter(const char *id, const char *placeholder, long value, const uint8_t length = 10)
        : WiFiManagerParameter("")
      {
        init(id, placeholder, String(value).c_str(), length, "", WFM_LABEL_BEFORE);
      }

      long getValue()
      {
        return String(WiFiManagerParameter::getValue()).toInt();
      }
  };



  char  char_to_byte(char c)
  {
    if ((c >= '0') && (c <= '9'))
    {
      return (c - 0x30);
    }
    if ((c >= 'A') && (c <= 'F'))
    {
      return (c - 55);
    }
  }

  String read_String(char add)
  {
    int i;
    char data[100]; //Max 100 Bytes
    int len = 0;
    unsigned char k;
    k = EEPROM.read(add);
    while (k != '\0' && len < 500) //Read until null character
    {
      k = EEPROM.read(add + len);
      data[len] = k;
      len++;
    }
    data[len] = '\0';

    return String(data);
  }


  void getChipID() {
    chipId = ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
    Serial.printf("ESP32ChipID=%04X", (uint16_t)(chipId >> 32)); //print High 2 bytes
    Serial.printf("%08X\n", (uint32_t)chipId); //print Low 4bytes.

  }

  void setupOTA()
  {
    //Port defaults to 8266
    //ArduinoOTA.setPort(8266);

    //Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(uint64ToString(chipId).c_str());

    //No authentication by default
    ArduinoOTA.setPassword(PASSWORD);

    //Password can be set with it's md5 value as well
    //MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]()
    {
      Serial.println("Start Updating....");

      Serial.printf("Start Updating....Type:%s\n", (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem");
    });

    ArduinoOTA.onEnd([]()
    {

      Serial.println("Update Complete!");

      ESP.restart();
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
    {
      String pro = String(progress / (total / 100)) + "%";
      int progressbar = (progress / (total / 100));
      //int progressbar = (progress / 5) % 100;
      //int pro = progress / (total / 100);

      drawUpdate(progressbar, 110, 0);


      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error)
    {
      Serial.printf("Error[%u]: ", error);
      String info = "Error Info:";
      switch (error)
      {
        case OTA_AUTH_ERROR:
          info += "Auth Failed";
          Serial.println("Auth Failed");
          break;

        case OTA_BEGIN_ERROR:
          info += "Begin Failed";
          Serial.println("Begin Failed");
          break;

        case OTA_CONNECT_ERROR:
          info += "Connect Failed";
          Serial.println("Connect Failed");
          break;

        case OTA_RECEIVE_ERROR:
          info += "Receive Failed";
          Serial.println("Receive Failed");
          break;

        case OTA_END_ERROR:
          info += "End Failed";
          Serial.println("End Failed");
          break;
      }


      Serial.println(info);
      ESP.restart();
    });

    ArduinoOTA.begin();
  }

  String uint64ToString(uint64_t input) {
    String result = "";
    uint8_t base = 10;

    do {
      char c = input % base;
      input /= base;

      if (c < 10)
        c += '0';
      else
        c += 'A' - 10;
      result = c + result;
    } while (input);
    return result;
  }


  void setupWIFI()
  {
    WiFi.setHostname(uint64ToString(chipId).c_str());

    byte count = 0;
    while (WiFi.status() != WL_CONNECTED && count < 10)
    {
      count ++;
      delay(500);
      Serial.print(".");
    }


    if (WiFi.status() == WL_CONNECTED)
      Serial.println("Connecting...OK.");
    else
      Serial.println("Connecting...Failed");

    reconnectMqtt();
  }

  void reconnectMqtt()
  {
    if (client.connect("weightCapture", "tatmqtt", "%tatmqttpassword%")) {


      Serial.println( F("Connect MQTT Success."));

    } else {
      Serial.println("Not connected");
    }
  }
  void writeString(char add, String data)
  {
    int _size = data.length();
    int i;
    for (i = 0; i < _size; i++)
    {
      EEPROM.write(add + i, data[i]);
    }
    EEPROM.write(add + _size, '\0'); //Add termination null character for String Data
    EEPROM.commit();
  }

  void _writeEEPROM(String data) {
    //Serial.print("Writing Data:");
    //Serial.println(data);
    writeString(10, data);  //Address 10 and String type data
    delay(10);
  }


  void splash() {

    tft.init();
    // Swap the colour byte order when rendering
    tft.setSwapBytes(true);
    tft.setRotation(1);  // landscape

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(5, (tft.height() / 2) - 50, 1);

    tft.setTextColor(TFT_YELLOW);


    tft.setTextFont(4);
    tft.println("Weight");
    tft.println("    Capture");
    tft.println("        by");
    tft.println("            TAT");

    delay(5000);
    //  tft.setTextPadding(180);


    Serial.println(F("Start..."));
    for ( int i = 0; i < 250; i++)
    {
      tft.drawString(".", 1 + 2 * i, 300, GFXFF);
      delay(10);
      //    Serial.println(i);
    }
    Serial.println("end");
  }


  void _initLCD() {
    tft.fillScreen(TFT_BLACK);
    // TFT
    splash();
    // MLX
    mlx.begin();
  }


  void configModeCallback (WiFiManager * myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());
  }

  void saveConfigCallback () {
    Serial.println("Should save config");
    shouldSaveConfig = true;
    Serial.print("saveConfigCallback:");
    Serial.println(sett.TOKEN);
  }



  void setup(void) {

    Serial.begin(115200);
    // make the pins outputs:
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH);

    delay(1000);
    Serial.println("Start");
    hwSerial.begin(4800, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);

    EEPROM.begin(512);
    _initLCD();
    if (EEPROM.read(10) == 255 ) {
      _writeEEPROM("147.50.151.130");
    }
    getChipID();
    tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.drawString("WiFi Setting", 5, (tft.height() / 2) - 30, 4);
    tft.drawString("(120 Sec)", 40, (tft.height() / 2) + 5, 4);
    tft.setTextColor(TFT_BLUE);


    wifiManager.setTimeout(120);
    HeartBeat();
    wifiManager.setAPCallback(configModeCallback);
    std::vector<const char *> menu = {"wifi", "info", "sep", "restart", "exit"};
    wifiManager.setMenu(menu);
    wifiManager.setClass("invert");
    wifiManager.setConfigPortalTimeout(120); // auto close configportal after n seconds
    wifiManager.setAPClientCheck(true); // avoid timeout if client connected to softap
    wifiManager.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

    WiFiManagerParameter blnk_Text("<b>Device setup.</b> <br>");
    sett.TOKEN[39] = '\0';   //add null terminator at the end cause overflow
    sett.SERVER[39] = '\0';   //add null terminator at the end cause overflow
    WiFiManagerParameter blynk_Token( "blynktoken", "device Token",  sett.TOKEN, 40);
    WiFiManagerParameter blynk_Server( "blynkserver", "Server",  sett.SERVER, 40);
    IntParameter blynk_Port( "blynkport", "Port",  sett.PORT);
    HeartBeat();
    wifiManager.addParameter( &blnk_Text );
    wifiManager.addParameter( &blynk_Token );
    wifiManager.addParameter( &blynk_Server );
    wifiManager.addParameter( &blynk_Port );


    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);




    wifiName.concat(uint64ToString(chipId));
    if (!wifiManager.autoConnect(wifiName.c_str())) {
      Serial.println("failed to connect and hit timeout");


    }
    deviceToken = wifiName.c_str();


    configTime(gmtOffset_sec, 0, ntpServer);
    client.setServer( sett.SERVER, sett.PORT );


    setupWIFI();
    setupOTA();



    runner.init();
    Serial.println("Initialized scheduler");


    runner.addTask(t2);
    //  Serial.println("added t2");
    runner.addTask(t3);
    //  Serial.println("added t3");
    runner.addTask(t4);
    //  Serial.println("added t4");
    //  runner.addTask(t6);
    runner.addTask(t7);
    delay(2000);

    t2.enable();
    //  Serial.println("Enabled t2");
    t3.enable();
    //  Serial.println("Enabled t3");
    t4.enable();
    //  Serial.println("Enabled t4");
    //  t6.enable();
    t7.enable();

    //  Serial.println("Initialized scheduler");
    tft.begin();

    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    weightSprite.createSprite(WEIGHT_WIDTH, WEIGHT_HEIGHT);
    weightSprite.fillSprite(TFT_BLACK);

    headerSprite.createSprite(HEADER_WIDTH, HEADER_HEIGHT);
    headerSprite.fillSprite(TFT_BLACK);

    wifiSprite.createSprite(WiFi_WIDTH, WiFi_HEIGHT);
    wifiSprite.fillSprite(TFT_BLACK);

    timeSprite.createSprite(TIME_WIDTH, TIME_HEIGHT);
    timeSprite.fillSprite(TFT_BLACK);



    drawWiFi();
    drawTime();
    
    header(deviceToken.c_str());

    //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
    xTaskCreatePinnedToCore(
      Task2code,   /* Task function. */
      "Task2",     /* name of task. */
      10000,       /* Stack size of task */
      NULL,        /* parameter of the task */
      1,           /* priority of the task */
      &Task2,      /* Task handle to keep track of created task */
      1);          /* pin task to core 1 */
    delay(500);

  }



  void loop() {
    //  time_t now;
    String str = "";
    int byteCount = 0;


    while (hwSerial.available()) {
      char c = hwSerial.read();

      rawWeight += c;

      diffMillis = millis();
    }



    weight = rawWeight.substring(4, rawWeight.length() - 3);
    weight.trim();

    drawWeight();
    if (rawWeight.length() > 0) {


      composeJson();


      int isSend = client.publish(sett.MODE, json.c_str());
      Serial.println(isSend);
      if(isSend < 1)
        rssi = 0;
    }


    rawWeight = "";

    runner.execute();

    ArduinoOTA.handle();
  }


  void drawWeight() {
    int offset = 0;
    weightSprite.fillScreen(TFT_BLACK);
    weightSprite.setTextColor(TFT_WHITE);

    if (_messageId <= 9) {
      offset = 60;
    } else if ((_messageId >= 10) && (_messageId <= 99)) {
      offset = 40;
    } else if ((_messageId >= 100) && (_messageId <= 999)) {
      offset = 20;
    } else if ((_messageId >= 1000) && (_messageId <= 999)) {
      offset = 0;
    }
    weightSprite.drawNumber(_messageId, offset, 0, 7); // Value in middle
    //    weightSprite.drawString("Kg", 143, 15, 1); // Value in middle

    weightSprite.pushSprite(0, 40);


  }
  String a0(int n) {
    return (n < 10) ? "0" + String(n) : String(n);
  }

  void drawTime() {

    String dateS = "";
    String timeS = "";
    //  _epoch = time(&now) + 25200; // GMT+7 Asia/BKK Timezone
    //  ts = _epoch;
    if (!getLocalTime(&timeinfo)) {
      //Serial.println("Failed to obtain time");
      return;
    }


    dateS = a0(timeinfo.tm_mday) + "/" + a0(timeinfo.tm_mon + 1) + "/" + String(timeinfo.tm_year + 1900);
    timeS = a0(timeinfo.tm_hour) + ":" + a0(timeinfo.tm_min);//  + ":" + a0(timeinfo.tm_sec);
    timeSprite.fillScreen(TFT_BLACK);

    timeSprite.setTextColor(TFT_YELLOW);
    timeSprite.drawString(dateS, 0, 0, 2); // Font 4 for fast drawing with background
    timeSprite.drawString(timeS, 0, 15, 2); // Font 4 for fast drawing with background

    timeSprite.pushSprite(0, 0);

  }

  void drawUpdate(int num, int x, int y)
  {
    updateSprite.createSprite(25, 20);
    updateSprite.fillScreen(TFT_BLACK);
    //  updateSprite.setFreeFont(FSB9);
    updateSprite.setTextColor(TFT_YELLOW);
    updateSprite.setTextSize(1);
    updateSprite.drawNumber(num, 0, 4);
    updateSprite.drawString("%", 20, 4);
    updateSprite.pushSprite(x, y);
    updateSprite.deleteSprite();
  }



  //====================================================================================
  // This is the function to draw the icon stored as an array in program memory (FLASH)
  //====================================================================================

  // To speed up rendering we use a 64 pixel buffer
#define BUFF_SIZE 64

  // Draw array "icon" of defined width and height at coordinate x,y
  // Maximum icon size is 255x255 pixels to avoid integer overflow

  // Print the header for a display screen
  void header(const char *string)
  {
    headerSprite.setTextSize(1);


    headerSprite.setTextColor(TFT_GREEN);
    headerSprite.drawString(string, 5, 2, 2); // Font 4 for fast drawing with background
    headerSprite.pushSprite(0, tft.height() - 15);


  }

  // Draw a + mark centred on x,y
  void drawDatum(int x, int y)
  {
    tft.drawLine(x - 5, y, x + 5, y, TFT_GREEN);
    tft.drawLine(x, y - 5, x, y + 5, TFT_GREEN);
  }

  void composeJson() {

    _messageId++;
    json = "";
    if (_messageId > 9999) _messageId = 0;
    json.concat(deviceToken);
    json.concat(",");
    json.concat(_messageId);
    json.concat(",");
    json.concat(diffMillis);
    json.concat(",");
    json.concat(rawWeight);
    Serial.println(json);




  }



  void tCallback() {
    Scheduler &s = Scheduler::currentScheduler();
    Task &t = s.currentTask();

    //  Serial.print("Task: "); Serial.print(t.getId()); Serial.print(":\t");
    //  Serial.print(millis()); Serial.print("\tStart delay = "); Serial.println(t.getStartDelay());
    //  delay(10);

    //  if (t1.isFirstIteration()) {
    //    runner.addTask(t2);
    //    t2.enable();
    //    //    Serial.println("t1CallgetProbe: enabled t2CallshowEnv and added to the chain");
    //  }


  }



  void t2CallShowEnv() {

    drawWiFi();
    Serial.print("deviceToken:");
    Serial.println(deviceToken);
  }
  void t3CallConnector() {
    Serial.println("Attach WiFi for，OTA "); Serial.println(WiFi.RSSI() );

    setupWIFI();
    setupOTA();
    if ( !client.connected() )
    {
      reconnectMqtt();
    }

  }

  void HeartBeat() {
    //   Sink current to drain charge from watchdog circuit
    pinMode(trigWDTPin, OUTPUT);
    digitalWrite(trigWDTPin, LOW);

    // Led monitor for Heartbeat
    digitalWrite(ledHeartPIN, LOW);
    delay(300);
    digitalWrite(ledHeartPIN, HIGH);

    // Return to high-Z
    pinMode(trigWDTPin, INPUT);

    Serial.println("Heartbeat");

  }

  void t4CallWatchDog() {
    HeartBeat();
  }

  void t5CallSendAttribute() {}

  void t7showTime() {


    drawTime();

  }
