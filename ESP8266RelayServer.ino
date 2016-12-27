#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "<your ssid>";
const char* password = "<your wifi password>";

ESP8266WebServer server(80);

boolean deviceStates[] = { false, false, false, false };  // Initial state of each relay
int relays[] = { 14, 12, 13, 16 };  // GPIO pins for each relay

void setup(void){

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.print("Connected to ");
  Serial.println(ssid);
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("MAC address: ");
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.println(getMacString(mac));
  
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  
  Serial.println("Initialize relays...");
  for (byte relay = 0; relay < sizeof(relays) / sizeof(int); relay++)  {
    
    pinMode(relays[relay], OUTPUT);
    digitalWrite(relays[relay], deviceStates[relay] ? HIGH : LOW);

    String request = "/relay/" + String(relay) + "/";
    Serial.println("Relay " + String(relay) + " url: " + request);
    server.on((request + "on").c_str(), [relay](){
      setState(relay, true);
    });
    server.on((request + "off").c_str(), [relay](){
      setState(relay, false);
    });
  }
  server.on("/relay/all/on", [](){
    setAll(true);
  });

  server.on("/relay/all/off", [](){
    setAll(false);
  });

  server.onNotFound(handleNotFound);
  server.begin();
  
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}

// This method is called when a request is made to the root of the server. i.e. http://myserver
void handleRoot() {
  String response = "{ \"relays\": [ ";
  String seperator = "";
  for (int relay = 0; relay < sizeof(relays) / sizeof(int); relay++)  {
    response += seperator + String(deviceStates[relay]);
    seperator = ", ";
  }
  response += " ] }";
  server.send(200, "application/json", response);
}

// This method is called when an undefined url is specified by the caller
void handleNotFound(){
  server.send(400, "text/plain", "{ \"message\": \"Invalid request\"");
}

void setAll(boolean state){
  for (byte relay = 0; relay < sizeof(relays) / sizeof(int); relay++)  {
    digitalWrite(relays[relay], state ? HIGH : LOW);
    deviceStates[relay] = state;
  }
  String response = "All Relays = " + String(state ? "ON" : "OFF");
  server.send(200, "application/json", "{ \"message\": \"" + response + "\" }");
}

void setState(int relay, boolean state) {
    digitalWrite(relays[relay], state ? HIGH : LOW);
    deviceStates[relay] = state;
    String response = "Relay " + String(relay) + " " + (state ? "ON" : "OFF");
    server.send(200, "application/json", "{ \"message\": \"" + response + "\" }");
    Serial.println(response);
}

String getMacString(byte *mac) {
  String result = "";
  String seperator = "";
  for(int b = 0; b < 6; b++) {
    result += seperator + String(mac[b], HEX);
    seperator = ":";
  }
  return result;
}
