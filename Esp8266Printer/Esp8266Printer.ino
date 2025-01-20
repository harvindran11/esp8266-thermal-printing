#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "XXXXXXXXX";
const char* password = "XXXXXXX";

// Create an instance of the web server on port 80
ESP8266WebServer server(80);
IPAddress printerAddr;
WiFiClient printer;
unsigned long lastReconnectAttempt = 0; // Time of the last WiFi reconnection attempt
const unsigned long reconnectInterval = 5000; // Interval for WiFi reconnection attempts in milliseconds


void setup() 
{
  Serial.begin(115200);

  connectToWiFi();


  // Start the server
  server.on("/postdata", HTTP_POST, handlePostRequest);
  server.begin();
}


void loop() 
{
  server.handleClient();
    // Check Wi-Fi connection
  checkWiFiConnection();
}

// Function to connect to Wi-Fi
void connectToWiFi() 
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Get the gateway IP
  IPAddress gateway = WiFi.gatewayIP();

  // Set a static IP within the same subnet
  IPAddress local_IP(gateway[0], gateway[1], gateway[2], 212); // Replace 212 with desired static IP
  IPAddress subnet(255, 255, 255, 0);
  if (!WiFi.config(local_IP, gateway, subnet)) 
  {
    Serial.println("Failed to configure static IP");
  }

  // Print the IP details
  Serial.print("Static IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway IP: ");
  Serial.println(gateway);
}

// Function to check Wi-Fi connectivity and reconnect if necessary
void checkWiFiConnection() 
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    // If Wi-Fi is disconnected, attempt to reconnect
    if (millis() - lastReconnectAttempt >= reconnectInterval) 
    {
      Serial.println("WiFi lost, attempting to reconnect...");
      connectToWiFi(); // Attempt to reconnect
      lastReconnectAttempt = millis(); // Update last reconnect attempt time
    }
  }
}

// This function will be called when a POST request is made to "/postdata"
void handlePostRequest() 
{
  String message = "Received data: ";
  
  // Check if the POST request has JSON data
  if (server.hasArg("plain")) 
  {
    String jsonString = server.arg("plain"); // Get the raw JSON string
    int ln = jsonString.length() + 10;
    DynamicJsonDocument doc(ln); // Create a static JSON document with a size of 256 bytes

    // Deserialize the JSON data
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) 
    {
      // If the JSON is invalid, send an error response
      message = "Failed to parse JSON";
      Serial.println(message);
      server.send(400, "text/plain", message);
      return;
    }

    // Extract the values from the JSON
    const char* printerIP = doc["printerIP"];  // Extract printerip
    const char* printerData = doc["printData"];  // Extract printerdata

    sendToPrinter(printerIP, printerData);

    // Prepare the message to send back to the client
    message = "Request received";
  } 
  else 
  {
    message = "No data received";
  }

  // Send the response back to the client
  server.send(200, "text/plain", message);
}

void sendToPrinter(String printerIP, String receipt) 
{
  if (!printerAddr.fromString(printerIP)) 
  {
    Serial.println(F("Invalid printer IP address."));
    return;
  }

  if (printer.connect(printerAddr, 9100)) 
  {
    Serial.println("Connected to printer at " + printerIP);

  for (int i = 0; receipt[i] != '\0'; i++) 
  {
    
      if (receipt[i] == '\r') 
      {
          continue; // Skip the rest of this loop iteration
      } 
      else if (receipt[i] == '\n') 
      {
        printer.write((uint8_t)'\n');
      } 
      else 
      {
        // Send normal character
        printer.write(receipt[i]);
        if (i % 50 == 0) delay(10);
      }
  }
  // Feed paper and cut
  printer.write((uint8_t)0x1B); // ESC
  printer.write((uint8_t)'d');  // Feed paper
  printer.write((uint8_t)5);    // Feed 5 lines

  printer.write((uint8_t)0x1B); // ESC
  printer.write((uint8_t)'m');  // Cut paper
    printer.flush();
    printer.stop();
  } 
  else 
  {
    Serial.println(F("Failed to connect to the printer."));
  }
}


