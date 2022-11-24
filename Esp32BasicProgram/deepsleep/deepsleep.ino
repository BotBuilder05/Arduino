/*********
  Xavier Charles
  Based on 
  Complete project details at http://randomnerdtutorials.com  
  and 
  https://electronza.com/gardena-water-timer-controller-arduino-uno/#prettyPhoto
  Using H-bride L9110 but just half of it as it was designed to control 2 motors.
  In order to change power signed I will use only  the Motor A pins
*********/

//Add bluetooth for logs
#include "BluetoothSerial.h"
#include <time.h>
#include <sys/time.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  7200        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

// Wake touchpad
#define Threshold 40 /* Greater the value, more the sensitivity */
touch_pad_t touchPin;


// Motor A
const int motorPinIA = 23;
const int motorPinIB = 22; 

void ouverture(){
  // Move the DC motor forward at maximum speed
  Serial.println("Ouverture Vanne");
  SerialBT.println("Ouverture Vanne");
  digitalWrite(motorPinIA, LOW);
  digitalWrite(motorPinIB, HIGH); 
  delay(5000);
}

void fermeture(){
  // Move DC motor backwards at maximum speed
  Serial.println("Fermeture");
  SerialBT.println("Fermeture");
  digitalWrite(motorPinIA, HIGH);
  digitalWrite(motorPinIB, LOW); 
  delay(5000);
}


/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


void print_wakeup_touchpad(){
  touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch(touchPin)
  {
    case 0  : Serial.println("Touch detected on GPIO 4"); break;
    case 1  : Serial.println("Touch detected on GPIO 0"); break;
    case 2  : Serial.println("Touch detected on GPIO 2"); break;
    case 3  : Serial.println("Touch detected on GPIO 15"); break;
    case 4  : Serial.println("Touch detected on GPIO 13"); break;
    case 5  : Serial.println("Touch detected on GPIO 12"); break;
    case 6  : Serial.println("Touch detected on GPIO 14"); break;
    case 7  : Serial.println("Touch detected on GPIO 27"); break;
    case 8  : Serial.println("Touch detected on GPIO 33"); break;
    case 9  : Serial.println("Touch detected on GPIO 32"); break;
    default : Serial.println("Wakeup not by touchpad"); break;
  }
}

void callback(){
  //placeholder callback function
}






void setup() {
  // sets the pins as outputs:
  pinMode(motorPinIA, OUTPUT);
  pinMode(motorPinIB, OUTPUT);

  time_t now;
  char strftime_buf[64];
  struct tm timeinfo;

  time(&now);
  // Set timezone to China Standard Time
  setenv("TZ", "CST-8", 1);
  tzset();

  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

  /*tm local;
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) { // if reset or power loss
    struct timeval val;
    loadStruct(&local); // e.g. load time from eeprom
    const time_t sec = mktime(&local); // make time_t
    localtime(&sec); //set time
  } else if (sp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER ) { // if reset or power loss
    struct timeval val;
    loadStruct(&local); // e.g. load time from eeprom
    const time_t sec = mktime(&local); // make time_t
    localtime(&sec); //set time
  } else {
    getLocalTime(&local);
  }*/

  Serial.begin(115200);
  SerialBT.begin("ESP32BT");
  Serial.println("Time in Gap: " + String(strftime_buf));

    struct tm tm;
    tm.tm_year = 2020 - 1900;
    tm.tm_mon = 10; // 0-january 11 december
    tm.tm_mday = 29;
    tm.tm_hour = 9;
    tm.tm_min = 17;
    tm.tm_sec = 0;
    time_t t = mktime(&tm);
    printf("Setting time: %s", asctime(&tm));
    struct timeval now2 = { .tv_sec = t };
    settimeofday(&now2, NULL);
  
  //ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
  Serial.println("The device started, now you can pair it with bluetooth!");
  SerialBT.println("The device started, now you can pair it with bluetooth!");
  delay(5000); // Take some time to open up the Serial Monitor

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  SerialBT.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  print_wakeup_touchpad();

  //Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T3, callback, Threshold);

  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_sleep_enable_timer_wakeup(60000000);
  //esp_sleep_enable_timer_wakeup(3600000000);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  SerialBT.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */
  ouverture();
  fermeture();
  Serial.println("Going to sleep now");
  SerialBT.println("Going to sleep now");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {
    // With deep sleep nothing is exceuted from loop function -> NO CODE THERE!!
}
