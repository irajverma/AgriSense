import serial
import asyncio
import websockets
import json

# ✅ Set up serial connection (use '/dev/ttyUSB0' if needed)
ser = serial.Serial('/dev/serial0', 115200, timeout=1)

# ✅ Store connected WebSocket clients
clients = set()

async def read_serial():
    while True:
        try:
            raw_data = ser.readline().decode('utf-8').strip()
            
            if raw_data and raw_data.startswith("{"):  # Ensure only JSON messages are processed
                try:
                    sensor_data = json.loads(raw_data)  # Convert to dictionary
                    await broadcast(json.dumps(sensor_data))  # Send pure JSON to WebSocket clients
                except json.JSONDecodeError:
                    pass  # Ignore malformed data
        except Exception as e:
            print(f"Serial Read Error: {e}")  # Log error for debugging

        await asyncio.sleep(0.1)  # Prevent CPU overload

# ✅ Broadcast JSON message to all WebSocket clients
async def broadcast(message):
    if clients:  
        await asyncio.gather(*(client.send(message) for client in clients))  # ✅ FIXED: Await send operation

# ✅ WebSocket server handler
async def handler(websocket, path):
    clients.add(websocket)  # Add new client
    try:
        async for _ in websocket:
            pass  # Keep connection alive
    except:
        pass
    finally:
        clients.remove(websocket)  # Remove client on disconnect

# ✅ Start WebSocket server
async def main():
    server = await websockets.serve(handler, "0.0.0.0", 5000)
    await asyncio.gather(read_serial(), server.wait_closed())

# ✅ Run the event loop
asyncio.run(main())
