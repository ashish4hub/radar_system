# Radar System

A non-blocking firmware architecture for an ATmega328P-based ultrasonic radar system with motorized scanning and ultrasonic ranging.

The system uses a servo-mounted HC-SR04 ultrasonic sensor to scan the environment and sends measurement data to a PC for visualization through a web-based radar dashboard.

## Features

- Bare-metal C firmware
- Non-blocking firmware architecture
- ATmega328P microcontroller
- Servo-based 180° scanning
- HC-SR04 ultrasonic ranging
- Input Capture Unit (ICU) for echo pulse measurement
- Timer/interrupt-driven operation
- Modular peripheral drivers
- UART communication
- Python-based serial data acquisition
- WebSocket-based real-time dashboard
- Browser-based radar visualization
- State-machine-based radar control

## System Architecture

```text
                ┌──────────────────────┐
                │      ATmega328P      │
                │                      │
                │  Radar Controller    │
                │          │           │
                │          ▼           │
                │     Servo Motor      │
                │          │           │
                │          ▼           │
                │      HC-SR04         │
                │          │           │
                │          ▼           │
                │     ICU / Timer      │
                │          │           │
                │          ▼           │
                │        UART          │
                └──────────┬───────────┘
                           │
                       USB Serial
                           │
                           ▼
                ┌──────────────────────┐
                │   Python Acquisition │
                │   radar_data_acq.py  │
                └──────────┬───────────┘
                           │
                       WebSocket
                           │
                           ▼
                ┌──────────────────────┐
                │    FastAPI Server    │
                │      server.py       │
                └──────────┬───────────┘
                           │
                           ▼
                ┌──────────────────────┐
                │   Browser Dashboard  │
                │    Radar Display     │
                └──────────────────────┘
```
# Firmware Architecture
Firmware is organised into layers to keep Hardware specific code and application logic seperate.

```text

           main.c
             |
       radar_control.c
               |
               |———> Send_data.c
               |———> sweep_control.c
               |———> obstacle_detection.c
                                |
                                |———> servo.c
                                |———> icu.c
                                |———> uart.c
                                |———> gpio.c
                                |———> timer.c
                                         |
                                     ATmega328P
                                          
```

# Project Structure
```text
radar_system/
     |
     |———> app/
     |      |———> radar_control.c
     |
     |———> driver/
     |        |
     |        |———> gpio/
     |        |       |———> gpio.c
     |        |
     |        |———> timer/
     |        |        |———> timer.c
     |        |
     |        |———> servo/
     |        |        |———> servo.c
     |        |
     |        |———> icu/
     |        |       |———> icu.c
     |        |
     |        |———> uart/
     |                |———> uart.c
     |
     |———> src/
     |      |———> obstacle_detection.c
     |      |———> sweep_control.c
     |      |———> send_data.c
     |
     |———> inc/
     |      |———> goio.h
     |      |———> timer.h
     |      |———> servo.h
     |      |———> icu.h
     |      |———> obstacle_detection.h
     |      |———> radar_control.h
     |      |———> send_data.h
     |      |———> uart.h
     |      |———> sweep_control.h
     |
     |———> radar_dashboard/
     |             |———> radar_data_acq.py
     |             |———> index.html
     |             |———> server.py
     |             |———> static/
     |                      |———> radar.js
     |                      |———> style.css
     |
     |———> main.c
     |———> makefile     
    
```
### Layer Responsibility 
| Layer | Responsibility |
|-------------|---------------|
|driver/      | Hardware-specific peripheral driver | 
|src/      | Application level control |
|inc/      | Public interface and definition |
|main.c      | Firmware entry point  |
|radar_dashboard/      | PC side data acquisition and visualization |

### Hardware Used
| Hardware | Application |
|-------------|---------------|
|ATmega328P      | Main MCU  |
|SG90 Servo      | Radar sensing positioning |
|HCSR04      | Ultrasonic detection  |

# Peripherals and Application
The firmware configures and uses Built-in Hardware peripherals for different applications.

### 8 bit Timer/Counter Timer0
`Timer0` is used in `timer driver` for generating hardware interrupt based precise and non blocking delays.

