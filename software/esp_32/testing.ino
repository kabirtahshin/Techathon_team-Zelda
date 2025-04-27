#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
const char *ssid = "ASL";
const char *password = "++++++++";

// Your Django API endpoint for placing orders
const char* serverName = "https://bistro-92.onrender.com/api/orders/";

// Secure client
WiFiClientSecure client;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");

  client.setInsecure(); // Ignore SSL certificate for now (testing)

  // Only once: Send the order
  sendOrder();
}

void loop() {
  // Nothing for now
}

void sendOrder() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient https;

    https.begin(client, serverName);  // Pass WiFiClientSecure
    https.addHeader("Content-Type", "application/json");

    // Create the order JSON
    DynamicJsonDocument doc(512);

    doc["table_number"] = 1; // Example: table 5

    JsonArray items = doc.createNestedArray("items");

    JsonObject item1 = items.createNestedObject();
    item1["item_id"] = 1;
    item1["quantity"] = 2;

    JsonObject item2 = items.createNestedObject();
    item2["item_id"] = 2;
    item2["quantity"] = 1;

    String requestBody;
    serializeJson(doc, requestBody);

    // Post the request
    int httpResponseCode = https.POST(requestBody);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String response = https.getString();
      Serial.println("Server response:");
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    https.end(); // Close connection
  } else {
    Serial.println("WiFi Disconnected");
  }
}
