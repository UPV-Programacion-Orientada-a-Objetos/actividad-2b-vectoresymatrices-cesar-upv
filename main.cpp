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
int capacidadPila = 10; // tamaño máximo de la pila
int sizePila = 0;       // cantidad de elementos en la pila

/* ====================================
        Funciones de Memoria
==================================== */
void inicializarAlmacen(void);
void liberarMemoria(void);
void redimensionarAlmacen(void);

/* ====================================
        Funciones del Menú
==================================== */
void colocarLote(void);
void controlCalidad(void);
void deshacerInspeccion(void);
void reportePorFila(void);
void busquedaPorComponente(void);

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

        case 2:
            imprimirLog("STATUS", "Opción seleccionada: 2 (Control de Calidad)\n");
            controlCalidad();
            break;

        case 3:
            imprimirLog("STATUS", "Opción seleccionada: 3 (Deshacer Inspección)\n");
            deshacerInspeccion();
            break;

        case 4:
            imprimirLog("STATUS", "Opción seleccionada: 4 (Reporte por Fila)\n");
            reportePorFila();
            break;

        case 5:
            imprimirLog("STATUS", "Opción seleccionada: 5 (Búsqueda por Componente)\n");
            busquedaPorComponente();
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

    // inicializar pila dinámica de inspecciones (capacidad fija 10)
    pilaIDLote = new int[capacidadPila];
    pilaResultado = new int[capacidadPila];
    sizePila = 0;

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

