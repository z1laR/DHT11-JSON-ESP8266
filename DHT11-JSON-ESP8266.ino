#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "IZZI-2104";
const char* password = "509551F22104";

#define DHTTYPE DHT11
#define DHTPIN D2

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);

  dht.begin();
  delay(2000);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Conectado inalambricamente");  
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  higrometro();
}

int higrometro()
{
  int h = dht.readHumidity();
  int t = dht.readTemperature();

  Serial.println("");
  Serial.println("Temperatura: ");
  Serial.print(t);
  Serial.println(" ° C");
  Serial.println("Humedad: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.println("");

  postHigrometro(t, h);

  delay(2000);
}

void postHigrometro(int t, int h)
{
  String server = "http://192.168.0.11:3000/api/temperatura/";

  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& JSONencoder = jsonBuffer.createObject();
  JSONencoder["temperatura"] = t;
  JSONencoder["humedad"] = h;

  char JSONmessageBuffer[256];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Estructura JSON:");
  Serial.println(JSONmessageBuffer);
  Serial.println("");
  
  HTTPClient http;
  http.begin(server);
  Serial.println("Insertando en -> " +server);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(JSONmessageBuffer);
  String payload = http.getString();
  
  Serial.println("");
  Serial.println("Repuesta del servidor:");
  Serial.println("");
  Serial.println(httpCode);
  Serial.println("");
  Serial.println("Respuesta MongoDB -> " +payload);

  http.end();
}
