import subprocess
import serial
import time
import socket

PORT = "COM6"
BAUD = 115200


def compile_esp():

    print("Compiling ESP32 firmware...")

    subprocess.run([
        "./Arduino-CLI.exe",
        "compile",
        "--fqbn",
        "esp32:esp32:esp32",
        "temperature"
    ], check=True)

    print("Compilation SUCCESS")


def flash_esp():

    print("Flashing ESP32 firmware...")

    time.sleep(5)

    subprocess.run([
        "./Arduino-CLI.exe",
        "upload",
        "-p",
        PORT,
        "--fqbn",
        "esp32:esp32:esp32",
        "temperature"
    ], check=True)

    print("Upload SUCCESS")


def open_serial():

    print("Opening serial port:", PORT)

    ser = serial.Serial(PORT, BAUD, timeout=5)

    time.sleep(3)

    return ser


def validate_wifi():

    print("Checking WiFi connection")

    ser = open_serial()

    wifi_connected = False

    for i in range(60):

        line = ser.readline().decode(errors="ignore").strip()

        print(line)

        if "WIFI_CONNECTED" in line:
            wifi_connected = True
            break

    ser.close()

    if not wifi_connected:
        raise Exception("WiFi connection failed")

    print("WiFi validation PASS")


def validate_mqtt_connection():

    print("Checking MQTT connection")

    ser = open_serial()

    mqtt_connected = False
    mqtt_failed = False

    for i in range(80):

        line = ser.readline().decode(errors="ignore").strip()

        print(line)

        if "MQTT_CONNECTED" in line:
            mqtt_connected = True
            break

        if "MQTT_FAILED" in line:
            mqtt_failed = True
            break

    ser.close()

    if mqtt_connected:
        print("MQTT connection PASS")
        return

    if mqtt_failed:
        raise Exception("MQTT authentication failed")

    raise Exception("MQTT server not reachable")


def validate_temperature():

    print("Checking temperature sensor output")

    ser = open_serial()

    temp_found = False

    for i in range(60):

        line = ser.readline().decode(errors="ignore").strip()

        print(line)

        if "TEMP:" in line:
            temp_found = True
            break

    ser.close()

    if not temp_found:
        raise Exception("Temperature data not found")

    print("Temperature validation PASS")


def validate_rssi():

    print("Checking WiFi RSSI")

    ser = open_serial()

    rssi_found = False

    for i in range(60):

        line = ser.readline().decode(errors="ignore").strip()

        print(line)

        if "RSSI:" in line:
            rssi_found = True
            break

    ser.close()

    if not rssi_found:
        raise Exception("RSSI not detected")

    print("RSSI validation PASS")


def validate_mqtt_publish():

    print("Checking MQTT publish status")

    ser = open_serial()

    publish_success = False

    for i in range(60):

        line = ser.readline().decode(errors="ignore").strip()

        print(line)

        if "MQTT_PUBLISH_SUCCESS" in line:
            publish_success = True
            break

    ser.close()

    if not publish_success:
        raise Exception("MQTT publish failed")

    print("MQTT publish validation PASS")