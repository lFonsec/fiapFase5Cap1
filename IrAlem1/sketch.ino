#include <DHT.h>
#include <WiFi.h>
#include "UbidotsEsp32Mqtt.h"

// Configurações da Rede Wi-Fi
const char* ssid = "Wokwi-GUEST";  // Substitua pelo SSID da rede Wi-Fi
const char* password = "";         // Substitua pela senha da rede (senha vazia no )
const char *UBIDOTS_TOKEN = "BBUS-luS5xSmE4i3QcujBTwzdy8AhsHxyAB";  // Put here your Ubidots TOKEN
const char *DEVICE_LABEL = "teste";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "humidade"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

int pinoDHT =  23;
int trigPin =  22;
int echoPin =  21;
DHT dht(pinoDHT, DHT22);

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}



void setup() {
  Serial.begin(115200);  // Inicializa o Monitor Serial


  // inicia o sensor dht
  dht.begin();
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("Conectando ao Wi-Fi...");

  // Inicia a conexão ao Wi-Fi
  WiFi.begin(ssid, password);

  // Aguarda a conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    
  }

  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
  // Exibe informações da conexão
  Serial.println("\nWi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}


void loop() {


// pega as informações de temperatura e umidade do sensor dht
float temp = dht.readTemperature();
float umi = dht.readHumidity();
// ativa o echosonar
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPin, LOW);


  // calcula a distancia de acordo com o tempo
  long duration = pulseIn(echoPin,HIGH);
  int distance = duration * 0.034/2;

  // imprime as informações coletados pelo sensor dht2
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println("°C");
  Serial.print("Umidade: ");
  Serial.print(umi);
  Serial.println("%");

  // imprime a distancia
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  int rssi = WiFi.RSSI();  // Lê a força do sinal da rede atual

  // Exibe a intensidade no Monitor Serial
  Serial.print("Intensidade do sinal Wi-Fi (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println("---------------");

  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    float value = analogRead(analogPin);
    ubidots.add(temperatura, temp); // Insert your variable Labels and the value to be sent
    ubidots.add(humidade, umi);
    ubidots.add(distancia, distance)
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
  delay(500);
}
