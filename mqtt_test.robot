*** Settings ***
Library    mqtt_helper.py

*** Test Cases ***

Compile ESP Firmware
    Compile Esp

Flash ESP Firmware
    Flash Esp

Validate WiFi Connection
    Validate Wifi

Validate MQTT Connection
    Validate Mqtt Connection

Validate Temperature Sensor
    Validate Temperature

Validate WiFi RSSI
    Validate Rssi

Validate MQTT Publish
    Validate Mqtt Publish