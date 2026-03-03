# 🛡️ Security Best Practices

OpenClaw is an **autonomous agent** that can execute shell commands on your system. This makes it powerful — and dangerous if misconfigured.

---

## ⚠️ The Risk

When you give OpenClaw the `*` (wildcard) bundled skill permission, you are essentially handing the AI:
- **Full terminal access** — it can run any `bash` command
- **File system access** — it can read, write, and delete files
- **Network access** — it can make HTTP requests, install packages, etc.

If an instruction is misunderstood, the AI can execute unintended commands.

### Known Vulnerabilities

- **"ClawJacked" Vulnerability:** Demonstrated that malicious websites could silently hijack local OpenClaw instances because the gateway binds to `localhost` without strict browser isolation by default.
- **Mass Actions:** There have been publicized cases of AI agents mass-deleting user data or modifying critical system files due to ambiguous instructions.

---

## 🔒 Recommended Security Configuration

### 1. Use Sandbox Mode (Default in Our Config)

The example configuration enables sandbox mode:

```json
"sandbox": {
  "mode": "all",
  "workspaceAccess": "ro",
  "scope": "session"
}
```

| Setting | Value | What It Does |
|---------|-------|--------------|
| `mode` | `"all"` | Sandbox all agent actions |
| `workspaceAccess` | `"ro"` | **Read-only** workspace — AI can read files but can't modify them |
| `scope` | `"session"` | Sandbox resets between sessions |

> **Recommendation:** Keep `workspaceAccess` at `"ro"` (read-only) while testing. Switch to `"rw"` only when you need the AI to directly edit your code files.

### 2. Protect the Gateway Auth Token

Your gateway uses token-based authentication:

```json
"auth": {
  "mode": "token",
  "token": "YOUR_SECRET_TOKEN"
}
```

- **Never share this token** publicly
- **Never commit it** to a Git repository
- The example config in this repo uses `__OPENCLAW_REDACTED__` as a placeholder

### 3. Restrict Dangerous Commands

The default config blocks potentially harmful commands:

```json
"denyCommands": [
  "camera.snap",
  "camera.clip",
  "screen.record",
  "calendar.add",
  "contacts.add",
  "reminders.add"
]
```

Consider adding more restrictions based on your needs.

### 4. Bind to Loopback Only

```json
"bind": "loopback"
```

This ensures the gateway is **only accessible from your machine** (`127.0.0.1`), not from other devices on your network. **Never change this to `0.0.0.0`** unless you understand the implications.

---

## 🐳 Docker Sandboxing (Recommended for Advanced Users)

For maximum security, run OpenClaw inside a Docker container:

```bash
# Run OpenClaw in an isolated container
docker run -it --rm \
  --name openclaw-sandbox \
  -p 18789:18789 \
  -v $(pwd)/workspace:/workspace:ro \
  openclaw/openclaw:latest
```

Benefits:
- AI cannot access your host filesystem (only mounted volumes)
- AI cannot install packages on your host
- AI cannot access other running processes
- Container is destroyed after each session (`--rm`)

> Mount your workspace as **read-only** (`:ro`) to prevent any file modifications.

---

## ✅ Security Checklist

Before using OpenClaw with sensitive code:

- [ ] Gateway bound to `loopback` only
- [ ] Auth token is set and not shared
- [ ] Sandbox mode is `"all"`
- [ ] Workspace access is `"ro"` (unless editing is needed)
- [ ] Dangerous commands are denied
- [ ] **No root/sudo access** given to the OpenClaw service
- [ ] Remote embedding API fields are **blank** (no cloud data leakage)
- [ ] Tailscale mode is `"off"` (unless you specifically need remote access)

---

## 📌 Remember

> The AI is only as safe as the permissions you give it. Start with **read-only** access and only open up more permissions as you gain confidence in the setup.
