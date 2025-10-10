#include <iostream>
#include <string>
#include <limits>    // std::numeric_limits
#include <algorithm> // std::find
#include <cstdio>    // std::snprintf
#include <iomanip>   // std::setw, std::fixed y std::setprecision

/* ====================================
        Struct LoteProduccion
==================================== */
typedef struct LoteProduccion
{
    int idLote;
    char nombreComponente[50];
    float pesoUnitario;
    int cantidadTotal;
} LoteProduccion;

/* ====================================
        Variables Globales
==================================== */
LoteProduccion ***almacen = nullptr; // almacén (matriz 2d)
int filasAlmacen = 0;
int columnasAlmacen = 0;
int sizeAlmacen = 0;

LoteProduccion *maestroLotes = nullptr; // maestro de lotes (vectores paralelos)
int *indicesDisponibles = nullptr;

int *pilaIDLote = nullptr; // pila de inspecciones (stack)
int *pilaResultado = nullptr;

/* ====================================
        Funciones de Memoria
==================================== */
void inicializarAlmacen(void);
void liberarMemoria(void);

/* ====================================
        Funciones del Menú
==================================== */
void colocarLote(void);

/* ====================================
        Funciones Auxiliares
==================================== */
int buscarCeldaDisponible(void);
LoteProduccion capturarDatosLote(void);
void registrarLote(int posFila, int posColumna);

void obtenerDato(std::string mensaje, std::string &valor);

template <typename T>
void obtenerDato(std::string mensaje, T &valor);

template <typename T>
void obtenerNumeroPositivo(std::string mensaje, T &valor);

template <typename T>
void obtenerNumeroEnRango(std::string mensaje, T &valor, int inicio, int fin);

template <typename T>
bool contieneDato(T arr[], T elemento, int size);

void cambiarColorCli(std::string color);
void imprimirLog(std::string tipo, std::string msj);

// ? ==== INICIO MAIN ===================================================
int main(int argc, char *argv[])
{
    std::cout << std::fixed;
    std::cout << std::setprecision(2); // floats a dos decimales

    imprimirLog("STATUS", "Inicializando programa. \n");
    inicializarAlmacen();

    int opcionMenu = -1;
    while (opcionMenu != 0)
    {
        std::cout << "\nAlphaTech - Electrónicos" << std::endl;
        std::cout << "─ Menú ─────────────────────────────────" << std::endl;
        std::cout << "1. Colocar Lote                       " << std::endl;
        std::cout << "2. Control de Calidad                 " << std::endl;
        std::cout << "3. Deshacer Inspección                " << std::endl;
        std::cout << "4. Reporte por Fila                   " << std::endl;
        std::cout << "5. Busqueda por Componente            " << std::endl;
        std::cout << "0. Salir                              " << std::endl;
        std::cout << "────────────────────────────────── · · ·" << std::endl;

        obtenerDato("Ingrese opción", opcionMenu);

        switch (opcionMenu)
        {
        case 1:
            imprimirLog("STATUS", "Opción seleccionada: 1 (Colocar Lote)\n");
            colocarLote();
            break;

        case 0:
            liberarMemoria();
            imprimirLog("STATUS", "Saliendo del programa. \n");
            break;

        default:
            imprimirLog("WARNING", "Opción fuera de rango.");
            break;
        }
    }

    return 0;
}
// ? ==== FIN MAIN ===================================================

/* ====================================
        Funciones de Memoria
==================================== */

