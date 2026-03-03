# 🛠 Setup Guide — Arch Linux

Complete step-by-step instructions to install and configure OpenClaw + Ollama as a local embedded testing environment.

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Step 1: Install Ollama](#step-1-install-ollama)
- [Step 2: Pull Local Models](#step-2-pull-local-models)
- [Step 3: Auto-Start Ollama on Boot](#step-3-auto-start-ollama-on-boot)
- [Step 4: Install OpenClaw](#step-4-install-openclaw)
- [Step 5: Configure OpenClaw for Local LLM](#step-5-configure-openclaw-for-local-llm)
- [Step 6: Set the Embedded Tester System Prompt](#step-6-set-the-embedded-tester-system-prompt)
- [Step 7: Install Custom Skills](#step-7-install-custom-skills)
- [Step 8: Create Desktop Shortcuts](#step-8-create-desktop-shortcuts)
- [Step 9: Verify Everything Works](#step-9-verify-everything-works)

---

## Prerequisites

| Requirement | Details |
|-------------|---------|
| **OS** | Arch Linux (or Arch-based: Manjaro, EndeavourOS) |
| **GPU** | NVIDIA GPU with 2GB+ VRAM (RTX 2050 recommended) |
| **NVIDIA Drivers** | Installed and working (`nvidia-smi` should show your GPU) |
| **AUR Helper** | `yay` or `paru` (for installing AUR packages) |
| **Browser** | Chromium (recommended) or Firefox |
| **Build Tools** | `gcc` installed (`sudo pacman -S gcc`) |

### Verify GPU is working

```bash
nvidia-smi
```

You should see your GPU listed with driver version and VRAM information.

---

## Step 1: Install Ollama

### Option A: From the AUR (recommended for Arch)

```bash
yay -S ollama
```

### Option B: Official install script

```bash
curl -fsSL https://ollama.com/install.sh | sh
```

### Verify installation

```bash
ollama --version
```

---

## Step 2: Pull Local Models

Download the models you want to use. We recommend all three for a robust fallback chain:

```bash
# Primary model — best for C/C++ code analysis
ollama pull qwen2.5-coder:1.5b

# First fallback — strong code-focused model
ollama pull deepseek-coder:1.3b

# Second fallback — general purpose
ollama pull llama3.2:1b
```

### Verify models are downloaded

```bash
ollama list
```

You should see all three models listed with their sizes.

---

## Step 3: Auto-Start Ollama on Boot

So you never have to manually start Ollama again after powering on:

```bash
sudo systemctl enable --now ollama
```

This does two things:
- `enable` — starts Ollama automatically on every boot
- `--now` — also starts it right now

### Verify Ollama is running

```bash
systemctl status ollama
```

You should see `active (running)` in green.

### Manual start/stop (if needed)

```bash
# Start manually
sudo systemctl start ollama

# Stop manually
sudo systemctl stop ollama

# Restart
sudo systemctl restart ollama
```

---

## Step 4: Install OpenClaw

> Refer to [OpenClaw's official documentation](https://openclaw.dev) for the latest installation method on Arch Linux.

After installation, start the gateway:

```bash
systemctl --user start openclaw-gateway.service
```

### Verify the gateway is running

Open your browser and navigate to:

```
http://127.0.0.1:18789
```

You should see the OpenClaw Control dashboard.

> ⚠️ **Port conflict?** If port `18789` is already in use, see [Troubleshooting](TROUBLESHOOTING.md#port-18789-already-in-use).

---

## Step 5: Configure OpenClaw for Local LLM

This is the critical step — switching OpenClaw from cloud models to your local Ollama engine.

### Via the Web UI

1. Open `http://127.0.0.1:18789` in Chromium
2. Navigate to **Settings** → **Config** in the left sidebar
3. Go to the **Agents** section → **Defaults** tab
4. Set the following values:

| Setting | Value |
|---------|-------|
| **Primary Model** | `ollama/qwen2.5-coder:1.5b` |
| **Fallback 1** | `ollama/deepseek-coder:1.3b` |
| **Fallback 2** | `ollama/llama3.2:1b` |

5. In the **Models / Providers** section, ensure the Ollama provider is configured:

| Setting | Value |
|---------|-------|
| **Base URL** | `http://127.0.0.1:11434/v1` |
| **API Type** | `openai-completions` |

6. Under **Skills → Allow Bundled**, set the value to `*` (wildcard — enables all built-in tools)
7. Click **Save** → **Apply**

### Via the JSON config (alternative)

You can directly edit your `openclaw.json` configuration file. See [`config/openclaw-example.json`](../config/openclaw-example.json) for a complete working example.

> ⚠️ **Important:** Leave the "Remote Embedding API Key" and "Remote Embedding Base URL" fields **blank**. You don't need cloud embeddings for local operation.

---

## Step 6: Set the Embedded Tester System Prompt

This tells the AI *how* to behave — transforming a generic chatbot into a strict embedded C code reviewer.

1. In the OpenClaw dashboard, go to **Agents** in the left sidebar
2. Find your agent's **System Prompt** / **Instructions** text box
3. Paste the contents of [`config/embedded-tester-prompt.txt`](../config/embedded-tester-prompt.txt)
4. Click **Save** → **Apply**

---

## Step 7: Install Custom Skills

Copy the included skill folders to OpenClaw's skill directory:

```bash
# Create the skills directory if it doesn't exist
mkdir -p ~/.openclaw/skills

# Copy all skills
cp -r skills/c-analyzer ~/.openclaw/skills/
cp -r skills/esp32-flasher ~/.openclaw/skills/
cp -r skills/stm32-flasher ~/.openclaw/skills/
```

> Run these commands from the root of this repository.

OpenClaw automatically scans `~/.openclaw/skills/` on startup. No UI configuration needed.

### Verify skills are loaded

In the OpenClaw chat, type:
```
Show me my available skills.
```

You should see 🐛 c-analyzer, ⚡ esp32-flasher, and 🔩 stm32-flasher listed.

---

## Step 8: Create Desktop Shortcuts

### OpenClaw Launcher (one-click to open dashboard)

```bash
cat > ~/.local/share/applications/openclaw.desktop << 'EOF'
[Desktop Entry]
Name=OpenClaw Control
Comment=Start OpenClaw Gateway and open the web interface
Exec=sh -c "systemctl --user start openclaw-gateway.service && chromium --app=http://127.0.0.1:18789"
Terminal=false
Type=Application
Categories=Utility;Development;
Icon=utilities-terminal
EOF

chmod +x ~/.local/share/applications/openclaw.desktop
```

### Ollama Background Starter (one-click to start the engine)

```bash
cat > ~/.local/share/applications/start-ollama.desktop << 'EOF'
[Desktop Entry]
Name=Start Ollama Engine
Comment=Run the Ollama background service silently
Exec=sh -c "nohup ollama serve > /dev/null 2>&1 &"
Terminal=false
Type=Application
Categories=Development;Utility;
Icon=system-run
EOF

chmod +x ~/.local/share/applications/start-ollama.desktop
```

### Refresh the application database

```bash
update-desktop-database ~/.local/share/applications/
```

Now search for **"OpenClaw Control"** or **"Start Ollama Engine"** in your application launcher.

> 💡 **Tip:** If Ollama is already set to auto-start on boot (Step 3), you only need the OpenClaw shortcut.

---

## Step 9: Verify Everything Works

### Quick verification checklist

```bash
# 1. Verify Ollama is running
systemctl status ollama

# 2. Verify models are available
ollama list

# 3. Verify Ollama responds to requests
curl http://127.0.0.1:11434/api/tags

# 4. Verify OpenClaw gateway is running
systemctl --user status openclaw-gateway.service

# 5. Open the dashboard
chromium --app=http://127.0.0.1:18789
```

### Test with buggy code

In the OpenClaw chat, paste the code from [`tests/motor_test.c`](../tests/motor_test.c) and type:

```
Run the c-analyzer skill on this code and tell me what the GCC compiler finds.
```

If the AI:
1. ✅ Runs GCC in the background
2. ✅ Identifies the missing semicolon
3. ✅ Warns about the uninitialized pointer
4. ✅ Explains the hardware impact (Hard Fault)

**Your setup is complete! 🎉**

---

## Next Steps

- 📖 Read the [Skills Guide](SKILLS_GUIDE.md) to create your own custom skills
- 🛡️ Review [Security](SECURITY.md) best practices for sandboxing
- 🔧 Check [Troubleshooting](TROUBLESHOOTING.md) if you hit any issues
