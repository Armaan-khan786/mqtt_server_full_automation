*** Settings ***
Library    mqtt_helper.py

*** Test Cases ***

Compile ESP Firmware
    Compile Esp

Flash ESP Firmware
    Flash Esp

Validate Temperature Data
    Validate Temperature

Validate WiFi RSSI
    Validate Wifi