# Schematics – ESP32 Solar Metrics Simulator

## Visão Geral

Este esquemático representa o circuito utilizado para a simulação de métricas de uma placa de energia solar por meio de quatro potenciômetros conectados a uma placa **ESP32**. Cada potenciômetro simula uma variável analógica (por exemplo: tensão, corrente, potência ou irradiância), que é lida pela ESP32 e posteriormente enviada via MQTT para o sistema de monitoramento.

O circuito foi montado em uma protoboard, utilizando conexões simples e componentes de baixo custo, com foco didático e experimental.

---

## Componentes Utilizados

- 1x ESP32 Dev Module  
- 4x Potenciômetros (10kΩ ou equivalente)  
- 1x Protoboard  
- Jumpers macho-macho  
- Fonte de alimentação via USB (O próprio notebook)

---

## Descrição das Conexões

Cada potenciômetro possui três terminais:

- **VCC** → 3.3V da ESP32  
- **GND** → GND da ESP32  
- **SIG** → Entrada analógica (ADC) da ESP32  

Todos os potenciômetros compartilham a mesma alimentação (3.3V) e o mesmo terra (GND).

### Mapeamento dos Pinos

| Potenciômetro | Métrica Simulada | Pino ESP32 |
|--------------|------------------|------------|
| Potenciômetro 1 | Métrica 1 | GPIO 32 |
| Potenciômetro 2 | Métrica 2 | GPIO 33 |
| Potenciômetro 3 | Métrica 3 | GPIO 34 |
| Potenciômetro 4 | Métrica 4 | GPIO 35 |

> Os pinos utilizados pertencem ao módulo ADC da ESP32, permitindo a leitura contínua de valores analógicos.

---

## Funcionamento do Circuito

- A ESP32 fornece alimentação de **3.3V** para os potenciômetros.
- A rotação do eixo de cada potenciômetro varia a tensão no pino de sinal.
- A ESP32 realiza leituras periódicas das entradas analógicas.
- Os valores são convertidos para dados digitais e enviados via Wi-Fi utilizando o protocolo **MQTT**.
- Os dados são processados e armazenados para posterior visualização em dashboards.

---

## Observações Importantes

- Não utilizar 5V nos potenciômetros, pois as entradas analógicas da ESP32 operam em **3.3V**.
- Os potenciômetros são utilizados exclusivamente para fins de simulação.
- O circuito foi desenvolvido com foco educacional, priorizando simplicidade e clareza.

---

## Diagrama do Circuito

A imagem abaixo representa a montagem do circuito na protoboard, incluindo as conexões entre a ESP32 e os potenciômetros.

