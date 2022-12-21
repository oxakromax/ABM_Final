from datetime import time

from main import comunica

def escritura(mensaje,puerto):
    while True:
        dato="f"
        #cadena = "{{{0},{0},{0},{0}}}".format(dato)
        #cadena="{rrrr}"
        time.sleep(3)
        puerto.write(mensaje.encode('utf-8'))
        print(f'{mensaje} enviado')

def escritura(puerto):
    while True:
        dato="f"
        #cadena = "{{{0},{0},{0},{0}}}".format(dato)
        #cadena="{rrrr}"
        mensaje=str(input("INGRESA:"))
        time.sleep(5)
        if mensaje is None:
            pass
        else:
            puerto.write(mensaje.encode('utf-8'))
            print(f'{mensaje} enviado')



def escritura(puerto):
    while True:
        dato="f"
        #cadena = "{{{0},{0},{0},{0}}}".format(dato)
        cadena="{aaaa}"
        puerto.write(cadena.encode('utf-8'))
        print(f'{cadena} enviado')
        time.sleep(3)
        cadena = "{rrrr}"
        puerto.write(cadena.encode('utf-8'))
        print(f'{cadena} enviado')