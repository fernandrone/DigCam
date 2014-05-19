import serial
import sys

# user imports
import instructions

# Insert instructions in this list. The command number corresponds to its code
# within the PIC.

def main():
    """
    Run serial communication.
    """
    ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=1) # open USB port
    print 'Reading from ' + ser.name
    print 'Command list:'
    instructions.initDatabase()
    instructions.listAll()

    while True:
        while ser.inWaiting() > 0:
            sys.stdout.write(ser.read())        # read serial data
        com = raw_input('Insert instruction: ') # ask user for command
        ser.write(instructions.encode(com))     # write serial data
        sys.stdout.write(ser.readline())
        
    ser.close();

if __name__ == "__main__":
    main()
