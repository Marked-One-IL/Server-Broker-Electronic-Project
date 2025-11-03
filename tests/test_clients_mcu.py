# test_clients_debug.py
import socket
import struct
import time
import threading
import random

def client_thread(client_id):
    """Client that connects, receives one packet every ~3 secs, then disconnects randomly"""
    while True:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            sock.connect(('127.0.0.1', 4601))
            print(f"[Client {client_id}] Connected")

            while True:
                data = sock.recv(12)
                if len(data) != 12:
                    break

                temperature, moisture, soil_moisture = struct.unpack('!fff', data)
                print(f"[Client {client_id}] Temp={temperature:.2f}, Moisture={moisture:.2f}, Soil={soil_moisture:.2f}")

                # Wait ~3 seconds before next packet
                time.sleep(3)

                # Randomly decide to disconnect
                if random.random() < 0.1:  # ~10% chance each iteration
                    print(f"[Client {client_id}] Disconnecting randomly")
                    break

        except Exception as e:
            print(f"[Client {client_id}] Error: {e}")
        finally:
            sock.close()
            # Wait a short random time before reconnecting
            time.sleep(random.uniform(1, 5))

def main():
    print("=== Debug Client Test ===")
    print("Random infinite clients connecting/disconnecting\n")

    client_id_counter = 1
    threads = []

    try:
        while True:
            # Randomly spawn a new client thread
            if random.random() < 0.3:  # ~30% chance per loop iteration
                t = threading.Thread(target=client_thread, args=(client_id_counter,))
                t.daemon = True
                t.start()
                threads.append(t)
                client_id_counter += 1

            time.sleep(1)

    except KeyboardInterrupt:
        print("\n[!] Stopping all clients...")

if __name__ == "__main__":
    main()
    input()
