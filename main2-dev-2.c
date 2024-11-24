#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <string.h> // Para manejo de cadenas

// Constantes
#define TAM 25            // Tamaño de cada bloque
#define FILAS 20          // Filas del tablero
#define COLUMNAS 10       // Columnas del tablero
#define ANCHO (COLUMNAS * TAM)
#define ALTO (FILAS * TAM)
#define HUD_ANCHO 220     // Espacio adicional para HUD

// Estructuras
typedef struct {
    int x, y; // Coordenadas
} Coord;

typedef struct {
    Coord orig;    // Posición del bloque central
    Coord perif[3]; // Bloques periféricos
    int color;     // Color de la pieza
} Pieza;

// Tablero
typedef int Tablero[COLUMNAS][FILAS];

// Prototipos
void InicializarTablero(Tablero tablero);
void DibujarTablero(const Tablero tablero);
void DibujarCuadrado(int x, int y, Color color);
void DibujarPieza(const Pieza *pieza);
void IncrustarPieza(Tablero tablero, const Pieza *pieza);
bool VerificarColision(const Tablero tablero, const Pieza *pieza);
void RotarDerecha(Pieza *pieza);
void RotarIzquierda(Pieza *pieza);
void GenerarNuevaPieza(Pieza *pieza);
bool FilaLlena(const Tablero tablero, int fila);
void ColapsarFila(Tablero tablero, int fila);
int EliminarFilas(Tablero tablero);
void MostrarHUD(int puntos, int nivel, const Pieza *siguiente);
void MostrarGameOver();

int esValido;

// Piezas predefinidas
Coord formas[7][3] = {
    {{1, 0}, {0, 1}, {1, 1}},  // Cuadrado
    {{1, 0}, {-1, 1}, {0, 1}}, // S
    {{0, 1}, {1, 1}, {-1, 0}}, // Z
    {{0, 1}, {0, -1}, {1, 1}}, // L
    {{0, 1}, {0, -1}, {-1, 1}},// L invertida
    {{-1, 0}, {1, 0}, {0, 1}}, // T
    {{0, 1}, {0, -1}, {0, 2}}  // Línea
};

int main(void) {
    // Inicialización de Raylib
    InitWindow(ANCHO + HUD_ANCHO, ALTO, "Tetris con Raylib");
    SetTargetFPS(60);
    srand(time(0));

    // Variables del juego
    Tablero tablero;
    InicializarTablero(tablero);

    Pieza actual, siguiente;
    GenerarNuevaPieza(&actual);
    GenerarNuevaPieza(&siguiente);
    actual.orig.x = COLUMNAS / 2 - 1;

    int puntos = 0, nivel = 0, tic = 0;

    while (!WindowShouldClose()) {
        // Entrada del usuario
        esValido = 1;
        if ((IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_DOWN)) || (IsKeyDown(KEY_RIGHT) && IsKeyDown(KEY_DOWN))) {
            esValido = 0; 
        }
        Pieza copia = actual;
        
        if (IsKeyPressed(KEY_DOWN)&esValido) actual.orig.y++;
        if (IsKeyPressed(KEY_UP)) RotarDerecha(&actual);
        if (IsKeyPressed(KEY_LEFT) & esValido) {
            Pieza copia = actual;  // Crear una copia de la pieza
            copia.orig.x--;        // Mover la copia a la izquierda
            if (!VerificarColision(tablero, &copia)) {
                actual = copia;    // Solo actualizar si no hay colisión
            }
        }
        if (IsKeyPressed(KEY_RIGHT) & esValido) {
            Pieza copia = actual;  // Crear una copia de la pieza
            copia.orig.x++;        // Mover la copia a la derecha
            if (!VerificarColision(tablero, &copia)) {
                actual = copia;    // Solo actualizar si no hay colisión
            }
        }

        
       
        int intervaloCaida = 30 - nivel * 2; // Reduce el intervalo a medida que sube el nivel
        if (intervaloCaida < 5) intervaloCaida = 5; // Establece un límite mínimo

        if (tic % intervaloCaida == 0) {
            copia = actual;
            actual.orig.y++;
        }

        // Verificar colisión
        if (VerificarColision(tablero, &actual)) {
            actual = copia; // Revertir movimiento
            if (tic % intervaloCaida == 0) { // Si colisiona al caer automáticamente
                IncrustarPieza(tablero, &actual);
                int filasEliminadas = EliminarFilas(tablero);
                puntos += filasEliminadas * filasEliminadas; // Puntuación basada en filas eliminadas
                if (puntos >= (nivel + 1) * 30) nivel++;

                // Generar nueva pieza
                actual = siguiente;
                GenerarNuevaPieza(&siguiente);
                actual.orig.x = COLUMNAS / 2 - 1;

                // Verificar si la nueva pieza colisiona al generarse (Game Over)
                if (VerificarColision(tablero, &actual)) {
                    MostrarGameOver();
                    break;
                }
            }
        }


        // Dibujar
        BeginDrawing();
        
            ClearBackground(BLACK);
            DibujarTablero(tablero);
            DibujarPieza(&actual);
            MostrarHUD(puntos, nivel, &siguiente);
        
        EndDrawing();

        
        tic++;
    }

    CloseWindow();
    return 0;
}

