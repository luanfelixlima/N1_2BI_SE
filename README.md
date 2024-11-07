# Projeto: Coleta e Envio de Dados de Sensores para a Nuvem com ESP32 e Azure

## Objetivo do Projeto

Este projeto tem como objetivo coletar dados de sensores de **temperatura**, **umidade** e **luminosidade** a partir de um **DHT11** e **LDR** conectados a um **ESP32**. Após a coleta, os dados são enviados para a nuvem (Azure), processados e armazenados em um banco de dados **MongoDB**. O painel (Dashboard) permite visualizar os 30 últimos registros e alerta quando algum valor de variável está fora do intervalo especificado.

## Componentes Utilizados

- **ESP32**: Microcontrolador com conectividade Wi-Fi e Bluetooth.
- **DHT11**: Sensor de temperatura e umidade.
- **LDR**: Sensor de luminosidade (dependente de luz).
- **LED On-Board do ESP32**: Indicador para alertar quando valores de temperatura, umidade ou luminosidade estão fora do intervalo especificado.
- **Microsoft Azure**: Plataforma de nuvem para o armazenamento e análise de dados.
- **MQTT**: Protocolo de comunicação para o envio de dados.
- **MongoDB**: Banco de dados NoSQL para armazenar os dados coletados.
- **SHT-Comm**: Utilizado para o envio de dados e a criação do Dashboard.

---

## Fluxo do Projeto

### 1. Coleta de Dados com os Sensores

- **DHT11**: O sensor de temperatura e umidade é lido periodicamente pelo ESP32, que retorna a temperatura (em °C) e a umidade relativa (em %).
- **LDR**: O sensor de luminosidade é lido pelo ESP32, que determina a intensidade de luz no ambiente, geralmente expressa como uma resistência variável.

Esses sensores são conectados ao ESP32 e têm seus valores lidos a cada ciclo de coleta, no qual as variáveis são extraídas e armazenadas em memória temporária.

### 2. Cálculo da Média

Antes de enviar os dados para a nuvem, é calculada a média dos valores coletados nos últimos 60 segundos (1 minuto). Essa média é usada para reduzir flutuações rápidas e garantir que os dados enviados para a nuvem representem uma medida mais estável.

- Média de Temperatura (°C)
- Média de Umidade (%)
- Média de Luminosidade (lux ou resistência, dependendo da configuração)

### 3. Envio dos Dados para a Nuvem

Os dados são enviados para a plataforma **Microsoft Azure** utilizando o protocolo **MQTT**. A comunicação MQTT é eficiente para a troca de mensagens entre dispositivos e a nuvem, garantindo que os dados sejam enviados de forma leve e confiável.

**Passos do envio para a nuvem**:
- O ESP32 se conecta à rede Wi-Fi e estabelece uma conexão com o broker MQTT.
- Os dados calculados (média do último minuto) são enviados para o tópico MQTT configurado.
- O Azure processa os dados recebidos e os armazena em um banco de dados **MongoDB** para posterior consulta e análise.

### 4. Armazenamento dos Dados no MongoDB

Após o recebimento dos dados via MQTT, o Azure processa e armazena essas informações em um banco de dados **MongoDB**. Cada entrada no banco contém:
- Timestamp (data e hora da leitura)
- Temperatura média
- Umidade média
- Luminosidade média

Esses dados ficam disponíveis para consultas e geração de relatórios no **Dashboard**.

### 5. Geração do Dashboard

- O **Dashboard** é uma interface gráfica que exibe os últimos 30 registros coletados.
- O Dashboard é gerado utilizando a ferramenta **SHT-Comm**, que integra com o MongoDB e permite a visualização em tempo real dos dados.
- No painel, é possível ver gráficos para cada variável (temperatura, umidade, luminosidade) com os valores médios ao longo do tempo.

### 6. Alerta de Anomalia

Sempre que um valor coletado estiver fora do intervalo esperado para cada variável (por exemplo, temperatura acima de 35°C ou abaixo de 15°C, umidade acima de 90%, luminosidade abaixo de 10 lux), o ESP32 acende um **LED on-board** como forma de alerta visual.

- O LED se acende quando um ou mais dos valores coletados estão fora do intervalo especificado.
- Além disso, no **Dashboard**, a anomalia também é indicada no gráfico, permitindo que o usuário veja visualmente quando os valores estão fora da faixa segura.

---

## Diagrama de Fluxo

1. **Leitura dos Sensores** (DHT11 e LDR)
   - O ESP32 coleta os dados de temperatura, umidade e luminosidade.
   
2. **Cálculo da Média**
   - As médias das variáveis coletadas nos últimos 60 segundos são calculadas.

3. **Envio dos Dados**
   - Os dados são enviados via MQTT para a plataforma Azure.

4. **Armazenamento em MongoDB**
   - Os dados são armazenados no MongoDB com um timestamp.

5. **Visualização no Dashboard**
   - O Dashboard exibe os 30 últimos registros e gráficos em tempo real.

6. **Alerta de Anomalia**
   - Se algum valor estiver fora do intervalo, o LED acende e a anomalia é visível no gráfico.

---

## Considerações Técnicas

### MQTT (Message Queuing Telemetry Transport)

**MQTT** é um protocolo de mensagens leve, ideal para dispositivos com recursos limitados, como o ESP32. Ele envia dados de forma eficiente e em tempo real para a nuvem, utilizando tópicos e mensagens. No projeto, a comunicação MQTT facilita o envio contínuo de dados para o Azure.

### Azure IoT Hub e MongoDB

- **Azure IoT Hub** gerencia a comunicação entre dispositivos (ESP32) e a nuvem, fornecendo um gateway seguro para dados de dispositivos.
- **MongoDB** é utilizado para armazenar os dados no Azure, pois é um banco de dados NoSQL flexível, capaz de lidar com grandes volumes de dados não estruturados e de fácil acesso para consultas em tempo real.

---

## Conclusão

Este projeto fornece uma solução prática para monitoramento remoto de variáveis ambientais, como temperatura, umidade e luminosidade, utilizando o ESP32 e a plataforma Azure. Com a integração de sensores, MQTT, armazenamento no MongoDB e a visualização no Dashboard, é possível não apenas monitorar dados em tempo real, mas também garantir que o sistema emita alertas visuais e gráficos para anomalias de forma eficiente.
