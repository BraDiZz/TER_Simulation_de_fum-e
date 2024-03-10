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

float distance_from_particle = 2.0;

struct Particle {
    glm::vec3 position;
    float time = 1000.f;
};

int idx(int i,int j,int nX,int nY){ // permet de connaitre l'indice dans un tableau
        return i*nY+j;
}

void setCube(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices) {
    // Supprime les données précédentes
    indices.clear();
    indexed_vertices.clear();

    float side = 2;
    // Sommets du cube
    indexed_vertices.push_back(glm::vec3(-side, -side, -side)); // 0
    indexed_vertices.push_back(glm::vec3(side, -side, -side));  // 1
    indexed_vertices.push_back(glm::vec3(side, side, -side));   // 2
    indexed_vertices.push_back(glm::vec3(-side, side, -side));  // 3
    indexed_vertices.push_back(glm::vec3(-side, -side, side));  // 4
    indexed_vertices.push_back(glm::vec3(side, -side, side));   // 5
    indexed_vertices.push_back(glm::vec3(side, side, side));    // 6
    indexed_vertices.push_back(glm::vec3(-side, side, side));   // 7

    //face 1 carré

    indices.push_back(0);
    indices.push_back(1);

    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(2);
    indices.push_back(3);

    indices.push_back(3);
    indices.push_back(0);

    //face 2    

    indices.push_back(4);
    indices.push_back(5);

    indices.push_back(5);
    indices.push_back(6);

    indices.push_back(6);
    indices.push_back(7);

    indices.push_back(7);
    indices.push_back(4);

    //connecté les faces

    indices.push_back(0);
    indices.push_back(4);

    indices.push_back(1);
    indices.push_back(5);

    indices.push_back(2);
    indices.push_back(6);

    indices.push_back(3);
    indices.push_back(7);

}



// Nombre de particules
const int numParticles = 100;
/*******************************************************************************/

glm::mat4 View;
glm::mat4 Model;
glm::mat4 Projection;

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
    }glfwMakeContextCurrent(window);

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
    // glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

    Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glUniformMatrix4fv(ProjectionMatrix, 1, GL_FALSE, &Projection[0][0]);

    /****************************************/
    std::vector<unsigned short> indices; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles;
    std::vector<glm::vec3> indexed_vertices;

    setCube(indices,indexed_vertices);


    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);


    //Chargement du fichier de maillage
    // std::string filename("chair.off");
    // loadOFF(filename, indexed_vertices, indices, triangles );

    Particle particles[numParticles];

    for (int i = 0; i < numParticles; ++i) {
        particles[i].position = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // Generate a buffer for the particle position
    GLuint particleBuffer;
    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
    glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");



    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // Calculez le vecteur de direction de la caméra vers la particule
    glm::vec3 camera_direction = glm::normalize(glm::vec3(0.f, 0.f, 0.f) - camera_position);

    // Mettez à jour la position de la caméra pour centrer la particule
    camera_position = glm::vec3(0.f, 0.f, 0.f) - camera_direction * distance_from_particle;

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
                    GL_LINES,      // mode
                    indices.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );

        glDisableVertexAttribArray(0);


do {
        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // for (int i = 0; i < numParticles; ++i) {
        //     particles[i].position += glm::vec3(0.1f, 0.1f, 0.0f) * deltaTime; // Exemple : translation vers la droite
        // }

        const float pi = glm::pi<float>();


        // const float translationRange = 5.0f; // Adjust the range as needed

        // for (int i = 0; i < numParticles; ++i) {
        //     // Set the initial position to the origin
        //     particles[i].position = glm::vec3(0.0f, 0.0f, 0.0f);

        //     // Generate random translations within the predefined range
        //     float translateX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * translationRange;
        //     float translateY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * translationRange;
        //     float translateZ = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * translationRange;

        //     // Update the position based on random translations and delta time
        //     particles[i].position += glm::vec3(translateX, translateY, translateZ) * deltaTime;
        // }

        // for (int i = 0; i < numParticles; ++i) {
        // // Générez des positions initiales aléatoires entre -0.5 et 0.5
        //         float initialX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.0f; // Adjust the scale as needed
        //         float initialY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.0f;
        //         float initialZ = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.0f;

        //         particles[i].position = glm::vec3(initialX, initialY, initialZ);

        //         // Générez un angle aléatoire sur une sphère
        //         float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * pi;
        //         float phi = static_cast<float>(rand()) / RAND_MAX * pi;

        //     // Calculez les coordonnées sphériques en coordonnées cartésiennes
        //         float x = std::sin(phi) * std::cos(theta);
        //         float y = std::sin(phi) * std::sin(theta);
        //         float z = std::cos(phi);

        //         glm::vec3 randomDirection(x, y, z);

        //     // Mise à jour de la position en fonction de la direction et du delta time
        //         particles[i].position += randomDirection * deltaTime;
        //     }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-0.01f, 0.01f);

        for (int i = 0; i < numParticles; ++i) {
            glm::vec3 randPos(dis(gen), 0.0005f, 0.f);
            particles[i].position += randPos; 
        }

        // Mettez à jour le VBO avec les nouvelles positions des particules
        glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles * sizeof(glm::vec3), &particles[0].position);
        // glBufferData(GL_ARRAY_BUFFER, numParticles * sizeof(glm::vec3), particles, GL_STATIC_DRAW);


        /*****************TODO***********************/
        // Model matrix : an identity matrix (model will be at the origin) then change
        Model = glm::mat4(1.f);
        // Model = glm::rotate(Model,angle,glm::vec3(0.f,1.f,0.f));

        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up
         View = glm::lookAt(camera_position, camera_position + camera_target, camera_up);

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms.

        glUniformMatrix4fv(ModelMatrix, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(ViewMatrix, 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(ProjectionMatrix, 1, GL_FALSE, &Projection[0][0]);

        // Model matrix : an identity matrix (model will be at the origin) then change

        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms

        /****************************************/


        // Set the particle position attribute
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
        glVertexAttribPointer(
            0,             // attribute
            3,             // size (vec3)
            GL_FLOAT,      // type
            GL_FALSE,      // normalized?
            0,             // stride
            (void*)0       // array buffer offset
        );

        // Draw the particle
        glDrawArrays(GL_POINTS, 0, numParticles);
        //glDrawArrays(GL_POINTS, 0, 1);

        glDisableVertexAttribArray(0);



        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    // glDeleteBuffers(1, &vertexbuffer);
    // glDeleteBuffers(1, &elementbuffer);
    glDeleteBuffers(1, &particleBuffer);
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

    //Camera zoom in and out
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_position += cameraSpeed * camera_target;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_position -= cameraSpeed * camera_target;
    
    float rotationSpeed = 1.0f;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            angle += rotationSpeed * deltaTime * 100;
            View = glm::rotate(View, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            angle -= rotationSpeed * deltaTime * 100;
            View = glm::rotate(View, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
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