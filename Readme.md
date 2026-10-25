# Radar System

A non-blocking firmware architecture for an ATmega328P-based ultrasonic radar system with motorized scanning and ultrasonic ranging.

The system uses a servo-mounted HC-SR04 ultrasonic sensor to scan the environment and sends measurement data to a PC for visualization through a web-based radar dashboard.

## Features

- Bare-metal C firmware
- State-machine based firmware architecture
- ATmega328P microcontroller
- Servo-based 180° scanning
- HC-SR04 ultrasonic ranging
- Input Capture Unit (ICU) for echo pulse measurement
- Timer/interrupt-driven operation
- Modular peripheral drivers
- UART communication
- Python-based serial data acquisition
- Web-dashboard based radar visualization

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
        |———> Send_data.c -------------->|
        |———> sweep_control.c ---------->|
        |———> obstacle_detection.c ----->|
                                         |———> servo.c --->|
                                         |———> icu.c ----->|
                                         |———> uart.c ---->|
                                         |———> gpio.c ---->|
                                         |———> timer.c --->|
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
     |        |———> gpio.c
     |        |———> timer.c
     |        |———> servo.c
     |        |———> icu.c
     |        |———> uart.c
     |
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
     |                     |———> radar.js
     |                     |———> style.css
     |
     |———> main.c
     |———> makefile
    
```
### Layer Responsibility 
| Layer | Responsibility |
|-------------|---------------|
|driver/      | Hardware-specific peripheral driver | 
|src/      | Specific task services built using hardware drivers|
|inc/      | Public interface and definition |
|app/ | Application to run services sequentially |
|main.c      | Firmware entry point  |
|radar_dashboard/      | PC side data acquisition and visualization |

### Hardware Used
| Hardware | Application |
|-------------|---------------|
|ATmega328P      | Main MCU  |
|SG90 Servo      | Radar sensing positioning |
|HCSR04      | Ultrasonic detection  |


# Drivers
| Driver | Purpose |
|-----------|-------------|
| `goio`    | GPIO pin configuration |
| `timer`   | Creating Non blocking delays and system timing |
| `uart`      | Radar data transmission with PC visualization |
| `servo`     | Controlling servo pulse and positioning |
| `icu`| Measuring distance using icu hardware on rising and falling edge|

*The peripheral drivers are implemented as reusable bare-metal C modules. Deatiled driver documentation is maintained separately in the [ATmega328P HAL](https://github.com/ashish4hub/atmega328p-hal) repository.*

# Radar Application Layer
The Radar Application is a state-machine driven high level application specific code layer built to sequentially run the `services` using state based progression and keep the system non blocking. The Application layer also acts as `cooperative superloop`, in which every service gets required `CPU cycles` to complete its specified task without blocking the `CPU`.


- `SERVO_pin`: The current application use `PD5` Pin of ATmega328P for servo pulse generation, any `GPIO` Pin can be used for the task. [servo driver](https://github.com/ashish4hub/atmega328p-hal)

- `RADAR_settle_time`: Wait time for the Radar to reach the required position in `milliseconds`. Can also be considered as `RADAR SPEED` and can be changed by the user to any desired `32-bit` value. 

- The servo `steps` used in the Application are obtained through servo calibration, the pulse duration for servo motors can vary for particular position or angle. The servo motor should be calibrated to obtain specific required pulse duration and replaced in `initial` and `final` step value in Application.

*At `RADAR_settle_time` values below 20 the servo makes unpredictable movement behaviour.*

# Radar Service Layer
The Radar system uses different service layers for different tasks.

## Sweep Control

The `Sweep control (sweep_control.c)` service layer is responsible for the movement and positioning of the `servo`. This service layer uses `servo` driver for communicating with the hardware registers. The service layer only implements the required service logic for `Radar system` rather than directly manipulating hardware registers for each opertion. The goal was to seperate register level hardware control and service/application logic.

Responsibilities:
- Acts as a bridge for communication between the high level application and low level servo driver.
- Changes direction of sweep at extreme values (0 and 180 degrees).
- Updates the current step value in servo driver according to application for generating pulse.

*The servo driver internally uses a step value for generating pulse for the servo motor.*

*`1 step` is approximately `100 microseconds`. The servo driver uses a `tick_100us` variable which is increamented on every `Timer2` interrupt fire which gives `1 tick_100us = 100 microseconds = 1 step value.`*

## Obstacle detection 
The higher level Radar application uses `Obstacle Detection (obstacle_detection.c)` service layer for distance measurement and obstacle detection. The service layer only implements the application specific logic and uses `ICU Driver` for accessing the hardware.

The Radar uses a `HCSR04` ultrasonic sensor for measuring distance and obstacles in its range. Current Architecture restricts its range to `200cm`, however the actual range of the sensor is found to be `~400cm`.

Responsibilities:
- Bridge for communication between ICU driver and Application layer.
- Trigger a pulse for measurement
- Update a completion flag when measuremnt is done
- Return the measuremnet result in CM.


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
The current `position` and `step` data is transmitted from the MCU to the PC over serial in the format `(step, distance)`. The PC runs a `python` program `radar_data_acq.py` which receives the transmitted data using `pyserial` and `parse` it in required format, `step: `, `distance:`.

## Data Broadcasting
The `parsed` data is then broadcasted over the dashboard using a `python` server `server.py`. The data is continuously updated and broadcast on the `dashboard`.
The python backend `server.py` acts as the bridge between the embedded system and the browser.
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
            |> Echo timing
            |
        Atmega328P
            |
            |> step, distance
            |
     radar_data_acq.py
            |
            |> structured data
            |
        server.py
            |
            |> JSON over WebSocket
            |
        radar.js
            | 
            |> angle, distance
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
This project is licensed under MIT License. See the [License](LICENSE) for details.

*Check [pd](pd) for circuit schematic and prototype*