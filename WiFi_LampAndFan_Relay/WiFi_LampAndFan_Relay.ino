/*
Authors : Moch. Febryandana Nurfahri
Last Updates : 05 July 2020

This program aims to automate turning on/off 2 devices (lamp and fan, in this case) using WiFi and HTML page (without switch)
I use NodeMCU 1.0 (ESP-12E Module) and 2 channel 5V relay module
Feel free to use, modify, and distribute on your own
*/

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
 
const char* ssid = "WifiAnakBawah";     // Change this to your WiFI SSID
const char* password = "wifibawahh";    // Change this to your WiFI Password
const long utcOffsetInSeconds = 25200;  // UTC Offset in seconds (i.e : +07:00 * 3600 = 25200)

int lamp = D2;  // IO Port for lamp on microcontroller
int fan = D1;   // IO Port for fan on microcontroller

IPAddress IP(192,168,1,100);      // IP Address for microcontroller
IPAddress NETMASK(255,255,255,0);
IPAddress NETWORK(192,168,1,1);
IPAddress DNS(192,168,1,1);
WiFiServer server(80);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
 
void setup() {
  Serial.begin(115200);
  timeClient.begin();
 
  pinMode(lamp, OUTPUT);
  pinMode(fan, OUTPUT);
  digitalWrite(lamp, LOW);
  digitalWrite(fan, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.config(IP, NETWORK, NETMASK, DNS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
 
void loop() {
  timeClient.update();
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  delay(1000);
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  int value1 = LOW;
  int value2 = LOW;
  
  // Set lamp according to the request
  if (request.indexOf("/LAMP=ON") != -1)  {
    digitalWrite(lamp, HIGH);
    value1 = HIGH;
  }
  if (request.indexOf("/LAMP=OFF") != -1)  {
    digitalWrite(lamp, LOW);
    value1 = LOW;
  }
  if (request.indexOf("/FAN=ON") != -1) {
    digitalWrite(fan, HIGH);
    value2 = HIGH;
  }
  if (request.indexOf("/FAN=OFF") != -1) {
    digitalWrite(fan, LOW);
    value2 = LOW;
  }

  // Mode alarm for lamp -- Experimental
  if(timeClient.getHours()== 5){
    digitalWrite(lamp, HIGH);
    value1 = HIGH;
    delay(3600000UL);
    digitalWrite(lamp, LOW);
    value1 = LOW;
  }
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // do not forget this one
  
  // Html code
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  
  client.print("Lamp pin is now: ");
  if(value1 == HIGH) {
    client.print("ON");
  } else {
    client.print("OFF");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LAMP=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LAMP=OFF\"\"><button>Turn Off </button></a><br />");
  client.println("<br><br><br>");
  
  client.print("Fan pin is now: ");
  if(value2 == HIGH) {
    client.print("ON");
  } else {
    client.print("OFF");
  }
  client.println("<br><br>");
  client.println("<a href=\"/FAN=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/FAN=OFF\"\"><button>Turn Off </button></a><br />");
  
  client.println("</html>");
  delay(1);

  // When client disconnected
  Serial.println("Client disonnected");
  Serial.println("");

}
