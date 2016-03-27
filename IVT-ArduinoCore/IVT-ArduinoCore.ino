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
  Serial.println(payload);
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

  StaticJsonBuffer<800> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  root["framledn"] = ivt[1];
  root["ute"] = ivt[2];
  root["tappvarmv"] = ivt[3];
  root["varmev1"] = ivt[4];
  root["varmev2"] = ivt[5];
  root["inne"] = ivt[6];
  root["hetgas"] = ivt[7];
  root["egtemp"] = ivt[8];
  root["tryckvakt"] = ivt[9];
  root["hogtryck"] = ivt[10];
  root["lagtryck"] = ivt[11];
  root["egsemester"] = ivt[12];
  root["kompressor"] = ivt[13];
  root["schuntoppen"] = ivt[14];
  root["schuntstangd"] = ivt[15];
  root["cirkpump"] = ivt[16];
  //root["18"] = ivt[17];
  root["larm"] = ivt[18];
  //root["20"] = ivt[19];
  //root["21"] = ivt[20];
  //root["22"] = ivt[21];
  root["bvframledning"] = ivt[22];
  //root["24"] = ivt[23];
  //root["25"] = ivt[24];
  //root["26"] = ivt[25];
  //root["27"] = ivt[26];
  //root["28"] = ivt[27];
  //root["29"] = ivt[28];
  root["extravv1"] = ivt[29];
  root["extravv2"] = ivt[30];
  root["extravv3"] = ivt[31];
  //root["33"] = ivt[32];  
  root["elpatron"] = ivt[33];
  //root["35"] = ivt[34];
  root["extravv4"] = ivt[35];
  //root["37"] = ivt[36];
  
  //root.prettyPrintTo(Serial);

  char buffer[800];
  root.printTo(buffer, sizeof(buffer));

  memset(ivt, 0, sizeof ivt);
  stringComplete = false;

  Serial.println(buffer);
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

