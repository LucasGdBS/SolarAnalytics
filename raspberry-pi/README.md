# Raspberry Pi

## 📡 Configuração do Broker MQTT

Para o broker MQTT, utilizamos o Mosquitto, que pode ser instalado no Raspberry Pi com o seguinte comando:

```bash
sudo apt update
sudo apt install mosquitto mosquitto-clients
```

Após a instalação, o Mosquitto será iniciado automaticamente. Você pode verificar o status do serviço com:

```bash
sudo systemctl status mosquitto
```

Para garantir que o Mosquitto inicie automaticamente na inicialização do sistema, use o comando:

```bash
sudo systemctl enable mosquitto
```

## Configuração do Mosquitto

```bash
sudo nano /etc/mosquitto/conf.d/default.conf
```

ou

```bash
sudo vim /etc/mosquitto/conf.d/default.conf
```

Adicione as seguintes linhas ao arquivo de configuração para permitir conexões anônimas

```bash
listener 1883
allow_anonymous true
```

Salve o arquivo e reinicie o serviço Mosquitto para aplicar as alterações:

```bash
sudo systemctl restart mosquitto
```

## Testando o Broker MQTT

Você pode testar o broker MQTT usando os clientes Mosquitto. Abra dois terminais no Raspberry Pi. No primeiro terminal, inscreva-se em um tópico (por exemplo, "test/topic"):

```bash
mosquitto_sub -h localhost -t test/topic
```

No segundo terminal, publique uma mensagem nesse tópico:

```bash
mosquitto_pub -h localhost -t test/topic -m "Hello, MQTT!"
```

Se tudo estiver configurado corretamente, você verá a mensagem "Hello, MQTT!" aparecer no primeiro terminal.

## Enviando mensagens MQTT a partir de outro dispositivo

Para enviar mensagens MQTT a partir de outro dispositivo, para este caso é necessário estar **na mesma rede**, você precisará do endereço IP do Raspberry Pi. Você pode encontrá-lo com o comando:

```bash
hostname -I
```

Use esse endereço IP no lugar de "localhost" nos comandos `mosquitto_sub` e `mosquitto_pub` em outro dispositivo na mesma rede.

```bash
mosquitto_sub -h <IP_DO_RASPBERRY_PI> -t test/topic
```

```bash
mosquitto_pub -h <IP_DO_RASPBERRY_PI> -t test/topic -m "Hello from another device!"
```
# Comunicar ESP32 com Raspberry Pi

##🌐 Conectar ESP32 e Raspberry Pi na mesma Wifi 2.4 Gz.

Para garantir comunicação estável entre os controladores ESP32, a Raspberry Pi (broker MQTT) e o computador responsável pelo dashboard, todo o sistema deve operar na mesma rede Wi-Fi de 2.4 GHz. Caso a rede tenha acentos, espaços a Raspberry Pi pode ter dificuldade de ler

Ao conectá-los ao Wi-Fi, cada dispositivo recebe um IP automaticamente via DHCP do roteador. Para evitar que o IP da Raspberry Pi mude após reinicializações, é possível criar uma reserva DHCP no próprio roteador localizando a seção de DHCP e associar o MAC da Raspberry a um IP fixo. Assim, a Raspberry sempre usará o mesmo endereço, o que facilita a configuração do MQTT, Node-RED, InfluxDB e Grafana. No nosso caso desse projeto é o IP abaixo:

