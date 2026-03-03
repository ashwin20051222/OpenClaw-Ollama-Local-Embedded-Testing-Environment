<p align="center">
  <h1 align="center">🦀 OpenClaw & Ollama</h1>
  <h3 align="center">Local Embedded Testing Environment</h3>
  <p align="center">
    A 100% offline, GPU-accelerated AI code reviewer for embedded C &amp; robotics firmware
    <br />
    <em>Runs entirely on your hardware. No cloud. No API keys. No cost.</em>
  </p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/OS-Arch%20Linux-1793D1?style=for-the-badge&logo=archlinux&logoColor=white" alt="Arch Linux" />
  <img src="https://img.shields.io/badge/Runtime-Ollama-000000?style=for-the-badge&logo=ollama&logoColor=white" alt="Ollama" />
  <img src="https://img.shields.io/badge/AI-100%25%20Local-00C853?style=for-the-badge" alt="100% Local" />
  <img src="https://img.shields.io/badge/GPU-NVIDIA%20RTX-76B900?style=for-the-badge&logo=nvidia&logoColor=white" alt="NVIDIA GPU" />
  <img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge" alt="MIT License" />
</p>

---

## 📋 Table of Contents

- [What Is This?](#-what-is-this)
- [Architecture](#-architecture)
- [Key Features](#-key-features)
- [Hardware Requirements](#-hardware-requirements)
- [Supported Models](#-supported-models)
- [Quick Start](#-quick-start)
- [Custom Skills](#-custom-skills)
- [Example: Testing Buggy Code](#-example-testing-buggy-code)
- [OpenClaw vs Raw Ollama / LM Studio](#-openclaw-vs-raw-ollama--lm-studio)
- [Documentation](#-documentation)
- [Project Structure](#-project-structure)
- [License](#-license)

---

## 🧠 What Is This?

This project combines **[Ollama](https://ollama.com)** (a local LLM runtime) with **[OpenClaw](https://openclaw.dev)** (an autonomous AI agent framework) to create a **dedicated embedded systems code tester** that runs entirely on your laptop.

Instead of just chatting with an AI about your code, this setup allows the AI to:

- 📂 **Read your files** directly from your filesystem
- 🔧 **Run GCC** to compile and find real syntax errors
- ⚡ **Flash firmware** to ESP32/STM32 boards via `esptool.py` or `st-flash`
- 🔄 **Self-correct** — if it finds an error, it can fix the code and recompile without your input
- 📊 **Generate bug reports** with exact line numbers, descriptions, and hardware impact analysis

> **Built for EEE students & embedded developers** who work with ESP32, STM32, Raspberry Pi Pico, and similar microcontrollers.

---

## 🏗 Architecture

```
┌──────────────────────────────────────────────────────────┐
│                    Your Arch Linux Laptop                │
│                                                          │
│  ┌─────────────┐    ┌──────────────┐    ┌─────────────┐  │
│  │   Ollama     │◄──►│   OpenClaw   │◄──►│   Custom    │  │
│  │  (LLM Engine)│    │  (Agent Hub) │    │   Skills    │  │
│  │             │    │              │    │             │  │
│  │ qwen2.5-    │    │ System Prompt│    │ c-analyzer  │  │
│  │ coder:1.5b  │    │ + Agent Loop │    │ esp32-flash │  │
│  │             │    │              │    │ stm32-flash │  │
│  └──────┬──────┘    └──────┬───────┘    └──────┬──────┘  │
│         │                  │                   │         │
│         │    Port 11434    │   Port 18789       │         │
│         │  (LLM inference) │  (Web dashboard)   │         │
│         │                  │                   │         │
│  ┌──────▼──────────────────▼───────────────────▼──────┐  │
│  │              NVIDIA RTX GPU (VRAM)                 │  │
│  │           Model loaded & running locally           │  │
│  └────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────┘
```

**Data flow:** You upload C code → OpenClaw reads the file → sends it to Ollama's local model → model analyzes it → OpenClaw uses skills (GCC, esptool) to verify → returns a structured bug report.

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| 🔒 **Fully Offline** | No internet required after initial setup. Zero data leaves your machine. |
| 💰 **Zero Cost** | No API keys, no subscriptions, no cloud credits. |
| 🎯 **Embedded-Focused** | System prompt tuned for GPIO, PWM, ADC, UART/I2C/SPI, PID control, state machines. |
| 🐛 **GCC Integration** | Runs real `gcc -Wall -Wextra -pedantic` checks — not just AI guesses. |
| ⚡ **Board Flashing** | Flash ESP32 and STM32 boards directly from the chat interface. |
| 🔄 **Autonomous Loop** | AI can compile → read errors → fix code → recompile without your input. |
| 📱 **Remote Access** | Optional Telegram/WhatsApp/Slack integration for mobile control. |
| 🛡️ **Sandboxed** | Configurable sandbox mode with read-only workspace access. |

---

## 💻 Hardware Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| **GPU** | Any NVIDIA with 2GB+ VRAM | NVIDIA RTX 2050+ (4GB VRAM) |
| **RAM** | 8 GB | 16 GB |
| **Storage** | 5 GB free | 10 GB free |
| **OS** | Arch Linux (or any systemd-based distro) | Arch Linux |
| **Browser** | Chromium / Firefox | Chromium (for `--app` mode) |

> **Note:** A 1.5B parameter model uses ~1-2 GB of VRAM, leaving plenty of headroom on a 4GB GPU for compilation tasks.

---

## 🤖 Supported Models

These models are tested and optimized for embedded C analysis on laptop hardware:

| Model | Parameters | Best For | Pull Command |
|-------|-----------|----------|-------------|
| **Qwen 2.5 Coder** ⭐ | 1.5B | C/C++ analysis, best overall | `ollama pull qwen2.5-coder:1.5b` |
| **DeepSeek Coder** | 1.3B | Code-focused, strong syntax checking | `ollama pull deepseek-coder:1.3b` |
| **Llama 3.2** | 1B | General purpose fallback | `ollama pull llama3.2:1b` |

> **⭐ Recommended primary model:** `qwen2.5-coder:1.5b` — best balance of speed and C code understanding at the 1B scale.

---

## 🚀 Quick Start

### 1. Install Ollama

```bash
# Arch Linux (AUR)
yay -S ollama

# Or use the official install script
curl -fsSL https://ollama.com/install.sh | sh
```

### 2. Pull a Local Model

```bash
ollama pull qwen2.5-coder:1.5b
```

### 3. Start Ollama (and enable auto-start on boot)

```bash
sudo systemctl enable --now ollama
```

### 4. Install & Configure OpenClaw

Follow the detailed instructions in the **[Setup Guide](docs/SETUP_GUIDE.md)**.

### 5. Install Custom Skills

```bash
# Copy the skills to OpenClaw's skill directory
cp -r skills/esp32-flasher ~/.openclaw/skills/
cp -r skills/c-analyzer ~/.openclaw/skills/
cp -r skills/stm32-flasher ~/.openclaw/skills/
```

### 6. Open the Dashboard

```bash
# Start the gateway and open the web interface
systemctl --user start openclaw-gateway.service
chromium --app=http://127.0.0.1:18789
```

> 📖 For the **full step-by-step walkthrough** including desktop shortcuts and system prompt configuration, see the [Setup Guide](docs/SETUP_GUIDE.md).

---

## 🔧 Custom Skills

OpenClaw skills are executable tools that give the AI the ability to interact with your system. Each skill is a `SKILL.md` file inside `~/.openclaw/skills/<skill-name>/`.

### Included Skills

| Skill | Emoji | Description |
|-------|-------|-------------|
| [c-analyzer](skills/c-analyzer/SKILL.md) | 🐛 | Runs strict GCC checks to find syntax errors and warnings |
| [esp32-flasher](skills/esp32-flasher/SKILL.md) | ⚡ | Flashes compiled firmware to ESP32 boards via `esptool.py` |
| [stm32-flasher](skills/stm32-flasher/SKILL.md) | 🔩 | Flashes firmware to STM32 boards via `st-flash` / `openocd` |

> 📖 Learn how to create your own custom skills in the **[Skills Guide](docs/SKILLS_GUIDE.md)**.

---

## 🧪 Example: Testing Buggy Code

Drop this intentionally buggy C file into the OpenClaw chat to validate your setup:

```c
#include <stdio.h>
#include <stdint.h>

void update_pwm_duty(uint8_t motor_id, uint32_t duty_cycle) {
    uint32_t max_duty = 4096;
    uint32_t *pwm_register; // ⚠️ Danger: Uninitialized pointer!

    if (duty_cycle > max_duty) {
        duty_cycle = max_duty;
    }

    // Bug: Missing semicolon + writing to random memory
    *pwm_register = duty_cycle

    printf("Motor %d PWM updated to %d\n", motor_id, duty_cycle);
}

int main() {
    printf("Starting motor controller...\n");
    update_pwm_duty(1, 2048);
    return 0;
}
```

**Tell the AI:** _"Run the c-analyzer skill on this code and tell me what the GCC compiler finds."_

**Expected result:** The AI should catch:
1. ❌ **Missing semicolon** on the `*pwm_register = duty_cycle` line
2. ⚠️ **Uninitialized pointer** — `pwm_register` doesn't point to a valid hardware address, will cause a Hard Fault on real hardware

> More test files are available in the [`tests/`](tests/) directory.

---

## ⚔️ OpenClaw vs Raw Ollama / LM Studio

| Capability | Ollama / LM Studio | OpenClaw + Ollama |
|-----------|-------------------|-------------------|
| Chat with AI | ✅ | ✅ |
| Read your project files | ❌ (manual copy-paste) | ✅ (automatic) |
| Run GCC compiler | ❌ (copy command, run manually) | ✅ (executes in background) |
| Flash firmware to boards | ❌ | ✅ (via custom skills) |
| Autonomous error correction | ❌ | ✅ (compile → fix → recompile loop) |
| Remote access via phone | ❌ | ✅ (Telegram, WhatsApp, Slack) |
| System shell access | ❌ | ✅ (configurable + sandboxed) |

> **TL;DR:** Ollama is the **brain**. OpenClaw is the **body** (hands + eyes). Together, they turn a chatbot into an autonomous tool-using agent.

---

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [Setup Guide](docs/SETUP_GUIDE.md) | Full step-by-step installation on Arch Linux |
| [Skills Guide](docs/SKILLS_GUIDE.md) | How to create and manage custom OpenClaw skills |
| [Troubleshooting](docs/TROUBLESHOOTING.md) | Common issues and fixes |
| [Security](docs/SECURITY.md) | Sandboxing, risks, and best practices |
| [Contributing](docs/CONTRIBUTING.md) | How to contribute to this project |

---

## 📁 Project Structure

```
.
├── README.md                          # This file
├── LICENSE                            # MIT License
├── config/
│   ├── openclaw-example.json          # Example OpenClaw configuration
│   ├── openclaw.desktop               # Desktop shortcut for OpenClaw
│   ├── start-ollama.desktop           # Desktop shortcut for Ollama
│   └── embedded-tester-prompt.txt     # System prompt for the AI persona
├── docs/
│   ├── SETUP_GUIDE.md                 # Full installation walkthrough
│   ├── SKILLS_GUIDE.md                # Custom skill creation guide
│   ├── TROUBLESHOOTING.md             # Common issues & fixes
│   ├── SECURITY.md                    # Security best practices
│   └── CONTRIBUTING.md                # Contribution guidelines
├── skills/
│   ├── c-analyzer/
│   │   └── SKILL.md                   # GCC static analysis skill
│   ├── esp32-flasher/
│   │   └── SKILL.md                   # ESP32 firmware flashing skill
│   └── stm32-flasher/
│       └── SKILL.md                   # STM32 firmware flashing skill
└── tests/
    ├── motor_test.c                   # Buggy PWM code for testing
    └── state_machine_test.c           # Buggy state machine for testing
```

---

## 📜 License

This project is licensed under the **MIT License** — see [LICENSE](LICENSE) for details.

---

<p align="center">
  <strong>Built with 🧠 Ollama + 🦀 OpenClaw on 🐧 Arch Linux</strong>
  <br />
  <em>By an EEE student, for embedded developers.</em>
</p>
