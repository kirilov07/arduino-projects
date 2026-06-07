<div align="center">

# Arduino Projects

### Hands-on embedded systems — sensors, actuators, and hardware control

![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)

</div>

Each folder is a self-contained project: wiring diagram, explanation, working code, and a photo or video demonstrating the result. Built and tested on real hardware.

---

## Projects

### Sensors & Input

| Folder | Sensor / Component | What it does |
|--------|--------------------|-------------|
| [ultrasonic-sensor](./ultrasonic-sensor/) | HC-SR04 | Distance measurement with zone alerts and bar graph |
| [dht-sensor](./dht-sensor/) | DHT22 | Temperature & humidity monitoring with LED alerts |
| [infrared-sensor](./infrared-sensor/) | IR Obstacle Sensor | 2WD robot with obstacle avoidance |
| [pir-alarm](./pir-alarm/) | HC-SR501 PIR | Motion-triggered alarm with escalating buzzer tone |
| [gas-sensor](./gas-sensor/) | MQ-2 | Gas/smoke detection with PPM readout and fan relay |
| [hall-rpm](./hall-rpm/) | A3144 Hall Effect | Hardware-interrupt tachometer with min/max/avg tracking |
| [mpu6050-gyro](./mpu6050-gyro/) | MPU6050 IMU | Complementary filter, tilt angles for Serial Plotter |
| [lm35-temperature](./lm35-temperature/) | LM35 Analog Sensor | Precision thermometer with trend indicator and alerts |

### Actuators & Output

| Folder | Sensor / Component | What it does |
|--------|--------------------|-------------|
| [servo-motor](./servo-motor/) | SG90 / MG996R | Pan-tilt control with smooth interpolation |
| [relay-control](./relay-control/) | 5V Relay Module | 4-channel active-LOW control with pulse and toggle |
| [stepper-motor](./stepper-motor/) | 28BYJ-48 + ULN2003 | Precise rotation: ROT / REV / SPIN commands |
| [led-matrix](./led-matrix/) | MAX7219 8×8 Matrix | Scrolling text and animations via MD_Parola |
| [neopixel-strip](./neopixel-strip/) | WS2812B + FastLED | 6 effects: rainbow, fire, meteor, breathing, chase, solid |
| [shift-register](./shift-register/) | 74HC595 IC | 8-LED chaser, binary counter, Knight Rider from 3 pins |
| [oled-monitor](./oled-monitor/) | SSD1306 OLED 0.96" | Live temperature/humidity with bar chart history |
| [touch-lamp](./touch-lamp/) | TTP223 + MOSFET | Single tap / double tap / hold gesture lamp control |
| [radar-scanner](./radar-scanner/) | HC-SR04 + Servo | 180° sweep radar with Processing visualizer sketch |

### Communication & Control

| Folder | Sensor / Component | What it does |
|--------|--------------------|-------------|
| [rfid-access](./rfid-access/) | MFRC522 SPI | UID-based access control with green/red LED + buzzer |
| [rtc-clock](./rtc-clock/) | DS3231 + LCD I2C | Real-time clock with temperature and serial time-set |
| [keypad-lock](./keypad-lock/) | 4×4 Keypad + LCD | PIN-protected relay with 3-fail lockout |
| [bluetooth-control](./bluetooth-control/) | HC-05 + SoftwareSerial | Wireless LED control via phone or BT terminal app |
| [ir-remote](./ir-remote/) | TSOP38238 + IRremote | Decode remote buttons and map to actions; LEARN mode |
| [joystick-controller](./joystick-controller/) | KY-023 Joystick | 2-axis pan-tilt servo with deadzone and smooth motion |
| [sound-reactive](./sound-reactive/) | Analog Mic + FastLED | VU meter with rolling noise floor and beat detection |
| [plant-watering](./plant-watering/) | Capacitive Soil + Relay | Auto-watering with dry-threshold check and cooldown |

---

## Folder structure

Each project follows this layout:

```
sensor-name/
├── README.md        ← wiring diagram, pipeline, how to run
├── code.ino         ← Arduino sketch
└── assets/          ← photos, videos, wiring diagrams
```

---

## Hardware used

- Arduino Uno / Mega 2560
- Breadboard + jumper wires
- Sensors and components listed per project

---

<div align="center">

Built by [Kiril Kirilov Kirilov](https://github.com/kirilov07) — Embedded Systems & AI Engineer

</div>
