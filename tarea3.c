#include <stdio.h>
#include <stdlib.h>
#include "tdas/stack.h"
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include "tdas/queue.h"
#include <string.h>
#include <time.h>

// Posibles acciones para ir de una posicion a otra.
#define arriba 1
#define abajo 2
#define izquierda 3
#define derecha 4

// grafo implicito
typedef struct {
    int maze[N][N]; // Matriz NxN que representa el tablero.
    int x;    // Posición x del agente.
    int y;    // Posición y del agente.
    int steps; // Pasos realizados hasta la posición actual.
    List* actions; //Secuencia de movimientos para llegar al estado.
} State;

//Funcion heuristica h(n): Calcula la Distancia de Manhattan (L1).
//Estima la distancia mas corta ignorando los obstaculos.
int distancia_L1(State* state) {
    //retorna la suma de los valores absolutos de la suma de las coordenadas finales.
    return abs(state->x - (N-1)) + abs(state->y - (N-1));
}

//Funcion que imprime el estado actual del tablero, considerando tambien las busquedas.
void imprimirEstado(const State *estado) {
    // Se crea un tablero N x N
    int mapaVisual[N][N];
    // Se copia el estado actual del tablero en el tablero recien creado.
    for(int x = 0; x < N; x++){
        for(int y = 0; y < N; y++){
            mapaVisual[x][y] = estado->maze[x][y];
        }
    }

    // Si la matriz es valida.
    if(estado->actions != NULL){
        // Variable para recorrer estados posibles.
        int* paso = list_first(estado->actions);
        // Si son movimientos validos.
        while(paso != NULL){
            // Se marca esa posicion como parte del camino a la meta.
            // 2 representa la ruta +.
            mapaVisual[paso[0]][paso[1]] = 2;
            // Pasar al siguiente movimiento valido.
            paso = list_next(estado->actions);
        }
    }

    printf("\n");
    // Se imprime el nuevo estado del tablero.
    for(int x = 0; x < N; x++){
        for(int y = 0; y < N; y++){
            // Inicio.
            if(x == 0 && y == 0)printf(" I ");
            // Meta.
            else if(x == N-1 && y == N-1)printf(" M ");
            // Posicion actual agente.
            else if(estado->x == x && estado->y == y)printf(" A ");
            // Ruta trazada.
            else if(mapaVisual[x][y] == 2)printf(" + ");
            // Espacio vacio.
            else if(mapaVisual[x][y] == 0)printf(" . ");
            // Obstaculo.
            else printf("[X]");
        }
        printf("\n");
    }
}

// Funcion que recibe un estado.
// Su objetivo es liberar un estado por completo, sus variables internas y el estado en si.
void liberar_estado(State* estado){
    // Se revisa si el estado es valido.
    if (estado == NULL)return;

    // Se revisa si la posicion actual del estado es valida.
    if (estado->actions != NULL){
        // Variable para recorrer las acciones validas (posiciones) de ese estado.
        int* paso = list_first(estado->actions);
        // Mientras el paso sea valido o existe.
        while(paso != NULL){
            // Se libera la posicion actual (paso).
            free(paso);
            // Aqui se pasa a la siguiente posicion en ese estado.
            paso = list_next(estado->actions);
        }
        // Se libera por completo la lista de acciones (por dentro).
        list_clean(estado->actions);
        // Se libera la lista de acciones en si.
        free(estado->actions);
    }
    // Se libera por completo el estado.
    free(estado);
}

// Esta funcion recibe el estado actual.
// Su objetivo es retornar verdadero si el estado actual es la meta, falso si aun no se ha llegado a la meta.
int isFinal (State* estadoActual){
    // Retorna verdadero si la posicion (x,y) actual es igual a la posicion (x,y) final.
    return (estadoActual->x == N - 1 && estadoActual->y == N - 1);
}

// Esta funcion recibe una dificultad
// Se encarga de generar un tablero-estado inicial por completo.
State crearEstadoInicial(int dificultad){
    // Se crea un estado para despues retornarlo (estado inicial).
    State estado;
    // Copiar el laberinto generado al estado que se creo anteriormente.
    generate_maze(estado.maze,  dificultad);
    // Se inicializa la posicion actual al inicio de laberinto, posicion (0,0).
    estado.x = 0;
    estado.y = 0;
    // Se inicializan los pasos que se han dado (0).
    estado.steps = 0;
    // Se crea la lista de acciones.
    estado.actions = list_create();
    // Se retorna el nuevo estado.
    return estado;
}

