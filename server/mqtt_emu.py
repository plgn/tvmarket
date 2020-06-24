# EMULATOR OF modbus2mqtt output, for development purposes

import itertools
import random
import time

import argparse

import paho.mqtt.publish as publish


HOSTNAME = '10.0.2.17' 
HOSTNAME = '127.0.0.1'

POSSIBLE_STATES = [
    (
   'stand{}/thing{}/{}'.format(s, t, d), v) for s, t, d, v in itertools.product([0], range(4), ('proximity', 'interaction'), (True, False))  
   ]

# print(POSSIBLE_STATES)


parser = argparse.ArgumentParser(description='arguments')
parser.add_argument('--random', action='store_true', dest='random')
parser.add_argument('-t', action='store', dest='topic')
parser.add_argument('-v', action='store', dest='value')


args = parser.parse_args()

if __name__ == '__main__':   

    if args.random: 
        message = random.choice(POSSIBLE_STATES)
        # time.sleep(random.random())  
    else:
        message = (args.topic, args.value) 

    print(message, '->', HOSTNAME)
    publish.single(message[0], message[1], hostname=HOSTNAME)
