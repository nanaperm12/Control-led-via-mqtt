#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define pin_lampu D2

const char* ssid = "robotikid";
const char* password = "123456789";
const char* host_server = "free.mqtt.iyoti.id";
int port_server = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long millis_sebelum = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan dari [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(pin_lampu, HIGH);
  } else {
    digitalWrite(pin_lampu, LOW);
  }

}

void menghubung_ulang() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke Broker MQTT...");
    String client_id = "RobotikID-";
    client_id += String(random(0xffff), HEX);

    if (client.connect(client_id.c_str())) {
      Serial.println("Terhubung");
      client.subscribe("rid/smart");
    } else {
      Serial.print("Gagal Terhubung, rc=");
      Serial.print(client.state());
      Serial.println(" Mencoba ulang, 5 detik lagi");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(pin_lampu, OUTPUT);
//  digitalWrite(pin_lampu, HIGH);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("Terhubung ke WiFi");
  Serial.println("Alamat IP: ");
  Serial.println(WiFi.localIP());
  
  client.setServer(host_server, port_server);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    menghubung_ulang();
  }
  client.loop();

  if (millis() - millis_sebelum > 2000) {
    String pesan = "Lampu: ";
    String status_pesan = "off";
    if(digitalRead(pin_lampu)){
      status_pesan = "on";
    }
    pesan += status_pesan;
    
    Serial.print("Publish Status Lampu: ");
    Serial.println(pesan);
    client.publish("rid/status", pesan.c_str());
    millis_sebelum = millis();
  }
}
