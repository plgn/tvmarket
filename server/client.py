import asyncio
import time

import paho.mqtt.client as mqtt
# from omxplayer.player import OMXPlayer
from pathlib import Path


HOST = '127.0.0.1'
VIDEO_PATH = Path('/home/pi/tech_video_comp0001-0630.mp4')


class Machine:

   # state_idle = 'IDLE'
   # state_opening = 'OPENING'
   # state_cycle_content = 'CYCLE CONTENT'
   # state_finish_content = 'FINISH CONTENT'
   # state_hold = 'HOLD ON'
   # state_return = 'RETURN'
   # state_qr = 'QR CODE'

    

    def __init__(self):
        # self.player = OMXPlayer(VIDEO_PATH, args=['--blank', '--loop'])

        self.state_idle = self.idle
        self.state_opening = self.opening
        self.state_cycle_content = self.cycle_content
        self.state_hold_on = self.hold_on
        self.state_finish_content = self.finish_content
        self.state_return = self.show_return
        self.state_qr = self.show_qr

        self.state = self.state_idle

    async def idle(self):
        print('idling')
        #await asyncio.sleep(1)

    async def opening(self):
        print('opening')
        #await asyncio.sleep(1)
        self.state = self.state_cycle_content

    async def cycle_content(self):
        print('cycling')
        await asyncio.sleep(1)

    async def hold_on(self):
        print('hold on')
        #await asyncio.sleep(1)
        self.state = self.state_finish_content 
    
    async def finish_content(self):
        print('finishing content')
        #await asyncio.sleep(1)
        self.state = self.state_return

    async def show_return(self):
        print('returning')
        #await asyncio.sleep(1)
        self.state = self.state_cycle_content

    async def show_qr(self):
        print('showing qr')
        #await asyncio.sleep(1)
        self.state = self.state_cycle_content


machine = Machine()

async def process_state():
    while True:
        await machine.state()
        await(asyncio.sleep(1))


### MQTT STUFF ###

def on_message(mosq, obj, msg):
    print(msg.topic + ' ' + str(msg.qos) + ' ' + str(msg.payload))

def parse_message(mosq, obj, msg):
    print('parsing_message: ', msg.topic, msg.payload)
    
    stand, thing, sensor = msg.topic.split('/')

    # machine.state = machine.state_opening

    if sensor == 'proximity':
        if msg.payload == b'True':  # person came
            if machine.state == machine.state_idle:
                machine.state = machine.state_opening
            elif machine.state in [machine.state_cycle_content, machine.state_qr]:
                machine.state = machine.state_hold_on
        else:  # person left
            if machine.state == machine.state_cycle_content:
                machine.state = machine.state_idle
    elif sensor == 'interaction':
        if msg.payload == b'True':  # thing taken
            if machine.state in [machine.state_cycle_content, machine.state_qr]: 
                machine.state = machine.state_hold_on
            else:
                machine.state = machine.state_qr
        else:  # thing put down
            machine.state = machine.state_cycle_content


mqttc = mqtt.Client()
mqttc.on_message = on_message
mqttc.connect(HOST, 1883, 60)
mqttc.subscribe('#', 0)

mqttc.message_callback_add("stand0/#", parse_message)


async def read_messages():
    while True:
        mqttc.loop()
        await asyncio.sleep(0.1)


loop = asyncio.get_event_loop()
task = loop.create_task(read_messages())
task2 = loop.create_task(process_state())


loop.run_forever()

