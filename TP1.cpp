// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/controls.hpp>

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;

int nX=16;
int nY=16;

bool orbital = true;

/*******************************************************************************/

struct Triangle {
    inline Triangle () {
        v[0] = v[1] = v[2] = 0;
    }
    inline Triangle (const Triangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];
    }
    inline Triangle (unsigned int v0, unsigned int v1, unsigned int v2) {
        v[0] = v0;   v[1] = v1;   v[2] = v2;
    }
    unsigned int & operator [] (unsigned int iv) { return v[iv]; }
    unsigned int operator [] (unsigned int iv) const { return v[iv]; }
    inline virtual ~Triangle () {}
    inline Triangle & operator = (const Triangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];
        return (*this);
    }
    // membres :
    unsigned int v[3];
};

struct Particle {
    glm::vec3 position;   
    glm::vec3 velocity;   
    float lifetime;       
    float size;          
    glm::vec4 color;
};

int idx(int i, int j, int nX, int nY){
    return i * nY + j; 
}

float generateRandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

// Fonction pour réinitialiser une particule avec de nouvelles valeurs aléatoires
void resetParticle(Particle& particle) {
    // Utilisation d'une distribution uniforme pour générer des valeurs aléatoires
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    // Réinitialiser la position de la particule à une position aléatoire dans un cube
    particle.position = glm::vec3(dis(gen), dis(gen), dis(gen));

    // Réinitialiser la vitesse de la particule avec une petite variation aléatoire
    particle.velocity = glm::vec3(dis(gen) * 0.1f, dis(gen) * 0.1f, dis(gen) * 0.1f);

    // Réinitialiser la durée de vie de la particule avec une valeur aléatoire entre 1 et 5
    particle.lifetime = dis(gen) * 4.0f + 1.0f;

    // Réinitialiser la taille de la particule avec une valeur aléatoire entre 0.1 et 1.0
    particle.size = dis(gen) * 0.5f + 0.5f;

    // Réinitialiser la couleur de la particule avec une teinte de gris aléatoire
    float gray = dis(gen) * 0.5f + 0.5f; // Valeur entre 0.5 et 1.0
    particle.color = glm::vec4(gray, gray, gray, 1.0f);
}

void updateParticles(std::vector<Particle>& particles, float deltaTime) {
    for (auto& particle : particles) {
        particle.position += particle.velocity * deltaTime;
        particle.lifetime -= deltaTime;
        if (particle.lifetime <= 0.0f) {
            resetParticle(particle);
        }
    }
}

void createCube(std::vector<glm::vec3>& vertices, std::vector<unsigned short>& indices) {
    // Define the vertices of the cube
    float sideLength = 1.0f;
    std::vector<glm::vec3> cubeVertices = {
        // Front face
        glm::vec3(-sideLength, -sideLength, sideLength),
        glm::vec3(sideLength, -sideLength, sideLength),
        glm::vec3(sideLength, sideLength, sideLength),
        glm::vec3(-sideLength, sideLength, sideLength),
        // Back face
        glm::vec3(-sideLength, -sideLength, -sideLength),
        glm::vec3(sideLength, -sideLength, -sideLength),
        glm::vec3(sideLength, sideLength, -sideLength),
        glm::vec3(-sideLength, sideLength, -sideLength)
    };

    // Define the indices of the cube
    std::vector<unsigned short> cubeIndices = {
        0, 1, 2, 3, // Front face
        1, 5, 6, 2, // Right face
        4, 5, 6, 7, // Back face
        0, 4, 7, 3, // Left face
        0, 1, 5, 4, // Bottom face
        2, 6, 7, 3  // Top face
    };

    // Add cube vertices to the vertices vector
    vertices.insert(vertices.end(), cubeVertices.begin(), cubeVertices.end());

    // Add cube indices to the indices vector
    indices.insert(indices.end(), cubeIndices.begin(), cubeIndices.end());
}

