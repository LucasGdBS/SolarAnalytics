# Raspberry Pi

## Configuração do Broker MQTT

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
