#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
using namespace std;
using namespace sf;

const int cellSize = 10; // Tamanio celda
const int width = 800;
const int height = 600;
const int numRows = height / cellSize;
const int numCols = width / cellSize;

// Matriz evaluar vecinas
const int offsets[][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

int main() {
  RenderWindow window(VideoMode(width, height), "Conway's Game of Life");

  bool grid[numRows][numCols] = {false};
  bool newGrid[numRows][numCols] = {false};

  Clock clock;
  const Time updateInterval = seconds(0.5f);
  bool drawing = false; // variable dibujo

  // Grid ===================================
  // Columnas
  vector<Vertex> verticalLines;
  for (int i = 0; i < numCols; ++i) {
    Vertex line[] = {Vertex(Vector2f(i * cellSize, 0), Color(50, 50, 50)),
                     Vertex(Vector2f(i * cellSize, height), Color(50, 50, 50))};
    verticalLines.insert(verticalLines.end(), begin(line), end(line));
  }

  // Filas
  vector<Vertex> horizontalLines;
  for (int i = 0; i < numRows; ++i) {
    Vertex line[] = {Vertex(Vector2f(0, i * cellSize), Color(50, 50, 50)),
                     Vertex(Vector2f(width, i * cellSize), Color(50, 50, 50))};
    horizontalLines.insert(horizontalLines.end(), begin(line), end(line));
  }

  // Cells ===============
  VertexArray cells(Quads);

  // inicio programa ===============================
  while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed)
        window.close();

      // Interaccion pintar celda ==================
      if (event.type == Event::MouseButtonPressed) {
        if (event.mouseButton.button == Mouse::Left) {
          drawing = true; // Activar dibujo
        }
      }

      if (event.type == Event::MouseButtonReleased) {
        if (event.mouseButton.button == Mouse::Left) {
          drawing = false; // Desactivar dibujo
        }
      }

      // Movimiento raton
      if (event.type == Event::MouseMoved) {
        if (drawing) {
          // Coordenadas raton
          int x = event.mouseMove.x / cellSize;
          int y = event.mouseMove.y / cellSize;
          if (x >= 0 && x < numCols && y >= 0 && y < numRows) {
            grid[y][x] = true; // Dibujar la celda
            newGrid[y][x] = true;
          }
        }
      }
    }

    window.clear(Color::Black);

    // Dibujar grid ===================================
    window.draw(&verticalLines[0], verticalLines.size(), Lines);
    window.draw(&horizontalLines[0], horizontalLines.size(), Lines);

    // Dibujar cells ===================================
    cells.clear();

    for (int i = 0; i < numRows; ++i) {
      for (int j = 0; j < numCols; ++j) {
        if (grid[i][j]) {
          // coordenadas de la celda
          float x = j * cellSize;
          float y = i * cellSize;

          // Agregar cells
          cells.append(Vertex(Vector2f(x, y), Color(200, 200, 200)));
          cells.append(Vertex(Vector2f(x + cellSize, y), Color(200, 200, 200)));
          cells.append(Vertex(Vector2f(x + cellSize, y + cellSize), Color(200, 200, 200)));
          cells.append(Vertex(Vector2f(x, y + cellSize), Color(200, 200, 200)));
        }
      }
    }

    window.draw(cells);

    // Calcular nuevo estado celdas ============================
    if (clock.getElapsedTime() >= updateInterval) {
      for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
          int aliveNeighbors = 0;

          // Contar vecinas vivas
          for (const auto &offset : offsets) {
            int neighborX = j + offset[1];
            int neighborY = i + offset[0];

            // Verificar limites grid
            if (neighborX >= 0 && neighborX < numCols && neighborY >= 0 && neighborY < numRows &&
                grid[neighborY][neighborX]) {
              aliveNeighbors++; // add vecina
            }
          }

          // Aplicar reglas ===============================
          if (grid[i][j]) {
            // Celda viva
            if (aliveNeighbors < 2 || aliveNeighbors > 3) {
              newGrid[i][j] = false; // Muere
            } else {
              newGrid[i][j] = true; // Vive
            }
          } else {
            // Celda muerta
            if (aliveNeighbors == 3) {
              newGrid[i][j] = true; // Nace
            }
          }
        }
      }
    }

    // Actualizar grid
    if (clock.getElapsedTime() > updateInterval) {
      memcpy(grid, newGrid, sizeof(grid));
      clock.restart(); // Reiniciar el contador de tiempo
    }

    window.display();
  }
}
