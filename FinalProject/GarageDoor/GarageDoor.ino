
/* Elick Coval
 * IoT @ UML 2020
 */

/* RFID Header files from https://github.com/miguelbalboa/rfid */
#include <SPI.h>
#include <MFRC522.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* WiFi/MQTT parameters */
#define WLAN_SSID       "FBIVan"
#define WLAN_PASS       "ThatsWhatSheSSID"
#define BROKER_IP       "192.168.1.142"


#define SS_PIN D2
#define RST_PIN D1
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

WiFiClient client;
PubSubClient mqttclient(client);

int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;


void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
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
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Scan RFID to see UID...");
  Serial.println("");
}
/* -------------------------------------------------------------------- */
void callback (char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
  Serial.write(payload, length); //print incoming messages
  Serial.println("");


  payload[length] = '\0'; // add null terminator to byte payload so we can treat it as a string

//  if (strcmp(topic, "/dogDoor") == 0){
//     if (strcmp((char *)payload, "open") == 0){
//        mqttclient.publish("/dogDoor","***Open Door****");
//     } else if (strcmp((char *)payload, "close") == 0){
//        mqttclient.publish("/dogDoor","***close Door****");
//     }
//  }
//  
}


/* Main loop - if ID scanned matches a specific tag, open the door*/
void loop() {
  if (!mqttclient.connected()) {
    connect();
  }
  mqttclient.loop();
  readsuccess = getid();
 
  if(readsuccess) {
    Serial.println(StrUID);
    Serial.println("");   
    if(StrUID == "499EEDC1"){
      mqttclient.publish("/dogDoor","activateDoor");
    }
  }
}


/* If ID is read, store it in the string StrUID */
int getid() {  
  if(!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
 
  Serial.println("THE UID OF THE SCANNED CARD IS:");
  
  for(int i=0;i<4;i++){
    readcard[i]=mfrc522.uid.uidByte[i]; //storing the UID of the tag in readcard
    array_to_string(readcard, 4, str);
    StrUID = str;
  }

  mfrc522.PICC_HaltA();
  return 1;
}


/* For displaying tag ID as readable chars */

void array_to_string(byte array[], unsigned int len, char buffer[]) {
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}


/* Setup Wifi and MQTT */

void connect() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("Wifi issue"));
    delay(3000);
  }
  Serial.print(F("Connecting to MQTT server... "));
  while(!mqttclient.connected()) {
    if (mqttclient.connect(WiFi.macAddress().c_str())) {
      Serial.println(F("MQTT server Connected!"));

       mqttclient.subscribe("/dogDoor");
       
    } else {
      Serial.print(F("MQTT server connection failed! rc="));
      Serial.print(mqttclient.state());
      Serial.println("try again in 10 seconds");
      // Wait 5 seconds before retrying
      delay(20000);
    }
  }
}
