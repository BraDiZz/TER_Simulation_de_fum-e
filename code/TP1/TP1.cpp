// Include standard headers
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <random>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

using namespace std;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <limits>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

// include de imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1300;
const unsigned int SCR_HEIGHT = 800;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f,  6.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

glm::mat4 CamNavigue; // cam 0
glm::mat4 CamOrbital; // cam 1

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;
/*******************************************************************************/

//variable du panneau de contrôle imgui
float side = 1.7; //taille du cube
float windDirection = 0; //direction du vent, angle?
float windStrength = 0; //force du vent
float gravity = 0;// force de la gravité
float lifeTime = 1.f; // durée de vie d'une particule en seconde
float nbParticule = 100.f;// nombre de particule
float cycle = 0.001; // cycle d'apparition des particules
float velocity = 0.01f;
bool isChecked = true;
const char *meshAvailable[] = { "Smoke","Sphère", "Chair", "Suzanne"};
int currentMesh = 0;



int idx(int i,int j,int nX,int nY){ // permet de connaitre l'indice dans un tableau
        return i*nY+j;
}

 glm::vec3 DrawWindArrow(float windStrength, float windDirection, ImVec4 smokeColor)
{
    // Définir la longueur de la flèche
    float arrowLength = 20.0f;
    float sizeA=10.f; // permet de réduire la taille de la flèche
    float sizeS=50.f; // taille du carré
    // Calculer les composantes x et y de la flèche en fonction de la direction du vent
    float arrowX = cosf(windDirection) * windStrength/sizeA * arrowLength;
    float arrowY = sinf(windDirection) * windStrength/sizeA * arrowLength;

    float sizeT = windStrength/2+10;

    // Dessiner la flèche
    ImVec2 startPos = ImGui::GetCursorScreenPos();
    startPos.x += 100; // Décalage de 10 pixels vers la droite
    startPos.y += 100;
    
    ImVec2 endPos(startPos.x + arrowX, startPos.y - arrowY); // Utiliser arrowY avec un signe négatif pour inverser la direction de la flèche
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(startPos.x - sizeS*2, startPos.y - sizeS*2), ImVec2(startPos.x + sizeS * 2, startPos.y + sizeS * 2), IM_COL32(0,0,0,255));
    
    ImGui::GetWindowDrawList()->AddLine(startPos, endPos, IM_COL32(smokeColor.x*255, smokeColor.y*255, smokeColor.z*255, smokeColor.w*255), 2.0f);
    ImGui::GetWindowDrawList()->AddTriangleFilled(ImVec2(endPos.x,endPos.y + sizeT * 0.5f), ImVec2(endPos.x, endPos.y - sizeT * 0.5f), ImVec2(endPos.x + sizeT * 0.5f, endPos.y), IM_COL32(smokeColor.x*255, smokeColor.y*255, smokeColor.z*255, smokeColor.w*255)); // Ajuster correctement les coordonnées pour le triangle
    return glm::vec3(arrowX/20000,0,arrowY/20000);
}

void setCube(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices,float side,int create) {
    // Supprime les données précédentes
    indices.clear();
    indexed_vertices.clear();

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

    if(create==0){
        indices.clear();
        indexed_vertices.clear();
    }

}


vec3 generate_deplacement(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-2.0f, 2.0f);

    std::uniform_real_distribution<float> dis2(1.0f, 10.0f);
    float y=0.001f*dis2(gen);
    float z=0.f;
    float x=dis(gen)*0.001;
    while(x==0){
        x=dis(gen)*0.001;
    }

    return vec3(x,y,z);
}

vec3 generate_position(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-10.0f, 10.0f);

    float y=-2.;
    float x=dis(gen)*0.01;
    float z=dis(gen)*0.01;
    return vec3(x,y,z);
}

//std::numeric_limits<float>::max();
//std::numeric_limits<float>::min();

float min_float(float a, float b){
    if(a<b){
        return a;
    }else{
        return b;
    }

}

float max_float(float a , float b){
    if(a>b){
        return a;
    }else{
        return b;
    }
}

vec3 generate_triangle(vec3 a, vec3 b, vec3 c){
    float Xmax,Xmin,Ymax,Ymin,Zmax,Zmin;
    Xmax=max_float(a[0],max_float(b[0],c[0]));
    Xmin=min_float(a[0],min_float(b[0],c[0]));
    Ymax=max_float(a[1],max_float(b[1],c[1]));
    Ymin=min_float(a[1],min_float(b[1],c[1]));
    Zmax=max_float(a[2],max_float(b[2],c[2]));
    Zmin=min_float(a[2],min_float(b[2],c[2]));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disx(Xmin, Xmax);
    std::uniform_real_distribution<float> disy(Ymin, Ymax);
    std::uniform_real_distribution<float> disz(Zmin, Zmax);

    return vec3(disx(gen),disy(gen),disz(gen));
}

float lifeglobal = lifeTime*200.f;

struct Particle{
    glm::vec3 position;
    float life = lifeglobal;
    vec3 deplacement;
    float baseVelocity = 0.01f;
};


