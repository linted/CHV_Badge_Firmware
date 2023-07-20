from usbd_cdc import cdc_data


class slcan():
    def __init__(self) -> None:
        self.dev = cdc_data().dev

    def send(self, msg_id:int, dlc:int, msg:bytes, remote:bool=False):
        self.dev.write(self.format(msg_id, dlc, msg, remote) + b'\r')

    ##
    #   slcan_parse:
    #       Takes in msg which is a byte string in slcan format
    #       On success, returns tuple(id, dlc, msg, remote) else None
    ##
    def parse(self, msg:bytes):# -> typing.Optional[typing.Tuple[int, int, bytes, bool]]:
        
        if msg[0] == 'T' or msg[0] == 'R': # Check if extended frame
            if len(msg) < 10:
                raise ValueError("Message too long")
            dlc = int(msg[9])
            if dlc > 8:
                raise ValueError("Invalid dlc length")
            return (int(msg[1:9]), dlc, msg[11:dlc], msg[0] == 'R')
    
        elif msg[0] == 't' or msg[0] == 'r': # Check if normal frame
            if len(msg) < 5:
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