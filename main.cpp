#include <cstdio>
#include <GL/glew.h>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

const char* vertexSource = R"(
  #version 150

  in vec2 position;

  void main() {
      gl_Position = vec4(position, 0.0, 1.0);
  }
)";

const char* fragmentSource = R"(
  #version 150

  out vec4 outColor;

  void main() {
      outColor = vec4(1.0, 1.0, 1.0, 1.0);
  }
)";

GLuint compileShader(GLenum shaderType, const char* shaderSource) {
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    char buffer[512];
    glGetShaderInfoLog(shader, 512, NULL, buffer);
    puts(buffer);
    abort();
  } else {
    return shader;
  }
}

int main() {
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;

  sf::Window window(sf::VideoMode(800, 600, 32),
                    "gg",
                    sf::Style::Titlebar | sf::Style::Close, settings);

  // Initialize GLEW.
  glewExperimental = GL_TRUE;
  glewInit();

  // Create Vertex Array Object.
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a Vertex Buffer Object and copy the vertex data to it.
  GLuint vbo;
  glGenBuffers(1, &vbo);

  GLfloat vertices[] = {
    0.0f,  0.5f,
    0.5f, -0.5f,
    -0.5f, -0.5f
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Create and compile the vertex and fragment shaders.
  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

  // Link the vertex and fragment shaders into a shader program.
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

  while (window.isOpen()) {
    sf::Event windowEvent;
    while (window.pollEvent(windowEvent)) {
      switch (windowEvent.type) {
        case sf::Event::Closed:
          window.close();
          break;
      }
    }

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a triangle from the 3 vertices
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap buffers
    window.display();
  }

  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  glDeleteBuffers(1, &vbo);

  glDeleteVertexArrays(1, &vao);

  return 0;
}