State* transicion(State* estadoActual, int accion){
    //Nuevo poscicion x (inicializada como copia de la posicion del estado "actual").
    int newX = estadoActual->x;
    //Nuevo posicion Y (inicializada como copia de la posicion del estado "actual").
    int newY = estadoActual->y;

    // Dependiendo de la accion que se quiere hacer se modifica la nuevo posicion que es copia de la original.
    // Si se quiere ir a una pocicion arriba entonces se le resta 1 a la posicion x.
    if (accion == 1) newX -= 1;
    // Si se quiere ir a una pocicion abajo entonces se le suma 1 a la posicion x.
    else if (accion == 2) newX += 1;
    // Si se quiere ir a una pocicion izquierda entonces se le resta 1 a la posicion y.
    else if (accion == 3) newY -= 1;
    // Si se quiere ir a una pocicion derecha entonces se le suma 1 a la posicion y.
    else if (accion == 4) newY += 1;

    // se revisa si la nueva posicion es valida.
    if (newX < 0 || newX >= N || newY < 0 || newY >= N || estadoActual->maze[newX][newY] == 1){
        return NULL;
    }

    // Se crea un nuevo estado para almacenar la nueva posicion.
    State *nuevoEstado = (State*)malloc(sizeof(State));

    // se copia el laberinto "actual" en el laberinto "nuevo" (nuevoEstado).
    for (int x = 0; x < N; x++){
        for (int y = 0; y < N; y++){
            nuevoEstado->maze[x][y] = estadoActual->maze[x][y];
        }
    }

    // Se copia la "nueva" posicion (newX, newY) en la posicion de nuevoEstado.
    nuevoEstado->x = newX;
    nuevoEstado->y = newY;
    // Se inician los pasos que se han dado en el nuevo estado, pero aumentado en 1 al estadoActual.
    nuevoEstado->steps = estadoActual->steps + 1;

    // Se crea la lista de acciones del nuevoEstado.
    nuevoEstado->actions = list_create();

    // Se revisa si el estadoActual tiene acciones "acumuladas".
    if (estadoActual->actions != NULL){
        // Se crea una varible auxiliar para recorrer las acciones del estadoActual.
        int *pasoPrevio = list_first(estadoActual->actions);
        // Se recorren las acciones del estadActual
        while(pasoPrevio != NULL){
            // Variable para guardar las posiciones de las acciones.
            int *copiaPaso = (int*)malloc(sizeof(int)*2);
            // Se copian posiciones (x,y) de cada accion de la lista de acciones del estadoActual .
            copiaPaso[0] = pasoPrevio[0];
            copiaPaso[1] = pasoPrevio[1];
            // Se agrega a la lista de acciones del nuevoEstado la copia de la accion actual del estadoActual.
            list_pushBack(nuevoEstado->actions, copiaPaso);
            // Se avanza a la siguiente accion de la lista de acciones del estadoActual.
            pasoPrevio = list_next(estadoActual->actions);
        }
    }

    // Como sobro una accion de las acciones del estadoActual,
    // se crea una variable para guardar esa accion.
    int* pasoActual = (int*)malloc(sizeof(int)*2);
    pasoActual[0] = newX;
    pasoActual[1] = newY;
    list_pushBack(nuevoEstado->actions, pasoActual);

    return nuevoEstado;
}

// Funcion que recibe un estado del laberinto.
// Se encarga de encontrar todos los movimientos posibles desde esa posicion actual..
// (ARRIBA, ABAJO, IZQUIERDA O DERECHA).
List* obtener_adyacentes(State* estadoActual){
    // Se crea lista para almacenar los estados adyacentes a ese estado.
    List* adyacentes = list_create();

    // Aqui me encargo de revisar las 4 posiciones adyacentes al estadoActual.
    for (int accion = 1; accion <= 4; accion++){
        // Aqui genero la transicion de un estado a otro, dependiendo de la accion (1-arriba, 2-abajo, 3-izquierda, 4-derecha).
        State* nuevoEstado = transicion(estadoActual, accion);
        // Mientras el estado sea valido (osea sea un nodo adyacente).
        if (nuevoEstado != NULL){
            // Se anade a la lista de adyacentes del estadoActual el nuevo estadoi.
            list_pushBack(adyacentes, nuevoEstado);
        }
    }
    // Se retorna la lista de estados adyacentes.
    return adyacentes;
}

