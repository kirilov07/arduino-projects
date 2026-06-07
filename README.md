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

| Folder | Sensor / Component | What it does |
|--------|--------------------|-------------|
| [ultrasonic-sensor](./ultrasonic-sensor/) | HC-SR04 | Distance measurement, obstacle detection |
| [dht-sensor](./dht-sensor/) | DHT22 | Temperature and humidity monitoring |
| [servo-motor](./servo-motor/) | SG90 / MG996R | Angle control, sweep, position hold |
| [relay-control](./relay-control/) | 5V Relay Module | Switching high-power devices on/off |
| [infrared-sensor](./infrared-sensor/) | IR Obstacle Sensor | Proximity detection, line following |

---

## Folder structure

Each project follows this layout:

```
sensor-name/
├── README.md        ← wiring, explanation, how to run
├── code.ino         ← Arduino sketch
└── assets/          ← photos, videos, wiring diagrams
```

---

## Hardware used

- Arduino Uno / Mega 2560
- Breadboard + jumper wires
- Sensors listed per project

---

<div align="center">

Built by [Kiril Kirilov Kirilov](https://github.com/kirilov07) — Embedded Systems & AI Engineer

</div>
