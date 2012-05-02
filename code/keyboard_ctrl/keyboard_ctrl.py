import sys
import serial

port_name = sys.argv[1]
baud_str = sys.argv[2]


port = serial.Serial(port_name, int(baud_str))
port.write('start 500 2400\n')


def keypress(): 
    """
    Waits for the user to press a key. Returns the ascii code 
    for the key pressed or zero for a function key pressed.
    """                             
    import msvcrt               
    while 1:
        if msvcrt.kbhit():              # Key pressed?
            a = ord(msvcrt.getch())     # get first byte of keyscan code     
            if a == 0 or a == 224:      # is it a function key?
                msvcrt.getch()          # discard second byte of key scan code
                return 0                # return 0
            else:
                return a                # else return ascii code



def go_forward():
    return "DATA 2 180 180"

def go_backward():
    return "DATA 2 0 0"

def turn_left():
    return "DATA 2 0 180"

def turn_right():
    return "DATA 2 180 0"

def stop():
    return "DATA 2 90 90"

def quit():
    exit()


key_handlers = {
    'w' : go_forward,
    'a' : turn_left,
    's' : stop,
    'd' : turn_right,
    'x' : stop,
    'q' : quit
}


for k,f in key_handlers.items():
    print "%s : %s" % (k, f.__name__)


while True:

    char = chr(keypress())

    cmd = key_handlers[char]()
    print cmd
    port.write(cmd + '\n')
