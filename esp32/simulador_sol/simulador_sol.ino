#include <WiFi.h>
#include <PubSubClient.h>

// ======== Wi-Fi ========
const char* ssid     = "outra_dimensao";
const char* password = "kika1234";

// ======== MQTT (broker de teste público) ========
const char* mqtt_server = "10.213.91.245";   // depois você trocar pelo IP da rasp
const int   mqtt_port   = 1883;
const char* mqtt_topic  = "solar/metrics"; // só escolher " qualquer " nome

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ======== Pinos dos potenciômetros ========
const int PIN_IRR   = 34; // Irradiação (W/m²)
const int PIN_EFF   = 35; // Eficiência (%)
const int PIN_AREA  = 32; // Área (m²)
const int PIN_COST  = 33; // Custo (R$)

// Faixas para mapeamento
const float IRR_MIN      = 0.0;
const float IRR_MAX      = 1000.0;   // W/m²

const float EFF_MIN_PCT  = 10.0;
const float EFF_MAX_PCT  = 25.0;     // %

const float AREA_MIN     = 0.5;
const float AREA_MAX     = 2.0;      // m²

const float COST_MIN     = 500.0;
const float COST_MAX     = 5000.0;   // R$

const int SMOOTH_N = 5; // média simples

// ======== Funções auxiliares ========
float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int readADC(int pin) {
  long acc = 0;
  for (int i = 0; i < SMOOTH_N; i++) {
    acc += analogRead(pin);
    delay(2);
  }
  return (int)(acc / SMOOTH_N);
}

void connectWifi() {
  Serial.println();
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int tentativas = 0;
  const int TENTATIVAS_MAX = 20;

  while (WiFi.status() != WL_CONNECTED && tentativas < TENTATIVAS_MAX) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Conectado ao Wi-Fi!");
    Serial.print("IP da ESP32: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Falha ao conectar no Wi-Fi.");
    Serial.println("Verifique SSID, senha e tipo de rede (2.4 GHz).");
  }
}

void connectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠ Wi-Fi não conectado, não dá pra conectar no MQTT.");
    return;
  }

  while (!mqttClient.connected()) {
    Serial.print("Conectando ao broker MQTT... ");
    // clientId aleatório pra evitar conflito
    String clientId = "ESP32Solar-" + String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("conectado!");
      // se precisasse assinar tópico, seria aqui (mqttClient.subscribe(...))
    } else {
      Serial.print("falha, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" tentando de novo em 2s...");
      delay(2000);
    }
  }
}

// ======== Setup ========
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Configura ADC
  analogSetAttenuation(ADC_11db);
  pinMode(PIN_IRR,  INPUT);
  pinMode(PIN_EFF,  INPUT);
  pinMode(PIN_AREA, INPUT);
  pinMode(PIN_COST, INPUT);

  // Wi-Fi
  connectWifi();

  // MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  randomSeed(micros());  // pra gerar clientId aleatório

  if (WiFi.status() == WL_CONNECTED) {
    connectMQTT();
  }

  Serial.println("\n{\"status\":\"boot\",\"msg\":\"ESP32 Solar Sim iniciado\"}");
}

// ======== Loop principal ========
void loop() {
  // mantém conexão MQTT viva
  if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  // Leituras brutas
  int rawIrr  = readADC(PIN_IRR);
  int rawEff  = readADC(PIN_EFF);
  int rawArea = readADC(PIN_AREA);
  int rawCost = readADC(PIN_COST);

  // Conversão para faixas "reais"
  float irrWm2  = mapf(rawIrr,  0.0, 4095.0, IRR_MIN, IRR_MAX);
  float effPct  = mapf(rawEff,  0.0, 4095.0, EFF_MIN_PCT, EFF_MAX_PCT);
  float areaM2  = mapf(rawArea, 0.0, 4095.0, AREA_MIN, AREA_MAX);
  float costBRL = mapf(rawCost, 0.0, 4095.0, COST_MIN, COST_MAX);

  // Potência estimada: P = Irradiação * Área * Eficiência
  float effDec = effPct / 100.0;
  float powerW = irrWm2 * areaM2 * effDec;

  // ===== Saída organizada no Serial =====
  Serial.println("\n================== MÉTRICAS DO PAINEL SOLAR ==================");
  Serial.print("Irradiação:       "); Serial.print(irrWm2, 1);  Serial.println(" W/m²");
  Serial.print("Eficiência:       "); Serial.print(effPct, 1);  Serial.println(" %");
  Serial.print("Área do Painel:   "); Serial.print(areaM2, 2);  Serial.println(" m²");
  Serial.print("Custo (estimado): R$ "); Serial.print(costBRL, 2); Serial.println();
  Serial.print("Potência Gerada:  "); Serial.print(powerW, 1); Serial.println(" W");
  Serial.println("--------------------------------------------------------------");

  // ===== Envio via MQTT em JSON =====
  if (mqttClient.connected()) {
    char payload[256];
    snprintf(
      payload,
      sizeof(payload),
      "{\"irr_wm2\":%.1f,\"eff_pct\":%.1f,\"area_m2\":%.2f,"
      "\"cost_brl\":%.2f,\"power_w\":%.1f}",
      irrWm2, effPct, areaM2, costBRL, powerW
    );

    bool ok = mqttClient.publish(mqtt_topic, payload);
    Serial.print("MQTT publish: ");
    Serial.println(ok ? "OK" : "ERRO");
  } else {
    Serial.println("⚠ Não conectado ao MQTT, não foi possível publicar.");
  }

  delay(2000); // atualiza a cada 2s
}
