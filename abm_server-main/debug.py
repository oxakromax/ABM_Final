import utilx
import serial
import time
import platform
print(f'Puertos disponibles {utilx.serial_ports()}')

debug=True

if debug and platform.system() == 'Darwin' or debug and platform.system() == 'Linux':
    puertos = ['/dev/tty.usbserial-1410']
elif debug and platform.system() == 'Windows':
    puertos = ['COM4']
else:
    puertos=utilx.serial_ports()

p=serial.Serial(port=str(puertos[0]), baudrate=9600, timeout=0.5, write_timeout=0)



def recibido(puerto):
    while True:
        byte = puerto.read()
        if len(byte)>0:
            strings = "".join(map(chr, byte))
            print(strings)
            if strings=='1':
                print("1 recibido")
            #print(str(byte.decode('utf-8').replace(' ', '')))


def escritura(puerto):
    while True:
        time.sleep(3)
        puerto.write("1".encode('utf-8'))
        print("1 enviado")

import threading
t1 = threading.Thread(target=recibido, args=(p,))
#t2 = threading.Thread(target=escritura, args=(p,))
t1.start()
#t2.start()
