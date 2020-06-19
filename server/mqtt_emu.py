# EMULATOR OF modbus2mqtt output, for development purposes

import itertools
import random
import time

import paho.mqtt.publish as publish


HOSTNAME = '127.0.0.1' 

POSSIBLE_STATES = [
    (
   'stand{}/thing{}/{}'.format(s, t, d), v) for s, t, d, v in itertools.product([0], range(4), ('proximity', 'interaction'), (True, False))  
   ]

print(POSSIBLE_STATES)


while True:

    message = random.choice(POSSIBLE_STATES)
    print(message, '->', HOSTNAME)
    publish.single(message[0], message[1], hostname=HOSTNAME)
    time.sleep(random.random())  

