import time
import machine


class leds() :
    # _leds = [
    #     (True, False, None, None),
    #     (False, True, None, None),
    #     (None, True, False, None),
    #     (None, False, True, None),
    #     (False, None, True, None),
    #     (None, None, False, True),
    #     (None, False, None, True),
    #     (False, None, None, True),
    #     (True, None, False, None)
    # ]
    _leds = [
        (False, True, None, None), #18
        (True, False, None, None), #15
        (None, False, True, None), #22
        (None, True, False, None), #19
        (True, None, False, None), #16
        (None, None, True, False), #23
        (None, True, None, False), #20
        (True, None, None, False), #17
        (False, None, True, None), #21
    ]

    _lines = [
        machine.Pin(6, machine.Pin.OUT),
        machine.Pin(5, machine.Pin.OUT),
        machine.Pin(4, machine.Pin.OUT),
        machine.Pin(3, machine.Pin.OUT),
    ]

    def __init__(self) -> None:
        self.count = 0
        self.itteration = 0
        self.speed = 100
        self.reverse = False

    def do_loop_step(self) -> None:
        if (self.itteration % self.speed) == 0:
            # self.itteration = 0
            for state, lineNo in zip(self._leds[self.count], range(len(self._lines))):
                if state == None:
                    self._lines[lineNo].init(mode=machine.Pin.IN)
                else:
                    self._lines[lineNo].init(mode=machine.Pin.OUT)
                    if state is True:
                        self._lines[lineNo].high()
                    else:
                        self._lines[lineNo].low()

            if self.reverse:
                self.count = (self.count-1) % len(self._leds)
            else:
                self.count = (self.count+1) % len(self._leds)
        self.itteration += 1


# _lines = [
#     machine.Pin(6, machine.Pin.OUT),
#     machine.Pin(5, machine.Pin.OUT),
#     machine.Pin(4, machine.Pin.OUT),
#     machine.Pin(3, machine.Pin.OUT),
# ]



# def do_led(a):
#     for s, l in zip(a, range(4)):
#         if s is None:
#             _lines[l].init(mode=machine.Pin.IN)
#         else:
#             _lines[l].init(mode=machine.Pin.OUT)
#             if s is True:
#                 _lines[l].high()
#             else:
#                 _lines[l].low()