// Black Clover episoder udkommer hver tirsdag kl. 13:25
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
 
/* Set these to your desired credentials. */
const char *ssid = "samuels"; //Enter your WIFI ssid
const char *password = "12341234"; //Enter your WIFI password

const String host = "kitsu.io";
const String url = "/api/edge/anime/13209";

const size_t capacity = JSON_ARRAY_SIZE(0) + 20*JSON_OBJECT_SIZE(1) + 23*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(16) + JSON_OBJECT_SIZE(19) + JSON_OBJECT_SIZE(31) + 5140;
DynamicJsonDocument doc(capacity);

int episodes = 153;
int episodesNew = 154;
int difference;

int led = D2;


void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(led, OUTPUT);
}

void loop() {

  digitalWrite(led, HIGH);
    // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status()!= WL_CONNECTED) {delay(500); Serial.print(".");}
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  client.setInsecure(); //eliminate the need for at certificate fingerprint... but allows an attacker to impersonate the webserver...
  const int httpsPort = 443;
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.println("GET " + url + " HTTP/1.1"); 
  client.println("Host: " + host);
  client.println("user-agent: curl/7.68.0");
  client.println("accept: */*");
  //client.println("Connection: close");
  client.println(); //end of header
  
  delay(1000); //wait a while to let the server respond (increase on shitty connections)

  // Read all the lines of the reply from server
  String line;
  while(client.available()){

    yield();
    String line = client.readStringUntil('\n'); //newline return as delimiter
    // Serial.println(line);
    
    if(line.startsWith("{\"data\":")) {
      deserializeJson(doc, line); //if the current line is the json string we want, parse it to make a json object.    
      Serial.println("found json!");
      Serial.println("\n");
      const char* anime = doc["data"]["attributes"]["titles"]["en_jp"]; // working
      episodesNew = doc["data"]["attributes"]["episodeCount"];
      Serial.println("Anime: " + String(anime));
      episodesNew = 154;
      Serial.println("Episode Count Before: " + String(episodes));
      Serial.println("Episode Count After: " + String(episodesNew));

      difference = episodesNew - episodes;
      
      if(episodes > episodesNew){
        Serial.println("Any new episodes?: No");
      }
      if(episodes < episodesNew){
        Serial.println("Any new episodes?: Yes, " + String(difference) + ".");
        for(int i = 0; i <= 10; i++){
          digitalWrite(led, HIGH);
          delay(200);
          digitalWrite(led, LOW);
          delay(200);
        }
      }
    }
  }
  digitalWrite(led, HIGH);
  delay(15000);
}
