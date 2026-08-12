# Instrument Pitch Tuner

Arduino sketch that reads an analog microphone, runs an FFT, and prints the detected musical note, octave, and cents deviation over Serial.

Extracted from the [Trumpet Pitch Tuner](https://github.com/LD07P/Trumpet-Pitch-Tuner) project as a standalone note-reading firmware.

## Features

- Real-time pitch detection via FFT (`arduinoFFT`)
- Note name and octave (A4 reference at 440 Hz)
- Cents deviation (positive = sharp, negative = flat)
- Noise gate and consistency filtering to reduce false readings

## Hardware

| Component | Arduino Pin |
|-----------|-------------|
| Microphone signal | A0 |
| Microphone VCC | 5V |
| Microphone GND | GND |

- **Board**: Arduino Uno (or compatible)
- **Mic**: Analog microphone module with ~1.25 V DC bias (centered around mid-scale for `analogRead`)

## Software

- [PlatformIO](https://docs.platformio.org/) (recommended) or Arduino IDE
- [arduinoFFT](https://github.com/kosme/arduinoFFT) v2.0.4+

## Build and upload (PlatformIO)

```bash
git clone https://github.com/LD07P/Instrument-Pitch-Tuner.git
cd Instrument-Pitch-Tuner
pio run --target upload
```

You can also build and upload from the PlatformIO sidebar or bottom bar in VS Code (or Cursor) instead of the CLI.

Open the Serial Monitor at **9600** baud, play or sing into the mic, and you should see lines like:

```
A4 +5
C5 -12
Eb4 +2
```

### Arduino IDE

1. Install **arduinoFFT** by kosme (Library Manager, v2.0.4+)
2. Open `src/main.cpp` (or copy it into a `.ino` sketch)
3. Select Arduino Uno and your port, then Upload

## Tuning knobs

In `src/main.cpp` you can adjust:
- `samples`: Number of FFT samples (must be power of 2, scales with larger microcontrollers, default: 128)
- `rfreq`: A4 reference (default `440.0`)
- `noiseAmplitudeThreshold`: RMS gate for silence
- `freqTolerance` / `requiredConsistentReads`: Stability before printing
- `micPin`: Analog input (default `A0`)
- `delay()`: Serial monitor update rate in milliseconds (default: 300ms)

## License

MIT — see [LICENSE](LICENSE).
