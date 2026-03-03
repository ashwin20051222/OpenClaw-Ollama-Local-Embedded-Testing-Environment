---
name: c-analyzer
description: Runs strict GCC compiler checks to find syntax errors, warnings, and potential bugs in C source code.
metadata:
  openclaw:
    emoji: "🐛"
    requires:
      bins: ["gcc"]
---

# C Code Syntax & Bug Analyzer

This skill allows you to act as a strict compiler to check C source code for syntax errors, missing headers, uninitialized variables, type mismatches, and logic warnings.

## When to Use

Activate this skill when the user asks you to:
- "test the code"
- "check for bugs"
- "run the syntax finder"
- "analyze this C file"
- "find errors in my code"
- "run the c-analyzer"

## Command

Execute the following bash command:

```bash
gcc -Wall -Wextra -pedantic -fsyntax-only <filename.c>
```

The `-fsyntax-only` flag checks the code **without producing an executable**, making it fast and non-destructive.

### Flag Explanation

| Flag | Purpose |
|------|---------|
| `-Wall` | Enable all standard warnings |
| `-Wextra` | Enable additional warnings beyond `-Wall` |
| `-pedantic` | Enforce strict ISO C compliance |
| `-fsyntax-only` | Check syntax without compiling to binary |

## Rules

1. **Replace `<filename.c>`** with the actual name of the C file the user wants analyzed.
2. **Read the terminal output carefully.** If GCC outputs any warnings or errors:
   - State the **exact line number** that caused the issue
   - Explain **what the bug is** in plain English
   - Explain the **hardware impact** (e.g., "This will cause a Hard Fault on STM32" or "Undefined behavior — the motor could spin at random speed")
   - Provide the **corrected C code**
3. **If the terminal returns no output**, the syntax is perfectly valid. You must then rely on your own AI logic to review the code for deeper hardware-level bugs that a compiler cannot catch, such as:
   - Bad state machine transitions
   - Incorrect PWM duty cycle calculations
   - Missing volatile qualifiers on hardware registers
   - Race conditions in interrupt handlers
   - Incorrect I2C/SPI timing
4. **For multiple files**, run the command on each file separately and aggregate the results.
5. **Never modify the original file** unless the user explicitly asks you to fix the code.

## Output Format

Format your bug report like this:

```
🐛 Bug Report for <filename.c>
═══════════════════════════════

📍 Issue #1
   File/Function: <name>
   Line Number:   <exact line>
   Type:          <Syntax Error | Warning | Logic Bug>
   Description:   <clear explanation>
   Hardware Impact: <what happens on real hardware>
   Fix:
   ```c
   <corrected code>
   ```

📍 Issue #2
   ...

✅ Summary: <N> issues found (<X> errors, <Y> warnings)
```
