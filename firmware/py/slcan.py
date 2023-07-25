from usbd_cdc import cdc_data


class slcan():
    _BITRATES = {
        10000: "S0",
        20000: "S1",
        50000: "S2",
        100000: "S3",
        125000: "S4",
        250000: "S5",
        500000: "S6",
        750000: "S7",
        1000000: "S8",
        83300: "S9",
    }
    _version = b'V0100\r'
    _err = b'\a'
    _success = b'\r'

    def __init__(self) -> None:
        self.dev = cdc_data().dev

    def send(self, msg_id:int, dlc:int, msg:bytes, remote:bool=False):
        self.dev.write(self.format(msg_id, dlc, msg, remote) + b'\r')

    def recv(self): # -> Optional[int, bool, Tuple[int, int, bytes, bool]]
        raw_msg = self.dev.read(11, '\r')
        if raw_msg == None or len(raw_msg) < 1:
            return None
        elif raw_msg[-1] != ord('\r'):
            # someone is sending stuff on the usb bus
            # that isn't slcan stuff
            # ... or we got caught in the middle of a transfer
            # Either way this msg isn't legit anymore so trash it
            return None
        msg = bytes(raw_msg).rstrip(b'\r')

        if msg[0] in ['T', 'R', 't', 'r']:
            # yay! it's an slcan message, parse and return!
            try:
                res = self.parse(msg)
                self.dev.write(self._success)
                return res
            except ValueError:
                self.dev.write(self._err)
                return None
            
        if msg[0] == b'S':
            # this is a standard speed setting message.
            speed = self._BITRATES.get(msg, None)
            if speed == None:
                self.dev.write(self._err) # send error msg
                return None
            else:
                self.dev.write(self._success)
                return speed
        # elif msg[0] == b's':
        #     # non-standard speed setting :(
        #     # we don't support this because it's complicated
        #     self.dev.write(self._err) # \a == error
        #     return None
        
        if msg[0] == b'O':
            # This is a open msg. We should start the bus
            self.dev.write(self._success) # just tell them everything good
            return True
        
        if msg[0] == b'C':
            # this is a close msg. We should stop the bus
            self.dev.write(self._success)
            return False
        
        if msg[0] == b'V':
            # version request, just send it and don't tell anyone
            self.dev.write(self._version)
            return None
        
        # if msg[0] == b'M':
        #     # this sets acceptance codes
        #     pass

        # if msg[0] == b'm':
        #     # this sets mask
        #     pass

        # default catch for all the other message types
        self.dev.write(self._err)
        return None
        

    ##
    #   slcan_parse:
    #       Takes in msg which is a byte string in slcan format
    #       On success, returns tuple(id, dlc, msg, remote) else None
    ##
    def parse(self, msg:bytes):# -> typing.Optional[typing.Tuple[int, int, bytes, bool]]:
        
        if msg[0] == 'T' or msg[0] == 'R': # Check if extended frame
            if len(msg) > 10:
                raise ValueError("Message too long")
            dlc = int(msg[9])
            if dlc > 8:
                raise ValueError("Invalid dlc length")
            return (int(msg[1:9]), dlc, msg[11:dlc], msg[0] == 'R')
    
        elif msg[0] == 't' or msg[0] == 'r': # Check if normal frame
            if len(msg) > 5:
                raise ValueError("Message too long")
            dlc = int(msg[4])
            if dlc > 8:
                raise ValueError("Invalid dlc length")
            return (int(msg[1:4]), dlc, msg[6:dlc], msg[0] == 'r')
        else:
            raise ValueError("Invalid message type")

    ##
    #   slcan_fmt:
    #       msg_id and dlc are int
    #       msg is the raw bytes of the message (ie. b'\x00\x01\x02')
    #       On success, returns slcan ascii bytes else None
    ##
    def format(self, msg_id:int, dlc:int, msg:bytes, remote:bool=False) -> bytes:
        if msg_id > 0x7ff: # is extended?
            return f"{'T' if not remote else 'R'}{msg_id:08X}{dlc:d}{msg.hex().upper()}".encode()
        else:
            return f"{'t' if not remote else 'r'}{msg_id:03X}{dlc:d}{msg.hex().upper()}".encode()