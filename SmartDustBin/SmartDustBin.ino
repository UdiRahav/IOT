/**
 * ID's:
 * Udi Rahav - 204694053
 * Ran Havaletzki - 205784283
 * 
 * This is the Smartbin code - a mechanism that will alert us when the bin is getting full, 
 * or when it reaches an alcohol scent level that requires replacement (i.e., it's stinky).
*/
#include "Config.h"
#include "notes.h"
/*
  AdafruitIO
*/
// Libraries
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

int prvCm = 0 ;
int gasValue_data = 0;
int prvGas = 0;
int cmValue_data = 0;
int bin[3] = {bin_capacity/3, bin_capacity/2, bin_capacity};
int smell[3] = {bin_smell+100,bin_smell+200,bin_smell+300};

//Number of alerts.
int Capacity_Alert = 0;
int Smell_Alert = 0;

double SPEAKER_COUNTER = 0;
bool SPEAKER_ALERT = true;
//bool SPEAKER_BUTTON_ON = false;

int notes[] = {
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4,
  NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
};

// Blynk info
char auth[] = BLYNK_KEY;
char ssid[] = wifi_name;
char pass[] = wifi_password;

// Functions
void connect();

// Store the MQTT server, client ID, username, and password in flash memory.
const char MQTT_SERVER[] = AIO_SERVER;

// Set a unique MQTT client ID using the AIO key + the date and time the sketch
// was compiled (so this should be unique across multiple devices for a user,
// alternatively you can manually set this to a GUID or other random value).
const char MQTT_CLIENTID[] = AIO_KEY __DATE__ __TIME__;
const char MQTT_USERNAME[] = AIO_USERNAME;
const char MQTT_PASSWORD[] = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Setup feeds for temperature & humidity
const char CAPACITY_FEED[] = AIO_USERNAME "/feeds/capacity";
Adafruit_MQTT_Publish capacity = Adafruit_MQTT_Publish(&mqtt, CAPACITY_FEED);

const char GAS_FEED[] = AIO_USERNAME "/feeds/Gas";
Adafruit_MQTT_Publish Gas = Adafruit_MQTT_Publish(&mqtt, GAS_FEED);

const char SPEAKER_FEED[] = AIO_USERNAME "/feeds/Speaker";
Adafruit_MQTT_Publish Speaker = Adafruit_MQTT_Publish(&mqtt, SPEAKER_FEED);


void setup() {
  Serial.begin(115200); // initialize the serial port:
  Blynk.begin(auth, ssid, pass);

  delay(10);
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  // wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    delay(10);

    pinMode(SPEAKER_PIN, OUTPUT);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  // connect to adafruit io
  connect();

}

void loop() {
  mqttPing();

  Blynk.run();
  prvCm = cmValue_data;
  prvGas = gasValue_data;
  cmValue_data = (bin_capacity - ping_cm());  // Grab the current state of the distance sensor
  gasValue_data = smelly();                   // Grab the current state of the alcohol sensor
  if(cmValue_data <= 5) {
    SPEAKER_COUNTER = 0;
  }
  printDustBinStatus();
  blynkNotifyEmail();
  publishData();
}

void mqttPing() {
  // ping adafruit io a few times to make sure we remain connected
  if (! mqtt.ping(3)) {
    // reconnect to adafruit io
    if (! mqtt.connected())
      connect();
  }
}

