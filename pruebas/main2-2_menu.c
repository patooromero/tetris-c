#include "raylib.h"
#include <stdbool.h>

// Constantes
#define TAM 25            // Tamaño de cada bloque
#define FILAS 20          // Filas del tablero
#define COLUMNAS 10       // Columnas del tablero
#define ANCHO (COLUMNAS * TAM)
#define ALTO (FILAS * TAM)
#define HUD_ANCHO 220     // Espacio adicional para HUD

typedef enum {
    MENU,
    JUEGO,
    PUNTAJES,
    SALIR
} EstadoJuego;

void MostrarMenu(EstadoJuego *estado) {
    ClearBackground(BLACK);
    DrawText("TETRIS", ANCHO / 2 - 50, 50, 40, WHITE);

    // Botón "Jugar"
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ANCHO / 2 - 60, 150, 120, 40})) {
        DrawRectangle(ANCHO / 2 - 60, 150, 120, 40, GRAY);
        if (IsMouseButtonPressed(MOUSE_LEFT)) {
            *estado = JUEGO;
        }
    } else {
        DrawRectangle(ANCHO / 2 - 60, 150, 120, 40, LIGHTGRAY);
    }
    DrawText("JUGAR", ANCHO / 2 - 50, 160, 20, BLACK);

    // Botón "Puntajes"
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ANCHO / 2 - 60, 210, 120, 40})) {
        DrawRectangle(ANCHO / 2 - 60, 210, 120, 40, GRAY);
        if (IsMouseButtonPressed(MOUSE_LEFT)) {
            *estado = PUNTAJES;
        }
    } else {
        DrawRectangle(ANCHO / 2 - 60, 210, 120, 40, LIGHTGRAY);
    }
    DrawText("PUNTAJES", ANCHO / 2 - 50, 220, 20, BLACK);

    // Botón "Salir"
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ANCHO / 2 - 60, 270, 120, 40})) {
        DrawRectangle(ANCHO / 2 - 60, 270, 120, 40, GRAY);
        if (IsMouseButtonPressed(MOUSE_LEFT)) {
            *estado = SALIR;
        }
    } else {
        DrawRectangle(ANCHO / 2 - 60, 270, 120, 40, LIGHTGRAY);
    }
    DrawText("SALIR", ANCHO / 2 - 50, 280, 20, BLACK);
}

void MostrarPuntajes(EstadoJuego *estado, int *puntajes, int cantidadPuntajes) {
    ClearBackground(BLACK);
    DrawText("PUNTAJES", ANCHO / 2 - 50, 50, 40, WHITE);

    for (int i = 0; i < cantidadPuntajes; i++) {
        DrawText(TextFormat("%d. %d", i + 1, puntajes[i]), ANCHO / 2 - 50, 100 + i * 30, 20, WHITE);
    }

    DrawText("Presiona ESC para volver al menú...", ANCHO / 2 - 100, ALTO - 50, 20, GRAY);
    if (IsKeyPressed(KEY_ESCAPE)) {
        *estado = MENU;
    }
}

int main(void) {
    InitWindow(ANCHO + HUD_ANCHO, ALTO, "Tetris con Raylib");
    SetTargetFPS(60);

    EstadoJuego estado = MENU;
    int puntajes[10] = {0}; // Array para almacenar puntajes
    int cantidadPuntajes = 0;

    while (estado != SALIR) {
        BeginDrawing();

        switch (estado) {
            case MENU:
                MostrarMenu(&estado);
                break;
            case JUEGO:
                // Aquí el código de tu juego
                if (IsKeyPressed(KEY_ESCAPE)) estado = MENU;
                break;
            case PUNTAJES:
                MostrarPuntajes(&estado, puntajes, cantidadPuntajes);
                break;
            default:
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

