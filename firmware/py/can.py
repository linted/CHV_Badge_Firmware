import canhack
import time
import slcan

def safe_can_tx(tx_queue, tx_lock, msg):
    with tx_lock:
        tx_queue.append(msg)

def safe_can_rx(rx_queue, rx_lock):
    with rx_lock:
        return rx_queue.popleft()


def handle_canbus(rx_queue, tx_queue, rx_lock, tx_lock):

    bus = canhack.INTERFACE()
    bus.send(id=1, dlc=1, data=b'\x01')

    while (True):
        time.sleep(1)

