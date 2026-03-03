---
name: esp32-flasher
description: Flashes compiled firmware.bin files to an ESP32 microcontroller over USB using esptool.py.
metadata:
  openclaw:
    emoji: "⚡"
    requires:
      bins: ["esptool.py"]
---

# ESP32 Firmware Flasher

This skill gives you the ability to flash compiled firmware directly to an ESP32 development board connected to the system via USB.

## When to Use

Activate this skill when the user asks you to:
- "flash the board"
- "upload the code"
- "program the ESP32"
- "burn the firmware"
- "flash it"

## Command

```bash
esptool.py --port /dev/ttyUSB0 --baud 460800 write_flash 0x10000 firmware.bin
```

### Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `--port` | `/dev/ttyUSB0` | Serial port (may be `/dev/ttyACM0` on some boards) |
| `--baud` | `460800` | Upload speed (use `115200` if you get connection errors) |
| `0x10000` | - | Flash offset for application firmware |
| `firmware.bin` | - | The compiled binary to flash |

## Rules

1. **Before running the command**, use your file system tools to verify that `firmware.bin` (or whatever binary the user specifies) actually exists in the current directory.

2. **If the file does not exist**, do NOT run the command. Tell the user:
   - They need to compile their C code into a `.bin` file first
   - If using Arduino IDE: look in the build output folder
   - If using PlatformIO: check `.pio/build/<env>/firmware.bin`
   - If using ESP-IDF: run `idf.py build` first

3. **If the port is different**, check for available ports:
   ```bash
   ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
   ```

4. **If the terminal returns a "Permission denied" error** for `/dev/ttyUSB0`:
   - Tell the user they need to add themselves to the `uucp` group on Arch Linux:
     ```bash
     sudo usermod -aG uucp $USER
     ```
   - They must **log out and log back in** for this to take effect.

5. **If the flash fails with "Failed to connect"**:
   - Ask the user to hold the **BOOT** button on the ESP32 while the command runs
   - Some boards need the BOOT button held during the initial connection phase

6. **After a successful flash**, remind the user to press the **RESET** button on the board (or unplug and replug USB) to start the new firmware.

## ESP32-CAM Specific Notes

For ESP32-CAM boards (which lack a USB-to-serial bridge):
- An external FTDI or CP2102 adapter is required
- Connect: `FTDI TX → ESP32 RX`, `FTDI RX → ESP32 TX`, `GND → GND`
- Connect `GPIO0` to `GND` before powering on to enter flash mode
- After flashing, disconnect `GPIO0` from `GND` and reset
