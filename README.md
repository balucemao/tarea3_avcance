Tarea 3.

Benjamin Alucema.

Fecha: 24-05-2026.

Rut: 22122629-1.


# mazeNavigator - Busqueda de rutas y pathfinding en grafos implicitos

- Importante : Ingresar la dificultad del laberinto como un numero entero entre [0, 100]

Este proyecto consiste en una herramienta escrita en lenguaje C diseñada para resolver un problema, en este caso un laberinto (N x N). Su principal objetivo es simular un agente que navega desde un punto de inicio (0, 0) hasta una meta (N-1, N-1) esquivando obstaculos, utilizando diferentes estrategias de inteligencia artificial para encontrar la ruta hacia la meta.

Para lograr esto, el problema se modela usando grafo implícito, donde cada estado representa la posición del agente y sus movimiwentos acumulados. Se implementan 3 algoritmos de búsqueda (DFS, BFS, A*) apoyados por diferentes TDAs (Pilas, Colas, Heaps) para analizar el impacto del "terreno" del laberinto en el rendimiento y garantizar la obtencion de la ruta mas corta.

## Estructuras y características necesarias para ejecutar el programa.

- TDAs requeridos: list.c, stack.c, queue.c, heap.c, extra.c

- Cabeceras: list.h, stack.h, queue.h, heap.h, extra.h

## Instrucciones de compilación.

### Cargar aplicación.

```bash
gcc tdas/*.c tarea3.c -Wno-unused-result -o mazeNavigator
```

## Ejecutar aplicación.

```bash
./mazeNavigator
```

## Funcionalidades principales:

- Generacion de laberintos (Inicial y opcion 2): El sistema usa una función recursiva para crear un camino seguro garantizado hacia la meta, y luego rellena el resto del mapa (laberinto) con obstaculos basandose en la dificultad porcentual (0-100) ingresado por el usuario.
- Laberinto actual (1): Muestra por pantalla la matriz actual indicando la posicion del agente (A), el inicio, la meta (M), los obstaculos ([x]) y el espacio libre (.). Además, calcula la distancia L1 ideal hacia la meta.
- Busqueda en profundidad - DFS (3): Utiliza el TDA stack/pila. El agente explora cada camino posible hasta llegar a un callejón sin salida antes de retroceder. No garantiza encontrar la ruta más corta, pero es útil cuando se quiere explorar topologías profundas.
- Búsqueda en anchura - BFS (4): Utiliza el TDA queue/cola. EL agente explora el entorno en "ondas" concentricas nivel a nivel. Al encontrar la meta, se garantiza matematicamente que el camino recorrido es el más corto posible en cantidad de pasos.
- Busqueda A* / Mejor primero (5): Utiliza el TDA heap/cola con prioridad. Es el algoritmo mas eficiente del programa. Suma los pasos acumulados del agente más una estimacion heuristica "optimista" (Distancia de Manhattan = h(n) = |x-xf| + |y-yf|). Esto permite que la busqueda apunte directamente hacia la meta, garantizando la ruta más corta procesando muchos menos nodos que BFS.
- Salir (6): Termina la ejecucion del programa de forma segura.

## Funcionalidades extras:

- liberar_estado: Debido a los multiples nodos (estados) en los grafos implicitos, se necesita implementar una funcion dedicada que libera todos los datos/variables/listas almacenadas en cada nodo, esto para prevenir fugas de memoria.

## Funcionalidades auxiliares:

- iniciar_visitados: Se encarga de inicializar y limpiar (llenar con 0) la matriz auxiliar de tamaño N x N cada vez que se ejecuta un nuevo algoritmo, preparando para la búsqueda.
- is_visited: Función de validación en tiempo contante o(1) que consulta la matriz auxiliar para verificar si las coordenadas (x, y) ya fueron exploradas antes.
- marcar_visitados: Registra una coordenada específica cambiando su estado a visitado (1). Trabaja en conjunto con "is_visited" para que el agente no recorra coordenadas previas de manera innecesaria ni genere ciclos infinitos de movimientos.

## Ejemplo de funcionamiento:

1. Al iniciar el programa se le pide al usuario ingresar una dificultad. Usuario ingresa 99.
2. El usuario selecciona la opcion 1: El sistema imprime el laberinto por pantalla y le muestra la distancia L1 ideal (por ejemplo 18 pasos).
3. El usuario selecciona la opcion 3 (DFS): El Programa procesa los estados utilizando la pila, encuentra la meta tras procesar 45 nodos y avisa que la ruta le tomo 28 pasos. Imprime el recorrido desde (0, 0) hasta la meta.
4. El usuario selecciona la opcion 4 (BFS): El programa utiliza una cola, se expande en todas las direcciones, procesa 120 nodos, pero encuentra el camino optimo real garantizando los 18 pasos exactos.
5. El usuario selecciona la opcion 5 (A*): El programa utiliza la cola de prioridad evaluando la heuristica. Encuentra la meta en los mismos 18 pasos optimos que BFS, pero informando que proceso tan solo 22 nodos, mostrando su eficiencia superior.
6. El usuario selecciona la opcion 2: Ingresa la dificultad 50 y el sistema genera un mapa mucha mas abierto.
7. El usuario selecciona la opcion 6: El programa libera todas las variables dinamicas y finaliza.