// ==========================================================
// SISTEMA DE MONITORAMENTO AMBIENTAL - GS FIAP
// Integrantes:
// - Joao Cazzarini      | RM: 
// - Enrico Bagli    | RM: 562541
// ==========================================================

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ==================== CONFIGURAÇÕES PADRÃO ====================
const char* default_SSID = "Wokwi-GUEST";
const char* default_PASSWORD = "";
const char* default_BROKER_MQTT = "135.237.162.152";
const int   default_BROKER_PORT = 1883;

const char* default_TOPICO_SUBSCRIBE = "/TEF/lamp001/cmd";
const char* default_TOPICO_PUBLISH_1 = "/TEF/lamp001/attrs";
const char* default_TOPICO_PUBLISH_2 = "/TEF/lamp001/attrs/l";
const char* default_TOPICO_PUBLISH_3 = "/TEF/lamp001/attrs/t";
const char* default_TOPICO_PUBLISH_4 = "/TEF/lamp001/attrs/h";
const char* default_TOPICO_PUBLISH_5 = "/TEF/lamp001/attrs/p";

const char* default_ID_MQTT = "fiware_001";
const int default_D4 = 2;

const char* topicPrefix = "lamp001";

// ==================== SENSOR SETUP ====================
#define DHT_PIN 4
#define LDR_PIN 34
#define PIR_PIN 15
DHT dht(DHT_PIN, DHT22);

// ==================== VARIÁVEIS EDITÁVEIS ====================
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;

char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* TOPICO_PUBLISH_3 = const_cast<char*>(default_TOPICO_PUBLISH_3);
char* TOPICO_PUBLISH_4 = const_cast<char*>(default_TOPICO_PUBLISH_4);
char* TOPICO_PUBLISH_5 = const_cast<char*>(default_TOPICO_PUBLISH_5);

char* ID_MQTT = const_cast<char*>(default_ID_MQTT);
int D4 = default_D4;

// ==================== OBJETOS PRINCIPAIS ====================
WiFiClient espClient;
PubSubClient MQTT(espClient);
char EstadoSaida = '0';

// ==================== CONTROLE DE LEITURA ====================
unsigned long lastSensorRead = 0;
const long sensorInterval = 5000;
int contadorLeituras = 0;

// ==================== FUNÇÕES DE INICIALIZAÇÃO ====================

// Inicializa comunicação serial
void initSerial() {
    Serial.begin(115200);
    Serial.println("== Sistema iniciado ==");
}

// Conexão Wi-Fi
void initWiFi() {
    Serial.println("Estabelecendo conexão com rede Wi-Fi...");
    reconectWiFi();
}

// Configura MQTT e define callback
void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

// Inicializa sensores
void initSensors() {
    dht.begin();
    pinMode(PIR_PIN, INPUT);
    Serial.println("Sensores configurados e prontos.");
}

// ==================== SETUP ====================
void setup() {
    InitOutput();
    initSerial();
    initSensors();
    initWiFi();
    initMQTT();

    delay(5000);
    MQTT.publish(TOPICO_PUBLISH_1, "s|on");
    Serial.println("Sistema completamente operacional.");
}

// ==================== LOOP PRINCIPAL ====================
void loop() {
    VerificaConexoesWiFIEMQTT(); 
    EnviaEstadoOutputMQTT();     

    if (millis() - lastSensorRead >= sensorInterval) {
        Serial.println("\n[Atualização dos Sensores]");
        
        handleLuminosity();
        handleTemperature();
        handleHumidity();
        handlePresence();

        lastSensorRead = millis();
        contadorLeituras++;
    }

    MQTT.loop();
    delay(100);
}

// ==================== FUNÇÕES AUXILIARES ====================

// Reconectar Wi-Fi quando necessário
void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED) return;

    WiFi.begin(SSID, PASSWORD);
    Serial.print("Conectando ao Wi-Fi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(150);
        Serial.print(".");
    }

    Serial.println("\nConexão estabelecida.");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());

    digitalWrite(D4, LOW);
}

