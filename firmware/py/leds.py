import time
import machine


line4 = machine.Pin(3, machine.Pin.OUT)
line3 = machine.Pin(4, machine.Pin.OUT)
line2 = machine.Pin(5, machine.Pin.OUT)
line1 = machine.Pin(6, machine.Pin.OUT)


# leds = [
#     (True, False, None, None),
#     (True, None, False, None),
#     (True, None, None, False),
#     (False, True, None, None),
#     (None, True, False, None),
#     (None, True, None, False),
#     (False, None, True, None),
#     (None, False, True, None),
#     (None, None, True, False)
# ]

leds = [
    (True, False, None, None),
    (False, True, None, None),
    (None, True, False, None),
    (None, False, True, None),
    (False, None, True, None),
    (None, None, False, True),
    (None, False, None, True),
    (False, None, None, True),
    (True, None, False, None)
]

lines = [
    line1,
    line2,
    line3,
    line4,
]

def led_on(led):
    if led > len(leds):
        return False
    for line in lines:
        line.init(mode=machine.Pin.IN)
    for state, lineNo in zip(leds[led], range(len(lines))):
        if state == None:
            continue
        lines[lineNo].init(mode=machine.Pin.OUT)
        if state is True:
            lines[lineNo].high()
        else:
            lines[lineNo].low()  
    return True

def do_loop():
    count = 0
    while True:
        print("Doing led: ",count)
        for line in lines:
            line.init(mode=machine.Pin.IN)

        for state, lineNo in zip(leds[count], range(len(lines))):
            if state == None:
                continue
            lines[lineNo].init(mode=machine.Pin.OUT)
            if state is True:
                lines[lineNo].high()
            else:
                lines[lineNo].low()

        count = (count+1) % 9
        # data_out.write(b"*Insert Can Bus data here*\n")
        time.sleep(1)