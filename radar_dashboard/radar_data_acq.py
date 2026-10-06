# Fetch step and distance data and stream it over network for visualization on other device

import serial

SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 9600

ser = serial.Serial(
    SERIAL_PORT,
    BAUD_RATE,
    timeout=1
)

def get_measurement():

    line = ser.readline().decode("utf-8").strip()
    if not line:
        return None
    try:
        step, distance = line.split(",")
        step = int(step)
        distance = int(distance)

        return step, distance
    except (ValueError, UnicodeDecodeError):
        return None