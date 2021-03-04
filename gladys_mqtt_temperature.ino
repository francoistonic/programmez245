#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define wifi_ssid "VOTRE_WIFI_SSID"
#define wifi_password "VOTRE_MOT_DE_PASSE_WIFI"

#define mqtt_server "VOTRE_IP_MQTT_SERVER"
#define mqtt_user "VOTRE_UTILISATEUR_MQTT" 
#define mqtt_password "VOTRE_MOT_DE_PASSE_MQTT"

#define temperature_topic "VOTRE_TOPIC_TEMPERATURE"
#define humidity_topic "VOTRE_TOPIC_HUMIDITY"

long lastMsg = 0;

#define DHTPIN D5    // Le pin sur lequel est branché le DHT
#define DHTTYPE DHT11 // Notre capteur DHT11

DHT dht(DHTPIN, DHTTYPE);     
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);    //Configuration de la connexion au serveur MQTT 
  dht.begin();
}

//Connexion au réseau WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion au Wi-Fi ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connexion WiFi etablie ");
  Serial.print("=> Addresse IP : ");
  Serial.print(WiFi.localIP());
}

//Reconnexion
void reconnect() {
  //Boucle jusqu'à obtenur une reconnexion
  while (!client.connected()) {
    Serial.print("Connexion au serveur MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Connecte avec succes");
    } else {
      Serial.print("Erreur : ");
      Serial.print(client.state());
      Serial.println("Tentative de reconnexion dans 5 secondes");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  // Envoi d'une valeur de température/humidité par minute
  if (now - lastMsg > 1000 * 60) {
    lastMsg = now;
    
    // Lecture de l'humidité ambiante
    float h = dht.readHumidity();
    // Lecture de la température en Celcius
    float t = dht.readTemperature();
    Serial.print("Temperature : ");
    Serial.print(t);
    Serial.print(" | Humidite : ");
    Serial.println(h); 

    // On envoie la température à Gladys Assistant
    client.publish(temperature_topic, String(t ).c_str(), true);
    // On envoie l'humidité à Gladys Assistant
    client.publish(humidity_topic, String(h).c_str(), true);
  }
}