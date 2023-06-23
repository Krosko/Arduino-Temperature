#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#define ONE_WIRE_BUS 5 // Der PIN D1 (GPIO 5) wird als BUS-Pin verwendet

// WiFI login credentials
const char * ssid = "wlan1313";
const char * password = "wlan1313pw";

// initialising Server
WiFiServer server(80);
String header;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temperature; // In dieser Variable wird die Temperatur gespeichert


void setup(){
  Serial.begin(115200);   // Serielle Schnittstelle initialisieren
  DS18B20.begin();       // DS18B20 initialisieren

  // Network connection
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("local IP-Adress: ");
  Serial.println(WiFi.localIP());

  // Fuckig server starten
  server.begin();
}


void loop(){
  // Hier bekommt des ding die Temperatur
  WiFiClient client = server.available();
  
  DS18B20.requestTemperatures();
  temperature = DS18B20.getTempCByIndex(0); // 0 --> erster Sensor am Bus
   // Serial.println(String(temperature) + " °C"); // Ausgabe im seriellen
  // Monitor
  delay(3000); // 3 Sekunden warten

  // HTML shit
  if (client) {
    Serial.println("new client");

    // String with incoming data from client
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        Serial.write(c);
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<html><head><title>Temperature Reading</title>");
            client.println("<link rel=\"stylesheet\" type=\"text/css\" href=\"https://example.com/style.css\">");
            client.println("</head><body>");
            client.println("<h1>Temperatur am Messstab ist: " + String(temperature) + "C " + "</h1>");
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";

    client.stop();
    Serial.println("client closed connection");
    Serial.println();
    }
  }

