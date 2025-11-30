# SolarAnalytics

## O que é SolarAnalytics?

SolarAnalytics é um sistema IoT criado para monitorar dados de geração solar em tempo real que coleta, organiza e visualiza o desempenho do painel solar de forma contínua e automática.

## Como funciona?
A ESP32 coleta informações simuladas como irradiância e potência, envia tudo via Wi-Fi para a Raspberry Pi usando MQTT, e o computador processa esses dados no Node-RED, armazena no InfluxDB e exibe gráficos no Grafana.

📡 ESP32 → 🍓 Raspberry Pi → 🛢️ InfluxDB → 🟥🔧 Node-RED → 📈 Grafana


## Estrutura e Responsabilidades de cada Módulo

```bash
├── README.md # Descrição do projeto, requisitos, instruções.
├── /docs # Relatório em PDF (ABNT2) + imagens.
├── /raspberry-pi # Configuração do broker MQTT.
├── /esp32 # Firmware dos módulos (FreeRTOS/MicroPython/C/C++/IDE Arduino).
├── /grafana # Configuração dos dashboards.
└── /schematics # Diagramas eletrônicos (Fritzing ou KiCad).
```

## Autores

| [![Lucas Gabriel](https://avatars.githubusercontent.com/LucasGdBS?v=4)](https://github.com/LucasGdBS) | [![Carlos Eduardo](https://avatars.githubusercontent.com/Carlos3du)](https://github.com/Carlos3du) | [![Maria Fernanda](https://avatars.githubusercontent.com/FernandaFBMarques)](https://github.com/FernandaFBMarques) | [![Gabriel Chaves](https://avatars.githubusercontent.com/Gabriel-Chaves0)](https://github.com/Gabriel-Chaves0) | [![Cristina Matsunaga](https://avatars.githubusercontent.com/Criismnaga)](https://github.com/Criismnaga) |
|:--:|:--:|:--:|:--:|:--:|
| **Lucas Gabriel** | **Carlos Eduardo** | **Maria Fernanda** | **Gabriel Chaves** | **Cristina Matsunaga** |
