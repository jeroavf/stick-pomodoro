# Stick Pomodoro

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

### Instalacao

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
