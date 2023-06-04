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

    can2040_py.mp_can_init()

    while (True):
        time.sleep(1)

