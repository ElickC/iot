#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi/MQTT parameters
#define WLAN_SSID       "Hifi"
#define WLAN_PASS       "lanbeforetime"
#define BROKER_IP       "192.168.4.54"

//pins
#define BUTTON 4
#define LED 5 

//vars
bool clickedflag = false;
bool lighton = false;

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

  //setup pins
  pinMode(LED, OUTPUT); // setup pin for input
}

void loop() {
  if (!mqttclient.connected()) {
    connect();
  }
  mqttclient.loop();  

  // read button state
  bool buttonclick = digitalRead(BUTTON);

  // if the button is clicked and it was not clicked previously
  if (buttonclick and not clickedflag and not lighton){

      // turn the pin on 
      mqttclient.publish("/piled","on"); // send message
      
      // set the clicked flag
      clickedflag = true;

      // set light on bool
      lighton = true;

      //sleep to wait for click to be done
      delay(1000);
  }

  if (buttonclick and not clickedflag and lighton){
    
      // turn the pin off 
      mqttclient.publish("/piled","off"); // send message

      // set light off bool
      lighton = false;
      
      //sleep to wait for click to be done
      delay(1000);
  }
  
      // if the button is not clicked now and it was clicked previously (a commplete click has now happened)
      if (not buttonclick and clickedflag){ 

      // set the clicked flag
      clickedflag = false;
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