// Callback MQTT para mensagens recebidas
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;

    for (int i = 0; i < length; i++)
        msg += (char)payload[i];

    Serial.print("Comando MQTT recebido: ");
    Serial.println(msg);

    String onCmd  = String(topicPrefix) + "@on|";
    String offCmd = String(topicPrefix) + "@off|";

    if (msg.equals(onCmd)) {
        digitalWrite(D4, HIGH);
        EstadoSaida = '1';
        Serial.println("LED ligado por comando remoto.");
    }

    if (msg.equals(offCmd)) {
        digitalWrite(D4, LOW);
        EstadoSaida = '0';
        Serial.println("LED desligado por comando remoto.");
    }
}

// Verifica Wi-Fi e MQTT
void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected()) 
        reconnectMQTT();
    reconectWiFi();
}

// Publica estado da saída a cada 30s
void EnviaEstadoOutputMQTT() {
    static unsigned long lastSend = 0;

    if (millis() - lastSend >= 30000) {
        if (EstadoSaida == '1')
            MQTT.publish(TOPICO_PUBLISH_1, "s|on");
        else
            MQTT.publish(TOPICO_PUBLISH_1, "s|off");

        lastSend = millis();
    }
}

// Efeito inicial no LED
void InitOutput() {
    pinMode(D4, OUTPUT);
    bool toggle = false;

    for (int i = 0; i <= 6; i++) {
        toggle = !toggle;
        digitalWrite(D4, toggle);
        delay(300);
    }
    digitalWrite(D4, LOW);
}

// Reconectar ao broker MQTT
void reconnectMQTT() {
    Serial.println("Tentando reconectar ao servidor MQTT...");

    int tentativas = 0;
    while (!MQTT.connected() && tentativas < 5) {
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conexão com broker MQTT restabelecida.");
            MQTT.subscribe(TOPICO_SUBSCRIBE);
        } else {
            Serial.print("Falha ao conectar. Tentativa ");
            Serial.println(tentativas + 1);
            delay(1500);
            tentativas++;
        }
    }
}

// ==================== FUNÇÕES DOS SENSORES ====================

// Luminosidade
void handleLuminosity() {
    int sensorValue = analogRead(LDR_PIN);
    int luminosity = map(sensorValue, 0, 4095, 0, 100);
    String mensagem = String(luminosity);

    Serial.print("Luminosidade: ");
    Serial.print(mensagem);
    Serial.println("%");

    MQTT.publish(TOPICO_PUBLISH_2, mensagem.c_str());
}

// Temperatura
void handleTemperature() {
    float temp = dht.readTemperature();

    if (isnan(temp)) {
        temp = 22.0 + (random(0, 150) / 10.0);
        Serial.println("Leitura inválida. Usando valor alternativo.");
    }

    String mensagem = String(temp, 1);

    Serial.print("Temperatura: ");
    Serial.print(mensagem);
    Serial.println("°C");

    MQTT.publish(TOPICO_PUBLISH_3, mensagem.c_str());
}

// Umidade
void handleHumidity() {
    float umidade = dht.readHumidity();

    if (isnan(umidade)) {
        umidade = 40.0 + (random(0, 400) / 10.0);
        Serial.println("Falha na leitura. Utilizando umidade simulada.");
    }

    String mensagem = String(umidade, 1);

    Serial.print("Umidade: ");
    Serial.print(mensagem);
    Serial.println("%");

    MQTT.publish(TOPICO_PUBLISH_4, mensagem.c_str());
}

// Presença
void handlePresence() {
    bool presenca = false;

    if (contadorLeituras % 3 == 0)
        presenca = true;

    bool real = digitalRead(PIR_PIN);

    if (real == LOW || real == HIGH)
        presenca = real;

    String mensagem = presenca ? "1" : "0";

    Serial.print("Presença: ");
    Serial.println(presenca ? "detectada" : "ausência");

    MQTT.publish(TOPICO_PUBLISH_5, mensagem.c_str());

    Serial.println("Envio concluído.");
    Serial.println("-------------------------");
}
