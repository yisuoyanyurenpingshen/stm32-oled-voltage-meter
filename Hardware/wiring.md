# Hardware Wiring

## OLED Connection

The OLED display is connected to STM32 through I2C.

| OLED Pin | STM32 Pin | Description |
|---|---|---|
| SCL / SCK | PB6 | I2C clock line |
| SDA | PB7 | I2C data line |
| VCC | 3.3V / 5V | Power supply |
| GND | GND | Ground |

```text
OLED SCL  ----  PB6
OLED SDA  ----  PB7
OLED VCC  ----  3.3V / 5V
OLED GND  ----  GND

Voltage Divider Circuit

The measured voltage is divided by two resistors before being connected to the STM32 ADC pin.


Measured Voltage + ---- R1 100.3kΩ ---- Divider Point ---- R2 15.66kΩ ---- GND
                                               |
                                               |
                                           PA0 / ADC_IN0
