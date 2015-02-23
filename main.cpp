#include <iostream>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SOIL/SOIL.h>

const char vertexSource[] = R"(
  #version 150

  in vec2 position;
  in vec3 color;
  in vec2 texcoord;

  out vec3 Color;
  out vec2 Texcoord;

  void main() {
    Texcoord = texcoord;
    Color = color;
    gl_Position = vec4(position, 0.0, 1.0);
  }
)";

const char fragmentSource[] = R"(
  #version 150

  in vec3 Color;
  in vec2 Texcoord;

  out vec4 outColor;

  uniform sampler2D texKitten;
  uniform sampler2D texPuppy;
  uniform float time;

  void main() {
    vec4 colKitten = texture(texKitten, Texcoord);
    vec4 colPuppy = texture(texPuppy, Texcoord);
    float blendFactor = (sin(time * 90.0) + 1.0) / 2.0;
    outColor = mix(colKitten, colPuppy, blendFactor);
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
    std::cerr << buffer << '\n';
    std::abort();
  } else {
    return shader;
  }
}

int main() {
  // Initialize SDL.
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_Window* window = SDL_CreateWindow(
      "gg", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
      SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  // Initialize GLEW.
  glewExperimental = GL_TRUE;
  glewInit();

  // Create Vertex Array Object.
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a Vertex Buffer Object and copy the vertex data to it.
  GLfloat vertices[] = {
  //  X      Y      R     G     B      S     T
    -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Top-left
     0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Top-right
     0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f  // Bottom-left
  };
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
  };
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
               GL_STATIC_DRAW);

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

  // Specify the layout of the vertex data.
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        nullptr);

  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void*)(2 * sizeof(float)));

  GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void*)(5 * sizeof(float)));

  // // Black/white checkerboard
  // float pixels[] = {
  //   0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
  //   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f
  // };
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

  // Set up textures.
  GLuint textures[2];
  glGenTextures(2, textures);

  int width, height;
  unsigned char* image;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);
  glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  image = SOIL_load_image("sample2.png", &width, &height, 0, SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);
  glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // TODO(tsion): Test mipmaps.
  // glGenerateMipmap(GL_TEXTURE_2D);

  GLint timeUniform = glGetUniformLocation(shaderProgram, "time");

  SDL_Event windowEvent;
  while (true) {
    if (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT) {
        break;
      }

      if (windowEvent.type == SDL_KEYUP &&
          windowEvent.key.keysym.sym == SDLK_ESCAPE) {
        break;
      }
    }

    // Clear the screen to black.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set the time uniform.
    glUniform1f(timeUniform, (GLfloat)clock() / (GLfloat)CLOCKS_PER_SEC);

    // Draw a rectangle from the 2 triangles using 6 indices.
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Swap buffers.
    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}
