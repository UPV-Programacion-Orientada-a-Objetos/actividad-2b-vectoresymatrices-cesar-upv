#include <iostream>

// estructura lote de producción
typedef struct {
 int idLote;
 char nombreComponente[50];
 float pesoUnitario;
 int cantidadTotal;
} LoteProduccion;

// almacén (matriz 2d)
LoteProduccion **almacen;

// maestro de lotes (vectores paralelos)
LoteProduccion *maestroLotes;
int *indicesDisponibles;

// pila de inspecciones (stack)
int *pilaIDLote;
int *pilaResultado;

int main() {

    // todo: iniciarlizarAlmacen()

    int opcionMenu = -1;
    while (opcionMenu != 0) {
        std::cout << "╭────────────────────────────────────────╮" << std::endl;
        std::cout << "│        AlphaTech - Electrónicos        │" << std::endl;
        std::cout << "├─ Menu ─────────────────────────────────┤" << std::endl;
        std::cout << "│ 1. Colocar Lote                        │" << std::endl;
        std::cout << "│ 2. Control de Calidad                  │" << std::endl;
        std::cout << "│ 3. Deshacer Inspección                 │" << std::endl;
        std::cout << "│ 4. Reporte por Fila                    │" << std::endl;
        std::cout << "│ 5. Busqueda por Componente             │" << std::endl;
        std::cout << "│ 0. Salir                               │" << std::endl;
        std::cout << "╰───────────────────────────────── · · · ╯" << std::endl;

        // todo: wrap a esto en una función preguntar x...
        std::cout << "Ingresar opción:" << std::endl;
        std::cout << "> ";
        std::cin >> opcionMenu;

        switch (opcionMenu)
        {
        case 1:
            // ! main workflow
            break;
        
        default:
            // todo: implementar función de logs...
            std::cout << "\033[33m[WARNING] Opción fuera de rango." << std::endl;
            break;
        }
    }
    
    return 0;
}