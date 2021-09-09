#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadTexture(char const * path);
void init();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    bool sign = true; // +

    PointLight pointLight;
    DirLight dirLight;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

};

ProgramState *programState;

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    glEnable(GL_DEPTH_TEST);
    init();


    // KOCKA

    Shader cubeShader("resources/shaders/cubeShader.vs", "resources/shaders/cubeShader.fs");

    float cubeVertices[] = {
            // pozicija         // normale           // koor za tex
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };


    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);


    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // tekstura za kocku
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/TexturesCom_BrokenGlass0048_1_S.jpg").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/TexturesCom_BrokenGlass0048_2_S.jpg").c_str());

    cubeShader.use();
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);

    // __________________________

    // PLANT
    Shader plantShader("resources/shaders/plantShader.vs", "resources/shaders/plantShader.fs");
    Model plant(FileSystem::getPath("resources/objects/IndoorPotPlant/indoor plant_02_obj/indoor plant_02.obj"));

    // _________


    // TETRAEDAR (tackasti izvor svetlosti)

    Shader pointLightShader("resources/shaders/pointLight.vs", "resources/shaders/pointLight.fs");

    float pointLightVertices[] = {
            0.0f, 0.0f, 0.0f, // 0
            1.0f, 0.0f, 0.0f, // 1
            0.0f, 1.0f, 0.0f, // 2
            0.0f, 0.0f, 1.0f  // 3
    };

    unsigned int pointLightIndices[] = {
            0, 1, 2,
            0, 2, 3,
            0, 1, 3,
            1, 2, 3
    };

    unsigned int pointLightVBO, pointLightVAO, pointLightEBO;
    glGenVertexArrays(1, &pointLightVAO);
    glGenBuffers(1, &pointLightVBO);
    glGenBuffers(1, &pointLightEBO);

    glBindVertexArray(pointLightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pointLightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointLightVertices), pointLightVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pointLightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointLightIndices), pointLightIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ____________________


    // render loop
    while (!glfwWindowShouldClose(window)) {

        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);


        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        glm::mat4 model;

        if(programState->pointLight.position.x >= 3.0f)
            programState->sign = false;
        if(programState->pointLight.position.x <= -3.0f)
            programState->sign = true;
        if(programState->sign)
            programState->pointLight.position.x += 0.01f;
        else
            programState->pointLight.position.x -= 0.01f;

        // TETRAEDAR

        pointLightShader.use();

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->pointLight.position);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.25f));

        pointLightShader.setMat4("model", model);
        pointLightShader.setMat4("view", view);
        pointLightShader.setMat4("projection", projection);


        glBindVertexArray(pointLightVAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        // _____________________________



        // KOCKA

        cubeShader.use();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        cubeShader.setMat4("model", model);
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);

        // podesavanje svetla
        cubeShader.setVec3("viewPos", programState->camera.Position);
        cubeShader.setFloat("material.shininess", 32.0f);

        // direkciono
        cubeShader.setVec3("dirLight.direction", programState->dirLight.direction);
        cubeShader.setVec3("dirLight.ambient", programState->dirLight.ambient);
        cubeShader.setVec3("dirLight.diffuse", programState->dirLight.diffuse);
        cubeShader.setVec3("dirLight.specular", programState->dirLight.specular);

        // tackasto
        cubeShader.setVec3("pointLight.position", programState->pointLight.position);
        cubeShader.setVec3("pointLight.ambient", programState->pointLight.ambient);
        cubeShader.setVec3("pointLight.diffuse", programState->pointLight.diffuse);
        cubeShader.setVec3("pointLight.specular", programState->pointLight.specular);
        cubeShader.setFloat("pointLight.constant", programState->pointLight.constant);
        cubeShader.setFloat("pointLight.linear", programState->pointLight.linear);
        cubeShader.setFloat("pointLight.quadratic", programState->pointLight.quadratic);



        // aktiviranje teksura
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // __________________________

        // PLANT
        plantShader.use();

        model = glm::mat4(1.0f);
        model = glm::translate(model,glm::vec3(0.0f, 0.5f, -3.0f) );
        model = glm::scale(model, glm::vec3(0.15));
        plantShader.setMat4("model", model);
        plantShader.setMat4("view", view);
        plantShader.setMat4("projection", projection);

        // svetlo
        plantShader.setVec3("viewPos", programState->camera.Position);
        plantShader.setFloat("material.shininess", 32.0f);

        // direkciono
        plantShader.setVec3("dirLight.direction", programState->dirLight.direction);
        plantShader.setVec3("dirLight.ambient", programState->dirLight.ambient);
        plantShader.setVec3("dirLight.diffuse", programState->dirLight.diffuse);
        plantShader.setVec3("dirLight.specular", programState->dirLight.specular);

        // tackasto
        plantShader.setVec3("pointLight.position", programState->pointLight.position);
        plantShader.setVec3("pointLight.ambient", programState->pointLight.ambient);
        plantShader.setVec3("pointLight.diffuse", programState->pointLight.diffuse);
        plantShader.setVec3("pointLight.specular", programState->pointLight.specular);
        plantShader.setFloat("pointLight.constant", programState->pointLight.constant);
        plantShader.setFloat("pointLight.linear", programState->pointLight.linear);
        plantShader.setFloat("pointLight.quadratic", programState->pointLight.quadratic);

        plant.Draw(plantShader);

        // _________________




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glDeleteVertexArrays(1, &pointLightVAO);
    glDeleteBuffers(1, &pointLightVAO);
    glDeleteBuffers(1, &pointLightEBO);

    delete programState;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}



void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;

    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void init(){

    programState = new ProgramState;

    programState->camera.MouseSensitivity = 0.05f;
    programState->clearColor = glm::vec3(0.3f, 0.5f, 0.5f);

    programState->pointLight.position = glm::vec3(-2.0f, 2.0f, -3.0f);
    programState->pointLight.ambient = glm::vec3(0.07f);
    programState->pointLight.diffuse = glm::vec3(0.75f);
    programState->pointLight.specular = glm::vec3(1.0f);
    programState->pointLight.constant = 1.0f;
    programState->pointLight.linear = 0.09f;
    programState->pointLight.quadratic = 0.03f;

    programState->dirLight.direction = glm::vec3(0.3f, 0.3f, -1.0f);
    programState->dirLight.ambient = glm::vec3(0.1f);
    programState->dirLight.diffuse = glm::vec3(0.4f);
    programState->dirLight.specular = glm::vec3(0.5f);
}
