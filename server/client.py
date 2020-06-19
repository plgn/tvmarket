import paho.mqtt.client as mqtt

import asyncio
import time


HOST = '127.0.0.1'

def on_message(mosq, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))


mqttc = mqtt.Client()
mqttc.on_message = on_message
mqttc.connect(HOST, 1883, 60)
mqttc.subscribe("#", 0)

mqttc.loop_forever()

