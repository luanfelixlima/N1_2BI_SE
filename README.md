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
- Na qual os intervalos são:
  
   Faixa de Temperatura: 15 < t < 25 ºC

   Faixa de Luminosidade: 0 < l < 30%

   Faixa de Umidade: 30% < u < 50%

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
Vide esquema abaixo:
![image](https://github.com/user-attachments/assets/ee45bd73-7c83-484e-bc2e-ede10a0efd4f)

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

---

# Project: Data Collection and Cloud Upload with ESP32 and Azure

## Project Objective

This project aims to collect **temperature**, **humidity**, and **light** data from a **DHT11** and **LDR** sensors connected to an **ESP32**. After the data is collected, it is uploaded to the cloud (Azure), processed, and stored in a **MongoDB** database. A dashboard allows the user to view the last 30 records and provides alerts when any variable goes out of the specified range.

## Components Used

- **ESP32**: Microcontroller with Wi-Fi and Bluetooth connectivity.
- **DHT11**: Temperature and humidity sensor.
- **LDR**: Light-dependent resistor (light sensor).
- **ESP32 On-Board LED**: Indicator to alert when temperature, humidity, or light values are out of range.
- **Microsoft Azure**: Cloud platform for data storage and analysis.
- **MQTT**: Messaging protocol for data transmission.
- **MongoDB**: NoSQL database for storing the collected data.
- **SHT-Comm**: Used for data transmission and dashboard creation.

---

## Project Workflow

### 1. Sensor Data Collection

- **DHT11**: The temperature and humidity sensor is read periodically by the ESP32, which returns the temperature (°C) and relative humidity (%).
- **LDR**: The light sensor is read by the ESP32, determining the light intensity in the environment, usually expressed as a variable resistance.

These sensors are connected to the ESP32 and their values are read on each collection cycle, with the data being temporarily stored in memory.

### 2. Average Calculation

Before sending the data to the cloud, the average of the values collected over the last 60 seconds (1 minute) is calculated. This average helps reduce rapid fluctuations and ensures that the data sent to the cloud represents a more stable measure.

- Temperature Average (°C)
- Humidity Average (%)
- Light Average (lux or resistance, depending on the configuration)

### 3. Sending Data to the Cloud

The data is sent to **Microsoft Azure** using the **MQTT** protocol. MQTT is efficient for lightweight and reliable messaging between devices and the cloud.

**Steps for sending data to the cloud**:
- The ESP32 connects to the Wi-Fi network and establishes a connection with the MQTT broker.
- The calculated data (average for the last minute) is sent to the configured MQTT topic.
- Azure processes the received data and stores it in a **MongoDB** database for later queries and analysis.
- In which the intervals are:
  
   Temperature Range: 15 < t < 25 ºC

   Light Intensity Range: 0 < l < 30%

   Humidity Range: 30% < u < 50%

### 4. Data Storage in MongoDB

After receiving the data via MQTT, Azure processes and stores this information in a **MongoDB** database. Each entry in the database contains:
- Timestamp (date and time of the reading)
- Average temperature
- Average humidity
- Average light intensity

These data entries are available for queries and report generation in the **Dashboard**.

### 5. Dashboard Generation

- The **Dashboard** is a graphical interface that displays the last 30 collected records.
- The Dashboard is generated using the **SHT-Comm** tool, which integrates with MongoDB and enables real-time data visualization.
- On the dashboard, graphs are displayed for each variable (temperature, humidity, light) showing their average values over time.

### 6. Anomaly Alert

Whenever a collected value is out of the expected range for each variable (e.g., temperature above 35°C or below 15°C, humidity above 90%, light below 10 lux), the ESP32 turns on the **on-board LED** as a visual alert.

- The LED lights up when one or more of the collected values fall outside the specified range.
- Additionally, on the **Dashboard**, the anomaly is visually indicated on the graph, allowing the user to easily spot when values are outside the safe range.

---

## Flow Diagram

1. **Sensor Reading** (DHT11 and LDR)
   - The ESP32 collects the temperature, humidity, and light data.

2. **Average Calculation**
   - The averages of the collected variables over the last 60 seconds are calculated.

3. **Sending Data**
   - The data is sent via MQTT to the Azure platform.

4. **Data Storage in MongoDB**
   - The data is stored in MongoDB with a timestamp.

5. **Dashboard Visualization**
   - The Dashboard displays the last 30 records and real-time graphs.

6. **Anomaly Alert**
   - If any value goes out of range, the LED lights up and the anomaly is visible on the graph.

---

## Technical Considerations

### MQTT (Message Queuing Telemetry Transport)

**MQTT** is a lightweight messaging protocol ideal for devices with limited resources, like the ESP32. It sends data efficiently and in real-time to the cloud, using topics and messages. In this project, MQTT facilitates continuous data transmission to Azure.

### Azure IoT Hub and MongoDB

- **Azure IoT Hub** manages communication between devices (ESP32) and the cloud, providing a secure gateway for device data.
- **MongoDB** is used for storing data on Azure, as it is a flexible NoSQL database capable of handling large volumes of unstructured data, making it easy to query in real-time.

---

## Conclusion

This project provides a practical solution for remotely monitoring environmental variables such as temperature, humidity, and light intensity, using the ESP32 and the Azure platform. By integrating sensors, MQTT, storage in MongoDB, and real-time dashboard visualization, it is possible not only to monitor data but also to ensure the system visually alerts and displays anomalies through graphs efficiently.

---
