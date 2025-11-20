#include <WiFi.h>

// ======== Wi-Fi ========
const char* ssid     = "iPhone de Maria Fernanda";
const char* password = "bubaloo123";

String get_wifi_status(int status) {
  switch (status) {
    case WL_IDLE_STATUS:     return "WL_IDLE_STATUS";
    case WL_SCAN_COMPLETED:  return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL:   return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED:  return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_CONNECTED:       return "WL_CONNECTED";
    case WL_DISCONNECTED:    return "WL_DISCONNECTED";
    default:                 return "UNKNOWN_STATUS";
  }
}

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

  // Conexão Wi-Fi
  Serial.println();
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int status = WiFi.status();
  Serial.print("Status inicial: ");
  Serial.println(get_wifi_status(status));

  int tentativas = 0;
  const int TENTATIVAS_MAX = 20;

  while (status != WL_CONNECTED && tentativas < TENTATIVAS_MAX) {
    delay(500);
    status = WiFi.status();
    Serial.print("Status: ");
    Serial.println(get_wifi_status(status));
    tentativas++;
  }

  if (status == WL_CONNECTED) {
    Serial.println("\n✅ Conectado ao Wi-Fi!");
    Serial.print("IP da ESP32: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Falha ao conectar no Wi-Fi.");
    Serial.println("Verifique SSID, senha e tipo de rede (2.4 GHz).");
  }

  Serial.println("\n{\"status\":\"boot\",\"msg\":\"ESP32 Solar Sim iniciado\"}");
}

// ======== Loop principal ========
void loop() {
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

  // Saída organizada
  Serial.println("\n================== MÉTRICAS DO PAINEL SOLAR ==================");
  Serial.print("Irradiação:       "); Serial.print(irrWm2, 1);  Serial.println(" W/m²");
  Serial.print("Eficiência:       "); Serial.print(effPct, 1);  Serial.println(" %");
  Serial.print("Área do Painel:   "); Serial.print(areaM2, 2);  Serial.println(" m²");
  Serial.print("Custo (estimado): R$ "); Serial.print(costBRL, 2); Serial.println();
  Serial.print("Potência Gerada:  "); Serial.print(powerW, 1); Serial.println(" W");

  Serial.println("--------------------------------------------------------------");

  delay(2000); // atualiza a cada 2s
}
