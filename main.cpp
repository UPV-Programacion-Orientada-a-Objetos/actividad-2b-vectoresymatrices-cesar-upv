#include <iostream>

typedef struct { // estructura lote de producción
    int idLote;
    char nombreComponente[50];
    float pesoUnitario;
    int cantidadTotal;
} LoteProduccion;

/* ====================================
        Variables Globales
==================================== */
LoteProduccion ***almacen; // almacén (matriz 2d)
int filasAlmacen;
int columnasAlmacen;

LoteProduccion *maestroLotes; // maestro de lotes (vectores paralelos)
int *indicesDisponibles;

int *pilaIDLote; // pila de inspecciones (stack)
int *pilaResultado;

/* ====================================
        Funciones Runtime
==================================== */
void inicializarAlmacen();

/* ====================================
        Funciones del Menú
==================================== */
void colocarLote();

/* ====================================
        Funciones Auxiliares
==================================== */
void cambiarColorCli(std::string color);
void imprimirLog(std::string tipo, std::string msj);
bool esInt(std::string s);
int preguntarInt(std::string msj, bool soloPositivos, bool sinLog);

int main(int argc, char* argv[]) {

    inicializarAlmacen();

    int opcionMenu = -1;
    while (opcionMenu != 0)
    {
        std::cout << "\n╭────────────────────────────────────────╮" << std::endl;
        std::cout << "│        AlphaTech - Electrónicos        │" << std::endl;
        std::cout << "├─ Menu ─────────────────────────────────┤" << std::endl;
        std::cout << "│ 1. Colocar Lote                        │" << std::endl;
        std::cout << "│ 2. Control de Calidad                  │" << std::endl;
        std::cout << "│ 3. Deshacer Inspección                 │" << std::endl;
        std::cout << "│ 4. Reporte por Fila                    │" << std::endl;
        std::cout << "│ 5. Busqueda por Componente             │" << std::endl;
        std::cout << "│ 0. Salir                               │" << std::endl;
        std::cout << "╰───────────────────────────────── · · · ╯" << std::endl;

        opcionMenu = preguntarInt("Opción a seleccionar: ", false, false);

        switch (opcionMenu) {
        case 1:
            colocarLote();
            break;

        case 0:
            imprimirLog("STATUS", "Saliendo del programa. \n");
            break;    

        default:
            imprimirLog("WARNING", "Opción fuera de rango.");
            break;
        }
    }

    return 0;
}

/* ====================================
        Funciones Runtime
==================================== */
void inicializarAlmacen() {
    // pedir dimensiones de la matriz
    filasAlmacen = preguntarInt("Filas iniciales del almacén: ", true, true);
    columnasAlmacen = preguntarInt("Columnas iniciales del almacén:", true, false);

    imprimirLog("STATUS", "Creando almacén inicial (de " + std::to_string(filasAlmacen) + " x " + std::to_string(columnasAlmacen) + ").");

    // arreglo de filas
    almacen = new LoteProduccion**[filasAlmacen];
    for (int i = 0; i < filasAlmacen; i++) {
        // celdas de las columnas
        almacen[i] = new LoteProduccion*[columnasAlmacen];

        // inicializar lotes en cada celda
        for (int j = 0; j < columnasAlmacen; j++) {
            almacen[i][j] = new LoteProduccion();
        }
    }

    imprimirLog("SUCCESS", "Almacén creado satisfactoriamente.");
}

/* ====================================
        Funciones del Menú
==================================== */
void colocarLote() {
    std::cout << "──────────➤ COLOCAR LOTE \n" << std::endl;
}

/* ====================================
        Funciones Auxiliares
==================================== */
void cambiarColorCli(std::string color) {
    int code;

    if (color == "RED" || color == "red")       code = 31;
    if (color == "GREEN" || color == "green")   code = 32;
    if (color == "YELLOW" || color == "yellow") code = 33;
    if (color == "WHITE" || color == "white")   code = 37;
    if (color == "CYAN" || color == "cyan")     code = 36;

    std::cout << "\033[" << code << "m";
}

void imprimirLog(std::string tipo, std::string msj) {
    std::string color;

    if (tipo == "WARNING" || tipo == "warning") color = "YELLOW";
    if (tipo == "STATUS" || tipo == "status")   color = "CYAN";
    if (tipo == "SUCCESS" || tipo == "success") color = "GREEN";
    if (tipo == "ERROR" || tipo == "error")     color = "RED";

    cambiarColorCli(color);
    std::cout << "──────────➤ " << tipo << ": " << msj << "\n";
    cambiarColorCli("WHITE");
}

bool esInt(std::string s) {
    try {
        std::stoi(s);
    } catch (const std::exception &e) {
        return false;
    }

    return true;
}

int preguntarInt(std::string msj, bool soloPositivos, bool sinLog) {
    std::string userInput;
    do {
        std::cout << "\n╭──────── ⋅ ⋅ ── INT ── ⋅ ⋅ ──────" << std::endl;
        std::cout << "│ " << msj << std::endl;
        std::cout << "│ > ";
        std::getline(std::cin, userInput);
        std::cout << "╰";

        if (!esInt(userInput)) {
            imprimirLog("WARNING", "Valor no númerico.");
            continue;
        }

        if (soloPositivos) {
            if (stoi(userInput) <= 0) {
                imprimirLog("WARNING", "Este número no puede ser menor o igual a cero.");
            }
        }
    } while (!esInt(userInput) || (soloPositivos && stoi(userInput) <= 0));

    if (sinLog) {
        std::cout << "──➤ \n" << std::endl; 
    }
    return stoi(userInput);
}