void centerCameraOnSurface(std::vector<glm::vec3>& vertices, glm::vec3& camera_position, glm::vec3& camera_target) {
    // on trouve le centre de la surface
    glm::vec3 center(0.0f);
    for (const auto& vertex : vertices) {
        center += vertex;
    }
    center /= static_cast<float>(vertices.size());

    // puis on ajuste la position et la cible de la caméra
    camera_position = center + glm::vec3(0.0f, 0.0f, 5.0f);
    camera_target = glm::normalize(center - camera_position);
}

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "TP1 - GLFW", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
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
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "vertex_shader.glsl", "fragment_shader.glsl" );

    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms
    GLuint ModelMatrix = glGetUniformLocation(programID, "Model");
    GLuint ViewMatrix = glGetUniformLocation(programID, "View");
    GLuint ProjectionMatrix = glGetUniformLocation(programID,"Projection");

    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glUniformMatrix4fv(ProjectionMatrix, 1, GL_FALSE, &Projection[0][0]);

    /****************************************/
    std::vector<unsigned short> indices; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles;
    std::vector<glm::vec3> indexed_vertices;


    // Déclaration et initialisation des particules
    std::vector<Particle> particles;
    for (int i = 0; i < 100; ++i) {
        Particle particle;
        resetParticle(particle); // Initialiser chaque particule avec des valeurs aléatoires
        particles.push_back(particle);
    }


    //Chargement du fichier de maillage
    // std::string filename("chair.off");
    // loadOFF(filename, indexed_vertices, indices, triangles );

    // Load it into a VBO
    // setTesselatedSquare(&indices,&indexed_vertices,nX,nY);

     // Création du cube
    createCube(indexed_vertices, indices); // A corrigez

    // Centrer la caméra sur la surface
    centerCameraOnSurface(indexed_vertices, camera_position, camera_target);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), &particles[0].position, GL_STREAM_DRAW);


    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do{
        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        updateParticles(particles, deltaTime);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);


        /*****************TODO***********************/
        // Model matrix : an identity matrix (model will be at the origin) then change
        glm::mat4 Model = glm::mat4(1.f);
        // Model = glm::rotate(Model,angle,glm::vec3(0.f,1.f,0.f));


        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up
        glm::mat4 View = glm::lookAt(camera_position, camera_position + camera_target, camera_up);

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms.
        glUniformMatrix4fv(ModelMatrix, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(ViewMatrix, 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(ProjectionMatrix, 1, GL_FALSE, &Projection[0][0]);

        /****************************************/

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // Draw the triangles !
        glDrawElements(
                    GL_QUADS,      // mode
                    indices.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );

        glDisableVertexAttribArray(0);

        // Mettre à jour le VBO des particules
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), &particles[0], GL_STREAM_DRAW);

        // 1rst attribute buffer : positions des particules
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(
            1, // attribute
            3, // size : x + y + z => 3
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Particle), // stride
            (void*)0 // array buffer offset
        );
        // 2nd attribute buffer : tailles des particules
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(
            3, // attribute
            1, // size : taille => 1
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Particle), // stride
            (void*)(3 * sizeof(float)) // array buffer offset (décalage de 3 float pour atteindre la taille)
        );
        // 3rd attribute buffer : couleurs des particules
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(
            4, // attribute
            4, // size : r + g + b + a => 4
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(Particle), // stride
            (void*)(4 * sizeof(float)) // array buffer offset (décalage de 4 float pour atteindre la couleur)
        );

        // Draw the particles
        glDrawArrays(GL_POINTS, 0, particles.size());

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

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

    //Camera zoom in and out
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_position += cameraSpeed * camera_target;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_position -= cameraSpeed * camera_target;

    // Use controls.hpp functions to handle input

    // camera strafe left and right
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera_position -= glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_position += glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeed;

    // camera move up and down
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera_position += camera_up * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera_position -= camera_up * cameraSpeed;

    // update camera target based on the new position
    camera_target = glm::normalize(camera_target);

    //TODO add translations
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}