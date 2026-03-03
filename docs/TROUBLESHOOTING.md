# 🔍 Troubleshooting

Common issues and their solutions when running OpenClaw + Ollama on Arch Linux.

---

## Table of Contents

- [Port 18789 Already in Use](#port-18789-already-in-use)
- [Ollama Not Responding (10+ Minute Hang)](#ollama-not-responding-10-minute-hang)
- [Connection Refused in Browser](#connection-refused-in-browser)
- [VRAM Exhaustion / Slow Responses](#vram-exhaustion--slow-responses)
- [Model Not Appearing in OpenClaw](#model-not-appearing-in-openclaw)
- [esptool.py Permission Denied](#esptoolpy-permission-denied)
- [OpenClaw Still Trying Cloud Models](#openclaw-still-trying-cloud-models)
- [Skills Not Loading](#skills-not-loading)

---

## Port 18789 Already in Use

**Symptom:** OpenClaw gateway fails to start. Terminal shows:
```
Error: address already in use 127.0.0.1:18789
```

**Cause:** Another instance of the OpenClaw gateway is already running.

**Solution (pick one):**

```bash
# Option 1: Stop the service cleanly
systemctl --user stop openclaw-gateway.service

# Option 2: Use OpenClaw's built-in command
openclaw gateway stop

# Option 3: Kill the process directly
# First, find the PID:
lsof -i :18789
# Then kill it:
kill -9 <PID>
```

Then restart:
```bash
systemctl --user start openclaw-gateway.service
```

---

## Ollama Not Responding (10+ Minute Hang)

**Symptom:** OpenClaw shows the loading animation (three dots) indefinitely. No response after 10+ minutes.

**Cause:** Ollama engine is not running, or OpenClaw isn't connected to the correct port.

**Solution:**

### 1. Check if Ollama is running

```bash
systemctl status ollama
```

If it says `inactive (dead)`:

```bash
sudo systemctl start ollama
```

### 2. Verify Ollama is accepting requests

```bash
curl http://127.0.0.1:11434/api/tags
```

If this returns a JSON list of your models, Ollama is alive.

### 3. Restart the OpenClaw gateway

```bash
systemctl --user restart openclaw-gateway.service
```

### 4. Refresh the browser and retry

- Press `Ctrl+R` to reload the dashboard
- Send your message again with a **clear instruction** (e.g., "Review this code for bugs")
- Small models can get stuck if you paste raw code without telling them what to do

---

## Connection Refused in Browser

**Symptom:** Chromium shows `ERR_CONNECTION_REFUSED` when navigating to `127.0.0.1`.

**Cause:** Wrong port or gateway is stopped.

**Solution:**

1. **Use the correct URL:** OpenClaw runs on port `18789`, not the default port `80`:
   ```
   http://127.0.0.1:18789
   ```

2. **Start the gateway if stopped:**
   ```bash
   systemctl --user start openclaw-gateway.service
   ```

3. **Verify it's listening:**
   ```bash
   ss -tlnp | grep 18789
   ```

---

## VRAM Exhaustion / Slow Responses

**Symptom:** Responses take 60+ seconds. System feels sluggish. GPU fan spins heavily.

**Cause:** Multiple models loaded in VRAM simultaneously, or a model that's too large for your GPU.

**Solution:**

### Check VRAM usage

```bash
nvidia-smi
```

### Unload unused models

```bash
# List running models
ollama ps

# Stop a specific model (Ollama unloads idle models automatically after ~5 min)
# Restart Ollama to force unload all:
sudo systemctl restart ollama
```

### Stick to 1B models

For a 4GB VRAM GPU (like RTX 2050):
- ✅ `qwen2.5-coder:1.5b` — uses ~1-2 GB VRAM
- ❌ `codellama:7b` — needs 4+ GB, will overflow to system RAM

> **Rule:** Only run **one model at a time**. OpenClaw's fallback system automatically switches models — it doesn't load them simultaneously.

---

## Model Not Appearing in OpenClaw

**Symptom:** Pulled a model with `ollama pull`, but it doesn't show up in OpenClaw's model dropdown.

**Solution:**

### 1. Verify the model is actually downloaded

```bash
ollama list
```

### 2. Use the correct name format in OpenClaw

OpenClaw requires the `ollama/` prefix:

| ❌ Wrong | ✅ Correct |
|----------|-----------|
| `qwen2.5-coder:1.5b` | `ollama/qwen2.5-coder:1.5b` |
| `deepseek-coder` | `ollama/deepseek-coder:1.3b` |

### 3. Restart the gateway

```bash
systemctl --user restart openclaw-gateway.service
```

---

## esptool.py Permission Denied

**Symptom:** The ESP32 flasher skill fails with:
```
Permission denied: '/dev/ttyUSB0'
```

**Cause:** Your user doesn't have access to the serial port.

**Solution:**

### Add yourself to the `uucp` group (Arch Linux)

```bash
sudo usermod -aG uucp $USER
```

> ⚠️ **You must log out and log back in** (or reboot) for this to take effect.

### Verify

```bash
groups
```

You should see `uucp` in the list.

### Check the port exists

```bash
ls -la /dev/ttyUSB*
```

If nothing appears, your ESP32 board may not be connected or recognized.

---

## OpenClaw Still Trying Cloud Models

**Symptom:** OpenClaw tries to use `minimax-m2.5:cloud` or other cloud models despite configuring local Ollama.

**Cause:** The primary model wasn't updated, or old model entries remain in the config.

**Solution:**

### 1. Check the Primary Model setting

In the OpenClaw dashboard: **Config** → **Agents** → **Defaults**

Ensure Primary Model is:
```
ollama/qwen2.5-coder:1.5b
```

**Not:**
```
ollama/minimax-m2.5:cloud   ← this is the cloud model!
```

### 2. Remove cloud model entries from config

If editing the JSON directly, remove any model entries with `:cloud` in their ID from the `models.providers.ollama.models` array.

### 3. Leave remote embedding fields blank

The "Remote Embedding API Key" and "Remote Embedding Base URL" fields should be **completely empty**.

### 4. Save, Apply, and restart

Click **Save** → **Apply** in the UI, then:
```bash
systemctl --user restart openclaw-gateway.service
```

---

## Skills Not Loading

**Symptom:** Custom skills don't appear when asking the AI "Show me my available skills."

**Solution:**

### 1. Verify the folder structure

```bash
ls -la ~/.openclaw/skills/
```

Each skill must be in its own subfolder with a `SKILL.md` file:
```
~/.openclaw/skills/
├── c-analyzer/
│   └── SKILL.md       ✅ Correct
├── SKILL.md           ❌ Wrong (not in a subfolder)
```

### 2. Verify the SKILL.md has valid YAML frontmatter

The file must start with `---` delimiters:
```yaml
---
name: c-analyzer
description: Runs GCC checks on C code.
---
```

### 3. Ensure Allow Bundled is set to `*`

In **Config** → **Skills** → **Allow Bundled**, the value should be `*`.

### 4. Restart the gateway

```bash
systemctl --user restart openclaw-gateway.service
```
