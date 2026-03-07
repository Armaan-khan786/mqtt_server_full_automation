import subprocess
import serial
import time
import socket


PORT = "COM6"
BAUD = 115200


def compile_esp():
    print("Starting ESP32 compilation...")

    subprocess.run([
        "./arduino-cli.exe",
        "compile",
        "--fqbn",
        "esp32:esp32:esp32",
        "temperature"
    ], check=True)

    print("Compilation successful")


def flash_esp():
    print("Uploading firmware to ESP32...")

    # wait for port stabilization
    time.sleep(5)

    subprocess.run([
        "./arduino-cli.exe",
        "upload",
        "-p", PORT,
        "--fqbn",
        "esp32:esp32:esp32",
        "temperature"
    ], check=True)

    print("Upload successful")


def validate_temperature():
    print("Checking temperature output from ESP32...")

    ser = serial.Serial(PORT, BAUD, timeout=5)
    time.sleep(3)

    temp_found = False

    for i in range(60):
        line = ser.readline().decode(errors="ignore").strip()
        print(line)

        if "temp" in line.lower() or "temperature" in line.lower():
            temp_found = True
            break

    ser.close()

    if not temp_found:
        raise Exception("Temperature message not received")

    print("Temperature validation PASS")


def check_internet():
    print("Checking real internet connectivity...")

    try:
        socket.create_connection(("8.8.8.8", 53), timeout=5)
        print("Internet connection OK")
        return True
    except OSError:
        return False


def validate_wifi():
    print("Validating WiFi + Internet + RSSI...")

    # Step 1 : Check Internet
    if not check_internet():
        raise Exception("Internet is OFF - validation failed")

    # Step 2 : Check ESP WiFi RSSI
    ser = serial.Serial(PORT, BAUD, timeout=5)
    time.sleep(3)

    rssi_found = False

    for i in range(60):
        line = ser.readline().decode(errors="ignore").strip()
        print(line)

        if "rssi" in line.lower():
            rssi_found = True
            break

    ser.close()

    if not rssi_found:
        raise Exception("ESP32 WiFi RSSI not detected")

    print("WiFi RSSI validation PASS")