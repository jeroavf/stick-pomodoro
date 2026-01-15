# Stick Pomodoro - Especificação Técnica

## 1. Visão Geral

**Produto:** Pomodoro Timer embarcado
**Hardware:** M5Stack StickC Plus 2
**Framework:** Arduino
**Propósito:** Gestão de tempo usando a técnica Pomodoro
**Inspiração UI:** [VolosR/tDisplayS3WeatherStation](https://github.com/VolosR/tDisplayS3WeatherStation)

---

## 2. Hardware

### 2.1 M5Stack StickC Plus 2

| Componente | Especificação | Uso no Projeto |
|------------|---------------|----------------|
| MCU | ESP32-PICO-V3-02 | Processamento principal |
| Display | TFT 1.14" 135x240 ST7789V2 | Interface do timer |
| Botão A | GPIO37 (frontal) | Iniciar/Pausar/Confirmar |
| Botão B | GPIO39 (lateral) | Navegação/Menu |
| Botão Power | AXP2101 | Liga/Desliga |
| Buzzer | GPIO2 | Alertas sonoros |
| LED | GPIO19 | Feedback visual |
| WiFi | 802.11 b/g/n | Interface web config |
| RTC | BM8563 | Manter hora |
| Flash | 8MB | Armazenamento |
| Bateria | 200mAh LiPo | Alimentação |

### 2.2 Orientação do Display

O display será usado em **modo landscape (240x135)** para melhor aproveitamento do espaço horizontal, similar ao projeto de referência.

---

## 3. Funcionalidades

### 3.1 Timer Pomodoro

| Modo | Duração Padrão | Cor Indicadora |
|------|----------------|----------------|
| Foco | 25 minutos | Branco/Cinza claro |
| Descanso Curto | 5 minutos | Cinza médio |
| Descanso Longo | 15 minutos | Cinza escuro |

**Ciclo:** `FOCO → CURTO → FOCO → CURTO → FOCO → CURTO → FOCO → LONGO → (repete)`

### 3.2 Alertas (Configurável)

- **Sonoro:** Buzzer com padrões de beep (quantidade configurável 1-10)
- **Visual:** Inversão de cores (quantidade configurável 1-10)
- **Modo:** Somente som | Somente visual | Ambos | Nenhum
- **Volume:** Baixo | Médio | Alto

### 3.3 Interface Web

- Access Point para configuração
- Dashboard com status do timer
- Página de configurações
- Histórico com estatísticas

### 3.4 Estatísticas

- Pomodoros completados por dia
- Tempo total de foco
- Histórico dos últimos 30 dias

---

## 4. Design de Interface (Estilo WeatherStation)

### 4.1 Filosofia de Design

Seguindo o projeto de referência:
- **Estética monocromática** (preto, branco e tons de cinza)
- **Sprites para double-buffering** (sem flicker)
- **Formas geométricas** ao invés de ícones bitmap
- **Layout dividido** em seções funcionais
- **Fontes customizadas** para diferentes hierarquias

### 4.2 Paleta de Cores (Grayscale)

```cpp
// Paleta de 13 tons de cinza (como no projeto de referência)
uint16_t grays[13];

void initGrays() {
    for (int i = 0; i < 13; i++) {
        int value = 210 - (i * 20);  // 210, 190, 170, ... 10
        if (value < 0) value = 0;
        grays[i] = tft.color565(value, value, value);
    }
}

// Índices de uso:
// grays[0]  = Mais claro (210,210,210) - backgrounds
// grays[3]  = Médio claro (150,150,150) - textos secundários
// grays[6]  = Médio (90,90,90) - divisórias
// grays[9]  = Escuro (30,30,30) - textos principais
// grays[12] = Preto (0,0,0) - fundo principal
```

### 4.3 Fontes Customizadas

| Nome | Tamanho | Uso |
|------|---------|-----|
| `fontTiny` | 9px | Labels pequenos |
| `fontSmall` | 12px | Informações secundárias |
| `fontMedium` | 18px | Títulos de seção |
| `fontBig` | 48px | Timer principal |
| `fontHuge` | 64px | Timer em tela cheia |

### 4.4 Layout das Telas

#### Tela Principal - Timer (240x135)

```
┌────────────────────────────────────────────────────────────┐
│ POMODORO                                    ● ● ● ○  1/4   │  <- Header (y: 0-20)
├───────────────────────────────┬────────────────────────────┤
│                               │   HISTÓRICO HOJE           │
│         F O C O               │   ┌──┬──┬──┬──┬──┬──┬──┐   │
│                               │   │▓▓│▓▓│▓▓│░░│░░│░░│░░│   │
│        24:59                  │   └──┴──┴──┴──┴──┴──┴──┘   │
│                               │   Pomodoros: 3             │
│       ▓▓▓▓▓▓▓▓▓▓░░░░         │   Foco: 1h 15m             │
│                               │                            │
├───────────────────────────────┴────────────────────────────┤
│  [A] Pausar          │  WiFi: Pomodoro-A1B2  │  14:32     │  <- Footer (y: 115-135)
└────────────────────────────────────────────────────────────┘

Divisão horizontal: x=138 (similar ao projeto de referência)
```

#### Tela de Descanso

```
┌────────────────────────────────────────────────────────────┐
│ POMODORO                                    ● ● ● ●  4/4   │
├───────────────────────────────┬────────────────────────────┤
│                               │   PRÓXIMO                  │
│      D E S C A N S O          │                            │
│                               │   ┌────────────────────┐   │
│        04:32                  │   │      FOCO          │   │
│                               │   │     25:00          │   │
│       ░░░░░░▓▓▓▓▓▓▓▓         │   └────────────────────┘   │
│                               │                            │
├───────────────────────────────┴────────────────────────────┤
│  [A] Pular             │  Descanso Longo        │  14:57  │
└────────────────────────────────────────────────────────────┘
```

#### Tela de Estatísticas

```
┌────────────────────────────────────────────────────────────┐
│ ESTATÍSTICAS                                        Hoje   │
├────────────────────────────────────────────────────────────┤
│                                                            │
│   ┌─────────────────────────────────────────────────────┐  │
│   │ █ █ █ █ █ █ █ █ █ █ █ █   │ Últimos 12 dias        │  │
│   │ ▓ ▓ ▓ ▓ ░ ░ ░ ░ ░ ░ ░ █   │ Max: 8 pomodoros       │  │
│   └─────────────────────────────────────────────────────┘  │
│                                                            │
│   ┌─────────────┐  ┌─────────────┐  ┌─────────────┐       │
│   │  HOJE       │  │  SEMANA     │  │  MÊS        │       │
│   │  6 pom      │  │  28 pom     │  │  89 pom     │       │
│   │  2h 30m     │  │  11h 40m    │  │  37h 05m    │       │
│   └─────────────┘  └─────────────┘  └─────────────┘       │
│                                                            │
├────────────────────────────────────────────────────────────┤
│  [A] Voltar                   │  [B] Config    │  14:32   │
└────────────────────────────────────────────────────────────┘
```

#### Tela de Configuração WiFi

```
┌────────────────────────────────────────────────────────────┐
│ CONFIGURAÇÃO                                               │
├────────────────────────────────────────────────────────────┤
│                                                            │
│   Conecte-se à rede WiFi:                                  │
│                                                            │
│   ┌─────────────────────────────────┐                      │
│   │  SSID: Pomodoro-A1B2            │                      │
│   │  Senha: pomodoro123             │                      │
│   └─────────────────────────────────┘                      │
│                                                            │
│   Acesse: http://192.168.4.1                               │
│                                                            │
├────────────────────────────────────────────────────────────┤
│  [A] Voltar                                      │  WiFi ● │
└────────────────────────────────────────────────────────────┘
```

### 4.5 Elementos Gráficos

#### Barra de Progresso do Timer

```cpp
// Barra de progresso com cantos arredondados
void drawProgressBar(int x, int y, int w, int h, float progress) {
    int fillWidth = (int)(w * progress);

    // Fundo da barra
    sprite.fillSmoothRoundRect(x, y, w, h, 4, grays[10]);

    // Preenchimento
    if (fillWidth > 0) {
        sprite.fillSmoothRoundRect(x, y, fillWidth, h, 4, grays[2]);
    }
}
```

#### Indicador de Ciclos (Dots)

```cpp
// Círculos indicando pomodoros completados
void drawCycleIndicator(int x, int y, int completed, int total) {
    int spacing = 12;
    for (int i = 0; i < total; i++) {
        if (i < completed) {
            sprite.fillCircle(x + (i * spacing), y, 4, grays[2]);  // Preenchido
        } else {
            sprite.drawCircle(x + (i * spacing), y, 4, grays[6]);  // Contorno
        }
    }
}
```

#### Gráfico de Barras (Histórico)

```cpp
// Gráfico de barras verticais para histórico
void drawBarChart(int x, int y, int w, int h, int* data, int count, int maxVal) {
    int barWidth = w / count - 2;

    for (int i = 0; i < count; i++) {
        int barHeight = map(data[i], 0, maxVal, 0, h);
        int barX = x + (i * (barWidth + 2));
        int barY = y + h - barHeight;

        // Cor baseada no valor (mais escuro = mais produtivo)
        int colorIdx = map(data[i], 0, maxVal, 8, 2);
        sprite.fillRect(barX, barY, barWidth, barHeight, grays[colorIdx]);
    }
}
```

### 4.6 Animações

| Evento | Animação |
|--------|----------|
| Timer acabou | Inversão de cores pulsante (3x) |
| Novo pomodoro | Fade in do timer |
| Mudança de tela | Transição horizontal |
| Pausa | Pulsação suave do timer |

---

## 5. Controles

### 5.1 Mapeamento de Botões

| Botão | Pressão Curta (<500ms) | Pressão Longa (>2s) |
|-------|------------------------|---------------------|
| **Botão A** (frontal) | Iniciar/Pausar timer | Resetar timer |
| **Botão B** (lateral) | Próxima tela | Ativar modo WiFi |

### 5.2 Navegação entre Telas

```
Timer Principal ←→ Estatísticas ←→ (Config WiFi quando ativado)
      ↑                  ↑
      └──── Botão B ─────┘
```

---

## 6. Interface Web

### 6.1 Conexão

```
SSID: Pomodoro-XXXX (últimos 4 dígitos do MAC)
Senha: pomodoro123
IP: 192.168.4.1
Porta: 80
```

### 6.2 Páginas

| Rota | Descrição |
|------|-----------|
| `/` | Dashboard - status atual e controles |
| `/config` | Configurações do timer |
| `/history` | Histórico completo |
| `/api/*` | Endpoints REST |

### 6.3 API REST

```
GET  /api/status     → { state, remaining, cycle, pomodoros_today }
GET  /api/config     → { focus_min, short_min, long_min, cycles, alerts }
POST /api/config     → Atualizar configurações
GET  /api/history    → Array com histórico de 30 dias
POST /api/control    → { action: "start" | "pause" | "reset" | "skip" }
```

---

## 7. Persistência

### 7.1 Preferences (NVS)

```cpp
// Namespace: "pomodoro"
Preferences prefs;

struct Settings {
    uint8_t focusMin = 25;
    uint8_t shortBreakMin = 5;
    uint8_t longBreakMin = 15;
    uint8_t cyclesForLong = 4;
    uint8_t alertMode = 3;   // 0=none, 1=sound, 2=visual, 3=both
    uint8_t volume = 1;      // 0=low, 1=medium, 2=high
    uint8_t beepCount = 3;   // Number of beeps (1-10)
    uint8_t flashCount = 3;  // Number of flashes (1-10)
};
```

### 7.2 LittleFS - Histórico

**Arquivo:** `/history.json`

```json
{
    "days": [
        {
            "date": "2025-01-14",
            "pomodoros": 6,
            "focus_seconds": 9000,
            "cycles_completed": 1
        }
    ]
}
```

---

## 8. Arquitetura Arduino

### 8.1 Estrutura de Arquivos

```
StickPomodoro/
├── StickPomodoro.ino       # Setup e loop principal
├── config.h                # Definições e constantes
├── types.h                 # Structs e enums
├── display.h               # Funções de renderização
├── display.cpp
├── timer.h                 # Lógica do Pomodoro
├── timer.cpp
├── storage.h               # Persistência (Prefs + LittleFS)
├── storage.cpp
├── alerts.h                # Buzzer e alertas visuais
├── alerts.cpp
├── buttons.h               # Tratamento de botões
├── buttons.cpp
├── webserver.h             # Servidor HTTP + páginas web
└── webserver.cpp           # HTML/CSS/JS embutidos via PROGMEM
```

### 8.2 Bibliotecas Arduino

```cpp
// Core
#include <M5StickCPlus2.h>      // Hardware abstraction (inclui display M5Canvas)
#include <WiFi.h>               // WiFi connectivity
#include <Preferences.h>        // NVS storage
#include <LittleFS.h>           // File system
#include <ArduinoJson.h>        // JSON parsing
#include <ESP32Time.h>          // RTC handling
```

### 8.3 Configuração Arduino IDE

- **Board:** M5Stick-C (ESP32)
- **Upload Speed:** 1500000
- **Partition Scheme:** Default 4MB with spiffs

**Bibliotecas necessárias (Library Manager):**
- M5StickCPlus2
- ArduinoJson
- ESP32Time

### 8.4 Fluxo Principal

```cpp
// StickPomodoro.ino

#include <M5StickCPlus2.h>
#include <ESP32Time.h>
#include "config.h"
#include "types.h"
#include "display.h"
#include "timer.h"
#include "storage.h"
#include "alerts.h"
#include "buttons.h"
#include "webserver.h"

Settings settings;
ESP32Time rtc;

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);

    displayInit();
    storageInit();
    loadSettings(&settings);
    timerInit();
    alertsInit();
    buttonsInit();
}

void loop() {
    M5.update();
    buttonsUpdate();
    handleButtons();

    if (timerUpdate(&settings) && timerJustFinished()) {
        alertPlay(settings.alertMode, settings.volume,
                  settings.beepCount, settings.flashCount);
    }

    alertsUpdate();

    if (wifiEnabled) {
        webServerHandle();
    }

    updateDisplay();
    delay(10);
}
```

---

## 9. Estados do Sistema

```
┌─────────┐
│  IDLE   │◄────────────────────────────┐
└────┬────┘                             │
     │ BtnA                             │
     ▼                                  │
┌─────────┐     timeout    ┌──────────┐ │
│  FOCUS  │───────────────►│  SHORT   │ │
│         │◄───────────────│  BREAK   │ │
└────┬────┘    timeout     └──────────┘ │
     │                                  │
     │ (4 ciclos)                       │
     ▼                                  │
┌─────────┐     timeout                 │
│  LONG   │─────────────────────────────┘
│  BREAK  │
└─────────┘

Qualquer estado + BtnA = PAUSED
PAUSED + BtnA = Resume
Qualquer estado + BtnA(longo) = IDLE
```

---

## 10. Consumo de Energia

| Estado | Consumo | Autonomia (200mAh) |
|--------|---------|-------------------|
| Timer ativo | ~80mA | ~2.5h |
| Display dim | ~40mA | ~5h |
| WiFi AP ativo | ~120mA | ~1.5h |
| Deep sleep | ~0.3mA | ~27 dias |

---

## 11. Critérios de Aceite

- [x] Timer preciso (±1 segundo por ciclo de 25min)
- [x] UI renderiza sem flicker (double-buffering com M5Canvas)
- [x] Alertas sonoros/visuais funcionais e configuráveis
- [x] Interface web responsiva
- [x] Configurações persistem após reinício
- [ ] Histórico armazena 30 dias
- [ ] Autonomia mínima de 2 horas

---

## 12. Referências

- [M5StickC Plus 2 Docs](https://docs.m5stack.com/en/core/M5StickC%20PLUS2)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [WeatherStation UI Reference](https://github.com/VolosR/tDisplayS3WeatherStation)