Check [ATmega328P-HAL-Timer-Driver](https://github.com/ashish4hub/atmega328p-hal/driver/timer/)

### 16 bit Timer/Counter  Timer1
`Timer1` is used in the `icu` driver where the peripheral is configured in `ICU` mode for precise `edge detection`. The `Input Capture Unit (ICU)`has selectable edges `Rising/Falling` which can be programmed to trigger on a rising (low to high) or falling edge (high to low). 

Check [ATmega328P-HAL-ICU-Driver](https://github.com/ashish4hub/atmega328p-hal/driver/icu/)

### 8 bit Timer/Counter Timer2
`Timer2` is configured in `CTC`mode with a prescaler `8` and a top values `OCR2A = 199`.The the Timer2 `Interrupt` is enabled (`OCIA` bit of the `TIMSK2` register is set) and `global interrupt`is enabled. This timer peripheral is used for generating accurate and precise pulse timing for servo motor positioning in `servo` driver.

### USART
The `USART/UART` Peripheral of `ATmega328P` is used in the `uart` driver for establishing `serial communication` with other peripherals and devices.
Check [ATmega328P-HAL-UART-Driver](https://github.com/ashish4hub/atmega328p-hal/driver/uart/)

## 
```text
          Peripheral
               |
               |———> Timer0
               |        |———> System timing / non blocking delays 
               |
               |———> Timer1
               |        |———> ICU
               |
               |———> Timer2
               |        |———> Servo Pulse timing
               |
               |———> Usart/Uart
                         |———> Serial communication/ Radar Data transmission 

```
`The firmware avoids using blocking delays for radar operation. Timing is handled by through Hardware peripherals, interrupts and state machine progression.`

# Drivers
| Driver | Purpose |
|-----------|-------------|
| `goio`    | GPIO pin configuration |
| `timer`   | Creating Non blocking delays and system timing |
| `uart`      | Radar data transmission with PC visualization |
| `servo`     | Controlling servo pulse and positioning |
| `icu`| Measuring distance using icu hardware on rising and falling edge|

*The peripheral drivers are implemented as reusable bare-metal C modules. Deatiled driver documentation is maintained separately in the [ATmega328P HAL](https://github.com/ashish4hub/atmega328p-hal) repository.*



# Radar Motion
The servo motor moves the sensors mounted on its top through a series of positions between 0 degree and 180 degree. The firmware internally uses a `step` value rather than directly using angles for `servo` positioning.


## Sweep Control
The `Radar Application (radar_control.c)` uses the `Sweep control (sweep_control.c)` service layer for the scanning and positioning of the `servo`. This service layer uses `servo` driver for communicating with the hardware registers. The service layer only implements the application specific logic for `Radar system` rather than directly manipulating hardware registers in application for each opertion. The goal was to seperate register level hardware control and application logic.

*The servo driver internally uses a step value for generating pulse for the servo motor. Each step between 5 to 25 corresponds to an angle between 0 to 180 degree.*

| Step | Angle in degree (approx) |
|------|---------|
| `5` | 0  |
| `10` | 45 |
| `15` | 90 |
| `20` | 135 |
| `25` | 180 |

*`1 step` is approximately `100 microseconds`. The servo driver uses a `tick_100us` variable which is increamented on every `Timer2` interrupt fire which gives `1 tick_100us = 100 microseconds = 1 step value.`*

# Radar Ranging / Scanning 
The Radar uses a `HCSR04` ultrasonic sensor for measuring distance and obstacles in its range. Current Architecture restricts its range to `200cm`, however the actual range of the sensor is found to be `~400cm`.

## Obstacle detection 
The higher level Radar application uses `Obstacle Detection (obstacle_detection.c)` service layer for distance measurement and obstacle detection. The service layer only implements the application specific logic and uses `ICU Driver` for accessing the hardware.

# Radar Dashboard
The current `postion` and `angle` is visualized on web based dashboard.
```text
              ATmega328P 
                  |
                  | uart
                  |
          radar_data_acq.py
                  |
           step, distance
                  |
              server.py
                  |
        FastAPI & HTTP + Websocket
                  |
              WebSocket
                  |
               Browser
                  |
              radar.js
                  |
                  |
            Radar Canvas (visualisation)
       

```

## Data Transmission 
The current `position` and `step` data is transmitted from the `MCU` to the `PC` over serial in the format `(step, distance)` The PC runs a `python` program `radar_data_acq.py` which receives the transmitted data and `parse` it in required format, `step: `, `distance:`.

## Data Broadcasting
The `parsed` data is then broadcasted over the dashboard using a `python` server `server.py`. The data is continuously updated and broadcast on the `dashboard`.
The python backend `server.py` acts as the bridge between embedded system and the browser.
### Main Functions (Backend)
- Serves the web application:
FastAPI serves the HTML, CSS and Js files required by the dashboard.

- Stream Radar measurements: 
A backgroud task continuously reads measurements from `radar_data_acq.py`. Each measurement is converted into `JSON` and broadcasted to all the connected WebSocket Clients/devices.
### Main function (Frontend)
The browser connects to the backend through a WebSocket connection.
- Parse `JSON` message.
- Convert servo steps into an angle
- Update current step, angle and distance
- Draw detected obstacles on the radar canvas
- Update the sweep visualisation 

```text
Angles is derived from firmware step value using,
angle = (step - 5) * 9
```

## Complete Data Flow
```text
          HCSR04
            |
            | Echo timing
            |
        Atmega328P
            | 
            | step, distance
            |
     radar_data_acq.py
            |
            | structured data
            |
        server.py
            |
            | JSON over WebSocket
            |
        radar.js
            | 
            | angle, distance
            |
      Radar Dashboard

```
*The embedded firmware is responsible for measurements and control, while the host software is responsible for `data transport` and `visualisation`.*

# Build & Flash 
The firmware is built using `AVR-GCC` toolchain and `Make`.
### Clone repository 
```bash
git clone github.com/ashish4hub/radar_system
```

### Build
Move to radar_system repository and and run:
```bash
make
```
This compiles the source files and generate the firmware artifacts, including the `.hex` file.

### Flash
Connect the ATmega328P programmer/ usb interface and run:
```bash
make flash
```
The generated `.hex` firmware image is programmed into the ATmega328P using `avrdude`.
### Clean
Remove the generated build artifacts:
```bash
make clean
```
### Requirements 
- avr-gcc
- avr-libc
- avrdude
- make
# Python Server  & Dashboard 
### Requirements 
- Python 3
- pip
- Serial connection with ATmega328P
- A wifi connection
### Setup
create a virtual environment:
```bash
python3 -m venv .venv source .venv/bin/activate
```
### Install required Python packages
```bash
pip install -r fastapi uvicorn[standard] pyserial
```
### Run
From the `radar_dashboard` directory run:
```bash
python server.py
```
The server starts on port `8000`
- On host device 
```text
http://localhost:8000
```
- On other device connected to same network (wifi)
```text
http://<host_ip>:8000
```
## License
This project is licensed under MIT License. See the [License](License) for details.






