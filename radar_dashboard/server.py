import asyncio
import json
from pathlib import Path

from fastapi import FastAPI,WebSocket
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles

import uvicorn

from radar_data_acq import get_measurement

BASE = Path(__file__).resolve().parent

app = FastAPI()

app.mount(
    "/static",StaticFiles(directory=BASE / "static"), name="static"
)

# Connecting browser
clients = set()

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    print("Websocket connection request received")
    await websocket.accept()
    print("websocket accepted")
    clients.add(websocket)
    print("client added. Total clients:", len(clients))

    try:
        while True:
            await websocket.receive_text()
    except Exception as e:
        print("websocket disconnected:",e)
    finally:

        clients.discard(websocket)
        print("client removed. Total client:",len(clients))

async def broadcast(step,distance):

    message = json.dumps({
        "step": step,
        "distance": distance
    })
    #print(f"Broadcasting: {message}")

    disconnected = []

    for client in clients:
        try:
            await client.send_text(message)
     #       print("sent to browser")
        except Exception as e:
      #      print("Websocket error:", e)
            disconnected.append(client)

            for client in disconnected:
                clients.discard(client)

async def serial_bridge():
    while True:
        measurement = await asyncio.to_thread(get_measurement)

        if measurement is None:
            continue
        step, distance = measurement

       # print(
        #    f"step: {step} | Distance: {distance} cm" 
        #)

        await broadcast(
            step,
            distance
        )

# Start Background data acquisition

@app.on_event("startup")
async def startup_event():

    asyncio.create_task(
        serial_bridge()
    )

@app.get("/")
async def index():
    return FileResponse(BASE / "index.html")

if __name__ == "__main__":
    uvicorn.run(
        app,
        host="0.0.0.0",
        port=8000
    )
