// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <random>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

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

glm::mat4 View;
glm::mat4 CamNavigue; // cam 0
glm::mat4 CamOrbital; // cam 1

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;
/*******************************************************************************/

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

    // Faces du cube avec des carrés
    // Face arrière
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(1);

    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);

    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(3);

    indices.push_back(3);
    indices.push_back(0);
    indices.push_back(0);

    //face bas

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(1);

    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(5);

    indices.push_back(5);
    indices.push_back(4);
    indices.push_back(4);

    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(0);

    //face gauche

    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(3);

    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(7);

    indices.push_back(7);
    indices.push_back(4);
    indices.push_back(4);

    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(0);

    //face droite

    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);

    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(6);

    indices.push_back(6);
    indices.push_back(5);
    indices.push_back(5);

    indices.push_back(5);
    indices.push_back(1);
    indices.push_back(1);

    //face haut

    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(3);

    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(7);

    indices.push_back(7);
    indices.push_back(6);
    indices.push_back(6);

    indices.push_back(6);
    indices.push_back(2);
    indices.push_back(2);

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
    window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, "TP1 - GLFW", NULL, NULL);
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
    glfwSetCursorPos(window, SCR_WIDTH/2, SCR_HEIGHT/2);

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

    /*********************int idx(int i,int j,int nX,int nY){ // permet de connaitre l'indice dans un tableau
        if(i==-1) i = nX-1;
        if(i==nX) i=0;
        if(j==-1) j = nY-1;
        if(j==nY) j=0;
        return i*nY+j;
    }*******************/
    

    //Chargement du fichier de maillage
    //std::string filename("chair.off");
    //loadOFF(filename, indexed_vertices, indices, triangles );

    // Load it into a VBO

    std::vector<unsigned short> indices; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles;
    std::vector<glm::vec3> indexed_vertices; // sommets

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


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);


        /*****************TODO***********************/
        // Model matrix : an identity matrix (model will be at the origin) then change


        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms
        
        //camera view 
        int TMID = glGetUniformLocation(programID, "ViewMatrix");
        View = glm::lookAt(camera_position,camera_target,camera_up);
        glUniformMatrix4fv(TMID, 1, GL_FALSE, &View[0][0]);

        glm::mat4 ProjectionMatrix = glm::perspective(45.0f,(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,100.0f);
        int Proj = glGetUniformLocation(programID, "ProjectionMatrix");
        
        glUniformMatrix4fv(Proj, 1, GL_FALSE, &ProjectionMatrix[0][0]);

        //std::cout<<"proj = "<<CamNavigue<<std::endl; //<<CamNavigue[1]<<CamNavigue[2]<<CamNavigue[4]<<std::endl;

        // glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);
        // glm::mat4 ViewTranslate = glm::translate(
        //     glm::mat4(1.0f),
        //     glm::vec3(0.0f, 0.0f, -Translate)
        // );
        // glm::mat4 ViewRotateX = glm::rotate(
        //     ViewTranslate,
        //     Rotate.y,
        //     glm::vec3(-1.0f, 0.0f, 0.0f)
        // );
        // glm::mat4 View = glm::rotate(
        //     ViewRotateX,
        //     Rotate.x,
        //     glm::vec3(0.0f, 1.0f, 0.0f)
        // );
        // glm::mat4 Model = glm::scale(
        //     glm::mat4(1.0f),
        //     glm::vec3(0.5f)
        // );
        // glm::mat4 MVP = Projection * View * Model;
        // glUniformMatrix4fv(LocationMVP, 1, GL_FALSE, glm::value_ptr(MVP));
    
        

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
                    GL_TRIANGLES,      // mode
                    indices.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );

        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

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

    //Camera zoom in and out
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera_position += cameraSpeed * camera_target;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera_position -= cameraSpeed * camera_target;


    


    //rotation qui marche
    //     float Translate = 0.0;
    //     float rotateX = 0.1;
    //     float rotateY = 0.1;

    //     glm::mat4 ViewTranslate = glm::translate(
    //         glm::mat4(1.0f),
    //         glm::vec3(0.0f, 0.0f, -Translate)
    //     );
    //     glm::mat4 ViewRotateX = glm::rotate(
    //         ViewTranslate,
    //         rotateY,
    //         glm::vec3(-1.0f, 0.0f, 0.0f)
    //     );
    //     View *= glm::rotate(
    //         ViewRotateX,
    //         rotateX,
    //         glm::vec3(0.0f, 1.0f, 0.0f)
    //     );
    // }


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
