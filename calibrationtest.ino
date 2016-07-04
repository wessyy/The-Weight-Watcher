#include <ESP8266WiFi.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "RIC";

/////////////////////
// Pin Definitions //
/////////////////////
const int Rightsidepin = 0;
const int Leftsidepin = 4;
const int LED_PIN = 5; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

WiFiServer server(80);
void initHardware();
void setupWiFi();

void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
  pinMode(4, OUTPUT);
  pinMode(0, OUTPUT);
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/led/0") != -1)
    val = 0; // Will write LED low
  else if (req.indexOf("/led/1") != -1)
    val = 1; // Will write LED high
  else if (req.indexOf("/read") != -1)
    val = -2; // Will print pin reads
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set GPIO5 according to the request
  if (val >= 0)
    digitalWrite(LED_PIN, val);

  client.flush();

  digitalWrite(Leftsidepin, HIGH);
  int voltageLeft = analogRead(ANALOG_PIN);
  digitalWrite(Leftsidepin, LOW);
  
  digitalWrite(Rightsidepin, HIGH);
  int voltageRight = analogRead(ANALOG_PIN);
  digitalWrite(Rightsidepin, LOW);
  
 
  
  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<head>\r\n<style>\r\n#total{\r\nfont-size:1250%;\r\npadding-top:20%;\r\npadding-left:7%;\r\n}\r\n#totalUnits{\r\nfont-size:20%;\r\n}\r\n</style>\r\n<head>\r\n<body>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    s += "LED is now ";
    s += (val)?"on":"off";
  }
  else if (val == -2)
  { // If we're reading pins, print out those values:
    //s += "Right Side (lbs) = ";
    //float voltage = analogRead(ANALOG_PIN) * (1.0 / 1023.0)*292.75-0.5168;
    //s += String(voltage*303.6-1.1243);
//
//    s += "Right Side (lbs) = ";
    int voltageR = (voltageRight * 0.2939 - 0.2402);
//    s += String(voltageR);
//
//    s += "<br>"; // Go to the next line.
//    
//    s += "Left Side (lbs) = ";
    int voltageL = (voltageLeft * 0.2939 - 0.2402);
//    s += String(voltageL);
//
//    s += "<br>"; // Go to the next line.
//
//    s += "Total Weight (lbs) = ";
    s += "<div id=total>"+String(voltageL + voltageR)+"<span id='totalUnits'>lbs</span></div>";
  }
  else
  {
    s += "Invalid Request.<br> Try refreshing page...";
  }
  s += "</body></html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "Weight Watcher WiFi";

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}

