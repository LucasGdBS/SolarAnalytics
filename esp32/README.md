# 🌞 ESP32 – Simulador de Placas Solares (com MQTT)

Este projeto utiliza uma **ESP32** e **4 potenciômetros** para simular métricas de um painel solar, como irradiação, eficiência, área e custo.  
Os valores são lidos, convertidos para faixas realistas e enviados em formato **JSON via MQTT** para um broker — ideal para visualização posterior em ferramentas como **Grafana**.

## 🔌 Funcionalidade Geral

A ESP32 realiza três funções principais:

1. **Conecta ao Wi-Fi**
2. **Lê os potenciômetros**
3. **Publica os dados via MQTT** no tópico:
```
solar/metrics
```

## 📡 Envio MQTT

A cada 2 segundos, a ESP32 envia um JSON como:
```json
{
  "irr_wm2": 512.3,
  "eff_pct": 17.4,
  "area_m2": 1.58,
  "cost_brl": 2480.10,
  "power_w": 1415.2
}
```

## 🧪 Testando sem Raspberry Pi

### ✔ MacOS
1. Instale Mosquitto:
```bash
brew install mosquitto
```
2. Escute o tópico:
```bash
mosquitto_sub -h test.mosquitto.org -t "solar/metrics" -v
```

### ✔ Windows
1. Baixe o Mosquitto em: https://mosquitto.org/download  
2. Abra o PowerShell no diretório de instalação:
```powershell
mosquitto_sub.exe -h test.mosquitto.org -t "solar/metrics" -v
```

### ✔ Linux (Ubuntu/Debian)
1. Instale Mosquitto:
```bash
sudo apt update
sudo apt install mosquitto-clients
```
2. Escute o tópico:
```bash
mosquitto_sub -h test.mosquitto.org -t "solar/metrics" -v
```

## 🛠 Dependências
- WiFi.h
- PubSubClient
- 4 potenciômetros nos pinos ADC 34, 35, 32 e 33
