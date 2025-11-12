# 00–99 Counter using Timer0 and External Interrupts (PIC16F887)

This project implements a **00 to 99 counter** controlled by interrupts and displayed on **two 7-segment displays**.  
It uses the **Timer0** module of the **PIC16F887** microcontroller with a delay of approximately **1 second** per increment.

---

## ⚙️ General Configuration

- **Microcontroller:** PIC16F887  
- **Internal oscillator frequency:** 125 kHz  
- **Oscillator mode:** INTRC (no clock output)  
- **Prescaler:** 1:128 (assigned to Timer0)  
- **TMR0 clock source:** Fosc / 4  

---

## ⏱️ Delay Time Calculation

General Timer0 formula:

\[
T_{delay} = \frac{4}{F_{osc}} \times Prescaler \times (256 - TMR0)
\]

Substituting values:

\[
T_{delay} = \frac{4}{125000} \times 128 \times 256 = 1.048576\,s
\]

Each Timer0 overflow generates an interrupt every ~1.05 seconds, incrementing the counter.

---

## 🔘 Inputs

| Pin | Function | Description |
|-----|-----------|-------------|
| RB0 | START | Enables counting (activates Timer0 interrupt) |
| RB1 | STOP | Pauses counting (disables Timer0 interrupt) |
| RB2 | RESET | Resets the counter to 00 |

---

## 💡 Outputs

| Port | Function |
|------|-----------|
| PORTD | Units digit display |
| PORTC | Tens digit display |

Each port drives a **7-segment display** (common cathode configuration).

---

## 🧠 Operation Summary

1. Press **START (RB0)** → Timer0 starts generating interrupts every second.  
2. The counter increments from **00 to 99**, updating both displays.  
3. Press **STOP (RB1)** → Timer0 interrupts are disabled, pausing the count.  
4. Press **RESET (RB2)** → Counter resets to 00 and Timer0 is disabled.  

---

## 🧩 Key Features

- Internal oscillator set at 125 kHz.  
- Timer0 overflow used to generate 1-second interrupts.  
- External interrupt on RB0 starts counting.  
- Uses lookup tables for 7-segment display encoding.  
- Fully interrupt-driven (no software delays).  

---

## 🛠️ Notes

If the prescaler is accidentally assigned to the **Watchdog Timer (WDT)** instead of **Timer0** (`OPTION_REGbits.PSA = 1`),  
the delay drastically reduces to:

\[
T_{delay} = \frac{4}{F_{osc}} \times (256 - TMR0) = 8.192\,ms
\]

So, make sure `OPTION_REGbits.PSA = 0` to keep the correct 1-second timing.