// * Inicializar el maestro de lotes, vectores paralelos y la matriz con el mismo tamaño
void inicializarAlmacen()
{
    obtenerNumeroPositivo("Filas iniciales del almacén", filasAlmacen);
    obtenerNumeroPositivo("Columnas iniciales del almacén", columnasAlmacen);

    sizeAlmacen = filasAlmacen * columnasAlmacen;
    imprimirLog("STATUS", "Creando almacén inicial (de " + std::to_string(filasAlmacen) + " x " + std::to_string(columnasAlmacen) + ").");
    imprimirLog("STATUS", "Tamaño total de la matriz: " + std::to_string(sizeAlmacen) + " celdas.");

    // llenar vectores paralelos
    maestroLotes = new LoteProduccion[sizeAlmacen];
    indicesDisponibles = new int[sizeAlmacen];
    for (int i = 0; i < sizeAlmacen; i++)
    {
        maestroLotes[i].idLote = -1;                // id nula
        maestroLotes[i].nombreComponente[0] = '\0'; // valores en 0 o nulo para evitar bugs
        maestroLotes[i].pesoUnitario = 0.0f;
        maestroLotes[i].cantidadTotal = 0;
        indicesDisponibles[i] = -1; // celda libre
    }

    // arreglo de filas
    almacen = new LoteProduccion **[filasAlmacen];
    for (int i = 0; i < filasAlmacen; i++)
    {
        // celdas de las columnas
        almacen[i] = new LoteProduccion *[columnasAlmacen];

        // asignar cada celda como nulo
        for (int j = 0; j < columnasAlmacen; j++)
        {
            almacen[i][j] = nullptr;
        }
    }

    imprimirLog("SUCCESS", "Inicialización exitosa.");
}

// * Delete y dirigir a nullptr toda la memoria dinámica
void liberarMemoria()
{
    if (almacen)
    {
        imprimirLog("STATUS", "Liberando memoria del almacén...");
        for (int i = 0; i < filasAlmacen; ++i)
        {
            delete[] almacen[i];
        }
        delete[] almacen;
        almacen = nullptr;
    }

    imprimirLog("STATUS", "Liberando memoria del maestro de lotes...");
    delete[] maestroLotes;
    maestroLotes = nullptr;
    delete[] indicesDisponibles;
    indicesDisponibles = nullptr;

    imprimirLog("STATUS", "Liberando memoria de la pila...");
    delete[] pilaIDLote;
    pilaIDLote = nullptr;
    delete[] pilaResultado;
    pilaResultado = nullptr;

    imprimirLog("SUCCESS", "Memoria liberada con éxito.");
}

/* ====================================
        Funciones del Menú
==================================== */
void colocarLote(void)
{
    int posFila;
    obtenerNumeroEnRango("Posición - número de fila", posFila, 1, filasAlmacen);
    posFila -= 1;

    int posColumna;
    obtenerNumeroEnRango("Posición - número de columna", posColumna, 1, columnasAlmacen);
    posColumna -= 1;

    registrarLote(posFila, posColumna);
}

/* ====================================
        Funciones Auxiliares
==================================== */
// * Buscar y encontrar el primer índice disponible
int buscarCeldaDisponible(void)
{
    imprimirLog("STATUS", "Buscando una celda disponible.");
    for (int i = 0; i < sizeAlmacen; i++)
    {
        if (indicesDisponibles[i] == -1)
        {
            imprimirLog("SUCCESS", "Celda encontrada. (" + std::to_string(i) + ")");
            return i;
        }
    }
    return -1;
}

// * Imprimir un formulario de datos para crear un nuevo LoteProduccion
LoteProduccion capturarDatosLote()
{
    LoteProduccion lote{};

    int idLote;
    do
    {
        obtenerNumeroPositivo("ID del nuevo lote", idLote);
    } while (contieneDato(indicesDisponibles, idLote, sizeAlmacen));

    int cantidadTotal;
    obtenerNumeroPositivo("Cantidad total disponible", cantidadTotal);

    std::string nombreString;
    obtenerDato("Nombre del componente", nombreString);
    std::snprintf(lote.nombreComponente, sizeof(lote.nombreComponente), "%s", nombreString.c_str());

    float pesoUnitario;
    obtenerNumeroPositivo("Peso unitario (kg)", pesoUnitario);

    lote.idLote = idLote;
    lote.cantidadTotal = cantidadTotal;
    lote.pesoUnitario = pesoUnitario;

    return lote;
}