// Funcion recibe una matriz.
// Se encarga de inicializar la matriz N x N con 0 para despues encontrar la solucion/ruta.
void iniciar_visitados(int visitados[N][N]){
    // Se recorre la matriz.
    for(int x = 0; x < N; x++){
        for (int y = 0; y < N; y++){
            // Aqui se inicializa la matriz con 0s.
            visitados[x][y] = 0;
        }
    }
}

// Funcion que recibe un estado (el laberinto y una posicion especifica).
// Se encarga de verificar si un nodo adyacente ya fue visitado o no.
int is_visited(int visitados[N][N], int x, int y){
    // Retorna true si en esa posicion hay un 1 (fue visitado).
    // Si no, retorna false.
    return visitados[x][y] == 1;
}

// Funcion que recibe un estado (laberinto y posicion  que se esta visitando).
// Se encarga de marcar un nodo como visitado.
void marcar_visitados(int visitados[N][N], int x, int y){
    // Aqui se marca el nodo con 1 (acaba de ser visitado).
    visitados[x][y] = 1;
}

// Busqueda en profundidad (DFS).
// Funcion que recibe un estado actual.
// Utiliza una pila (Stack) LIFO.
// Explora un camino hasta el final, en ese momento retrocede si no encuentra la ruta. No asegura la ruta mas corta.
void dfs(State estadoInicial){
    printf("\n\n------ Busqueda En Profundidad ------\n\n");

    // Se crea la pila para almacenar los estados por explorar.
    Stack* pila = stack_create(NULL);
    // Se inicializa la matriz de visitados con 0.
    int visitados[N][N];
    iniciar_visitados(visitados);

    // Se crea variable "inicial". Se prepara el estado inicial y se ingresa a la pila.
    State* inicial = (State*)malloc(sizeof(State));
    *inicial = estadoInicial;
    inicial->actions = list_create();
    stack_push(pila, inicial);

    // Variable para llevar el registro de cuantos nodos se procesaron.
    int contador = 0;

    // Mientras queden estados por explorar en la pila.
    while(stack_top(pila) != NULL){
        // Se extrae el nodo en la cima de la pila (osea el ultimo ingresado).
        State* pilaActual = (State*)stack_pop(pila);
        // Se aumenta el contador de visitados.
        contador++;       

        // Si el estado extraido actual es la meta, se termina la busqueda.
        if (isFinal(pilaActual)){
            printf("Ruta encontrada!!\n\n");
            printf("Pasos totales ruta [%d]\n", pilaActual->steps);
            printf("Nodos procesados (vueltas) : %d\n", contador);
            printf("Inicio - - > ");

            // Imprimir la ruta paso a paso desde la lista de acciones.
            int* pasoActual = list_first(pilaActual->actions);
            while(pasoActual != NULL){
                printf("(%d,%d) - ", pasoActual[0], pasoActual[1]);
                pasoActual = list_next(pilaActual->actions);
            }
            printf("- > Meta\n\n");
            // Se imprime el estado actual (camino) hasta la meta.
            imprimirEstado(pilaActual);

            // Aqui limpio/libero la memoria. Se vacia la pila de estados sobrantes.
            while (stack_top(pila) != NULL){
                State* estadoBorrar = (State*)stack_pop(pila);
                liberar_estado(estadoBorrar);
            }
            // Se libera pila por completo.
            free(pila);
            // Se libera el estado actual.
            liberar_estado(pilaActual);
            return;
        }

        // Si la posicion actual aun no ha sido visitada.
        if (!is_visited(visitados, pilaActual->x, pilaActual->y)){
            // Se marca como visitada para no volver a recorrerla.
            marcar_visitados(visitados, pilaActual->x, pilaActual->y);

            // Se obtienen todos los movimientos validos desde este estado.
            List* adyacentes = obtener_adyacentes(pilaActual);
            State* vecino = (State*)list_first(adyacentes);

            // Se revisa cada nodo adyacente .
            while(vecino != NULL){
                // Si el vecino no ha sido visitado, entra a la pila.
                if (!is_visited(visitados, vecino->x, vecino->y)) stack_push(pila, vecino);
                // Si ya fue visitado, se descarta y libera la memoria.
                else liberar_estado(vecino);
                vecino = (State*)list_next(adyacentes);
            }
            // Se limpia y libera la lista de adyacentes temporal (desde dentro)
            list_clean(adyacentes);
            // Se libera la lista de adyacentes temporal por completo
            free(adyacentes);

            // El estado actual ya tiene sus ramificaciones, se libera.
            liberar_estado(pilaActual);
        }
        // Si el estado extraido ya se habia visitado antes, se descarta
        else liberar_estado(pilaActual);
    }

    // Si la pila se vacio y nunca retorno, no hay ruta. Se libera.
    free(pila);
    printf("El algoritmo no encontro una ruta hacia la meta!!\n");
}

