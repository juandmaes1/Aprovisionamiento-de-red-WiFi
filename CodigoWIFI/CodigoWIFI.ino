#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>    // Para servidor web
#include <DNSServer.h>    // Para portal cautivo
#include <Preferences.h>  // Para almacenamiento NVS

const char* apSSID = "nicogru";  // SSID del AP
const char* apPass = "12345678"; // Contraseña del AP

String staSSID = "";  // SSID guardado
String staPass = "";  // Pass guardado

Preferences prefs;                    
WebServer server(80);                 
DNSServer dnsServer;                  
const byte DNS_PORT = 53;             

const int resetPin = 0;               

bool loadCredentials() {
  prefs.begin("wifi-creds", false);
  staSSID = prefs.getString("ssid", "");
  staPass = prefs.getString("pass", "");
  prefs.end();
  return (staSSID != "");
}

void saveCredentials(String ssid, String pass) {
  prefs.begin("wifi-creds", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();
}

void clearCredentials() {
  prefs.begin("wifi-creds", false);
  prefs.clear();
  prefs.end();
}

void handleRoot() {
  server.sendHeader("Location", "/config");
  server.send(303);
}

void handleConfig() {
  String html = 
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<style>"
    "body { font-family: Arial, sans-serif; background: #f2f2f2; text-align: center; padding: 40px; }"
    "h1 { color: #333; }"
    "form { background: white; padding: 20px; border-radius: 10px; display: inline-block; margin: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }"
    "input[type='text'], input[type='password'] { width: 200px; padding: 8px; margin: 5px 0; border: 1px solid #ccc; border-radius: 5px; }"
    "input[type='submit'] { background: #007BFF; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; font-size: 16px; }"
    "input[type='submit']:hover { background: #0056b3; }"
    "</style></head><body>"
    "<h1>Configura WiFi</h1>"
    "<form action='/save' method='POST'>"
    "<p><input type='text' name='ssid' placeholder='SSID'></p>"
    "<p><input type='password' name='pass' placeholder='Contraseña'></p>"
    "<p><input type='submit' value='Guardar'></p>"
    "</form>"
    "<form action='/reset' method='POST'>"
    "<p><input type='submit' value='Resetear WiFi'></p>"
    "</form>"
    "</body></html>";
  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("pass")) {
    staSSID = server.arg("ssid");
    staPass = server.arg("pass");
    saveCredentials(staSSID, staPass);
    server.send(200, "text/html", "<h1>Credenciales guardadas! Reiniciando...</h1>");
    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "text/html", "<h1>Error: Campos requeridos</h1>");
  }
}

void handleReset() {
  server.send(200, "text/html", "<h1>Credenciales borradas! Reiniciando...</h1>");
  Serial.println("Reset solicitado desde /reset. Borrando credenciales...");
  clearCredentials();
  delay(1000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(resetPin, INPUT_PULLUP);

  if (digitalRead(resetPin) == LOW) {
    Serial.println("Reset detectado! Borrando credenciales...");
    clearCredentials();
    delay(2000);
  }

  bool hasCreds = loadCredentials();

  if (!hasCreds) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPass);
    Serial.println("Modo AP iniciado. Conéctate a " + String(apSSID));

    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

    server.on("/", HTTP_GET, handleRoot);
    server.on("/config", HTTP_GET, handleConfig);
    server.on("/save", HTTP_POST, handleSave);
    server.on("/reset", HTTP_POST, handleReset);
    server.begin();
    Serial.println("Servidor web iniciado en " + WiFi.softAPIP().toString());
  } else {
    WiFi.begin(staSSID.c_str(), staPass.c_str());
    Serial.print("Conectando a " + staSSID + "...");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi conectado! IP: " + WiFi.localIP().toString());
      HTTPClient http;
      http.begin("http://example.com"); 
      int httpCode = http.GET();
      if (httpCode > 0) {
        Serial.printf("Código HTTP: %d\n", httpCode);
        String payload = http.getString();
        Serial.println("Respuesta: " + payload);
      } else {
        Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.println("\nFallo al conectar. Borrando creds y reiniciando...");
      clearCredentials();
      ESP.restart();
    }
  }
}

void loop() {
  if (WiFi.getMode() == WIFI_AP) {
    dnsServer.processNextRequest();
    server.handleClient();
  }
}
