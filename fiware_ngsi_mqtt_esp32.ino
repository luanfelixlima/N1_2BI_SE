#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Configurações - variáveis editáveis
const char* default_SSID = "AndroidAPDC48"; // Nome da rede Wi-Fi
const char* default_PASSWORD = "pkri87080."; // Senha da rede Wi-Fi
const char* default_BROKER_MQTT = "20.206.204.120"; // IP do Broker MQTT
const int default_BROKER_PORT = 1883; // Porta do Broker MQTT
const char* default_TOPICO_SUBSCRIBE = "/TEF/lamp06x/cmd"; // Tópico MQTT de escuta
const char* default_TOPICO_PUBLISH_1 = "/TEF/lamp06x/attrs"; // Tópico MQTT de envio de informações para Broker
const char* default_TOPICO_PUBLISH_2 = "/TEF/lamp06x/attrs/l"; // Tópico MQTT de envio de informações para Broker
const char* default_TOPICO_PUBLISH_3 = "/TEF/lamp06x/attrs/h";
const char* default_TOPICO_PUBLISH_4 = "/TEF/lamp06x/attrs/t";
const char* default_ID_MQTT = "fiware_06x"; // ID MQTT
const int default_D4 = 2; // Pino do LED onboard
// Declaração da variável para o prefixo do tópico
const char* topicPrefix = "lamp06x";

// DEFINIÇÕES PARA O DHT
const int dht_pin = 12;

#define DHT_TYPE DHT22
DHT dht(dht_pin, DHT_TYPE);

// Variáveis para configurações editáveis
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;
char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* TOPICO_PUBLISH_3 = const_cast<char*>(default_TOPICO_PUBLISH_3);
char* TOPICO_PUBLISH_4 = const_cast<char*>(default_TOPICO_PUBLISH_4);
char* ID_MQTT = const_cast<char*>(default_ID_MQTT);
int D4 = default_D4;

WiFiClient espClient;
PubSubClient MQTT(espClient);
char EstadoSaida = '0';

// Variáveis globais para armazenar os valores atuais de temperatura, umidade e luminosidade
float globalTemperature;
float globalHumidity;
int globalLuminosity;

// Variáveis para armazenar a soma e o contador das leituras
float tempSum = 0;
float humSum = 0;
int lumSum = 0;
int numSamples = 0;

// Temporizadores
unsigned long lastSerialPrintTime = 0;
unsigned long lastMqttPublishTime = 0;

void initSerial() {
    Serial.begin(115200);
}

void initWiFi() {
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconectWiFi();
}

void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

void setup() {
    dht.begin();
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
    delay(5000);
    MQTT.publish(TOPICO_PUBLISH_1, "s|on");
}

void loop() {
    VerificaConexoesWiFIEMQTT();
    handleLuminosity();
    handleDHT();
    EnviaEstadoOutputMQTT();
    checkConditions();
    printAndPublishAverages();
    MQTT.loop();
    delay(1000);
}

void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED)
        return;
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());

    // Garantir que o LED inicie desligado
    digitalWrite(D4, LOW);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i];
        msg += c;
    }
    Serial.print("- Mensagem recebida: ");
    Serial.println(msg);

    // Forma o padrão de tópico para comparação
    String onTopic = String(topicPrefix) + "@on|";
    String offTopic = String(topicPrefix) + "@off|";

    // Compara com o tópico recebido
    if (msg.equals(onTopic)) {
        digitalWrite(D4, HIGH);
        EstadoSaida = '1';
    }

    if (msg.equals(offTopic)) {
        digitalWrite(D4, LOW);
        EstadoSaida = '0';
    }
}

void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected())
        reconnectMQTT();
    reconectWiFi();
}

