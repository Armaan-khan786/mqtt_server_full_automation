import asyncio
import logging
from amqtt.broker import Broker

# ---------- LOGGING ----------
logging.basicConfig(level=logging.INFO)

# ---------- MQTT CONFIG ----------

config = {
    "listeners": {
        "default": {
            "type": "tcp",
            "bind": "0.0.0.0:1885"
        }
    },

    "sys_interval": 10,

    "auth": {
        "allow-anonymous": False,
        "password-file": "./passwords.txt"
    },

    "topic-check": {
        "enabled": False
    }
}

# ---------- START BROKER ----------

async def start_broker():

    broker = Broker(config)

    await broker.start()

    print("\n==============================")
    print("MQTT SERVER RUNNING ON PORT 1885")
    print("Waiting for ESP32 connection...")
    print("==============================\n")

    # Keep server alive
    while True:
        await asyncio.sleep(1)

# ---------- MAIN ----------

if __name__ == "__main__":

    print("Starting Python MQTT Broker...")

    try:
        asyncio.run(start_broker())

    except KeyboardInterrupt:
        print("\nMQTT Broker stopped.")