// Busqueda en anchura (BFS).
// Funcion que recibe un estado actual.
// Utiliza una cola (queue) FIFO.
// Explora un camino en ondas, siempre garantiza encontrar el camino mas corto (en pasos).
void bfs(State estadoInicial){
    printf("\n\n------ Busqueda En Anchura ------\n\n");

    // Se crea la cola para iniciar la busqueda en anchura.
    Queue* cola = queue_create(NULL);

    // Matriz para rastrear los nodos ya procesados.
    int visitados[N][N];
    iniciar_visitados(visitados);

    // Se inicializa y encola el estado inicial/partida.
    State* estadoInicialCola = (State*)malloc(sizeof(State));
    *estadoInicialCola = estadoInicial;
    estadoInicialCola->actions = list_create();
    queue_insert(cola, estadoInicialCola);

    // Variable para llevar el registro de cuantos estados se procesaron.
    int contador = 0;

    while(queue_front(cola) != NULL){
        // Se extrae el estado mas antiguo (el que esta al frente en la cola).
        State* colaActual = (State*)queue_remove(cola);
        // En cada ciclo se aumentan los pasos hasta encontrar la meta.
        contador++;

        // Se verifica si colaActual (estado actual recorrido) es la meta.
        if (isFinal(colaActual)){
            printf("Ruta encontrada con exito!!\n");
            printf("Pasos totales en ruta encontrada [%d]\n", colaActual->steps);
            printf("Nodos procesados (vueltas) : %d\n", contador);

            printf("Inicio - - > ");
            int* pasoActual = list_first(colaActual->actions);
            while(pasoActual != NULL){
                printf("(%d,%d) - ", pasoActual[0], pasoActual[1]);
                pasoActual = list_next(colaActual->actions);
            }
            printf("- > Meta\n\n");

            // Se imprime el estado actual (camino) hasta la meta.
            imprimirEstado(colaActual);

            // Se limpian los nodos sobrantes que quedaron en la cola.
            while(queue_front(cola) != NULL){
                State* estadoBorrar = (State*) queue_remove(cola);
                liberar_estado(estadoBorrar);
            }
            free(cola);
            liberar_estado(colaActual);
            return;
        }

        // Aqui se evita repetir estados solo viajando a estados no visitados.
        if (!is_visited(visitados, colaActual->x, colaActual->y)){
            // Se marca el estado actual como visitado.
            marcar_visitados(visitados, colaActual->x, colaActual->y);

            // Se generan todos los estados adyacentes al estado actual (hijos) validos.
            List* vecinosAdyacentes = obtener_adyacentes(colaActual);
            State* vecino = (State*)list_first(vecinosAdyacentes);

            while(vecino != NULL){
                // Si el adyacente es nuevo, se envia al final de la cola.
                if (!is_visited(visitados, vecino->x, vecino->y))queue_insert(cola, vecino);
                else liberar_estado(vecino);
                vecino = (State*)list_next(vecinosAdyacentes);
            }

            // Prevenir memory leaks.
            // Aqui se limpia y libera la lista de estados adyacentes (por dentro). 
            list_clean(vecinosAdyacentes);
            // Aqui se libera y limpia la lista de estados adyacentes por completo.
            free(vecinosAdyacentes);

            // Se libera el estado actual.
            liberar_estado(colaActual);
        }
        // Si ya fue visitado ese estado simplemente lo descarto.
        else liberar_estado(colaActual);
    }

    // Se libera la cola si es que nunca se encontro la meta.
    free(cola);
    printf("El algoritmo no encontro una ruta hacia la meta!!\n");
}

