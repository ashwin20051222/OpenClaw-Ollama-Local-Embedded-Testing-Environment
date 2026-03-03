# 🤝 Contributing

Thank you for your interest in contributing! This project aims to make local AI-powered embedded testing accessible to everyone.

---

## How to Contribute

### 🐛 Report a Bug

1. Open an [Issue](../../issues/new)
2. Include your Arch Linux version, Ollama version, and OpenClaw version
3. Describe the expected vs actual behavior
4. Include relevant terminal output or screenshots

### 🔧 Add a New Skill

The most valuable contributions are **new skills** for different embedded platforms:

1. Fork this repository
2. Create a new folder under `skills/`:
   ```
   skills/your-skill-name/
   └── SKILL.md
   ```
3. Follow the format described in the [Skills Guide](docs/SKILLS_GUIDE.md)
4. Test your skill locally with OpenClaw
5. Submit a Pull Request

**Skill ideas we'd love:**
- Arduino CLI build & upload skill
- PlatformIO integration
- Valgrind memory leak checker
- Clang-tidy static analyzer
- JTAG/SWD debugging via OpenOCD
- Logic analyzer capture viewer

### 📚 Improve Documentation

- Fix typos or unclear instructions
- Add screenshots or diagrams
- Translate documentation to other languages
- Add troubleshooting entries for issues you've encountered

### 💡 Suggest a Feature

Open an [Issue](../../issues/new) with the label `enhancement` and describe:
- What you want to achieve
- Your current workflow
- How the feature would improve it

---

## Development Setup

```bash
# Clone the repo
git clone https://github.com/YOUR_USERNAME/openclaw-ollama-embedded-tester.git
cd openclaw-ollama-embedded-tester

# Install skills locally for testing
cp -r skills/* ~/.openclaw/skills/
```

---

## Commit Messages

Use clear, descriptive commit messages:

```
feat(skill): add PlatformIO build skill
fix(docs): correct Ollama install command for Arch
docs: add VRAM troubleshooting section
```

---

## Code of Conduct

Be respectful, inclusive, and constructive. We're all here to learn and build cool things with embedded systems. 🤖
