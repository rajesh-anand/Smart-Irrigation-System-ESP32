#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

const char* ssid = "";
const char* password = "";

String apiKey = "";
String city = "";

#define SOIL_PIN 34
#define RAIN_PIN 35
#define RELAY_PIN 26
#define DHT_PIN 4

#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

WebServer server(80);

float temp = 0, hum = 0;
int soil = 0, rain = 0;

float apiTemp = 0, apiHum = 0;
bool apiRain = false;

bool pump = false;
bool autoMode = true;

bool wifiOK = false;
bool apiOK = false;

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < 20) {
    delay(500);
    Serial.print(".");
    i++;
  }

  wifiOK = (WiFi.status() == WL_CONNECTED);

  if (wifiOK) {
    Serial.println("\nConnected: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nOffline Mode");
  }
}

void fetchWeather() {
  if (!wifiOK) return;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  String url = "https://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";

  if (https.begin(client, url)) {
    int code = https.GET();

    if (code == 200) {
      String payload = https.getString();

      DynamicJsonDocument doc(2048);
      if (!deserializeJson(doc, payload)) {
        apiTemp = doc["main"]["temp"];
        apiHum  = doc["main"]["humidity"];
        String w = doc["weather"][0]["main"];
        apiRain = (w == "Rain" || w == "Drizzle");
        apiOK = true;
      } else {
        apiOK = false;
      }
    } else {
      apiOK = false;
    }

    https.end();
  }
}

void readSensors() {
  soil = analogRead(SOIL_PIN);
  rain = analogRead(RAIN_PIN);

  temp = dht.readTemperature();
  hum  = dht.readHumidity();

  if (isnan(temp)) temp = 0;
  if (isnan(hum)) hum = 0;
}

void controlPump() {
  bool dry = soil > 2500;
  bool raining = rain < 2000;

  if (autoMode) {
    if (wifiOK && apiOK)
      pump = (dry && !raining && !apiRain);
    else
      pump = (dry && !raining);
  }

  digitalWrite(RELAY_PIN, pump ? LOW : HIGH);
}

String webpage() {

  String soilStatus = (soil > 2500) ? "DRY" : "WET";
  String rainNow = (rain < 2000) ? "YES" : "NO";

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="refresh" content="5">
<title>Smart Irrigation</title>

<style>
body {
  background:#0f172a;
  color:white;
  font-family:sans-serif;
  text-align:center;
}

h1 { color:#38bdf8; }

.container {
  display:flex;
  justify-content:center;
  flex-wrap:wrap;
  gap:20px;
  margin-top:20px;
}

.card {
  background:#1e293b;
  padding:20px;
  width:250px;
  border-radius:15px;
  box-shadow:0 0 20px rgba(56,189,248,0.2);
}

.val {
  font-size:20px;
  color:#22c55e;
  margin:8px;
}

button {
  padding:10px;
  margin:5px;
  border:none;
  border-radius:8px;
  background:#38bdf8;
  font-weight:bold;
}
</style>
</head>

<body>

<h1>Smart Irrigation Dashboard</h1>

<div class="container">

<div class="card">
<h2>Weather (API)</h2>
<p class="val">Temp: )rawliteral" + String(apiTemp) + R"rawliteral( °C</p>
<p class="val">Humidity: )rawliteral" + String(apiHum) + R"rawliteral( %</p>
<p class="val">Rain Predicted: )rawliteral" + String(apiRain?"YES":"NO") + R"rawliteral(</p>
</div>

<div class="card">
<h2>Field Status</h2>
<p class="val">Soil: )rawliteral" + soilStatus + R"rawliteral(</p>
<p class="val">Rain Now: )rawliteral" + rainNow + R"rawliteral(</p>
<p class="val">Temp: )rawliteral" + String(temp) + R"rawliteral( °C</p>
</div>

<div class="card">
<h2>Pump</h2>
<p>Mode: )rawliteral" + String(autoMode?"AUTO":"MANUAL") + R"rawliteral(</p>
<p>Status: )rawliteral" + String(pump?"ON":"OFF") + R"rawliteral(</p>

<a href="/auto"><button>Auto</button></a>
<a href="/manual"><button>Manual</button></a>
<a href="/on"><button>ON</button></a>
<a href="/off"><button>OFF</button></a>
</div>

<div class="card">
<h2>Status</h2>
<p>WiFi: )rawliteral" + String(wifiOK?"Online":"Offline") + R"rawliteral(</p>
<p>API: )rawliteral" + String(apiOK?"OK":"Fail") + R"rawliteral(</p>
</div>

</div>

</body>
</html>
)rawliteral";

  return html;
}

void handleRoot(){ server.send(200,"text/html",webpage()); }
void handleAuto(){ autoMode=true; handleRoot(); }
void handleManual(){ autoMode=false; handleRoot(); }
void handleOn(){ if(!autoMode) pump=true; handleRoot(); }
void handleOff(){ if(!autoMode) pump=false; handleRoot(); }

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  dht.begin();

  connectWiFi();

  server.on("/", handleRoot);
  server.on("/auto", handleAuto);
  server.on("/manual", handleManual);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
}

unsigned long lastWeather = 0;

void loop() {
  server.handleClient();

  wifiOK = (WiFi.status() == WL_CONNECTED);

  readSensors();

  if (wifiOK && millis() - lastWeather > 10000) {
    fetchWeather();
    lastWeather = millis();
  }

  controlPump();

  delay(2000);
}