// Busqueda A* (Mejor Primero).
// Funcion que recibe un estado actual.
// Utiliza una cola de prioridad (heap).
// Combina el costo real g(n) con la heurisitca h(n) para ser mas eficiente que BFS.
void busquedaPrioridad(State estadoInicial){
    printf("\n\n------ Busqueda A* (Heuristica Manhattan) ------\n\n");

    // Se crea el heap.
    Heap* heap = heap_create();
    
    int visitados[N][N];
    iniciar_visitados(visitados);

    // Prepara el estado inicial.
    State* inicial = (State*)malloc(sizeof(State));
    *inicial = estadoInicial;
    inicial->actions = list_create();

    //  Se calcula inicialmente el lugar de prioridad: f(N) = g(N) + h(N). Pasos + distancia_L1.
    int prioridadInicial = inicial->steps + distancia_L1(inicial);

    // Se inserta con prioridad negativa para simular un min-heap y que extraiga primero los menores costos.
    heap_push(heap, inicial, -prioridadInicial);

    int contador = 0;

    // Mientras el heap no este vacio.
    while(heap_top(heap) != NULL){
        // Sacar el estado con mejor prioridad (menor costo).
        State* estadoActual = (State*)heap_top(heap);
        heap_pop(heap);
        contador++;

        // Se revisa si el estado actual es la meta.
        if(isFinal(estadoActual)){

            // Se imprime la ruta encontrada hasta la meta usando la busqueda A*.
            printf("Ruta Encontrada!!\n\n");
            printf("Pasos totales ruta [%d]\n", estadoActual->steps);
            printf("Nodos procesados (vueltas) : %d\n", contador);

            printf("Inicio - - >");
            // Se accede al primer estado (ubicacion) en las acciones que se hicieron hasta la meta.
            int* pasoActual = list_first(estadoActual->actions);
            // Hasta que no hatan mas pasos (se encontro la meta).
            while(pasoActual != NULL){
                // Se imprime cada posicion que se recorrio para encontrar la meta.
                printf("(%d,%d) - ", pasoActual[0], pasoActual[1]);
                // Se accede al siguiente estado (accion).
                pasoActual = list_next(estadoActual->actions);
            }
            printf("- > Meta\n\n");
            
            // Se imprime el estado actual (camino) hasta la meta.
            imprimirEstado(estadoActual);

            // Limpieza general del heap residual.
            while(heap_top(heap) != NULL){
                State* estadoBorrar = (State*)heap_top(heap);
                heap_pop(heap);
                liberar_estado(estadoBorrar);
            }
            heap_clean(heap);
            liberar_estado(estadoActual);
            return;
        }

        // Si no ha sido visitado, generamos transiciones.
        if (!is_visited(visitados, estadoActual->x, estadoActual->y)){
            // Se marca el estado actual como visitado para no volver a el en el futuro.
            marcar_visitados(visitados, estadoActual->x, estadoActual->y);
            // Lista que almacena todos los estados adyacentes al nodo.
            List* vecinosAdyacentes = obtener_adyacentes(estadoActual);

            // Variable que almacena el primer vecino de la lista creada (estados).
            State* vecino = (State*)list_first(vecinosAdyacentes);

            // Mientras hayan estados en la lista.
            while(vecino != NULL){
                if (!is_visited(visitados, vecino->x, vecino->y)){
                    //f(n) = g(n) + h(n).
                    //prioridad = pasos acumulados + Distancia manhattan.
                    int prioridad = vecino->steps + distancia_L1(vecino);

                    // Insertamos el vecino en el heap con prioridad negativa (para encontrar menor costo).
                    heap_push(heap, vecino, -prioridad);
                }
                // Si ya se visito el estado simplemente se descarta.
                else liberar_estado(vecino);
                // Se accede al siguiente estado adyacente.
                vecino = (State*)list_next(vecinosAdyacentes);
            }

            // Aqui se previenen los memory leaks. Liberar lista.
            list_clean(vecinosAdyacentes);
            free(vecinosAdyacentes);

            // Se libera el estado actual.
            liberar_estado(estadoActual);
        }
        // Si ya se visito el estado, se libera.
        else liberar_estado(estadoActual);
    }

    // Limpieza si es que no se encontro la meta.
    heap_clean(heap);
    printf("El algoritmo no encontro una ruta hacia la meta!!\n");
}