// * Registrar un nuevo lote de acuerdo a una posición
void registrarLote(int posFila, int posColumna)
{
    if (posFila < 0 || posFila >= filasAlmacen || posColumna < 0 || posColumna >= columnasAlmacen)
    {
        imprimirLog("ERROR", "Índices de celda fuera de rango.");
        return;
    }

    if (almacen[posFila][posColumna] != nullptr)
    {
        imprimirLog("WARNING", "Esta posición ya está ocupada por: " + std::string(almacen[posFila][posColumna]->nombreComponente));
        return;
    }

    int celda = buscarCeldaDisponible();
    if (celda == -1)
    {
        imprimirLog("WARNING", "No hay celdas disponibles.");
        return;
    }

    imprimirLog("STATUS", "Iniciando colocación de lote.");
    std::cout << std::endl;
    LoteProduccion nuevoLote = capturarDatosLote();

    maestroLotes[celda] = nuevoLote;
    indicesDisponibles[celda] = nuevoLote.idLote;
    almacen[posFila][posColumna] = &maestroLotes[celda];

    imprimirLog("SUCCESS", "Se ha capturado el nuevo lote " + std::string(nuevoLote.nombreComponente) + ".");
}

// * Sobrecarga de obtención de datos para obtener strings
void obtenerDato(std::string mensaje, std::string &valor)
{
    mensaje += ": ";
    std::cout << mensaje;
    std::getline(std::cin >> std::ws, valor);
}

// * Obtener tipos de datos primitivos (sin strings ni booleanos)
template <typename T>
void obtenerDato(std::string mensaje, T &valor)
{
    mensaje += ": ";
    std::cout << mensaje;

    while (!(std::cin >> valor))
    {
        imprimirLog("WARNING", "Entrada no válida, por favor, intente de nuevo.");
        std::cin.clear();                                                   // limpiar el error state
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpiar el línea

        std::cout << std::endl
                  << mensaje;
    }

    // limpiar buffer de vuelta, ya que se pudo haber escrito algo más
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// * Obtener número arriba de cero
template <typename T>
void obtenerNumeroPositivo(std::string mensaje, T &valor)
{
    obtenerDato(mensaje, valor);
    
    while (valor <= 0)
    {
        imprimirLog("WARNING", "Este número tiene que ser positivo, por favor, intente de nuevo.");

        std::cout << std::endl;
        obtenerDato(mensaje, valor);
    }
}

// * Obtener número dentro de rango
template <typename T>
void obtenerNumeroEnRango(std::string mensaje, T &valor, int inicio, int fin)
{
    // modificar mensaje original para incluir el rango
    mensaje += " [" + std::to_string(inicio) + " - " + std::to_string(fin) + "]";
    obtenerDato(mensaje, valor);

    while (valor > fin || valor < inicio)
    {
        imprimirLog("WARNING", "Este número no está dentro del rango permitido, por favor, intente de nuevo.");

        std::cout << std::endl;
        obtenerDato(mensaje, valor);
    }
}

// * Informa si un array contiene un dato en específico o no (necesita conocer el tamaño del array)
template <typename T>
bool contieneDato(T arr[], T elemento, int size)
{
    T *resultado = std::find(arr, arr + size, elemento);
    return resultado != arr + size;
}

// * Estilizar logs usando colores ANSI
void cambiarColorCli(std::string color)
{
    int code = 0;

    if (color == "RED" || color == "red")
        code = 31;
    if (color == "GREEN" || color == "green")
        code = 32;
    if (color == "YELLOW" || color == "yellow")
        code = 33;
    if (color == "WHITE" || color == "white")
        code = 37;
    if (color == "CYAN" || color == "cyan")
        code = 36;

    std::cout << "\033[" << code << "m";
}

// * Imprimir logs para debug
void imprimirLog(std::string tipo, std::string msj)
{
    std::string color = "WHITE";
    if (tipo == "WARNING" || tipo == "warning")
        color = "YELLOW";
    else if (tipo == "STATUS" || tipo == "status")
        color = "CYAN";
    else if (tipo == "SUCCESS" || tipo == "success")
        color = "GREEN";
    else if (tipo == "ERROR" || tipo == "error")
        color = "RED";

    cambiarColorCli(color);
    std::cout << "[" << tipo << "] " << msj << std::endl;
    std::cout << "\033[0m";
}
