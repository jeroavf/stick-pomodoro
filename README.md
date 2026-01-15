# Stick Pomodoro

Pomodoro Timer embarcado para M5Stack StickC Plus 2.

## Funcionalidades

- Timer Pomodoro: 25 min foco / 5 min descanso / 15 min descanso longo
- Interface grayscale inspirada em [tDisplayS3WeatherStation](https://github.com/VolosR/tDisplayS3WeatherStation)
- Alertas sonoros e visuais configuráveis
- Interface web para configurações
- Histórico de 30 dias

## Hardware

- M5Stack StickC Plus 2
- Display TFT 1.14" (135x240)
- ESP32-PICO-V3-02

## Build

### Requisitos

- [Arduino IDE](https://www.arduino.cc/en/software) 2.x
- Board Manager: ESP32 by Espressif
- Bibliotecas:
  - M5StickCPlus2
  - ArduinoJson
  - ESP32Time

### Instalacao

1. Instale o Arduino IDE
2. Adicione o URL do ESP32 em Preferences > Additional Board URLs:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. Instale "esp32 by Espressif" no Board Manager
4. Instale as bibliotecas via Library Manager:
   - M5StickCPlus2
   - ArduinoJson
   - ESP32Time
5. Selecione a board: **M5Stick-C**
6. Abra `StickPomodoro/StickPomodoro.ino`
7. Compile e faça upload

## Controles

| Botão | Pressão Curta | Pressão Longa (2s) |
|-------|---------------|-------------------|
| A (frontal) | Iniciar/Pausar | Resetar |
| B (lateral) | Próxima tela | Ativar WiFi |

## Interface Web

1. Pressione Botão B por 2s para ativar WiFi
2. Conecte-se à rede `Pomodoro-XXXX`
3. Acesse `http://192.168.4.1`

## Licença

MIT License - veja [LICENSE](LICENSE)
