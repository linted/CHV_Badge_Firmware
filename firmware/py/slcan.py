# import typing

##
#   slcan_parse:
#       Takes in msg which is a byte string in slcan format
#       On success, returns tuple(type, id, dlc, msg) else None
##
def slcan_parse(msg:bytes):# -> typing.Optional[typing.Tuple[str, int, int, bytes]]:
    # Check if extended frame
    if msg[0] == 'T' or msg[0] == 'R':
        if len(msg) < 10:
            return None
        dlc = int(msg[9])
        if dlc > 8:
            return None             # Error: DLC is an invalid length TODO
        return (msg[0], int(msg[1:9]), dlc, msg[11:dlc])
    # Check if normal frame
    elif msg[0] == 't' or msg[0] == 'r':
        if len(msg) < 5:
            return None
        dlc = int(msg[4])
        if dlc > 8:
            return None             # Error: DLC is an invalid length TODO
        return (msg[0], int(msg[1:4]), dlc, msg[6:dlc])
    else:
        return None                 # Error: invalid msg type TODO

##
#   slcan_fmt:
#       msg_type is one of["T", "R", "t", "r"]
#       msg_id and dlc are int
#       msg is the raw bytes of the message (ie. b'\x00\x01\x02')
#       On success, returns tuple(type, id, dlc, msg) else None
##
def slcan_fmt(msg_type:str, msg_id:int, dlc:int, msg:bytes):
    if msg_type == 'T' or msg_type == 'R':
        return f"{msg_type}{msg_id:08X}{dlc:d}{msg.hex().upper()}"
    elif msg_type == 't' or msg_type == 'r':
        return f"{msg_type}{msg_id:03X}{dlc:d}{msg.hex().upper()}"
    else:
        return None