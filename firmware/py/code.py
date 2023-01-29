import usb_cdc
import time

data_out = usb_cdc.data

while True:
    data_out.write(b"*Insert Can Bus data here*\n")
    time.sleep(1)