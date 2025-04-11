import serial
import asyncio
import websockets
import json

# ✅ Set up the serial port (use '/dev/ttyUSB0' if using a USB-to-Serial adapter)
ser = serial.Serial('/dev/serial0', 115200, timeout=1)

# Store connected clients
clients = set()

# Function to read from serial and broadcast to WebSocket clients
async def read_serial():
    while True:
        try:
            data = ser.readline().decode('utf-8').strip()
            if data:
                print(f"Received Data: {data}")
                # Convert data to JSON format
                data_parts = data.split(",")
                if len(data_parts) == 3:
                    json_data = json.dumps({
                        "temperature": data_parts[0],
                        "humidity": data_parts[1],
                        "soilMoisture": data_parts[2]
                    })
                    await broadcast(json_data)  # Send to all connected clients
        except Exception as e:
            print(f"Serial Read Error: {e}")
        await asyncio.sleep(0.1)  # Small delay to prevent CPU overload

# Function to broadcast messages to all connected clients
async def broadcast(message):
    if clients:  # Only send if clients are connected
        await asyncio.wait([client.send(message) for client in clients])

# WebSocket server handler
async def handler(websocket, path):
    clients.add(websocket)  # Add new client
    try:
        async for message in websocket:
            pass  # Keep connection alive
    except:
        pass
    finally:
        clients.remove(websocket)  # Remove client on disconnect

# Start WebSocket server
async def main():
    server = await websockets.serve(handler, "0.0.0.0", 5000)  # Listen on port 5000
    print("WebSocket Server Started on ws://0.0.0.0:5000")

    # Run both WebSocket and Serial tasks
    await asyncio.gather(server.wait_closed(), read_serial())

# Run the event loop
asyncio.run(main())
