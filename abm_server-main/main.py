import time
import keyboard
import utilx
import serial
import platform
import threading

def greeting():
    print('''
Bienvenido al App Communication, recuerda que debes conectarte via bluetooh a los dispositivos, 
si estas en modo debug,solo se usaran los puertos seleccionados, 
caso contrario se usaran todos los puertos com disponibles, 
asi que recuerda no tener ningun otro dispositivo conectado a la maquina.
O ingresar correctamente los COM que vas a utilizar si estas en modo debug.

    ''')
    input("Una vez preparado, Presiona cualquier tecla para continuar")
    return True

debug = True
R1='/dev/tty.R1'
R2='/dev/tty.C3P0'
B1='/dev/tty.GARRA1'
B2='/dev/tty.G2'


if debug and platform.system() == 'Darwin' or debug and platform.system() == 'Linux':
    puertos = [R1] #asignacion de roles, de forma creciente, orden 0,1,2,3

elif debug and platform.system() == 'Windows':

    puertos = ['COM6','COM12','COM15'] #aca de agrega los puertos en windows,
    #puertos = ['COM12','COM15','COM15'] #aca de agrega los puertos en windows,

else:
    puertos = utilx.serial_ports()  # toma todos los puertos com conectados sin distincion

continuar = greeting()
if continuar:
    comunica = []
    for p in puertos:
        #se abre los puertos y se agrega a la lista comunica
        print("abriendo puerto", p)
        try:
            comunica.append(serial.Serial(port=str(p), baudrate=9600, timeout=0.5, write_timeout=0))
        except Exception as e:
            print("Error al abrir puerto", e)





def stop():
    strings="{ffff}"
    for p in comunica:
        p.write(strings.encode('utf-8'))


def manejo():
    while True:  
        try:  
            if keyboard.is_pressed('z'): 
                stop()
                time.sleep(3)
                break  
        except:
            break 




def recibido(puerto, numero_puerto: int):
    "los puertos van desde {0,1,2,3}"
    while True:
        byte = puerto.read()
        if len(byte) > 0:
            strings = "".join(map(chr, byte))
            if strings:
                #esto se agrega debido a que ocurre un error de parseo de python en los corchetes
                strings = strings.replace("\n", "")
                strings = strings.replace(" ", "")

                if strings=="z":
                    stop()


                else:
                    for p in comunica:
                        #aca se juega con la posicion de los puertos, por lo cual es importante tener la identificacion,
                        #estos van por indice de la lista, des decir comunica[0] es el puerto 0

                        if numero_puerto == 0:
                            cadena = "{{{0}000}}".format(strings)

                            print(f"recibi {cadena} del puerto 0")
                            if p.is_open:
                                p.write(cadena.encode('utf-8'))
                            else:
                                print("Ocurrio un error con el puerto", p.port)
                        elif numero_puerto == 1:
                            cadena = "{{0{0}00}}".format(strings)

                            if p.is_open:
                                p.write(cadena.encode('utf-8'))
                            else:
                                print("Ocurrio un error con el puerto", p.port)
                        elif numero_puerto == 2:
                            cadena = "{{00{0}0}}".format(strings)
                            if p.is_open:
                                p.write(cadena.encode('utf-8'))
                            else:
                                print("Ocurrio un error con el puerto", p.port)
                        elif numero_puerto == 3:
                            cadena = "{{000{0}}}".format(strings)
                            if p.is_open:
                                p.write(cadena.encode('utf-8'))
                            else:
                                print("Ocurrio un error con el puerto", p.port)
                        print(numero_puerto,cadena, strings)


def iniciar_autos(puerto):
        cadena="{ssss}"

        puerto.write(cadena.encode('utf-8'))
        print(f'{cadena} enviado')

if __name__ =='__main__':
    print("iniciando autos")

    for p in comunica:
        iniciar_autos(p)
    

    time.sleep(3)
    for p in comunica:
        #por cada puerto abierto agregado a la lista comunica se crea un thread con su posicion
        t1 = threading.Thread(target=recibido, args=(p, comunica.index(p)))
        #t2 = threading.Thread(target=escritura, args=(p,))
        #t3 = threading.Thread(target=iniciar_autos, args=(p,))
        #t2.start()
        #t3.start()
        t1.start()
    t2 = threading.Thread(target=manejo)
    t2.start()
    print("Esperando terminar")
    time.sleep(0)
    print("terminado")