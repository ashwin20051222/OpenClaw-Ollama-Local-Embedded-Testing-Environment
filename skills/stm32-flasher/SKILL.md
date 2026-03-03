---
name: stm32-flasher
description: Flashes firmware to STM32 microcontrollers using st-flash or openocd.
metadata:
  openclaw:
    emoji: "🔩"
    requires:
      bins: ["st-flash"]
---

# STM32 Firmware Flasher

This skill gives you the ability to flash compiled firmware to STM32 microcontrollers connected via ST-Link or SWD debugger.

## When to Use

Activate this skill when the user asks you to:
- "flash the STM32"
- "upload to the board"
- "program the STM32"
- "burn the firmware to STM32"
- "flash it to the blue pill"
- "flash it to the black pill"

## Commands

### Using st-flash (simplest)

```bash
st-flash write firmware.bin 0x8000000
```

### Using OpenOCD (alternative)

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program firmware.bin verify reset exit 0x8000000"
```

### Common Target Configs

| Board | OpenOCD Target | Flash Address |
|-------|---------------|---------------|
| STM32F103 (Blue Pill) | `target/stm32f1x.cfg` | `0x8000000` |
| STM32F407 (Discovery) | `target/stm32f4x.cfg` | `0x8000000` |
| STM32H750 | `target/stm32h7x.cfg` | `0x8000000` |

## Rules

1. **Before running the command**, verify the firmware file exists:
   - Check for `.bin` files: `ls *.bin`
   - Check for `.hex` files: `ls *.hex`
   - If using `.hex` format with st-flash: `st-flash --format ihex write firmware.hex`

2. **If the file does not exist**, tell the user to compile first:
   - STM32CubeIDE: check `Debug/` or `Release/` folder for the `.bin`
   - Makefile project: run `make` first
   - PlatformIO: check `.pio/build/<env>/firmware.bin`

3. **If st-flash returns "Failed to connect"**:
   - Verify the ST-Link debugger is connected
   - Check `lsusb` for ST-Link device
   - Try: `st-info --probe` to detect connected boards
   - User may need udev rules: suggest checking Arch Wiki for ST-Link setup

4. **If permission denied**, the user needs udev rules for ST-Link:
   ```bash
   # Create udev rule
   sudo tee /etc/udev/rules.d/49-stlinkv2.rules << 'EOF'
   SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666"
   SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666"
   EOF
   
   # Reload udev rules
   sudo udevadm control --reload-rules
   sudo udevadm trigger
   ```

5. **After successful flash**, the board auto-resets. Inform the user the new firmware is running.

6. **For `.elf` files**, use OpenOCD instead of st-flash:
   ```bash
   openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program firmware.elf verify reset exit"
   ```
