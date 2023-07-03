import time
import collections
from types import Tuple

import canbus

import slcan
from leds import leds

class can():

    def __init__(self) -> None:
        self.bus = canbus.bus()
        self.rx_msg_queue = collections.deque(tuple(), 20)
        self.tx_msg_queue = collections.deque(tuple(), 20)
    
    def send(self, arbid:int, dlc:int, data:bytes) -> None:
        self.tx_queue.append((arbid,dlc,data))

    def recv(self) -> Tuple[int, int, bytes]:
        return self.rx_queue.popleft()

    # Don't call this. It doesn't exist.
    # Stop looking at it
    def _send(self) -> None:
        msg = self.tx_queue.popleft()
        if msg == None:
            return
        try:
            self.bus.send(id=msg[0], dlc=msg[1], data=msg[2])
        except Exception:
            pass

    def _recv(self) -> None:
        # there can only be 10 messages in the queue
        # when we get here. So if things are added 
        # while we are processing messages, we don't
        # want to get stuck forever
        for i in range(10):
            if len(self.rx_msg_queue) >= 20:
                # queue can't handle any more, 
                # why keep shoving stuff in?
                return
            res = bus.try_recv()
            if res == None:
                break # Nothing to read
            self.rx_msg_queue.append(res)


def handle_canbus(bus):

    led_handler = leds()

    while (True):
        # send our message
        bus.send(arbid=1, dlc=1, data=b'\x01')
        
        # shhh you don't see this
        bus._send()
        bus._recv()

        leds.do_loop_step()
        time.sleep(.01)

