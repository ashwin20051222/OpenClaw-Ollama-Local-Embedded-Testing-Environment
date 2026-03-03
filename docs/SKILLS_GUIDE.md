# 🔧 Skills Guide — Creating Custom OpenClaw Skills

Learn how to create, install, and manage custom skills that give your AI agent the ability to interact with your system.

---

## Table of Contents

- [What Are Skills?](#what-are-skills)
- [Skills vs Knowledge](#skills-vs-knowledge)
- [Skill Directory Structure](#skill-directory-structure)
- [SKILL.md Format](#skillmd-format)
- [Creating a Custom Skill](#creating-a-custom-skill)
- [Installing Skills](#installing-skills)
- [Verifying Skills](#verifying-skills)
- [Bundled vs Custom Skills](#bundled-vs-custom-skills)
- [Example: PlatformIO Build Skill](#example-platformio-build-skill)

---

## What Are Skills?

In OpenClaw, a **skill** is an **executable tool** — not a document, not a syllabus, not a list of formulas.

A skill tells the AI:
1. **What tool it has** (e.g., `gcc`, `esptool.py`, `st-flash`)
2. **When to use it** (e.g., "when the user says 'flash the board'")
3. **How to use it** (e.g., the exact command to run)
4. **What to do with the output** (e.g., "read the GCC errors and explain them")

---

## Skills vs Knowledge

This is the most common misunderstanding:

| | Skills | Knowledge |
|---|--------|-----------|
| **What it is** | An executable tool | Information the AI uses to think |
| **Where it goes** | `~/.openclaw/skills/` folder | **System Prompt** text box |
| **Example** | "Run `gcc -Wall` on this file" | "PWM Duty Cycle = (ON time / Total Period) × 100%" |
| **Analogy** | Handing a mechanic a **wrench** | Teaching them about **engines** |

> 💡 **Rule of thumb:** If it's a formula, concept, or instruction on *how to think* → put it in the System Prompt. If it's a command the AI should *execute* → make it a skill.

---

## Skill Directory Structure

```
~/.openclaw/skills/
├── c-analyzer/
│   └── SKILL.md          # Required — the main instruction file
├── esp32-flasher/
│   └── SKILL.md
└── my-custom-skill/
    ├── SKILL.md           # Required
    ├── scripts/           # Optional — helper scripts
    │   └── build.sh
    └── templates/         # Optional — code templates
        └── main.c
```

Every skill **must** have a `SKILL.md` file. Everything else is optional.

---

## SKILL.md Format

Every `SKILL.md` file has two sections:

### 1. YAML Frontmatter (metadata)

```yaml
---
name: my-skill-name
description: One-line description of what this skill does.
metadata:
  openclaw:
    emoji: "🔧"
    requires:
      bins: ["gcc", "make"]    # Required binaries on the system
---
```

### 2. Markdown Body (instructions)

Plain English instructions telling the AI:
- **What** the tool does
- **When** to use it (trigger phrases)
- **How** to use it (exact commands)
- **Rules** to follow (safety checks, error handling)

```markdown
# My Custom Skill

This skill allows you to [do something].

When the user asks you to "[trigger phrase]", execute:

`command --flags <arguments>`

**Rules:**
1. Before running, verify [precondition].
2. If [error condition], tell the user [recovery steps].
```

---

## Creating a Custom Skill

### Step 1: Create the folder

```bash
mkdir -p ~/.openclaw/skills/my-skill-name
```

### Step 2: Create the SKILL.md file

```bash
nano ~/.openclaw/skills/my-skill-name/SKILL.md
```

### Step 3: Write the skill

Follow the [SKILL.md format](#skillmd-format) above.

### Step 4: Save and exit

Press `Ctrl+O` → `Enter` → `Ctrl+X` in nano.

OpenClaw automatically detects the new skill — no restart needed.

---

## Installing Skills

### From this repository

```bash
cp -r skills/<skill-name> ~/.openclaw/skills/
```

### From a GitHub repo

```bash
cd ~/.openclaw/skills/
git clone https://github.com/user/openclaw-skill-name.git
```

### Manual creation

```bash
mkdir -p ~/.openclaw/skills/<skill-name>
nano ~/.openclaw/skills/<skill-name>/SKILL.md
```

---

## Verifying Skills

In the OpenClaw chat interface, type:

```
Show me my available skills.
```

The AI should list all skills found in `~/.openclaw/skills/`, showing their emoji and name.

---

## Bundled vs Custom Skills

| Type | Location | Configuration |
|------|----------|---------------|
| **Bundled** | Built into OpenClaw | Enable via `Allow Bundled: *` in settings |
| **Custom** | `~/.openclaw/skills/` | Automatically detected |

**Bundled skills** include core tools like file reading, terminal access, and browser control. Setting `*` in the "Allow Bundled" field enables all of them.

**Custom skills** are your own tools. They extend the AI's capabilities with domain-specific commands.

---

## Example: PlatformIO Build Skill

Here's a complete example of a skill for building and uploading PlatformIO projects:

```markdown
---
name: platformio-builder
description: Builds and uploads PlatformIO embedded projects.
metadata:
  openclaw:
    emoji: "🏗️"
    requires:
      bins: ["pio"]
---

# PlatformIO Project Builder

This skill lets you build, clean, and upload PlatformIO embedded projects.

## Build

When the user says "build the project" or "compile":

`pio run`

## Upload

When the user says "upload" or "flash":

`pio run --target upload`

## Clean

When the user says "clean":

`pio run --target clean`

**Rules:**
1. Before building, verify that `platformio.ini` exists in the current directory.
2. If it doesn't exist, ask the user which directory contains their PlatformIO project.
3. Always read the build output and explain any errors clearly, including the exact file and line number.
```

---

## Tips for Writing Effective Skills

1. **Be explicit about trigger phrases** — tell the AI exactly which user messages should activate the skill
2. **Include safety checks** — always verify files exist before running commands
3. **Handle errors gracefully** — tell the AI what to do when commands fail
4. **Keep commands simple** — one clear command per action
5. **Test with the AI** — after creating a skill, ask the AI to use it and verify it works correctly
