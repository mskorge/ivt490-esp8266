/*
 *  IVT 490 serial to MQTT
 *  Michael Skorge
 */

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

//WIFI settings
const char* ssid = "MyWIFI";
const char* password = "secret";

String inputString;
boolean stringComplete = false;
char inChar;
int counter;
int commaPosition;
float ivt[50];

//IP address of MQTT server
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
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT server");
      //Publish message on connect
      //client.publish("outTopic", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
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
  
  root["framledn"] = ivt[1]; //Framledning
  root["ute"] = ivt[2]; //Ute
  root["tappvarmvtopp"] = ivt[3]; //Tappvarmvatten Topp
  root["varmevmitt"] = ivt[4]; //Varmvatten Mitt
  root["varmevbotton"] = ivt[5]; //Värmevatten Botten
  root["inne"] = ivt[6]; //Rumstemp
  root["hetgas"] = ivt[7]; //Hetgas
  root["egtemp"] = ivt[8]; //Extra Acc-Tank
  root["tryckvakt"] = ivt[9]; //Tryckvakt
  root["hogtryck"] = ivt[10]; //Högtryck
  root["lagtryck"] = ivt[11]; //Lågtryck
  root["egsemester"] = ivt[12]; //Semester aktiv?
  root["kompressor"] = ivt[13]; //Kompressor aktiv
  root["schuntoppen"] = ivt[14]; //SV1 Öppna
  root["schuntstangd"] = ivt[15]; //SV1 Stäng
  root["cirkpump"] = ivt[16]; //P1 Rad
  //root["flakt"] = ivt[17]; //Fläkt
  root["larm"] = ivt[18]; //Larm aktiv
  //root["20"] = ivt[19]; //Extern P2
  //root["21"] = ivt[20]; //LLT GT1
  //root["22"] = ivt[21]; //LL GT1
  root["bvframledning"] = ivt[22]; //BV GT1
  //root["24"] = ivt[23]; //UL GT1
  //root["25"] = ivt[24]; //LL GT3:2
  //root["26"] = ivt[25]; //ULT GT3:2
  //root["27"] = ivt[26]; //UL GT3:2
  //root["28"] = ivt[27]; //LL GT3:3
  //root["29"] = ivt[28]; //BV GT3:3
  root["extravv1"] = ivt[29]; //SV3 BV Förskj
  root["extravv2"] = ivt[30]; //Effekt ink vit VV behov
  root["extravv3"] = ivt[31]; //Tillskotstimer VV behov
  //root["33"] = ivt[32];   //Tappv prio
  root["elpatron"] = ivt[33]; //Tillskott i %/10
  //root["35"] = ivt[34]; //Tillskott RAD
  root["extravv4"] = ivt[35]; //Tillskott Tillägg
  //root["37"] = ivt[36]; //Default SV2 Open

  //Debug to serial terminal
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

