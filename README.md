GS – Edge Computing & Computer Systems
📡 Sistema de Monitoramento Inteligente de Ambiente – Grupo IZI-Ia

Este projeto integra um ESP32 ao ecossistema FIWARE utilizando o protocolo MQTT para criar um sistema de monitoramento ambiental em tempo real.
O dispositivo coleta dados do ambiente (luminosidade, temperatura, umidade e presença) e também permite o controle remoto do LED onboard, funcionando como um nó IoT completo dentro da arquitetura de computação de borda.

👨‍💻 Integrantes do Grupo

Enrico Bagli RM:562541
Joao Cazzarini RM:

⚙️ Recursos Implementados
📶 Conectividade

Conexão automática à rede Wi-Fi configurada no código.

Estabelecimento de comunicação com o Broker MQTT responsável pela integração com o FIWARE.

💡 Controle Remoto

O LED onboard do ESP32 (GPIO 2) pode ser acionado via comandos MQTT:

Comando MQTT	Função
`lamp001@on	`
`lamp001@off	`

Status do LED é publicado periodicamente em:
/TEF/lamp001/attrs

🌡️ Coleta de Dados Ambientais

O sistema realiza leitura periódica e publica os valores nos tópicos correspondentes:

Sensor	Tópico MQTT	Descrição
Luminosidade (potenciômetro/LDR – GPIO34)	/TEF/lamp001/attrs/l	Percentual de luz
Temperatura (DHT22)	/TEF/lamp001/attrs/t	°C
Umidade (DHT22)	/TEF/lamp001/attrs/h	%
Presença (PIR)	/TEF/lamp001/attrs/p	1 = detectado / 0 = ausente
🛠️ Stack Tecnológica

ESP32 DevKit

Arduino IDE

Bibliotecas:

WiFi.h

PubSubClient.h

DHT.h

FIWARE Orion Context Broker

Protocolo MQTT

Computação de Borda / Edge Computing

🔌 Mapeamento de Hardware
Componente	Porta/Pino	Observação
LED onboard	GPIO 2	Indicador visual
Sensor de luminosidade	GPIO 34	Leitura analógica
Sensor PIR	GPIO 15	Detecção de movimento
DHT22 (Temp/Umidade)	GPIO 4	Comunicação digital
📡 Estrutura de Tópicos MQTT
📥 Tópico de Entrada (Comandos):

"/TEF/lamp001/cmd" — Recebe comandos de controle (ex.: ligar/desligar LED)

📤 Tópicos de Publicação:

Estado do LED → "/TEF/lamp001/attrs"

Luminosidade → "/TEF/lamp001/attrs/l"

Temperatura → "/TEF/lamp001/attrs/t"

Umidade → "/TEF/lamp001/attrs/h"

Presença → "/TEF/lamp001/attrs/p"

📘 Descrição Resumida do Funcionamento

O ESP32 inicializa sensores e estabelece conexão Wi-Fi.

O firmware conecta ao Broker MQTT configurado.

Periodicamente, o dispositivo coleta informações ambientais e publica nos tópicos.

O LED pode ser ligado ou desligado remotamente mediante mensagens MQTT.

O FIWARE Orion recebe, trata e disponibiliza o contexto para aplicações externas.

🚀 Objetivo Geral do Projeto

Demonstrar a implementação prática de um dispositivo IoT usando conceitos de computação de borda, integrando hardware, comunicação em rede e gerenciamento de contexto através do FIWARE, aplicando boas práticas de sistemas embarcados e IoT.
