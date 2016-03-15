/*
 *  IVT 490 serial to MQTT
 *  Michael Skorge
 */

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

const char* ssid = "MyWIFI";
const char* password = "secret";
String inputString;         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
char inChar;
int counter;
int commaPosition;
float ivt[50];
IPAddress MQTTserver(192, 168, 2, 191);
WiFiClient wclient;
PubSubClient client(MQTTserver, 1883, wclient);


SoftwareSerial swSer(14, 12, false, 128);

float stringToFloat(String input){
  String stringTemp = input;
  char stfarray[stringTemp.length() + 1];
  stringTemp.toCharArray(stfarray, sizeof(stfarray));
  float stf = atof(stfarray);
  stringTemp = "";
  memset(stfarray, 0, sizeof stfarray);
  return stf; 
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup() {
    Serial.begin(115200);
    swSer.begin(9600);
    delay(10);
    
    setup_wifi();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void mqttCommand(char* payload) {
  Serial.println("Publishing command");
  client.publish("/ivt/output", payload);
  }

void fetchSerial ()
{
  while(swSer.available() > 0)
  {
    inChar = swSer.read();
    if(inChar != 32){
      inputString += inChar;
    }
    
    delay(2);
  }
  inChar = '0';
  Serial.println(inputString);
  stringComplete = true;
}

void splitString ()
{ 
  commaPosition = inputString.indexOf(';');
  while(commaPosition >= 0)
  {
    commaPosition = inputString.indexOf(';');
    if(commaPosition != -1)
    {
      ivt[counter] = stringToFloat(inputString.substring(0,commaPosition));
      inputString = inputString.substring(commaPosition+1, inputString.length());    
    }
    else
    {
      if(inputString.length() > 0){
        ivt[counter] = stringToFloat(inputString.substring(0,commaPosition));
      }
    }
    counter++;
  }
  counter = 0;
  inputString = "";
  commaPosition = 0;

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["out"] = ivt[2];
  root["in"] = ivt[6];
  root["gt1"] = ivt[1];
  root["gt31"] = ivt[3];
  root["el"] = ivt[33];
  root["comp"] = ivt[13];
  root.prettyPrintTo(Serial);

  char buffer[256];
  root.printTo(buffer, sizeof(buffer));

  memset(ivt, 0, sizeof ivt);
  stringComplete = false;

  mqttCommand(buffer);
  
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if(swSer.available() > 0)
  {
    fetchSerial();
  }

  if(stringComplete == true){
    delay(10);
    splitString();
    delay(50);
  }
  
}

