#include <SFML/System.hpp>
#include <SFML/Window.hpp>

int main() {
  sf::Window w(sf::VideoMode(1366, 768), "gg", sf::Style::Fullscreen);
  sf::sleep(sf::seconds(1.f));
  return 0;
}
