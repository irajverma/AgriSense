import serial
ser = serial.Serial('/dev/serial0', 115200, timeout=6)
while True:
        
        data = ser.readline().decode('utf-8').strip()
        
        print(f"Received Raw Data: {data}") 
