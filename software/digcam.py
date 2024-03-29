#!/usr/bin/env python
#!/usr/bin/python -i

import numpy as np
from PIL import Image
import serial
import sys
import os

# user imports
import instructions

# global variables
ser = 0
version = 0.1
WIDTH = 400
HEIGHT = 300

# constants
LOG = chr(253)
ACK = chr(254)
END = chr(255)

def main():
    """
    Run serial communication.
    """
    global ser # serial communication handler
    global version
    
    print 'Welcome to DigCam Terminal!'
    
    # if can't open USB port, quit
    if openSerialPort() == -1:
        sys.exit()           
        
    # display instructions
    helpMessage()   
    
    # deletes trash       
    while ser.inWaiting() > 0: 
        ser.read() 
         
    # main loop
    while True:
        
        com = raw_input('> Insert instruction: ')   # ask user for command
        
        if com == 'exit':  
            ser.close();
            print 'Goodbye!'
            break
        
        elif com == 'help':
            helpMessage()
            
        elif com == 'about':
            print 'DigCam Terminal Version ' + str(version)
            print 'Authors:'
            print '\t Fernando Barbosa'
            print '\t Joao Kunzel'
            print '\t Roberto Walter'
            
        else: # then it wasn't a terminal command
            code = instructions.encode(com);
            
            if code != -1:                  # check if there was an error
                ser.write(code)             # write serial data
                
                # wait for acknowledgement
                answer = ser.read()         # check answer
                while answer != ACK:        # ACK - code for acknowledgement
                    answer = ser.read()
                    
                    if answer == '':
                        print 'Error: connection timeout!'
                        sys.exit()
                    
                answer = ser.read()         # check answer
 
                # print until end of stream             
                while answer != END:        # END - code for end of stream
                    if answer == LOG:
                        readImage()
                    else:
                        sys.stdout.write(answer)
                    
                    answer = ser.read()
                    
            
def openSerialPort():
    """
    Tries to find and open the correct serial port.
    """
    global ser # serial communication handler 
    
    while True:
        port = raw_input('> Insert COM/USB port number ("'"exit"'" to quit): ');
        if port == 'exit':
            return -1;
        
        try:
            ser = serial.Serial(int(port), 115200, timeout=5)   # Windows
        except serial.serialutil.SerialException:
            try:
                ser = serial.Serial('/dev/ttyUSB' + port, 115200, timeout=5)    # Linux
            except serial.serialutil.SerialException:
                print 'Error: did not find PicDev! Check your USB connection or try another port.'
            else:
                return 0
        else:
            return 0          
   
def readImage():
    """
    Read the photo.
    """
    imgarray = np.empty([HEIGHT,WIDTH])
    for i in range(0, HEIGHT):
        for j in range(0, WIDTH):
            answer = ser.read()
            if answer != '': 
                imgarray[i][j] = ord(answer) # write ASCII code (integer value) of char    
            else: # timeout
                j = j - 1 # try again to get a char!
            print '\rByte count = {0}'.format(i*WIDTH + j + 1),
            
    imgarray = imgarray.astype(np.uint8)  # convert to uint8
    img = Image.fromarray(imgarray)
    img.show()   
    img.save(os.path.join(os.pardir, "temp\\photo.bmp"))
    
    print 'Image saved in "'"DigCam\\temp\\.photo.bmp"'"'

    txt = open(os.path.join(os.pardir, "temp\\grayscale.txt"), "w+")
    for i in range(0, HEIGHT):
        for j in range(0, WIDTH):
            txt.write(str(int(imgarray[i][j])) + " ") # write ASCII code (integer value) of char
    print 'Grayscale matrix saved in "'"DigCam\\temp\\grayscale.txt"'"'
             
def helpMessage():
    """
    Display help message.
    """
    global ser # serial communication handler
    print 'Reading from ' + ser.name
    print 'Terminal command list:'
    print '\t' +  'about' + '\t\t' + 'Display program info'
    print '\t' +  'exit' + '\t\t' + 'Exit program'
    print '\t' +  'help'  + '\t\t' + 'Display the instruction list'
    print 'Camera command list:'
    instructions.initDatabase()
    instructions.listAll()

if __name__ == "__main__":
    main()
