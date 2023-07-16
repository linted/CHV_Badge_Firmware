import time
try:
    from usbd import device, cdc
    ud = device.get_usbdevice()
    cdc.setup_CDC_device()
    ctrl_cdc = cdc.CDCControlInterface('cdc interface')
    data_cdc = cdc.CDCDataInterface('data interface')
    ud.add_interface(ctrl_cdc)
    ud.add_interface(data_cdc)
    ud.reenumerate()
    time.sleep(3)
    # print(data_cdc.ep_in)
    # sending something over CDC
    for i in range(10):
        data_cdc.write(b'Hello World' + str(i) + '\r\n')
        time.sleep(0.25)
    # receiving something..
    # print("Got :" + str(data_cdc.read(10)) + ":done")
except Exception as e:
    time.sleep(2)
    raise e