// Implementaciones de las funciones
void InicializarTablero(Tablero tablero) {
    for (int x = 0; x < COLUMNAS; x++)
        for (int y = 0; y < FILAS; y++)
            tablero[x][y] = 0;
}

void DibujarCuadrado(int x, int y, Color color) {
    DrawRectangle(x * TAM, y * TAM, TAM, TAM, color);
    DrawRectangleLines(x * TAM, y * TAM, TAM, TAM, WHITE);
}

void DibujarTablero(const Tablero tablero) {
    for (int x = 0; x < COLUMNAS; x++) {
        for (int y = 0; y < FILAS; y++) {
            if (tablero[x][y] != 0)
                DibujarCuadrado(x, y, (Color){tablero[x][y], 100, 100, 255});
        }
    }
}

void DibujarPieza(const Pieza *pieza) {
    Color color = (Color){pieza->color, 150, 150, 255};
    DibujarCuadrado(pieza->orig.x, pieza->orig.y, color);
    for (int i = 0; i < 3; i++)
        DibujarCuadrado(pieza->orig.x + pieza->perif[i].x, pieza->orig.y + pieza->perif[i].y, color);
}

void IncrustarPieza(Tablero tablero, const Pieza *pieza) {
    // Registrar el bloque central
    tablero[pieza->orig.x][pieza->orig.y] = pieza->color;

    // Registrar los bloques periféricos
    for (int i = 0; i < 3; i++) {
        int x = pieza->orig.x + pieza->perif[i].x;
        int y = pieza->orig.y + pieza->perif[i].y;
        tablero[x][y] = pieza->color;
    }
}


bool VerificarColision(const Tablero tablero, const Pieza *pieza) {
    // Verificar el bloque central
    if (pieza->orig.x < 0 || pieza->orig.x >= COLUMNAS || pieza->orig.y >= FILAS)
        return true;
    if (pieza->orig.y >= 0 && tablero[pieza->orig.x][pieza->orig.y] != 0)
        return true;

    // Verificar los bloques periféricos
    for (int i = 0; i < 3; i++) {
        int x = pieza->orig.x + pieza->perif[i].x;
        int y = pieza->orig.y + pieza->perif[i].y;
        if (x < 0 || x >= COLUMNAS || y >= FILAS)
            return true;
        if (y >= 0 && tablero[x][y] != 0) // Colisión con pieza existente
            return true;
    }

    return false; // No hay colisión
}



void RotarDerecha(Pieza *pieza) {
    for (int i = 0; i < 3; i++) {
        int temp = pieza->perif[i].x;
        pieza->perif[i].x = -pieza->perif[i].y;
        pieza->perif[i].y = temp;
    }
}

void GenerarNuevaPieza(Pieza *pieza) {
    pieza->orig.x = COLUMNAS / 2; //COLUMNAS / 2
    pieza->orig.y = 0; //0
    pieza->color = GetRandomValue(1, 6) * 40; // Color aleatorio
    int tipo = GetRandomValue(0, 6);
    for (int i = 0; i < 3; i++) {
        pieza->perif[i] = formas[tipo][i];
    }
}

bool FilaLlena(const Tablero tablero, int fila) {
    for (int x = 0; x < COLUMNAS; x++) {
        if (tablero[x][fila] == 0) return false;
    }
    return true;
}

void ColapsarFila(Tablero tablero, int fila) {
    for (int y = fila; y > 0; y--) {
        for (int x = 0; x < COLUMNAS; x++) {
            tablero[x][y] = tablero[x][y - 1];
        }
    }
    // Vaciar la fila superior
    for (int x = 0; x < COLUMNAS; x++) {
        tablero[x][0] = 0;
    }
}

int EliminarFilas(Tablero tablero) {
    int filasEliminadas = 0;

    // Itera por las filas desde abajo hacia arriba
    for (int fila = FILAS - 1; fila >= 0; fila--) {
        if (FilaLlena(tablero, fila)) {
            ColapsarFila(tablero, fila);
            filasEliminadas++;
            fila++; // Vuelve a verificar la misma fila tras el colapso
        }
    }

    return filasEliminadas;
}


void MostrarHUD(int puntos, int nivel, const Pieza *siguiente) {
    DrawRectangle(ANCHO, 0, HUD_ANCHO, ALTO, GRAY);    
    DrawText("TETRIS", ANCHO + 20, 20, 20, WHITE);
    DrawText(TextFormat("Puntos: %d", puntos), ANCHO + 20, 60, 20, WHITE);
    DrawText(TextFormat("Nivel: %d", nivel + 1), ANCHO + 20, 100, 20, WHITE);
    DrawText("Siguiente:", ANCHO + 20, 140, 20, WHITE);
    DibujarPieza(siguiente);
}

void MostrarGameOver() {
    DrawText("GAME OVER", ANCHO / 2 - 50, ALTO / 2 - 10, 20, RED);
    DrawText("Presiona ESPACIO para salir...", ANCHO / 2 - 100, ALTO / 2 + 20, 20, GRAY);
    while (!IsKeyPressed(KEY_SPACE)) {
        BeginDrawing();
        EndDrawing();
    }
}
