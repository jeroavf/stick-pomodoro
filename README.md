# Stick Pomodoro

Pomodoro Timer for M5Stack StickC Plus 2.

*[Versão em Português](#português)*

## Features

- Pomodoro Timer: 25 min focus / 5 min short break / 15 min long break
- Grayscale UI inspired by [tDisplayS3WeatherStation](https://github.com/VolosR/tDisplayS3WeatherStation)
- Configurable sound and visual alerts (beeps/flashes count)
- Web interface for settings (Access Point mode)
- Persistent settings (survive restart)
- Pomodoro history tracking

## Hardware

- M5Stack StickC Plus 2
- TFT Display 1.14" (240x135 landscape)
- ESP32-PICO-V3-02

## Build

### Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) 2.x
- Board Manager: ESP32 by Espressif

### Libraries (Library Manager)

- M5StickCPlus2
- ArduinoJson
- ESP32Time

### Installation

1. Install Arduino IDE
2. Add ESP32 URL in **Preferences > Additional Board URLs**:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. Install **esp32 by Espressif** in Board Manager
4. Install libraries via Library Manager
5. Select board: **M5Stick-C**
6. Open `StickPomodoro/StickPomodoro.ino`
7. Compile and upload

## Controls

| Button | Short Press | Long Press (2s) |
|--------|-------------|-----------------|
| A (front) | Start/Pause | Reset timer |
| B (side) | Next screen | Toggle WiFi |

## Web Interface

1. Long press Button B (2s) to enable WiFi
2. Connect to `Pomodoro-XXXX` network (password: `pomodoro123`)
3. Open `http://192.168.4.1`

### Available Settings

| Parameter | Default | Range |
|-----------|---------|-------|
| Focus Time | 25 min | 1-60 min |
| Short Break | 5 min | 1-30 min |
| Long Break | 15 min | 1-60 min |
| Cycles for Long Break | 4 | 1-10 |
| Alert Mode | Both | None/Sound/Visual/Both |
| Volume | Medium | Low/Medium/High |
| Beep Count | 3 | 1-10 |
| Flash Count | 3 | 1-10 |

## Project Structure

```
StickPomodoro/
├── StickPomodoro.ino   # Main
├── config.h            # Constants
├── types.h             # Structs/Enums
├── display.h/cpp       # UI (M5Canvas)
├── timer.h/cpp         # Pomodoro logic
├── storage.h/cpp       # NVS persistence
├── alerts.h/cpp        # Buzzer/Visual
├── buttons.h/cpp       # Input handling
└── webserver.h/cpp     # HTTP + HTML
```

## License

MIT License - see [LICENSE](LICENSE)

---

# Português

Pomodoro Timer embarcado para M5Stack StickC Plus 2.

## Funcionalidades

- Timer Pomodoro: 25 min foco / 5 min descanso / 15 min descanso longo
- Interface grayscale inspirada em [tDisplayS3WeatherStation](https://github.com/VolosR/tDisplayS3WeatherStation)
- Alertas sonoros e visuais configuráveis (quantidade de beeps/flashes)
- Interface web para configurações (Access Point)
- Configurações persistentes (sobrevivem a reinício)
- Histórico de pomodoros

## Hardware

- M5Stack StickC Plus 2
- Display TFT 1.14" (240x135 landscape)
- ESP32-PICO-V3-02

## Build

### Requisitos

- [Arduino IDE](https://www.arduino.cc/en/software) 2.x
- Board Manager: ESP32 by Espressif

### Bibliotecas (Library Manager)

- M5StickCPlus2
- ArduinoJson
- ESP32Time

### Instalação

1. Instale o Arduino IDE
2. Adicione o URL do ESP32 em **Preferences > Additional Board URLs**:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. Instale **esp32 by Espressif** no Board Manager
4. Instale as bibliotecas via Library Manager
5. Selecione a board: **M5Stick-C**
6. Abra `StickPomodoro/StickPomodoro.ino`
7. Compile e faça upload

## Controles

| Botão | Pressão Curta | Pressão Longa (2s) |
|-------|---------------|-------------------|
| A (frontal) | Iniciar/Pausar | Resetar timer |
| B (lateral) | Próxima tela | Ativar/Desativar WiFi |

## Interface Web

1. Pressione Botão B por 2s para ativar WiFi
2. Conecte-se à rede `Pomodoro-XXXX` (senha: `pomodoro123`)
3. Acesse `http://192.168.4.1`

### Configurações Disponíveis

| Parâmetro | Padrão | Range |
|-----------|--------|-------|
| Tempo de Foco | 25 min | 1-60 min |
| Descanso Curto | 5 min | 1-30 min |
| Descanso Longo | 15 min | 1-60 min |
| Ciclos para Descanso Longo | 4 | 1-10 |
| Modo de Alerta | Ambos | Nenhum/Som/Visual/Ambos |
| Volume | Médio | Baixo/Médio/Alto |
| Número de Beeps | 3 | 1-10 |
| Número de Flashes | 3 | 1-10 |

## Estrutura do Projeto

```
StickPomodoro/
├── StickPomodoro.ino   # Main
├── config.h            # Constantes
├── types.h             # Structs/Enums
├── display.h/cpp       # UI (M5Canvas)
├── timer.h/cpp         # Lógica Pomodoro
├── storage.h/cpp       # Persistência NVS
├── alerts.h/cpp        # Buzzer/Visual
├── buttons.h/cpp       # Input handling
└── webserver.h/cpp     # HTTP + HTML
```

## Licença

MIT License - veja [LICENSE](LICENSE)