// * Duplicar capacidad del almacén y vectores paralelos
void redimensionarAlmacen()
{
    imprimirLog("STATUS", "Redimensionando almacén...");

    int nuevasFilas = filasAlmacen * 2;
    int nuevoSize = sizeAlmacen * 2;

    // crear nuevos vectores paralelos
    LoteProduccion *nuevoMaestro = new LoteProduccion[nuevoSize];
    int *nuevosIndices = new int[nuevoSize];

    // copiar datos existentes en los vectores paralelos
    for (int i = 0; i < sizeAlmacen; ++i)
    {
        nuevoMaestro[i] = maestroLotes[i];
        nuevosIndices[i] = indicesDisponibles[i];
    }

    // terminar de inicializar los nuevos campos
    for (int i = sizeAlmacen; i < nuevoSize; ++i)
    {
        nuevoMaestro[i].idLote = -1;
        nuevoMaestro[i].nombreComponente[0] = '\0';
        nuevoMaestro[i].pesoUnitario = 0.0f;
        nuevoMaestro[i].cantidadTotal = 0;
        nuevosIndices[i] = -1;
    }

    // crear nueva matriz duplicando solamente el número de filas
    LoteProduccion ***nuevoAlmacen = new LoteProduccion **[nuevasFilas];
    for (int i = 0; i < nuevasFilas; ++i)
    {
        nuevoAlmacen[i] = new LoteProduccion *[columnasAlmacen];
    }

    // inicializar toda la matriz nueva en nullptr
    for (int i = 0; i < nuevasFilas; ++i)
    {
        for (int j = 0; j < columnasAlmacen; ++j)
        {
            nuevoAlmacen[i][j] = nullptr;
        }
    }

    // asignar celdas existentes, validando con el maestro
    for (int i = 0; i < filasAlmacen; ++i)
    {
        for (int j = 0; j < columnasAlmacen; ++j)
        {
            if (almacen[i][j] == nullptr)
            {
                nuevoAlmacen[i][j] = nullptr;
            }
            else
            {
                // recorrer el maestro y la matriz vieja y ver si hay un lote comparando punteros
                int kEncontrado = -1;
                for (int k = 0; k < sizeAlmacen; ++k)
                {
                    if (&maestroLotes[k] == almacen[i][j])
                    {
                        kEncontrado = k;
                        break;
                    }
                }

                // reflejar ese mismo indice en los nuevos vectores
                if (kEncontrado >= 0)
                {
                    nuevoAlmacen[i][j] = &nuevoMaestro[kEncontrado];
                }
                else
                {
                    nuevoAlmacen[i][j] = nullptr;
                }
            }
        }
    }

    // liberar memoria de las filas viejas
    for (int i = 0; i < filasAlmacen; ++i)
    {
        delete[] almacen[i];
    }
    delete[] almacen;
    delete[] maestroLotes;
    delete[] indicesDisponibles;

    // asignar nuevos punteros y tamaños
    almacen = nuevoAlmacen;
    maestroLotes = nuevoMaestro;
    indicesDisponibles = nuevosIndices;

    filasAlmacen = nuevasFilas;
    sizeAlmacen = nuevoSize;

    imprimirLog("SUCCESS", "Capacidad duplicada con éxito. (" + std::to_string(sizeAlmacen) + " celdas)");
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

void controlCalidad()
{
    int idInspeccion;
    obtenerNumeroPositivo("ID de Lote a inspeccionar", idInspeccion);

    if (!contieneDato(indicesDisponibles, idInspeccion, sizeAlmacen))
    {
        imprimirLog("WARNING", "El ID ingresado no existe en el maestro de lotes.");
        return;
    }

    int resultado;
    obtenerNumeroEnRango("Resultado (1 = Aprobado, 0 = Rechazado)", resultado, 0, 1);

    // si la pila está llena, eliminar el elemento más antiguo
    if (sizePila == capacidadPila)
    {
        imprimirLog("STATUS", "Pila llena. Eliminando el elemento más antiguo.");
        for (int i = 1; i < capacidadPila; ++i)
        {
            pilaIDLote[i - 1] = pilaIDLote[i];
            pilaResultado[i - 1] = pilaResultado[i];
        }
        sizePila -= 1; // espacio al final
    }

    // push al final
    pilaIDLote[sizePila] = idInspeccion;
    pilaResultado[sizePila] = resultado;
    sizePila += 1;

    std::string texto = (resultado == 1) ? "Aprobado (1)" : "Rechazado (0)";
    imprimirLog("SUCCESS", "Evento PUSH a Pila: Lote " + std::to_string(idInspeccion) + " | Resultado " + texto + ".");
}

void deshacerInspeccion()
{
    if (sizePila <= 0)
    {
        imprimirLog("WARNING", "La pila de inspecciones está vacía.");
        return;
    }

    // elemento en el tope
    int idx = sizePila - 1;
    int id = pilaIDLote[idx];
    int res = pilaResultado[idx];
    sizePila -= 1;

    std::string texto = (res == 1) ? "Aprobado (1)" : "Rechazado (0)";
    imprimirLog("STATUS", "POP de Pila: Evento Lote " + std::to_string(id) + " | Resultado " + texto + ".");
    imprimirLog("SUCCESS", "Historial de inspección revertido (solo se deshizo el registro del evento).");
}

void reportePorFila()
{
    int filaInput;
    obtenerNumeroEnRango("Posición - número de fila", filaInput, 1, filasAlmacen);

    int fila = filaInput - 1;
    std::cout << "--- Reporte de Fila " << fila << " ---" << std::endl;

    for (int col = 0; col < columnasAlmacen; ++col)
    {
        LoteProduccion *p = almacen[fila][col];

        std::cout << "(" << fila << ", " << col << "): ";
        if (p != nullptr)
        {
            std::cout << "ID: " << p->idLote << ", Nombre: " << p->nombreComponente << std::endl;
        }
        else
        {
            std::cout << "Vacío" << std::endl;
        }
    }
}

void busquedaPorComponente(void)
{
    std::string nombre;
    obtenerDato("Nombre del componente a buscar", nombre);

    imprimirLog("STATUS", "Buscando componente '" + nombre + "' en el almacén...");

    int encontrados = 0;
    for (int i = 0; i < filasAlmacen; ++i)
    {
        for (int j = 0; j < columnasAlmacen; ++j)
        {
            LoteProduccion *p = almacen[i][j];
            if (p != nullptr)
            {
                if (std::string(p->nombreComponente) == nombre)
                {
                    std::cout << "(" << i << ", " << j << "): Cantidad: " << p->cantidadTotal << std::endl;
                    ++encontrados;
                }
            }
        }
    }

    if (encontrados == 0)
    {
        imprimirLog("WARNING", "No se encontraron lotes con el componente especificado.");
    }
    else
    {
        imprimirLog("SUCCESS", "Se encontraron " + std::to_string(encontrados) + " coincidencia(s).");
    }
}

/* ====================================
        Funciones Auxiliares
==================================== */
// * Buscar y encontrar el primer índice disponible
int buscarCeldaDisponible()
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
    while (true)
    {
        obtenerNumeroPositivo("ID del nuevo lote", idLote);
        if (!contieneDato(indicesDisponibles, idLote, sizeAlmacen))
            break;

        imprimirLog("WARNING", "El ID ya existe en el almacén. Por favor, ingrese otro ID.");
        std::cout << std::endl;
    }

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

    int celda = buscarCeldaDisponible();
    if (celda == -1)
    {
        imprimirLog("STATUS", "No hay espacio. Duplicando capacidad...");
        redimensionarAlmacen();
        celda = buscarCeldaDisponible();
        if (celda == -1)
        {
            imprimirLog("ERROR", "No fue posible obtener una celda libre tras redimensionar.");
            return;
        }
    }

    if (almacen[posFila][posColumna] != nullptr)
    {
        imprimirLog("WARNING", "Esta posición ya está ocupada por: " + std::string(almacen[posFila][posColumna]->nombreComponente));
        return;
    }

    imprimirLog("STATUS", "Iniciando colocación de lote.");
    std::cout << std::endl;
    LoteProduccion nuevoLote = capturarDatosLote();

    maestroLotes[celda] = nuevoLote;
    indicesDisponibles[celda] = nuevoLote.idLote;
    almacen[posFila][posColumna] = &maestroLotes[celda];

    imprimirLog("SUCCESS", "Se ha capturado el nuevo lote '" + std::string(nuevoLote.nombreComponente) + "'.");
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
