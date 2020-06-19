import asyncio
import time

import paho.mqtt.client as mqtt
from omxplayer.player import OMXPlayer
from pathlib import Path


HOST = '127.0.0.1'
VIDEO_PATH = Path('/home/pi/tech_video_comp0001-0630.mp4')


class Machine:

    state_idle = 'IDLE'
    state_opening = 'OPENING'
    state_hold = 'HOLD ON'
    state_qr = 'QR CODE'

    def __init__(self):
        self.state = self.state_idle
        self.player = OMXPlayer(VIDEO_PATH, args=['--blank', '--loop'])

def on_message(mosq, obj, msg):
    print(msg.topic + ' ' + str(msg.qos) + ' ' + str(msg.payload))

def idle(mosq, obj, msg):
    print(msg.topic + ' ' + str(msg.qos) + ' ' + str(msg.payload))
    machine.state = machine.state_idle
    machine.process_state()

mqttc = mqtt.Client()
mqttc.on_message = on_message
mqttc.connect(HOST, 1883, 60)
mqttc.subscribe('#', 0)

machine = Machine()

mqttc.loop_forever()

