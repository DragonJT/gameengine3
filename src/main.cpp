#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

static void glfw_error_callback(int err, const char* msg) {
  std::cerr << "GLFW error " << err << ": " << msg << "\n";
}

static void framebuffer_size_callback(GLFWwindow*, int w, int h) {
  glViewport(0, 0, w, h);
}

static GLuint compileShader(GLenum type, const char* src) {
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src, nullptr);
  glCompileShader(s);

  GLint ok = 0;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    GLint len = 0;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0');
    glGetShaderInfoLog(s, len, nullptr, log.data());
    std::cerr << "Shader compile error:\n" << log << "\n";
  }
  return s;
}

static GLuint linkProgram(GLuint vs, GLuint fs) {
  GLuint p = glCreateProgram();
  glAttachShader(p, vs);
  glAttachShader(p, fs);
  glLinkProgram(p);

  GLint ok = 0;
  glGetProgramiv(p, GL_LINK_STATUS, &ok);
  if (!ok) {
    GLint len = 0;
    glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0');
    glGetProgramInfoLog(p, len, nullptr, log.data());
    std::cerr << "Program link error:\n" << log << "\n";
  }

  glDetachShader(p, vs);
  glDetachShader(p, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);
  return p;
}

int main() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) return 1;

  // Modern core context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* win = glfwCreateWindow(1280, 720, "Lit Cube (GLM)", nullptr, nullptr);
  if (!win) {
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);
  glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to init GLAD\n";
    return 1;
  }

  std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";

  glEnable(GL_DEPTH_TEST);

  // Interleaved position + normal (36 vertices, cube)
  const float vertices[] = {
    // pos                // normal
    -0.5f,-0.5f,-0.5f,    0.0f, 0.0f,-1.0f,
     0.5f,-0.5f,-0.5f,    0.0f, 0.0f,-1.0f,
     0.5f, 0.5f,-0.5f,    0.0f, 0.0f,-1.0f,
     0.5f, 0.5f,-0.5f,    0.0f, 0.0f,-1.0f,
    -0.5f, 0.5f,-0.5f,    0.0f, 0.0f,-1.0f,
    -0.5f,-0.5f,-0.5f,    0.0f, 0.0f,-1.0f,

    -0.5f,-0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
     0.5f,-0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
     0.5f, 0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
     0.5f, 0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
    -0.5f,-0.5f, 0.5f,    0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f,-0.5f,   -1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f,-0.5f,   -1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f,-0.5f,   -1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f, 0.5f,   -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f,

     0.5f, 0.5f, 0.5f,    1.0f, 0.0f, 0.0f,
     0.5f, 0.5f,-0.5f,    1.0f, 0.0f, 0.0f,
     0.5f,-0.5f,-0.5f,    1.0f, 0.0f, 0.0f,
     0.5f,-0.5f,-0.5f,    1.0f, 0.0f, 0.0f,
     0.5f,-0.5f, 0.5f,    1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, 0.5f,    1.0f, 0.0f, 0.0f,

    -0.5f,-0.5f,-0.5f,    0.0f,-1.0f, 0.0f,
     0.5f,-0.5f,-0.5f,    0.0f,-1.0f, 0.0f,
     0.5f,-0.5f, 0.5f,    0.0f,-1.0f, 0.0f,
     0.5f,-0.5f, 0.5f,    0.0f,-1.0f, 0.0f,
    -0.5f,-0.5f, 0.5f,    0.0f,-1.0f, 0.0f,
    -0.5f,-0.5f,-0.5f,    0.0f,-1.0f, 0.0f,

    -0.5f, 0.5f,-0.5f,    0.0f, 1.0f, 0.0f,
     0.5f, 0.5f,-0.5f,    0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f,    0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f,    0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,    0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f,-0.5f,    0.0f, 1.0f, 0.0f,
  };

  GLuint vao=0, vbo=0;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  const char* vsSrc = R"GLSL(
    #version 330 core
    layout (location=0) in vec3 aPos;
    layout (location=1) in vec3 aNormal;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProj;

    out vec3 vWorldPos;
    out vec3 vNormal;

    void main() {
      vec4 world = uModel * vec4(aPos, 1.0);
      vWorldPos = world.xyz;

      // correct normal transform
      vNormal = mat3(transpose(inverse(uModel))) * aNormal;

      gl_Position = uProj * uView * world;
    }
  )GLSL";

  const char* fsSrc = R"GLSL(
    #version 330 core
    out vec4 FragColor;

    in vec3 vWorldPos;
    in vec3 vNormal;

    uniform vec3 uLightPos;
    uniform vec3 uViewPos;

    uniform vec3 uObjectColor;
    uniform vec3 uLightColor;

    void main() {
      vec3 N = normalize(vNormal);
      vec3 L = normalize(uLightPos - vWorldPos);

      // ambient
      float ambientStrength = 0.15;
      vec3 ambient = ambientStrength * uLightColor;

      // diffuse
      float diff = max(dot(N, L), 0.0);
      vec3 diffuse = diff * uLightColor;

      // specular (Blinn-Phong)
      vec3 V = normalize(uViewPos - vWorldPos);
      vec3 H = normalize(L + V);
      float spec = pow(max(dot(N, H), 0.0), 64.0);
      float specStrength = 0.6;
      vec3 specular = specStrength * spec * uLightColor;

      vec3 color = (ambient + diffuse + specular) * uObjectColor;
      FragColor = vec4(color, 1.0);
    }
  )GLSL";

  GLuint prog = linkProgram(compileShader(GL_VERTEX_SHADER, vsSrc),
                            compileShader(GL_FRAGMENT_SHADER, fsSrc));

  // Uniform locations (cache once)
  glUseProgram(prog);
  const GLint locModel = glGetUniformLocation(prog, "uModel");
  const GLint locView  = glGetUniformLocation(prog, "uView");
  const GLint locProj  = glGetUniformLocation(prog, "uProj");
  const GLint locLightPos = glGetUniformLocation(prog, "uLightPos");
  const GLint locViewPos  = glGetUniformLocation(prog, "uViewPos");
  const GLint locObjCol   = glGetUniformLocation(prog, "uObjectColor");
  const GLint locLightCol = glGetUniformLocation(prog, "uLightColor");

  // Basic “camera”
  glm::vec3 camPos   = glm::vec3(1.8f, 1.4f, 2.2f);
  glm::vec3 camTarget= glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 camUp    = glm::vec3(0.0f, 1.0f, 0.0f);

  glm::vec3 lightPos = glm::vec3(1.2f, 1.5f, 1.0f);

  while (!glfwWindowShouldClose(win)) {
    glfwPollEvents();
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(win, GLFW_TRUE);

    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    float aspect = (h == 0) ? 1.0f : (float)w / (float)h;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Animate a little
    float t = (float)glfwGetTime();

    glm::mat4 model(1.0f);
    model = glm::rotate(model, t * 0.8f, glm::vec3(0.3f, 1.0f, 0.1f));

    glm::mat4 view = glm::lookAt(camPos, camTarget, camUp);
    glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

    glUseProgram(prog);
    glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(locView,  1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(locProj,  1, GL_FALSE, glm::value_ptr(proj));

    // light slowly orbits
    glm::vec3 animLight = lightPos + glm::vec3(std::cos(t) * 0.4f, 0.0f, std::sin(t) * 0.4f);
    glUniform3fv(locLightPos, 1, glm::value_ptr(animLight));
    glUniform3fv(locViewPos,  1, glm::value_ptr(camPos));

    glm::vec3 objectColor(0.2f, 0.55f, 0.9f);
    glm::vec3 lightColor (1.0f, 1.0f, 1.0f);
    glUniform3fv(locObjCol,   1, glm::value_ptr(objectColor));
    glUniform3fv(locLightCol, 1, glm::value_ptr(lightColor));

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(win);
  }

  glDeleteProgram(prog);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
