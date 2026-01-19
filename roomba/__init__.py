# versión básica de agente inteligente

entorno = [  # 1 significa polvo
    [0,0,0,1],
    [0,0,1,0],
    [0,1,0,0],
    [0,1,1,0]
]

posicion = (0,0)
orientacion = "S"   # N, S, E, O

def mostrar_entorno(entorno, posicion):
    x, y = posicion
    for i in range(len(entorno)):
        fila = ""
        for j in range(len(entorno[0])):
            if (i, j) == (y, x):
                fila += "R "
            elif entorno[i][j] == 1:
                fila += "* "
            else:
                fila += ". "
        print(fila)
    print()

def girar_izquierda(orientacion):
    if orientacion == "N":
        return "O"
    elif orientacion == "O":
        return "S"
    elif orientacion == "S":
        return "E"
    elif orientacion == "E":
        return "N"

def girar_derecha(orientacion):
    if orientacion == "N":
        return "E"
    elif orientacion == "E":
        return "S"
    elif orientacion == "S":
        return "O"
    elif orientacion == "O":
        return "N"

def agente_v2(entorno, posicion):
    x, y = posicion

    if entorno[y][x] == 1:
        return "Aspirar"

    if y + 1 < len(entorno):
        return "Mover hacia adelante"

    if x + 1 < len(entorno[0]):
        return "Girar izquierda"

    if x - 1 >= 0:
        return "Girar derecha"

    return "Mover hacia atras"

def ejecutar_v2(entorno, posicion, orientacion, accion):
    x, y = posicion

    if accion == "Aspirar":
        entorno[y][x] = 0

    elif accion == "Mover hacia adelante":
        if orientacion == "S" and y + 1 < len(entorno):
            y += 1
        elif orientacion == "N" and y - 1 >= 0:
            y -= 1
        elif orientacion == "E" and x + 1 < len(entorno[0]):
            x += 1
        elif orientacion == "O" and x - 1 >= 0:
            x -= 1
        else:
            print("No dispareeen!!!")

    elif accion == "Mover hacia atras":
        if orientacion == "S" and y - 1 >= 0:
            y -= 1
        elif orientacion == "N" and y + 1 < len(entorno):
            y += 1
        elif orientacion == "E" and x - 1 >= 0:
            x -= 1
        elif orientacion == "O" and x + 1 < len(entorno[0]):
            x += 1
        else:
            print("No dispareeen!!!")

    elif accion == "Girar izquierda":
        orientacion = girar_izquierda(orientacion)

    elif accion == "Girar derecha":
        orientacion = girar_derecha(orientacion)

    return entorno, (x, y), orientacion


# ejecución
for paso in range(10):
    print("Orientación:", orientacion)
    mostrar_entorno(entorno, posicion)
    accion = agente_v2(entorno, posicion)
    entorno, posicion, orientacion = ejecutar_v2(entorno, posicion, orientacion, accion)
    print("Acción:", accion)