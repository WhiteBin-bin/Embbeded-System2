import serial
import requests
import random, time
import re
import json

flag = False
headers = {'Content-Type': 'application/json'}

# config COM port
while(1):
    time.sleep(0.1)
    portName = input("Enter the port name:")
    try:
        # config COM port section
        ser = serial.Serial(
            port=portName,
            baudrate=9600,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=0
        )
        break
    except:
       # Disconnected or port name Invaild
       print("Invaild Value")


while(1):
    time.sleep(0.1)
    if ser.readable():
        smo = ser.readline()
        if not smo:
            continue
        msg = smo.decode()
        print(msg)
        if msg[:2]!="IR":
            continue
        msg = msg.replace('\x00', '').replace('\n', '')
        # 반복문 넣기
        print(msg)
        try:
            data = {'value' : msg}
            print(data)
        except:
            print("Invaild Value")
            continue
        start = time.time()
        try:
            requests.post("http://localhost:8000/sensor/setSensor", data = json.dumps(data),headers=headers)
            # lora보드로 데이터 1줄 받는게 총 16초 걸림 
        except:
            print("Invaild Command")
        end = time.time()
        print(f"{end - start:.5f} sec")