import time
import collections

import canbus

# import slcan
from leds import leds

class can():

    def __init__(self) -> None:
        self.bus = canbus.bus()
        self.rx_queue = collections.deque(tuple(), 20)
        self.tx_queue = collections.deque(tuple(), 20)
    
    def send(self, arbid:int, dlc:int, data:bytes) -> None:
        self.tx_queue.append((arbid,dlc,data))

    def recv(self):# -> Tuple[int, int, bytes]:
        return self.rx_queue.popleft()

    # Don't call this. It doesn't exist.
    # Stop looking at it
    def _send(self) -> None:
        try:
            msg = self.tx_queue.popleft()
        except Exception:
            return
        if msg == None:
            return
        # try:
        self.bus.send(id=msg[0], dlc=msg[1], data=msg[2])
        # except Exception:
        #     pass

    def _recv(self):
        # there can only be 10 messages in the queue
        # when we get here. So if things are added 
        # while we are processing messages, we don't
        # want to get stuck forever
        msgs = []
        for _ in range(10):
            # queue can't handle any more, 
            # why keep shoving stuff in?
            queue_len = len(self.rx_queue)
            if queue_len >= 20 or queue_len == 0:
                return msgs
            try:
                res = self.bus.try_recv()
                self.rx_queue.append(res)
                msgs.append(res)
            except Exception:
                return msgs
        return msgs


def handle_canbus(bus):

    led_handler = leds()
    led_handler.speed = 10

    counter = 0
    while (True):
        counter += 1
        # send our message
        
        # shhh you don't see this
        bus._send()
        msgs = bus._recv()

        for msg in msgs:
            if msg[0] == 0x10 and msg[1] >= 1:
                led_handler.speed = int.from_bytes(msg[2],'little')
                bus.send(arbid=0x10 + 0x40, dlc=1, data=b'\x01')
            elif msg[0] == 0x12 and msg[1] == 7:
                if msg[2] == b'forward':
                    led_handler.reverse = False
                    bus.send(arbid=0x10 + 0x40, dlc=8, data=b'onwards!')
                elif msg[2] == b'reverse':
                    led_handler.reverse = True
                    bus.send(arbid=0x10 + 0x40, dlc=8, data=b'retreat!')

        led_handler.do_loop_step()
        time.sleep(.01)

