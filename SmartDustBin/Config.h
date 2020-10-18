#define BLYNK_PRINT Serial
//PINS
#define SPEAKER_PIN D7
#define  AOUTpin 0        //the AOUT pin of the alcohol sensor goes into analog pin A0 of the arduino
#define TRIGGER_PIN  D5   // Arduino pin D5 tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     D0   // Arduino pin D0 tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 50   // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define bin_capacity 30   // Please define your bin capacity.
#define bin_smell 250     // Please define your bin smell.

//Define wifi
#define wifi_name ""         //Please define your wifi name.
#define wifi_password ""  //Please define your wifi password.


#define BLYNK_KEY   "" // You should get Auth Token in the Blynk App.

#define AIO_USERNAME    "" // Please define your adafruit username.
#define AIO_KEY         "" //// Please define your adafruit auth key.

#define USER_EMAIL "" //Please define  your email here 

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883


//ESP
#define ESP_SSID    wifi_name
#define ESP_PASS    wifi_password
//WLAN
#define WLAN_SSID   wifi_name
#define WLAN_PASS   wifi_password
