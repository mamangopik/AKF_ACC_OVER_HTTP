#include <WiFi.h>
#include <HTTPClient.h>
#include <MQTT.h>

#define OVER_MQTT 0
#define OVER_HTTP 1

WiFiClient net;
MQTTClient client(512, 512);

uint32_t ch = OVER_HTTP;
uint32_t DATA_SIZE = 128;
// Replace with your network credentials
void over_http();
void over_mqtt();
void connect_mqtt();
String jsonify(uint32_t id);

uint32_t data_id = 0;
const char *ssid = "esp8266Tester-1";
const char *password = "makanbang";

// Replace with your server URL
const char *serverName = "http://192.168.15.70:7777/api/over_http";
const char *mqtt_broker = "192.168.15.70";

String jsonify(uint32_t id)
{
  String json_data = "{";
  json_data += "\"x_values\":[";
  for (int i = 0; i < DATA_SIZE; i++)
  {
    json_data += String(random(-2000, 2000));
    if (i != DATA_SIZE - 1)
    {
      json_data += ",";
    }
  }
  json_data += "],\"y_values\":[";
  for (int i = 0; i < DATA_SIZE; i++)
  {
    json_data += String(random(-2000, 2000));
    if (i != DATA_SIZE - 1)
    {
      json_data += ",";
    }
  }
  json_data += "],\"z_values\":[";
  for (int i = 0; i < DATA_SIZE; i++)
  {
    json_data += String(random(-2000, 2000));
    if (i != DATA_SIZE - 1)
    {
      json_data += ",";
    }
  }
  json_data += "],\"xkf_values\":[";
  for (int i = 0; i < DATA_SIZE; i++)
  {
    json_data += String((random(-2000, 2000) + 0.1));
    if (i != DATA_SIZE - 1)
    {
      json_data += ",";
    }
  }
  json_data += "],\"ykf_values\":[";
  for (int i = 0; i < DATA_SIZE; i++)
  {
    json_data += String((random(-2000, 2000) + 0.1));
    if (i != DATA_SIZE - 1)
    {
      json_data += ",";
    }
  }
  json_data += "],\"zkf_values\":[";
  for (int i = 0; i < DATA_SIZE; i++)
  {
    json_data += String((random(-2000, 2000) + 0.1));
    if (i != DATA_SIZE - 1)
    {
      json_data += ",";
    }
  }
  json_data += "],";
  if (ch == OVER_HTTP)
  {
    json_data += "\"protocol\": \"HTTP\",";
  }
  if (ch == OVER_MQTT)
  {
    json_data += "\"protocol\": \"MQTT\",";
  }
  json_data += "\"sensor_type\": \"accelerometer\",";
  json_data += "\"id_data\":" + String(id);
  json_data += ",\"RSSI\":" + String(WiFi.RSSI());
  json_data += ",\"sampling_frequency\": 200,\"packet_size\": 128,\"hw_unix_time\": 1713472258,\"hw_time\": \"2024-4-18/20:30:58\"}";

  return json_data;
}

void connect_mqtt()
{
  while (!client.connect(String(WiFi.macAddress()).c_str(), "public", "public"))
  {
    Serial.print("connecting MQTT...");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  vTaskDelay(100 / portTICK_PERIOD_MS);
}

void over_http()
{
  HTTPClient http;

  // Specify request destination
  http.begin(serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/json");

  // Prepare JSON payload
  // Send HTTP POST request
  int httpResponseCode = http.POST(jsonify(data_id));

  // Check the response
  if (httpResponseCode > 0)
  {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
    digitalWrite(2, !digitalRead(2));
  }
  else
  {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }
  // Free resources
  http.end();
}
void over_mqtt()
{
  if (!client.connected())
  {
    connect_mqtt();
  }
  String payload = jsonify(data_id);
  if (client.publish("/test/performance", payload.c_str(), payload.length(), false, 0))
  {
    digitalWrite(2, !digitalRead(2));
  }
}
void setup()
{
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  for (int i = 0; i < 20; i++)
  {
    digitalWrite(2, !digitalRead(2));
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(2, !digitalRead(2));
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");
  if (ch == OVER_MQTT)
  {
    const int mqtt_port = 1883;
    client.begin(mqtt_broker, mqtt_port, net);
  }
}

void loop()
{
  // Send HTTP POST request
  if (WiFi.status() == WL_CONNECTED)
  {
    if (ch == OVER_HTTP)
    {
      over_http();
    }
    if (ch == OVER_MQTT)
    {
      over_mqtt();
    }
    data_id++;
    if (data_id >= 1000)
    {
      data_id = 0;
    }
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
  vTaskDelay(10 / portTICK_PERIOD_MS);
}