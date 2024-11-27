#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Configurações Wi-Fi
const char* ssid = "Coisas";
const char* password = "Iot*2024";

// Configurações MQTT
const char* mqtt_server = "test.mosquitto.org";
const char* topic_temp = "mestrado/iot/aluno/Daniel/temperatura";
const char* topic_hum = "mestrado/iot/aluno/Daniel/umidade";

// Configurações DHT
#define DHTPIN D1 // Pino de dados do DHT
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Criando obj de conexão
WiFiClient espClient;
PubSubClient client(espClient);

// Criando procedimento de conf de WI-FI
void setup_wifi() {
  delay(10);
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lê os valores do sensor
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Verifica se os valores são válidos
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha na leitura do sensor DHT!");
    return;
  }

  // Publica os dados no MQTT
  char temp_str[8];
  char hum_str[8];
  dtostrf(temperatura, 6, 2, temp_str);
  dtostrf(umidade, 6, 2, hum_str);

  client.publish(topic_temp, temp_str);
  client.publish(topic_hum, hum_str);

  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" °C\tUmidade: ");
  Serial.print(umidade);
  Serial.println(" %");

  delay(5000); // Aguarda 5 segundos antes de publicar novamente
}
