// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow *window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;
using namespace std;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/texture.hpp>
#include <TP1/Terrain.hpp>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

glm::vec3 cameraPosOrbit = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 cameraTargetOrbit = glm::vec3(0., 0., 0.);

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// rotation
float angle = 0.;
float zoom = 1.;


bool libre = true;
bool orbital = false;


/*******************************************************************************/

GLuint vertexbuffer;
GLuint elementbuffer;

// fonction pour centrer la caméra 
void centrerCamera(std::vector<glm::vec3> vertices, glm::vec3& camera_position, glm::vec3& camera_target) {
    // on trouve le centre de la surface
    glm::vec3 center(0.0f);
    for (const auto& vertex : vertices) {
        center += vertex;
    }
    center /= static_cast<float>(vertices.size());

    // puis on ajuste la position et la cible de la caméra
    camera_position = center + glm::vec3(0.0f, 2.0f, 2.0f);
    camera_target = glm::normalize(center - camera_position);
}

Object soleil;
glm::vec3 centreSphere(0.0f, 0.0f, 0.0f);


// Fonction pour déplacer un objet à une hauteur fixe au-dessus du terrain
void moveObjectAboveTerrain(Object& object, Terrain& terrain, float offset) {
    // Obtenez la position actuelle de l'objet
    glm::vec3 objectPosition = object.transform.getPosition();

    // Trouvez le triangle du terrain sur lequel se trouve l'objet
    Triangle triangle = terrain.findTriangleAt(objectPosition); 


    // Vérifiez si le triangle est valide
    if (triangle.isValid()) {
        // Effectuez une interpolation barycentrique pour calculer la hauteur du terrain à la position de l'objet
        float terrainHeight = triangle.interpolateHeight(objectPosition.x, objectPosition.z, terrain.getVertices());

        // Ajoutez l'offset à la hauteur du terrain pour définir la hauteur à laquelle placer l'objet
        float desiredHeight = terrainHeight + offset;

        // Définissez la nouvelle position de l'objet avec la hauteur désirée
        object.transform.setPosition(glm::vec3(objectPosition.x, desiredHeight, objectPosition.z));
    }
    // else {
    //     // Ne déplacez pas l'objet et affichez un message d'erreur
    //     std::cout << "Aucun triangle de terrain valide trouvé. L'objet ne sera pas déplacé." << std::endl;
    // }
}


int main(void)
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "TP1 - GLFW", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    // glEnable(GL_CULL_FACE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");

    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms

    /****************************************/
    // std::vector<std::vector<unsigned short>> triangles;

    glm::mat4 Model  = glm::mat4(1.f);
    glm::mat4 View =  glm::mat4(1.f);
    glm::mat4 Projection = glm::mat4(1.f);

    // Chargement du fichier de maillage
    std::string filename("chair.off");
    // loadOFF(filename, indexed_vertices, indices, triangles );

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // GLuint grass = loadTexture2DFromFilePath("../textures/grass.png");
    float distTerreSoleil = 12.;
    float distLuneTerre = 4.5; 

    soleil.loadObject("./sphere.off");
    soleil.loadTexture("../textures/sun.jpg");
    soleil.transform.Scale(vec3(0.3, 0.3, 0.3));
    soleil.transform.Translate(vec3(2, 2, 0));

    // Terrain plan;
    // plan.makePlan(16, 16, 4, 4, 0, 0, true, "../textures/grass.png");
    // plan.loadTexture("../textures/grass.png");
    Terrain plan(16, 16, 4, 4, 0, 0, true, "../textures/grass.png",0);
    plan.loadTexture("../textures/grass.png");

    plan.add(soleil);

    // centrer la caméra sur le plan
    centrerCamera(plan.getVertices(), camera_position, camera_target);
    centrerCamera(plan.getVertices(), cameraPosOrbit, cameraTargetOrbit); // have to try to fix this...

    Object terre;
    terre.loadObject("./sphere.off");
    terre.loadTexture("../textures/earth.jpg");

    do
    {
        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // appelez la fonction moveObjectAboveTerrain pour déplacer l'objet au-dessus du terrain
        moveObjectAboveTerrain(soleil, plan, 0.5f);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        /*****************TODO***********************/
        // Model matrix : an identity matrix (model will be at the origin) then change

        // View matrix : camera/view transformation lookat() utiliser cameraFree_position cameraFree_target cameraFree_up
        if (libre)
        {
            View = glm::lookAt(camera_position, camera_position + camera_target, camera_up);
            // glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        }

        if (orbital)
        {
            View = glm::lookAt(cameraPosOrbit, cameraTargetOrbit, camera_up);
        }

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        Projection = glm::perspective(glm::radians(45.f), 4.0f / 3.0f, 0.1f, 100.0f);
        
        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms
        glUniformMatrix4fv(glGetUniformLocation(programID, "View"), 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "Projection"), 1, GL_FALSE, &Projection[0][0]);

        // terre.transform.Translate(vec3(-distTerreSoleil, 0, 0));
        // terre.transform.Rotation(vec3(0, 1, 0), glm::radians(0.5f));
        // terre.transform.Translate(vec3(distTerreSoleil, 0, 0));

        // lune.transform.Translate(vec3(-distLuneTerre, 0, 0));
        // lune.transform.Rotation(vec3(0, 1, 0), glm::radians(0.5f));
        // lune.transform.Translate(vec3(distLuneTerre, 0, 0));
   
        // soleil.update();
        // soleil.draw(programID);

        plan.update();
        plan.draw(programID);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
        libre = false; orbital = true;
    }

    if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS){
        libre = true; orbital = false;
    }

    float cameraSpeed = 2.5 * deltaTime;

    if(libre){
        //Camera zoom in and out
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera_position += cameraSpeed * camera_target;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera_position -= cameraSpeed * camera_target;

        // camera move left and right
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera_position -= glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera_position += glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeed;

        // move up and down
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera_position += camera_up * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera_position -= camera_up * cameraSpeed;
    }

    if(orbital){
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            cameraPosOrbit += glm::normalize(camera_up) * cameraSpeed;
        if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            cameraPosOrbit -= glm::normalize(camera_up) * cameraSpeed;
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPosOrbit += glm::normalize(glm::cross(cameraTargetOrbit - cameraPosOrbit, camera_up))*cameraSpeed;
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPosOrbit -= glm::normalize(glm::cross(cameraTargetOrbit - cameraPosOrbit, camera_up))*cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        soleil.transform.Translate(vec3(0.2, 0, 0));
        centreSphere.x += 0.2 * 0.02;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        soleil.transform.Translate(vec3(-0.2, 0, 0));
        centreSphere.x += -0.2 * 0.2;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        soleil.transform.Translate(vec3(0., 0, -0.2));
        centreSphere.z += -0.2 * 0.2;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        soleil.transform.Translate(vec3(0, 0, 0.2));
        centreSphere.z += 0.2 * 0.2;
    }

    // creer des controls pour translater le soleil sur l'axe y (en haut et en bas)
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        soleil.transform.Translate(vec3(0, 0.2, 0));
        centreSphere.y += 0.2 * 0.2;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        soleil.transform.Translate(vec3(0, -0.2, 0));
        centreSphere.y += -0.2 * 0.2;
    }

    // if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS){
    //     resolution++;
    //     setTesselatedSquare(indices, indexed_vertices);
    //     glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    //     glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    //     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
    // }
    // if(glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS){
    //     if(resolution>0){ resolution--;}
    //     setTesselatedSquare(indices, indexed_vertices);
    //     glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    //     glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    //     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
    // }

    // update camera target based on the new position
    camera_target = glm::normalize(camera_target);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}