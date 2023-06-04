import _thread
import collections
import can
import leds
import time


# These are global so that if you are in the REPL you can access them.
# DO NOT ACCESS THESE FROM REAL CODE.
# That's bad practice and you should be ashamed.
rx_msg_queue = collections.deque(tuple(), 20)
tx_msg_queue = collections.deque(tuple(), 20)
rx_lock = _thread.allocate_lock()
tx_lock = _thread.allocate_lock()

def main():
    # start background thread
    _thread.start_new_thread(can.handle_canbus, (rx_msg_queue,tx_msg_queue,rx_lock,tx_lock))

    count = 0
    while True:
        print(f"Led {count}")

        leds.led_on(count)

        count = (count+1) % 9
        time.sleep(1)




if __name__ == "__main__":
    main()