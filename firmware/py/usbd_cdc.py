import time
try:
    from usbd import device, cdc
    ud = device.get_usbdevice()
    cdc.setup_CDC_device()
    ctrl_cdc = cdc.CDCControlInterface('')
    data_cdc = cdc.CDCDataInterface('')
    data_cdc2 = cdc.CDCDataInterface('')
    ud.add_interface(ctrl_cdc)
    ud.add_interface(data_cdc)
    # ud.add_interface(data_cdc2)
    ud.reenumerate()
    time.sleep(10)
    print(data_cdc.ep_in)
    # sending something over CDC
    for i in range(10):
        data_cdc.write(b'Hello World' + str(i) + '\n')
        # data_cdc2.write(b'Hello World' + str(i) + '\n')
        time.sleep(0.25)
    # receiving something..
    print("Got :" + str(data_cdc.read(10)) + ":done")
except Exception as e:
    time.sleep(5)
    raise e