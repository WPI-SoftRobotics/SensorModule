import serial
import signal
from datetime import datetime

now = datetime.now()

current_time = now.strftime("%H-%M-%S")

ser = serial.Serial('COM3', 115200)

def signal_handler(signal, frame):
    print('You pressed Ctrl+C!')
    ser.close()
    exit(0)

signal.signal(signal.SIGINT, signal_handler)

# Listen to serial port
with open(f"{current_time}.txt", 'a') as f:
#open file
    while True:
        # Read line and write to file
        out = ser.readline().decode('utf-8')
        print(out)
        f.write(out)