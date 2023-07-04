import _thread
import engine
import time

msg = '''
Car Hacking Village Main Badge Defcon 31
You can get started by sending and receiving can
messages using the "canbus" variable.
Good luck!
'''

# These are global so that if you are in the REPL you can access them.
# DO NOT ACCESS THESE FROM REAL CODE.
# That's bad practice and you should be ashamed.
canbus = engine.can()

# thread = _thread.start_new_thread(engine.handle_canbus, (canbus,))

try:
    while True:
        time.sleep(10)
except KeyboardInterrupt:
    print(msg)