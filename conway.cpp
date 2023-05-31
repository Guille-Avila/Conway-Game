#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
using namespace std;
using namespace sf;

const int cellSize = 10; // Size cell
const int width = 800;
const int height = 600;
const int numRows = height / cellSize;
const int numCols = width / cellSize;

// Matrix to get neighbors
const int offsets[][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

int main() {
  RenderWindow window(VideoMode(width, height), "Conway's Game of Life");

  bool grid[numRows][numCols] = {false};
  bool newGrid[numRows][numCols] = {false};

  Clock clock;
  const Time updateInterval = seconds(0.5f);
  bool drawing = false; // variable dibujo

  // Grid ===================================
  // Columns
  vector<Vertex> verticalLines;
  for (int i = 0; i < numCols; ++i) {
    Vertex line[] = {Vertex(Vector2f(i * cellSize, 0), Color(50, 50, 50)),
                     Vertex(Vector2f(i * cellSize, height), Color(50, 50, 50))};
    verticalLines.insert(verticalLines.end(), begin(line), end(line));
  }

  // Rows
  vector<Vertex> horizontalLines;
  for (int i = 0; i < numRows; ++i) {
    Vertex line[] = {Vertex(Vector2f(0, i * cellSize), Color(50, 50, 50)),
                     Vertex(Vector2f(width, i * cellSize), Color(50, 50, 50))};
    horizontalLines.insert(horizontalLines.end(), begin(line), end(line));
  }

  // Cells ===============
  VertexArray cells(Quads);

  // Start Program ===============================
  while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed)
        window.close();

      // Interaction draw cell ==================
      if (event.type == Event::MouseButtonPressed) {
        if (event.mouseButton.button == Mouse::Left) {
          drawing = true; // start drawing
        }
      }

      if (event.type == Event::MouseButtonReleased) {
        if (event.mouseButton.button == Mouse::Left) {
          drawing = false; // end drawing
        }
      }

      // Move mouse
      if (event.type == Event::MouseMoved) {
        if (drawing) {
          // Coordinates mouse
          int x = event.mouseMove.x / cellSize;
          int y = event.mouseMove.y / cellSize;
          if (x >= 0 && x < numCols && y >= 0 && y < numRows) {
            grid[y][x] = true; // draw cell
            newGrid[y][x] = true;
          }
        }
      }
    }

    window.clear(Color::Black);

    // Draw grid ===================================
    window.draw(&verticalLines[0], verticalLines.size(), Lines);
    window.draw(&horizontalLines[0], horizontalLines.size(), Lines);

    // Draw cells ===================================
    cells.clear();

    for (int i = 0; i < numRows; ++i) {
      for (int j = 0; j < numCols; ++j) {
        if (grid[i][j]) {
          // Coordinates cells
          float x = j * cellSize;
          float y = i * cellSize;

          // Add cells
          cells.append(Vertex(Vector2f(x, y), Color(200, 200, 200)));
          cells.append(Vertex(Vector2f(x + cellSize, y), Color(200, 200, 200)));
          cells.append(Vertex(Vector2f(x + cellSize, y + cellSize), Color(200, 200, 200)));
          cells.append(Vertex(Vector2f(x, y + cellSize), Color(200, 200, 200)));
        }
      }
    }

    window.draw(cells);

    // Calculate new state cells ============================
    if (clock.getElapsedTime() >= updateInterval) {
      for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
          int aliveNeighbors = 0;

          // Count neighbors alive
          for (const auto &offset : offsets) {
            int neighborX = j + offset[1];
            int neighborY = i + offset[0];

            // Verufy limits grid
            if (neighborX >= 0 && neighborX < numCols && neighborY >= 0 && neighborY < numRows &&
                grid[neighborY][neighborX]) {
              aliveNeighbors++; // add neighbor
            }
          }

          // Rules ===============================
          if (grid[i][j]) {
            // cell alive
            if (aliveNeighbors < 2 || aliveNeighbors > 3) {
              newGrid[i][j] = false; // die
            } else {
              newGrid[i][j] = true; // life
            }
          } else {
            // Celda muerta
            if (aliveNeighbors == 3) {
              newGrid[i][j] = true; // born
            }
          }
        }
      }
    }

    // Update grid
    if (clock.getElapsedTime() > updateInterval) {
      memcpy(grid, newGrid, sizeof(grid));
      clock.restart(); // Restart countdown
    }

    window.display();
  }
}
