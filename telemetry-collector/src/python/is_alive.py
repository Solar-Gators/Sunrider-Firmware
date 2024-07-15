"""Basic script that sends PI diagnostic info"""

import time
from udp import send_tlm

DELAY_TIME = 5

while 1:
    print("sending alive message")
    send_tlm({
        "model": "pi/alive"
    })
    time.sleep(1)
