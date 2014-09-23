#include <cstdio>
#include <GL/glew.h>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

int main() {
  sf::Window window(sf::VideoMode(800, 600), "gg", sf::Style::Close);
  glewExperimental = GL_TRUE;
  glewInit();

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  printf("%u\n", vertexBuffer);

  bool running = true;
  while (running) {
    sf::Event windowEvent;
    while (window.pollEvent(windowEvent)) {
      switch (windowEvent.type) {
        case sf::Event::Closed:
          running = false;
          break;
        case sf::Event::KeyPressed:
          if (windowEvent.key.code == sf::Keyboard::Escape)
            running = false;
          break;
      }
    }
  }
  return 0;
}