void playSound() {
  for (int i = 0 ; i < 5 ; i++) {
    tone(SPEAKER_PIN, notes[i]);
    delay(100);
  }
  tone(SPEAKER_PIN, LOW);
  //SPEAKER_ALERT = true;
}
void blynkNotifyEmail() {
  capacityAlert();
  smellAlert();
}
void capacityAlert() {
  if(cmValue_data == 0){
    Capacity_Alert = 0;
  }
  if ((cmValue_data > bin[0]) && (Capacity_Alert == 0)) {
    Capacity_Alert++;
    Serial.println("bin[0]");
    Blynk.notify("ESP8266 Alert - bin is third full!");
    Blynk.email(USER_EMAIL, "SmartSmell Alert", "Bin is third full !");
  }
  else if ((cmValue_data > bin[1]) && (Capacity_Alert == 1)) {
    Capacity_Alert++;
    Serial.println("bin[1]");
    Blynk.notify("ESP8266 Alert - bin is third full!");
    Blynk.email(USER_EMAIL, "SmartSmell Alert", "Bin halfway!");
  }
  else if ((cmValue_data > bin[2] - 5) && (Capacity_Alert == 2)) {
    Capacity_Alert++;
    Serial.println("bin[2]");
    playSound();
    Blynk.notify("ESP8266 Alert - bin is third full!");
    Blynk.email(USER_EMAIL, "SmartSmell Alert", "Bin OverLoad! you should throw the bag");
  }
  else {
    delay(100);
  }
}

void smellAlert() {
  if(gasValue_data <= bin_smell + 50){
    Smell_Alert = 0;
  }
  if ((gasValue_data > smell[0]) && (Smell_Alert == 0)) {
    Smell_Alert++;
    Serial.println("smell[0]");
    Blynk.notify("ESP8266 Alert - Bin is little bit smelly!");
    Blynk.email(USER_EMAIL, "SmartSmell Alert", "Bin is little bit smelly!");

  }
  else if ((gasValue_data > smell[1]) && (Smell_Alert == 1)) {
    Smell_Alert++;
    Serial.println("smell[1]");
    playSound();
    Blynk.notify("ESP8266 Alert - Bin is stink!");
    Blynk.email(USER_EMAIL, "SmartSmell Alert", "Bin is stink!");
  }
  else if ((gasValue_data > smell[2]) && (Smell_Alert == 2)) {
    Smell_Alert++;
    Serial.println("smell[2]");
    playSound();
    Blynk.notify("ESP8266 Alert - Bin is disgusting! you should throw the bag");
    Blynk.email(USER_EMAIL, "SmartSmell Alert", "Bin is disgusting! you should throw the bag");
    
  }
  else {
    delay(100);
  }
}

void publishData() {
  if ((prvCm != cmValue_data) || (prvGas != gasValue_data)) {
    if (! capacity.publish(cmValue_data))
      Serial.println("Failed to publish capacity");
    else
      Serial.println("capacity published!");

    if (! Gas.publish(gasValue_data))
      Serial.println("Failed to publish gas");
    else
      Serial.println("gas published!");
  }
  if (SPEAKER_ALERT == true) {
    if (! Speaker.publish(SPEAKER_COUNTER))
      Serial.println("Speaker_failed");
    else
      Serial.println("Speaker published!");
  }
  delay(10000);
}

void connect() {
  Serial.print("Connecting to Adafruit IO... ");
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println("Wrong protocol"); break;
      case 2: Serial.println("ID rejected"); break;
      case 3: Serial.println("Server unavail"); break;
      case 4: Serial.println("Bad user/pass"); break;
      case 5: Serial.println("Not authed"); break;
      case 6: Serial.println("Failed to subscribe"); break;
      default: Serial.println("Connection failed"); break;
    }
    if (ret >= 0) {
      mqtt.disconnect();
    }
    Serial.println("Retrying connection...");
    delay(5000);
  }
  Serial.println("Adafruit IO Connected!");
}

int ping_cm() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, LOW);
  int duration = pulseIn(ECHO_PIN, HIGH, MAX_DISTANCE * 59);
  return duration / 59;
}

int smelly() {
  yield();
  return analogRead(AOUTpin); //reads the analaog value from the alcohol sensor's AOUT pin
}

void printDustBinStatus() {
  Serial.print("CM: ");
  Serial.println(cmValue_data, DEC);
  Serial.print("Gas: ");
  Serial.println(gasValue_data, DEC);
}

BLYNK_WRITE(V0) {
  SPEAKER_COUNTER = SPEAKER_COUNTER + 0.5;
  playSound();
}
