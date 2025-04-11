import serial
import asyncio
import websockets
import json

# ✅ Set up the serial port (adjust to your setup if needed)
ser = serial.Serial('/dev/serial0', 115200, timeout=1)

# Store connected WebSocket clients
clients = set()

# Function to read from serial and broadcast JSON data to WebSocket clients
async def read_serial():
    while True:
        try:
            data = ser.readline().decode('utf-8').strip()
            if data:
                try:
                    json_data = json.loads(data)  # Convert string to JSON
                    print(f"Received JSON Data: {json_data}")
                    await broadcast(json.dumps(json_data))  # Send JSON to WebSocket clients
                except json.JSONDecodeError:
                    print(f"Invalid JSON: {data}")  # Handle incorrect JSON format
        except Exception as e:
            print(f"Serial Read Error: {e}")
        await asyncio.sleep(0.1)  # Small delay to prevent CPU overload

# Function to broadcast messages to all connected WebSocket clients
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
