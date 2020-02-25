#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi/MQTT parameters
#define WLAN_SSID       "PizzaTheHut"
#define WLAN_PASS       "fbivan"
#define BROKER_IP       "192.168.1.97"

//pins
#define LED 5 

// vars
int lightstate;
char number[50];

WiFiClient client;
PubSubClient mqttclient(client);

void callback (char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
  Serial.write(payload, length); //print incoming messages
  Serial.println("");

  payload[length] = '\0'; // add null terminator to byte payload so we can treat it as a string

  if (strcmp(topic, "/arled") == 0){
     if (strcmp((char *)payload, "on") == 0){
        digitalWrite(LED, HIGH);
     } else if (strcmp((char *)payload, "off") == 0){
        digitalWrite(LED, LOW);
     }
  }
}

void setup() {
  Serial.begin(9600);

   // setup pin as output
  pinMode(LED, OUTPUT);
    
  // connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // connect to mqtt server
  mqttclient.setServer(BROKER_IP, 1883);
  mqttclient.setCallback(callback);
  connect();

}

void loop() {
  if (!mqttclient.connected()) {
    connect();
  }
  mqttclient.loop();

  //vars to keep track of time
  static const unsigned long REFRESH_INTERVAL = 1000; // ms
  static unsigned long lastRefreshTime = 0;


  //if time between now and last update is more than time interval
  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime += REFRESH_INTERVAL;
    lightstate = analogRead(A0);
    sprintf(number, "%d", lightstate);
    mqttclient.publish("/light",number); // send message
    Serial.println(number);
  }
    
}

void connect() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("Wifi issue"));
    delay(3000);
  }
  Serial.print(F("Connecting to MQTT server... "));
  while(!mqttclient.connected()) {
    if (mqttclient.connect(WiFi.macAddress().c_str())) {
      Serial.println(F("MQTT server Connected!"));

       mqttclient.subscribe("/light");
       mqttclient.subscribe("/arled");
      
    } else {
      Serial.print(F("MQTT server connection failed! rc="));
      Serial.print(mqttclient.state());
      Serial.println("try again in 10 seconds");
      // Wait 5 seconds before retrying
      delay(20000);
    }
  }
}