int main() {

    // Inicializador de la semilla de aleatoriedad.
    srand(time(NULL));

    // Revisar readme para un ejemplo especifico del funcionamiento de la funcion de adyacentes, y como compilar.
    printf("\n=============================================\n");
    printf(" Tarea 3: Navegacion laberinto N x N");
    printf("\n=============================================\n");

    int dificultad;
    State estadoDePrueba;
    // Variable para saber si ya hay un laberinto en la memoria.
    int laberintoGenerado = 0;
    // Variable de opciones para funcionamiento de funciones y el programa.
    int opcion;

    do{
        printf("\nIngresar dificultad del laberinto (NUMERO : 0 - 100)\n");
        if(scanf(" %d", &dificultad) != 1){
            while(getchar() != '\n');
            dificultad = -1;
        }
        if (dificultad < 0 || dificultad > 100) {
            printf("\nDificultad no permitida, ingresar una entre 0 - 100\n");
        }
    }while(dificultad < 0 || dificultad > 100);


    // Inicializacion del estado origen usando la funcion crear estado inicial.
    printf("\nGeneracion del laberinto de dificultad : %d\n", dificultad);
    estadoDePrueba = crearEstadoInicial(dificultad);
    laberintoGenerado = 1;

    do{
        printf("\n=============== MENU ===============\n");
        printf("1. Ver laberinto actual\n");
        printf("2. Generar un nuevo laberinto\n");
        printf("3. Ejecutar DFS (Busqueda en Profundidad)\n");
        printf("4. Ejecutar BFS (Busqueda en Anchura)\n");
        printf("5. Ejecutar A* (Busqueda Mejor Primero / Por Prioridad)\n");
        printf("6. Salir\n");
        printf("=====================================\n");
        printf("Ingrese una opcion: ");

        // Validacion de entrada para evitar errores de lectura.
        if(scanf(" %d", &opcion) != 1){
            while(getchar() != '\n');
            opcion = 0;
        }

        // Manejo de las opciones del programa.
        switch(opcion){

            case 1:
                if(laberintoGenerado){
                    printf("\nLaberinto actual dificultad [%d]\n", dificultad);
                    imprimirEstado(&estadoDePrueba);
                    printf("Distancia L1 ideal : %d pasos \n", distancia_L1(&estadoDePrueba));
                }
                break;

            case 2:
                // Ingresar nueva dificultad.
                do {
                    printf("\nIngresar nueva dificultad (0 - 100)\n");
                    if(scanf(" %d", &dificultad) != 1){
                        while(getchar() != '\n');
                        dificultad = -1;
                    }
                    if(dificultad < 0 || dificultad > 100) printf("Dificultad no permitida\n");
                }while(dificultad < 0 || dificultad > 100);

                // Liberacion de memoria del laberinto anterior antes de sobreescribirlo.
                list_clean(estadoDePrueba.actions);
                free(estadoDePrueba.actions);
                
                estadoDePrueba = crearEstadoInicial(dificultad);
                printf("\nNuevo aberinto generado\n");
                break;

            case 3:
                // Ejecucion busqueda DFS.
                dfs(estadoDePrueba);
                break;

            case 4:
                // Ejecucion busqueda BFS.
                bfs(estadoDePrueba);
                break;

            case 5:
                // Ejecucion busqueda algoritmo A*.
                busquedaPrioridad(estadoDePrueba);
                break;

            case 6:
                // Salida del programa.
                printf("\nSaliendo del programa!!\n");

                // Limpiza de la memoria de Estados antes de terminar la ejecucion.
                if (estadoDePrueba.actions != NULL){
                    list_clean(estadoDePrueba.actions);
                    free(estadoDePrueba.actions);
                }
                break;
            
            default:
                // Si el usuario ingresa opcion invalidad se pide una nueva.
                printf("\nOpcion invalida!!. Porfavor ingresar numero del 1 al 6\n");
                break;
        }
        // Pausa y limpieza de la consola.
        if (opcion != 6){
            presioneTeclaParaContinuar();
            limpiarPantalla();
        }
    }while(opcion != 6);

    return 0;
}