bool generate=false;

bool sphere_generate = false;

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


    //Chargement du fichier de maillage
    //std::string filename("chair.off");
    //loadOFF(filename, indexed_vertices, indices, triangles );

    // Load it into a VBO

    int M = glGetUniformLocation(programID,"ModelMatrix");
    int V = glGetUniformLocation(programID, "ViewMatrix");
    int P = glGetUniformLocation(programID, "ProjectionMatrix");

    Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glUniformMatrix4fv(P, 1, GL_FALSE, &Projection[0][0]);

    std::vector<unsigned short> indices; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles;
    std::vector<glm::vec3> indexed_vertices; // sommets

    std::vector<unsigned short> indices_mesh; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles_mesh;
    std::vector<glm::vec3> indexed_vertices_mesh; // sommets

    

    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    
    GLuint vertexbuffer;
    GLuint elementbuffer;
    GLuint particleBuffer;
    

    std::vector<Particle> particles;

    
    

    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
        
    

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;


    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Setup style
    ImGui::StyleColorsDark();

    do{

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

         processInput(window);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Poll and handle events
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.8f, 0.0f, 0.8f, 0.75f)); // changer la couleur du fond de la fenêtre imgui
        // Create your ImGui window here
        ImGui::Begin("Panneau de contrôle");

        
        
        ImGui::SliderFloat("Taille du cube", &side, 0.01f, 4.f);

        // Sélecteur de couleur RGB
        static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Initialiser à blanc
        ImGui::ColorEdit4("Color", color);

        ImGui::SliderFloat("Angle du vent", &windDirection, 0.0f, 360.0f);
        ImGui::SliderFloat("Force du vent", &windStrength, 0.0f, 40.0f,"%1.f");
        ImGui::SliderFloat("Force de la gravité", &gravity, 0.0f, 0.0002f,"%.6f");
        vec3 gravityVector = {0,-gravity,0};
        float arrowDirection = fmod(windDirection, 360.0f) * (3.14159265358979323846f / 180.0f); // Convertir en radians
        // Generate samples and plot them
        //float samples[100];
        // for (int n = 0; n < 100; n++)
        //     samples[n] = sinf(n * 0.2f + ImGui::GetTime() * windStrength);
        // Dans la boucle principale de votre programme :
        // Calculer la force du vent
        // Dessiner la flèche représentant la direction du vent
        


        //ImGui::PlotLines("Samples", samples, 100);
        // Utiliser la couleur sélectionnée pour dessiner le cube
        ImVec4 smokeColor = ImVec4(color[0], color[1], color[2], 1.0f);
        glm::vec3 windVector = DrawWindArrow(windStrength, arrowDirection,smokeColor);
        ImGui::Dummy(ImVec2(0.0f, 200.0f)); // Ajouter un espace vertical
        ImGui::SliderFloat("Nombre de particule", &nbParticule, 1.0f, 200.0f,"%2.f"); // à déterminer
        ImGui::SliderFloat("Vitesse d'expulsion des particules", &velocity, 0.0f, 0.03f,"%.3f"); // à déterminer
        ImGui::SliderFloat("Vitesse de cycle en ms", &cycle, 1.0f, 1000.0f); // à déterminer
        ImGui::SliderFloat("Durée de vie des particules", &lifeTime, 1.0f, 1000.0f); // à déterminer


        if (ImGui::BeginCombo("Mesh disponible", meshAvailable[currentMesh]))
            {
                for (int i = 0; i < IM_ARRAYSIZE(meshAvailable); i++)
                {
                    bool isSelected = (currentMesh== i);
                    if (ImGui::Selectable(meshAvailable[i], isSelected))
                        currentMesh = i;

                    // Si l'option est sélectionnée, définissez le focus pour que la liste déroulante se ferme immédiatement
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


        ImGui::Checkbox("Montrer le cube", &isChecked);

        if (isChecked)
        {
            setCube(indices,indexed_vertices,side,1);
        }
        else
        {
            setCube(indices,indexed_vertices,side,0);
        }

        

        // Créer un bouton ImGui
        if (ImGui::Button("Lancer la simulation"))
        {
            //printf("Le bouton a été cliqué !\n");
            generate=!generate;
        }
        if(generate){
            ImGui::Text("en cours"); 
        }
        else{
            ImGui::Text("arretez");
        }
        

        ImGui::Text("nbParticule: %d", particles.size());



        

        ImGui::PopStyleColor(); // Restaurer la couleur de fond par défaut après la fin de la fenêtre important!!!!

        ImGui::End();


        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

        // Generate a buffer for the indices as well
        
        glGenBuffers(1, &elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);


        //genere de nouvelle particule tant que generate vaut true
        
        if(currentMesh==0){
            std::vector<Particle> acc_stock;

            if(generate){
                for(int i=0;i<nbParticule;i++){
                    Particle acc;
                    acc.position = generate_position();
                    acc.life = lifeTime;
                    acc.baseVelocity=velocity;
                    acc.deplacement=generate_deplacement();
                    acc_stock.push_back(acc);
                }
            }
            
        
            int taille_prov=particles.size();
            for (int i = 0; i < taille_prov; ++i) {
                particles[i].life=particles[i].life-1;
                if(particles[i].life>0){
                    particles[i].position += particles[i].deplacement; 
                    particles[i].position += windVector;
                    particles[i].position += vec3(0,particles[i].baseVelocity,0);
                    particles[i].baseVelocity += gravityVector.y;
                    acc_stock.push_back(particles[i]);
                }else{
                    
                    // particles[i].position=generate_position();
                    // particles[i].life=lifeglobal;
                    // acc_stock.push_back(particles[i]);
                }
            }

            particles.clear();
            particles.resize(acc_stock.size());
            for(int i=0;i<acc_stock.size();i++){
                particles[i]=acc_stock[i];
            }
        }

        if(currentMesh==1){
            indexed_vertices_mesh.clear();
            indices_mesh.clear();
            loadOFF("./data/sphere.off",indexed_vertices_mesh,indices_mesh,triangles_mesh);
            if(generate && !sphere_generate){
                particles.clear();
                particles.resize(indexed_vertices_mesh.size());
                for(int i=0;i<indexed_vertices_mesh.size();i++){
                    particles[i].position=indexed_vertices_mesh[i];
                }

                for(int i=0;i<indices_mesh.size();i+=3){
                    vec3 x=indexed_vertices_mesh[indices_mesh[i]];
                    vec3 y=indexed_vertices_mesh[indices_mesh[i+1]];
                    vec3 z=indexed_vertices_mesh[indices_mesh[i+2]];
                    for(int j=0;j<nbParticule;j++){
                        Particle acc;
                        acc.position=generate_triangle(x,y,z);
                        particles.push_back(acc);
                    }
                }

            sphere_generate=true;

            }else{
                if(!generate){
                    particles.clear();
                    sphere_generate=false;
                }
                
            }
        }

        if(currentMesh==2){
            indexed_vertices_mesh.clear();
            indices_mesh.clear();
            loadOFF("./data/chair.off",indexed_vertices_mesh,indices_mesh,triangles_mesh);
            if(generate && !sphere_generate){
                particles.clear();
                particles.resize(indexed_vertices_mesh.size());
                for(int i=0;i<indexed_vertices_mesh.size();i++){
                    particles[i].position=indexed_vertices_mesh[i];
                }

                for(int i=0;i<indices_mesh.size();i+=3){
                    vec3 x=indexed_vertices_mesh[indices_mesh[i]];
                    vec3 y=indexed_vertices_mesh[indices_mesh[i+1]];
                    vec3 z=indexed_vertices_mesh[indices_mesh[i+2]];
                    for(int j=0;j<nbParticule;j++){
                        Particle acc;
                        acc.position=generate_triangle(x,y,z);
                        particles.push_back(acc);
                    }
                }

            sphere_generate=true;

            }else{
                if(!generate){
                    particles.clear();
                    sphere_generate=false;
                }
                
            }
        }

        if(currentMesh==3){
            indexed_vertices_mesh.clear();
            indices_mesh.clear();
            loadOFF("./data/suzanne.off",indexed_vertices_mesh,indices_mesh,triangles_mesh);
            if(generate && !sphere_generate){
                particles.clear();
                particles.resize(indexed_vertices_mesh.size());
                for(int i=0;i<indexed_vertices_mesh.size();i++){
                    particles[i].position=indexed_vertices_mesh[i];
                }

                for(int i=0;i<indices_mesh.size();i+=3){
                    vec3 x=indexed_vertices_mesh[indices_mesh[i]];
                    vec3 y=indexed_vertices_mesh[indices_mesh[i+1]];
                    vec3 z=indexed_vertices_mesh[indices_mesh[i+2]];
                    for(int j=0;j<nbParticule;j++){
                        Particle acc;
                        acc.position=generate_triangle(x,y,z);
                        particles.push_back(acc);
                    }
                }

            sphere_generate=true;

            }else{
                if(!generate){
                    particles.clear();
                    sphere_generate=false;
                }
                
            }
        }
        
        
        

        
        
       
         
        
        glGenBuffers(2, &particleBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

       // Mettre à jour les données des particules
        glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
        //glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles * sizeof(glm::vec3), &particles[0].position);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);


         


        Model = glm::mat4(1.f);


         View = glm::lookAt(camera_position, camera_position + camera_target, camera_up);

        Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms.
        
        glUniformMatrix4fv(M, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(V, 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(P, 1, GL_FALSE, &Projection[0][0]);


        

        /****************************************/
        

        glUniform3f(glGetUniformLocation(programID,"c"),1.,1.,1.);

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


        glUniform3f(glGetUniformLocation(programID,"c"),smokeColor.x,smokeColor.y,smokeColor.z);
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
        glDrawArrays(GL_POINTS, 0, particles.size());
        //glDrawArrays(GL_POINTS, 0, 1);

        glDisableVertexAttribArray(0);


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        //glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();



    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteBuffers(2, &particleBuffer);

    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);


    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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
