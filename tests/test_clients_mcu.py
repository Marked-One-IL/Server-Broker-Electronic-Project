# test_clients_debug.py
import socket
import struct
import time
import threading

def client_thread(client_id):
    """Client that shows exactly what's happening"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        sock.connect(('127.0.0.1', 4601))
        print(f"[Client {client_id}] Connected")
        
        packet_count = 0
        unique_data = set()
        last_print_time = time.time()
        
        while True:
            # Receive without delay to see actual data rate
            data = sock.recv(12)
            if len(data) == 12:
                packet_count += 1
                temperature, moisture, soil_moisture = struct.unpack('!fff', data)
                current_data = (temperature, moisture, soil_moisture)
                unique_data.add(current_data)
                
                # Print every second to avoid spam
                current_time = time.time()
                if current_time - last_print_time >= 3.0:
                    print(f"[Client {client_id}] Packets: {packet_count} | "
                          f"Unique: {len(unique_data)} | "
                          f"Current: Temp={temperature:.2f}, Moisture={moisture:.2f}, Soil={soil_moisture:.2f}")
                    last_print_time = current_time
                    packet_count = 0
                    unique_data.clear()
                
    except Exception as e:
        print(f"[Client {client_id}] Error: {e}")
    finally:
        sock.close()

def main():
    print("=== Debug Client Test ===")
    print("This will show how many packets you're actually receiving")
    print("Press Ctrl+C to stop\n")
    
    client = threading.Thread(target=client_thread, args=(1,))
    client.daemon = True
    client.start()
    
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n[!] Stopping...")

if __name__ == "__main__":
    main()