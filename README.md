# avr-bare-metal

> Bare-metal Arduino Uno programming in C, Zig, and Rust — no frameworks, just registers and unit tests.

Learn embedded systems by implementing the same projects in three languages, with testable code that runs on both your host machine and the AVR microcontroller.

## Why This Repo?

Most Arduino tutorials use the Arduino IDE and its abstractions (`digitalWrite`, `analogWrite`, etc.). This repo teaches you what's actually happening at the register level — the same skills you'd use for any embedded system, not just Arduino.

Each project is implemented in **C**, **Zig**, and **Rust** so you can compare approaches and pick what works for you.

## Quick Start
```bash
git clone https://github.com/yourusername/avr-bare-metal
cd avr-bare-metal

# Install toolchains (Ubuntu/Debian)
sudo apt update
sudo apt install gcc-avr avr-libc avrdude

# Try the C version first
cd c/01-blink
make
make flash   # With Arduino plugged in

# Run tests (no hardware needed!)
cd ../02-pwm-fade
make test
```

## Projects

### 01-blink — Hello World

**What it does:** Toggles the onboard LED at 1Hz (500ms on, 500ms off).

**What you'll learn:**
- GPIO output: setting pin direction with `DDRB`
- Bit manipulation: `|=`, `&=`, `~`, `<<` operators
- Memory-mapped I/O: registers are just memory addresses
- Delay loops and timing

**Key registers:**
```
DDRB  (0x24) — Data Direction Register B (0=input, 1=output)
PORTB (0x25) — Output Register B (0=low, 1=high)
PB5   (bit 5) — Pin 13, the onboard LED
```

---

### 02-pwm-fade — Software PWM

**What it does:** Smoothly fades the LED brightness up and down using PWM.

**What you'll learn:**
- PWM (Pulse Width Modulation): simulate analog output with digital pins
- Duty cycle: percentage of time the signal is HIGH
- Gamma correction: why linear PWM looks wrong to human eyes
- Hardware abstraction: separate logic from hardware for testing
- Unit testing embedded code without hardware

**Key concepts:**
```
PWM Frequency = 1 / (256 steps × 40µs) ≈ 98Hz
Duty Cycle    = ON time / Total time
Gamma 2.2     = Compensates for human logarithmic brightness perception
```

**Testing pattern:**
```
┌─────────────────────────────────────────────────┐
│              Application Logic                   │
│         (gamma table, PWM state machine)         │
└─────────────────┬───────────────────────────────┘
                  │
         ┌────────▼────────┐
         │       HAL       │ ← Abstract interface
         └────────┬────────┘
                  │
       ┌──────────┴──────────┐
       │                     │
┌──────▼──────┐       ┌──────▼──────┐
│   Real AVR  │       │    Mock     │
│  Registers  │       │  (testing)  │
└─────────────┘       └─────────────┘
  (on device)           (on host)
```

---

### 03-uart — Serial Communication (Coming Soon)

**What you'll learn:**
- UART protocol: start bit, data bits, stop bit
- Baud rate calculation from clock frequency
- Ring buffers for async communication
- printf debugging on embedded

---

### 04-timer-interrupt — Hardware Timers (Coming Soon)

**What you'll learn:**
- Timer/Counter registers: TCCR, TCNT, OCR
- Prescalers: dividing clock frequency
- Interrupts: ISR, volatile, atomic access
- Non-blocking code: do work while waiting

---

## Language Comparison

| Aspect | C | Zig | Rust |
|--------|---|-----|------|
| **Toolchain** | avr-gcc | zig (built-in AVR) | rustc nightly + avr-hal |
| **Setup difficulty** | Easy | Easy | Medium |
| **AVR support** | Mature, stable | Good, stable | Experimental (nightly) |
| **Register access** | Direct pointers | Direct pointers | Via HAL crate |
| **Testing** | Manual (assert.h) | Built-in | Built-in (cargo test) |
| **Memory safety** | Manual | Manual + checks | Compiler enforced |
| **Binary size** | Baseline | Similar to C | ~10-20% larger |
| **Compile time** | Fast | Fast | Slow |
| **Best for** | Learning fundamentals | Modern C alternative | Safety-critical code |

### Code Comparison: Setting LED High

**C:**
```c
PORTB |= (1 << PB5);
```

**Zig:**
```zig
const PORTB: *volatile u8 = @ptrFromInt(0x25);
PORTB.* |= (1 << 5);
```

**Rust:**
```rust
led.set_high();  // Using arduino-hal
// Or raw: (*PORTB).modify(|r, w| w.bits(r.bits() | (1 << 5)));
```

## Running Tests

Each language has built-in tests that verify the PWM logic without hardware:

**C:**
```bash
cd c/02-pwm-fade
make test
```

**Zig:**
```bash
cd zig/02-pwm-fade
zig build test
```

**Rust:**
```bash
cd rust/02-pwm-fade
cargo test
```