void EnviaEstadoOutputMQTT() {
    unsigned long currentTime = millis();

    if (currentTime - lastSerialPrintTime >= 10000)
    {
      if (EstadoSaida == '1') {
          MQTT.publish(TOPICO_PUBLISH_1, "s|on");
          Serial.println("- Led Ligado");
      }

      if (EstadoSaida == '0') {
          MQTT.publish(TOPICO_PUBLISH_1, "s|off");
          Serial.println("- Led Desligado");
      }
      Serial.println("- Estado do LED onboard enviado ao broker!");
    }
}

void InitOutput() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
    boolean toggle = false;

    for (int i = 0; i <= 10; i++) {
        toggle = !toggle;
        digitalWrite(D4, toggle);
        delay(200);
    }
}

void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE);
        } else {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Haverá nova tentativa de conexão em 2s");
            delay(2000);
        }
    }
}

void handleLuminosity() {
    const int potPin = 34;
    int sensorValue = analogRead(potPin);
    int luminosity = map(sensorValue, 0, 4095, 0, 100);
    // Serial.print("Valor da luminosidade: ");
    // Serial.println(luminosity);

    lumSum += luminosity;
    numSamples++;

    globalLuminosity = luminosity;
}

void handleDHT() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Serial.print("Valor da Temperatura: ");
    // Serial.println(temperature);
    // Serial.print("Valor da Humidade: ");
    // Serial.println(humidity);

    tempSum += temperature;
    humSum += humidity;

    globalTemperature = temperature;
    globalHumidity = humidity;
}

void checkConditions() {
    bool outOfRange = false;

    // Verificar faixa de temperatura
    if (globalTemperature <= 15 || globalTemperature >= 25) {
        outOfRange = true;
    }

    // Verificar faixa de luminosidade
    if (globalLuminosity <= 0 || globalLuminosity >= 30) {
        outOfRange = true;
    }

    // Verificar faixa de umidade
    if (globalHumidity <= 30 || globalHumidity >= 50) {
        outOfRange = true;
    }

    // Acender ou apagar o LED com base nas condições
    if (outOfRange) {
        digitalWrite(D4, HIGH);
        EstadoSaida = '1';
    } else {
        digitalWrite(D4, LOW);
        EstadoSaida = '0';
    }
}


void printAndPublishAverages() {
    unsigned long currentTime = millis();

    // Imprimir os logs a cada 5 segundos
    if (currentTime - lastSerialPrintTime >= 5000) {
        lastSerialPrintTime = currentTime;
        
        Serial.print("Leitura atual - Temperatura: ");
        Serial.print(globalTemperature);
        Serial.print(" ºC, Umidade: ");
        Serial.print(globalHumidity);
        Serial.print(" %, Luminosidade: ");
        Serial.print(globalLuminosity);
        Serial.println(" %");
    }

    // Enviar a média via MQTT a cada 10 segundos
    if (currentTime - lastMqttPublishTime >= 10000) {
        lastMqttPublishTime = currentTime;

        // Calcular a média
        float avgTemp = tempSum / numSamples;
        float avgHum = humSum / numSamples;
        int avgLum = lumSum / numSamples;

        // Enviar a média via MQTT
        String tempMsg = "Temperatura média: " + String(avgTemp) + " ºC";
        String humMsg = "Umidade média: " + String(avgHum) + " %";
        String lumMsg = "Luminosidade média: " + String(avgLum) + " %";

        MQTT.publish(TOPICO_PUBLISH_2, String(avgLum).c_str());
        MQTT.publish(TOPICO_PUBLISH_3, String(avgHum).c_str());
        MQTT.publish(TOPICO_PUBLISH_4, String(avgTemp).c_str());

        // Exibir a média no Serial Monitor com destaque
        Serial.println("========== Média dos Últimos 60s ==========");
        Serial.println(tempMsg);
        Serial.println(humMsg);
        Serial.println(lumMsg);
        Serial.println("===========================================");

        // Reiniciar os acumuladores e o contador de amostras
        tempSum = 0;
        humSum = 0;
        lumSum = 0;
        numSamples = 0;
    }
}
