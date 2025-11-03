# test_main_mcu.py
import socket
import struct
import time
import random

class MainMCUClient:
    def __init__(self, host='127.0.0.1', port=4600):
        self.host = host
        self.port = port
        self.sock = None

    def connect(self):
        """Connect to the MainMCU server"""
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))
        print(f"[+] Connected to {self.host}:{self.port}")

    def send_sensor_data(self, temperature, moisture, soil_moisture):
        """Send sensor data as big-endian floats (matches C++ htonl logic)"""
        data = struct.pack('!fff', temperature, moisture, soil_moisture)
        print(f"[>] Sending: {data.hex()} | "
              f"Temp={temperature:.2f}, Moisture={moisture:.2f}, Soil={soil_moisture:.2f}")
        self.sock.sendall(data)

    def disconnect(self):
        """Close the connection"""
        if self.sock:
            self.sock.close()
            self.sock = None
            print("[x] Disconnected")

def run_forever():
    """Send sensor data forever with consistent 3-second intervals"""
    client = MainMCUClient()
    try:
        client.connect()

        while True:  # infinite loop
            temp = 20.0 + random.uniform(-5.0, 10.0)
            moisture = 50.0 + random.uniform(-20.0, 30.0)
            soil_moisture = 40.0 + random.uniform(-15.0, 25.0)

            client.send_sensor_data(temp, moisture, soil_moisture)
            time.sleep(3)  # Consistent 3-second sleep

    except Exception as e:
        print(f"[!] Error: {e}")
        # optionally reconnect if needed
    finally:
        client.disconnect()

if __name__ == "__main__":
    run_forever()
    input()