```bash
10.213.91.245`
```
## Enviar dados JSON do ESP32 para o tópico MQTT

No código do ESP32, verifique se o nome e a senha da rede Wi-Fi estão exatamente iguais à rede usada pelo computador:

```bash
// ======== Wi-Fi ========
const char* ssid     = "<NOME_DA_WIFI>";
const char* password = "<SENHA_DA_WIFI>";
```
 Confirme também se o campo do servidor MQTT contém o IP correto da Raspberry Pi.

```bash
// ======== MQTT (broker de teste público) ========
const char* mqtt_server = "10.213.91.245";  
const int   mqtt_port   = 1883;
const char* mqtt_topic  = "solar/metrics";
```

O próximo passo é conectar o ESP32 no computador usando o cabo USB e fazer o upload do código pela IDE Arduino. Abra o Monitor Serial configurado com baud rate de `115200` para acompanhar em tempo real a conexão do ESP32 ao Wi-Fi, ao broker MQTT e o envio contínuo dos dados simulados.

## Verificar recebimento de dados na Raspberry Pi

No terminal da rasp, é possível acompanhar o recebimento das mensagens em tempo real com o comando: 
```bash
mosquitto_sub -h 10.213.91.245 -t solar/metrics -v
```
Se estiver funcionando, o terminal da Raspberry mostrará algo assim:

```bash
solar/metrics {"irr_wm2":512.3,"eff_pct":17.4,"area_m2":1.58,"cost_brl":2480.10,"power_w":1415.2}
solar/metrics {"irr_wm2":498.7,"eff_pct":16.9,"area_m2":1.58,"cost_brl":2480.10,"power_w":1382.4}
solar/metrics {"irr_wm2":530.1,"eff_pct":17.6,"area_m2":1.58,"cost_brl":2480.10,"power_w":1436.8}
```

## Verificar recebimento de dados no computador onde vai rodar o Grafana

Optamos por executar o Grafana no computador e não na Raspberry Pi porque a Rasp tem desempenho limitado e poderia ficar lenta ao processar o dashboard junto com o broker MQTT. 

Para confirmar que o computador está recebendo corretamente os dados enviados pelo ESP32 via Raspberry, basta assinar o tópico MQTT usando o terminal do próprio PC.

```bash
mosquitto_sub -h 10.213.91.245 -t solar/metrics -v
```
Se estiver funcionando, o terminal do PC mostrará algo assim:

```bash
solar/metrics {"irr_wm2":512.3,"eff_pct":17.4,"area_m2":1.58,"cost_brl":2480.10,"power_w":1415.2}
solar/metrics {"irr_wm2":498.7,"eff_pct":16.9,"area_m2":1.58,"cost_brl":2480.10,"power_w":1382.4}
solar/metrics {"irr_wm2":530.1,"eff_pct":17.6,"area_m2":1.58,"cost_brl":2480.10,"power_w":1436.8}
```

# 🛢️ Instalação e configuração o InfluxDB no PC

Adotamos o InfluxDB como banco de dados porque ele é otimizado para séries temporais, permitindo armazenar medições sucessivas e ele se integra de forma nativa com o Grafana, o que simplifica a visualização posterior.

## Instalar o InfluxDB Version 1.6.7 


Faça a instalação de acordo com seu sistema operacional 

Para verificar se o InfluxDB está átivo 

```bash
systemctl status influxdb
```

Abrir o cliente do InfluxDB
```bash
influx
```

Listar todos os bancos de dados existentes
```bash
show databases
```
Selecionar o banco de dados onde o Node-RED está gravando (o nosso é telegraf)

```bash
use telegraf
```
Listar as measurements dentro desse banco e consultar dados salvos 
```bash
show measurements
select * from solar_metrics limit 10
```

# 🔴 Instalação e configuração do Node-Red

Node-RED é uma ferramenta de integração visual que permite criar fluxos de processamento conectando blocos prontos, chamados de nós. Ele roda normalmente no navegador e facilita a comunicação entre dispositivos, serviços e bancos de dados sem precisar escrever código extenso. No contexto do projeto, ele funciona como a ponte que recebe as mensagens MQTT da Raspberry, transforma os dados e envia tudo automaticamente para o InfluxDB.

## Instalar o Node-Red Version 4.1.2

Faça a instalação de acordo com seu sistema operacional 

Iniciar o Node-RED 
```bash
node-red
```
Com o serviço rodando, abra no navegador:
```bash
http://127.0.0.1:1880
```
Clique em importar e importe o json nodered-flow-to-import.json. 

O fluxo aparece imediatamente no workspace e depois finaliza clicando em “Deploy” para aplicar as configurações.

Quando o status aparecer connected deu tudo certo. 

# Instalação e configuração do Grafana

## Instalar o Grafana Version 12.3.0

Faça a instalação de acordo com seu sistema operacional 

Iniciar o serviço

```bash
sudo systemctl start grafana-server
```

Verificar se o Grafana está ativo

```bash
systemctl status grafana-server
```

Acessar o grafana no navegador

```bash
http://localhost:3000
```

Login padrão:

```bash
usuário: admin
senha: admin
```
No menu lateral, acessamos a opção Data Sources e escolhemos o InfluxDB como tipo de conexão. Informamos o endereço do banco, que no nosso caso está rodando localmente em:
```bash
http://127.0.0.1:8086
```

Em seguida, selecionamos o banco de dados utilizado pelo Node-RED, como por exemplo telegraf. Como o InfluxDB está trabalhando na versão clássica (v1.x), basta escolher o modo correto no campo Query Language. Depois de preencher todos os parâmetros, clicamos em Save & Test para verificar a conexão.

Se conectou, vc está pronto para criar as visualizações em tempo real no Grafana
