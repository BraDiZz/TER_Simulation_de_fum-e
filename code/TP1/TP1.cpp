#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;

using namespace std;
using namespace glm;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <limits>
#include <map>


#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/stb_image.h>
#include <common/texture.hpp>

// include de imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "eigen/Eigen/Dense"

void processInput(GLFWwindow *window);

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
float time_global = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;
/*******************************************************************************/

std::vector<std::vector<std::vector<glm::vec3>>> gradientField;
std::vector<std::vector<std::vector<float>>> scalarField;

//variable du panneau de contrôle imgui
float side = 1.7; //taille du cube
int resolution=1;
float windDirection = 0; //direction du vent, angle?
float windStrength = 0; //force du vent
float gravity = 0;// force de la gravité
float lifeTime = 350.f; // durée de vie d'une particule en seconde
float nbParticule = 40.f;// nombre de particule
float paticuleSize = 8.f;
float cycle = 0.001; // cycle d'apparition des particules
float velocity = 0.01f;
float oscillationX = 0.f;
float oscillationY = 0.f;
float oscillationZ = 0.f;
bool isChecked = true;
const char *meshAvailable[] = { "Smoke","Sphère", "Chair", "Suzanne"};
int currentMesh = 0;
int acc=0;

struct GridData {
    // Information to store in the grid's cells
    vec3 princ;
    vec3 princ_normal;
    //Mesh acc;
    float nb;
    std::vector<int> indice;

};

struct Grid {
    std::vector<GridData> cells;

    vec3 minPos, maxPos;
    int resolution;

    int getCellX(vec3 pos) { return resolution * (pos[0] - minPos[0]) / (maxPos[0] - minPos[0]); }
    int getCellY(vec3 pos) { return resolution * (pos[1] - minPos[1]) / (maxPos[1] - minPos[1]); }
    int getCellZ(vec3 pos) { return resolution * (pos[2] - minPos[2]) / (maxPos[2] - minPos[2]); }

    int getIndex(int x, int y, int z) { return x * resolution * resolution + y * resolution + z; }
    int getIndex(vec3 pos) { return getCellX(pos) * resolution * resolution + getCellY(pos) * resolution + getCellZ(pos); }
};

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
    return glm::vec3(arrowX/10000,0,arrowY/10000);
}

void setCube(std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices,float side,int create) {
    indices.clear();indexed_vertices.clear();
    indexed_vertices.push_back(glm::vec3(-side, -side, -side));indexed_vertices.push_back(glm::vec3(side, -side, -side));indexed_vertices.push_back(glm::vec3(side, side, -side));indexed_vertices.push_back(glm::vec3(-side, side, -side));indexed_vertices.push_back(glm::vec3(-side, -side, side));indexed_vertices.push_back(glm::vec3(side, -side, side)); 
    indexed_vertices.push_back(glm::vec3(side, side, side));indexed_vertices.push_back(glm::vec3(-side, side, side)); 
    indices.push_back(0);indices.push_back(1);indices.push_back(1);indices.push_back(2);indices.push_back(2);indices.push_back(3);indices.push_back(3);indices.push_back(0);  indices.push_back(4);indices.push_back(5);indices.push_back(5);
    indices.push_back(6);indices.push_back(6);indices.push_back(7);indices.push_back(7);indices.push_back(4);indices.push_back(0);indices.push_back(4);indices.push_back(1);indices.push_back(5);indices.push_back(2);indices.push_back(6);indices.push_back(3);indices.push_back(7);
    if(create==0){
        indices.clear();
        indexed_vertices.clear();
    }
}

void setGrid(std::vector<unsigned short> &indices_grid, std::vector<glm::vec3> &indexed_vertices_grid, float gridSize, int resolution, int create) {
    indices_grid.clear();
    indexed_vertices_grid.clear();    
    float cellSize = gridSize / resolution;
    // Vertices
    for (int z = 0; z <= resolution; ++z) {
        for (int y = 0; y <= resolution; ++y) {
            for (int x = 0; x <= resolution; ++x) {
                indexed_vertices_grid.push_back(glm::vec3(x * cellSize - gridSize / 2.0f, y * cellSize - gridSize / 2.0f, z * cellSize - gridSize / 2.0f));
            }
        }
    }
    // indices_grid for horizontal lines
    for (int z = 0; z < resolution; ++z) {
        for (int y = 0; y < resolution; ++y) {
            for (int x = 0; x < resolution; ++x) {
                unsigned short index = (z * (resolution + 1) + y) * (resolution + 1) + x;
                indices_grid.push_back(index);
                indices_grid.push_back(index + 1);
            }
        }
    }
    // indices_grid for vertical lines
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            for (int y = 0; y < resolution; ++y) {
                unsigned short index = (z * (resolution + 1) + y) * (resolution + 1) + x;
                indices_grid.push_back(index);
                indices_grid.push_back(index + (resolution + 1));
            }
        }
    }
    // indices_grid for depth lines
    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x < resolution; ++x) {
            for (int z = 0; z < resolution; ++z) {
                unsigned short index = (z * (resolution + 1) + y) * (resolution + 1) + x;
                indices_grid.push_back(index);
                indices_grid.push_back(index + (resolution + 1) * (resolution + 1));
            }
        }
    }
    // Indices for closing the grid
    for (int z = 0; z <= resolution; ++z) {
        for (int y = 0; y <= resolution; ++y) {
            indices_grid.push_back((resolution + 1) * (resolution + 1) * z + y * (resolution + 1));
            indices_grid.push_back((resolution + 1) * (resolution + 1) * z + y * (resolution + 1) + resolution);
        }
    }
    for (int z = 0; z <= resolution; ++z) {
        for (int x = 0; x <= resolution; ++x) {
            indices_grid.push_back((resolution + 1) * (resolution + 1) * z + x);
            indices_grid.push_back((resolution + 1) * (resolution + 1) * z + resolution * (resolution + 1) + x);
        }
    }
    for (int y = 0; y <= resolution; ++y) {
        for (int x = 0; x <= resolution; ++x) {
            indices_grid.push_back(y * (resolution + 1) + x);
            indices_grid.push_back(resolution * (resolution + 1) * (resolution + 1) + y * (resolution + 1) + x);
        }
    }
    if (create == 0) {
        indices_grid.clear();
        indexed_vertices_grid.clear();
    }
}

void fillScalarField(std::vector<std::vector<std::vector<float>>>& scalarField, int resolution) {
    // Réinitialisation du générateur de nombres aléatoires
    //printf("remplissage field\n");
    srand(time(NULL));

    scalarField.resize(resolution);
    for (int i = 0; i < resolution; ++i) {
        scalarField[i].resize(resolution);
        for (int j = 0; j < resolution; ++j) {
            scalarField[i][j].resize(resolution);
            for (int k = 0; k < resolution; ++k) {
                // Remplissage avec des valeurs aléatoires entre 0 et 10
                scalarField[i][j][k] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.00003f)); //.0001f
            }
        }
    }
    
}
void calculateGradient(const std::vector<std::vector<std::vector<float>>>& scalarField, float cellSize, std::vector<std::vector<std::vector<glm::vec3>>>& gradientField) {
    int resolution = scalarField.size();
    // Calcul du gradient
    gradientField.resize(resolution);
    for (int i = 1; i < resolution - 1; ++i) {
        //printf("allright i\n");
        gradientField[i].resize(resolution);
        for (int j = 1; j < resolution - 1; ++j) {
            //printf("allright j\n");
            gradientField[i][j].resize(resolution);
            for (int k = 1; k < resolution - 1; ++k) {
                //printf("allright k\n");
                float gradientX = (scalarField[i + 1][j][k] - scalarField[i - 1][j][k]) / (2 * cellSize);
                float gradientY = (scalarField[i][j + 1][k] - scalarField[i][j - 1][k]) / (2 * cellSize);
                float gradientZ = (scalarField[i][j][k + 1] - scalarField[i][j][k - 1]) / (2 * cellSize);

                gradientField[i][j][k] = glm::vec3(gradientX, gradientY, gradientZ);

                // Pour déboguer
                //printf("Gradient en (%d, %d, %d): (%f, %f, %f)\n", i, j, k, gradientX, gradientY, gradientZ);
            }
        }
    }
}
void applyGradientToParticles(const std::vector<std::vector<std::vector<glm::vec3>>>& gradientField, float cellSize, const std::vector<glm::vec3> & position,std::vector<glm::vec3> & deplacement) {
    for(int m=0;m<position.size();m++){
        int i = static_cast<int>((position[m][0] >= 0 ? position[m][0] : -position[m][0]) / cellSize); // Utilisation de la valeur absolue pour obtenir l'indice
        int j = static_cast<int>((position[m][1] >= 0 ? position[m][1] : -position[m][1]) / cellSize);
        int k = static_cast<int>((position[m][2] >= 0 ? position[m][2] : -position[m][2]) / cellSize);
        if(i >= 0 && i < gradientField.size() && j >= 0 && j < gradientField[i].size() && k >= 0 && k < gradientField[i][j].size()) {

            // Obtenir le gradient de la cellule correspondante
            glm::vec3 gradient = gradientField[i][j][k];

            // Appliquer le gradient à la particule (exemple: ajuster la vitesse)
            // Exemple: Ajuster la vitesse de la particule en fonction du gradient
            deplacement[m] = deplacement[m] + gradient;
        }
    }
}

vec3 generate_deplacement(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-2.0f, 2.0f);
    std::uniform_real_distribution<float> dis2(1.0f, 10.0f);
    float y=0.0005f*dis2(gen);
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
    std::vector<glm::vec3> position;
    std::vector<float> life;
    std::vector<glm::vec3> deplacement;
    std::vector<float> baseVelocity;
};

bool start=false;
bool sphere_generate = false;
float transp=100.;

void tri_profondeur(Particle &p){
    map<float,vec3> sorted_pos;
    map<float,float> sorted_life;
    map<float,vec3> sorted_dep;
    map<float,float> sorted_vel;
    int t=p.position.size();
    for(int i=0;i<t;++i){
        float dist=length(camera_position-p.position[i]);
        sorted_pos[dist]=p.position[i];
        sorted_life[dist]=p.life[i];
        sorted_dep[dist]=p.deplacement[i];
        sorted_vel[dist]=p.baseVelocity[i];
    }
    p.position.clear();
    p.life.clear();
    p.deplacement.clear();
    p.baseVelocity.clear();
    for(std::map<float,glm::vec3>::reverse_iterator it = sorted_pos.rbegin(); it != sorted_pos.rend(); ++it){
        p.position.push_back(it->second);
    }
    for(std::map<float,float>::reverse_iterator it = sorted_life.rbegin(); it != sorted_life.rend(); ++it){
        p.life.push_back(it->second);
    }
    for(std::map<float,glm::vec3>::reverse_iterator it = sorted_dep.rbegin(); it != sorted_dep.rend(); ++it){
        p.deplacement.push_back(it->second);
    }
    for(std::map<float,float>::reverse_iterator it = sorted_vel.rbegin(); it != sorted_vel.rend(); ++it){
        p.baseVelocity.push_back(it->second);
    }
}

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


    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); 
    glfwPollEvents();
    glfwSetCursorPos(window, SCR_WIDTH/2, SCR_HEIGHT/2);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);

    //glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders2( "vertex_shader.glsl", "geometry_shader.glsl","fragment_shader.glsl" );
    GLuint programID2 = LoadShaders( "vertexCube.glsl", "fragmentCube.glsl");

    GLuint smoke = loadTexture2DFromFilePath("../textures/smoke.png");



    int M = glGetUniformLocation(programID,"ModelMatrix");
    int V = glGetUniformLocation(programID, "ViewMatrix");
    int P = glGetUniformLocation(programID, "ProjectionMatrix");
    int M2 = glGetUniformLocation(programID2,"ModelMatrix");
    int V2 = glGetUniformLocation(programID2, "ViewMatrix");
    int P2 = glGetUniformLocation(programID2, "ProjectionMatrix");

    Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glUniformMatrix4fv(P, 1, GL_FALSE, &Projection[0][0]);

    std::vector<unsigned short> indices; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles;
    std::vector<glm::vec3> indexed_vertices; // sommets

    std::vector<unsigned short> indices_mesh; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles_mesh;
    std::vector<glm::vec3> indexed_vertices_mesh; // sommets

    std::vector<unsigned short> indices_grid; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles_grid;
    std::vector<glm::vec3> indexed_vertices_grid; // sommets

    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    Grid grid;
    grid.resolution = resolution;
    grid.minPos=vec3(-side,-side,-side);
    grid.maxPos=vec3(side,side,side);
    grid.cells.resize(pow(grid.resolution,3));

    
    GLuint vertexbuffer;
    GLuint elementbuffer;
    GLuint particleBuffer;
    GLuint lifeBuffer;
    
    Particle particles;
    

    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    glUseProgram(programID2);
    GLuint LightID2 = glGetUniformLocation(programID2, "LightPosition_worldspace");
    
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
    do{
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        time_global+=deltaTime;

        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.8f, 0.0f, 0.8f, 0.75f)); // changer la couleur du fond de la fenêtre imgui
        ImGui::Begin("Panneau de contrôle");
        ImGui::SliderFloat("Taille du cube", &side, 0.01f, 4.f);
        ImGui::SliderInt("Resolution de la grille", &resolution,1.f,30.f);
        grid.resolution = resolution;
        static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Initialiser à blanc
        ImGui::ColorEdit4("Color", color);
        ImGui::SliderFloat("Angle du vent", &windDirection, 0.0f, 360.0f);
        ImGui::SliderFloat("Force du vent", &windStrength, 0.0f, 40.0f,"%1.f");
        ImGui::SliderFloat("Force de la gravité", &gravity, 0.0f, 0.0004f,"%.6f");
        vec3 gravityVector = {0,-gravity,0};
        float arrowDirection = fmod(windDirection, 360.0f) * (3.14159265358979323846f / 180.0f); // Convertir en radians
        ImVec4 smokeColor = ImVec4(color[0], color[1], color[2], 1.0f);
        glm::vec3 windVector = DrawWindArrow(windStrength, arrowDirection,smokeColor);
        fillScalarField(scalarField,resolution);
        calculateGradient(scalarField,side/resolution,gradientField);
        applyGradientToParticles(gradientField,side/resolution,particles.position,particles.deplacement);
        ImGui::Dummy(ImVec2(0.0f, 200.0f)); // Ajouter un espace vertical
        ImGui::SliderFloat("Nombre de particule", &nbParticule, 1.0f, 200.0f,"%2.f"); // à déterminer
        ImGui::SliderFloat("Vitesse d'expulsion des particules", &velocity, 0.0f, 0.03f,"%.3f"); // à déterminer
        ImGui::SliderFloat("Vitesse de cycle en ms", &cycle, 1.0f, 1000.0f); // à déterminer
        ImGui::SliderFloat("Durée de vie des particules", &lifeTime, 1.0f, 1000.0f); // à déterminer
        ImGui::SliderFloat("Taille des particules", &paticuleSize, 1.0f, 20.0f); 
        ImGui::SliderFloat("Transparence", &transp, 20.0f, 300.0f);// à déterminer
        if (ImGui::BeginCombo("Mesh disponible", meshAvailable[currentMesh]))
            {
                for (int i = 0; i < IM_ARRAYSIZE(meshAvailable); i++)
                {
                    bool isSelected = (currentMesh== i);
                    if (ImGui::Selectable(meshAvailable[i], isSelected))
                        currentMesh = i;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

        ImGui::Checkbox("Montrer le cube", &isChecked);
        if (isChecked){
            //setCube(indices,indexed_vertices,side,1);
            setGrid(indices_grid,indexed_vertices_grid,side*2,grid.resolution,1);

        }else{
            //setCube(indices,indexed_vertices,side,0);
            setGrid(indices_grid,indexed_vertices_grid,side*2,grid.resolution,0);
        }

        if (ImGui::Button("Lancer la simulation")){
            start=!start;
        }
        if(start){
            ImGui::Text("en cours"); 
        }else{
            ImGui::Text("arretez");
        }
        ImGui::Text("nbParticule: %d", particles.position.size());
        ImGui::PopStyleColor(); // Restaurer la couleur de fond par défaut après la fin de la fenêtre important!!!!
        ImGui::End();

        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices_grid.size() * sizeof(glm::vec3), &indexed_vertices_grid[0], GL_STATIC_DRAW);

        glGenBuffers(1, &elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_grid.size() * sizeof(unsigned short), &indices_grid[0] , GL_STATIC_DRAW);



        if(currentMesh==0){
            std::vector<Particle> acc_stock;
            if(start){
                for(int i=0;i<nbParticule;i++){
                    particles.position.push_back(generate_position());
                    particles.life.push_back(lifeTime);
                    particles.baseVelocity.push_back(velocity);
                    particles.deplacement.push_back(generate_deplacement());
                }
                int taille_prov=particles.position.size();
            for (int i = 0; i < taille_prov; ++i) {
                particles.life[i]=particles.life[i]-1;
                if(particles.life[i]>0){
                    particles.position[i] += particles.deplacement[i]; 
                    particles.position[i] += windVector;
                    particles.position[i] += vec3(0,particles.baseVelocity[i],0);
                    particles.baseVelocity[i] += gravityVector.y;
                }else{
                        
                    particles.position.erase(particles.position.begin()+i);
                    particles.deplacement.erase(particles.deplacement.begin()+i);
                    particles.life.erase(particles.life.begin()+i);
                    particles.baseVelocity.erase(particles.baseVelocity.begin()+i);
                }
            }
            }
            
            
        }
        /*
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
                if(!start){
                    particles.clear();
                    sphere_start=false;
                }
                
            }
        }

        if(currentMesh==2){
            indexed_vertices_mesh.clear();
            indices_mesh.clear();
            loadOFF("./data/chair.off",indexed_vertices_mesh,indices_mesh,triangles_mesh);
            if(start && !sphere_start){
                particles.clear();
                particles.resize(indexed_vertices_mesh.size());
                for(int i=0;i<indexed_vertices_mesh.size();i++){
                    particles[i].position=indexed_vertices_mesh[i];
                }

                for(int i=0;i<indices_mesh.size();i+=3){
                    vec3 x=indexed_vertices_mesh[indices_mesh[i]];
                    vec3 y=indexed_vertices_mesh[indices_mesh[i+1]];
                    vec3 z=indexed_vertices_mesh[indices_mesh[i+2]];gravityVector
                    for(int j=0;j<nbParticule;j++){
                        Particle acc;
                        acc.position=start_triangle(x,y,z);
                        particles.push_back(acc);
                    }
                }

            sphere_generate=true;

            }else{
                if(!star){
                    particles.clear();
                    sphere_generate=false;
                }
                
            }
        }

        if(currentMesh==3){
            indexed_vertices_mesh.clear();
            indices_mesh.clear();
            loadOFF("./data/suzanne.off",indexed_vertices_mesh,indices_mesh,triangles_mesh);
            if(star && !sphere_generate){
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
                if(!star){
                    particles.clear();
                    sphere_generate=false;
                }
                
            }
        }*/

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, smoke);

        GLuint textureLocation = glGetUniformLocation(programID, "particleTexture");
        glUniform1i(textureLocation, 0); 
            


        //permet d'ordonner en fonction de la profondeur     
        tri_profondeur(particles);

        glGenBuffers(2, &particleBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
        glBufferData(GL_ARRAY_BUFFER, particles.position.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.position.size() * sizeof(glm::vec3), &particles.position[0]);


        glGenBuffers(2, &lifeBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, lifeBuffer);
        glBufferData(GL_ARRAY_BUFFER, particles.life.size() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, lifeBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.life.size() * sizeof(float), &particles.life[0]);


        Model = glm::mat4(1.f);
        View = glm::lookAt(camera_position, camera_position + camera_target, camera_up);
        Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(M, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(V, 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(P, 1, GL_FALSE, &Projection[0][0]);

        glUseProgram(programID2);

        glUniform3f(glGetUniformLocation(programID2,"c"),1.,1.,1.);

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


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElements(
                    GL_LINES,      // mode
                    indices_grid.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );

        glDisableVertexAttribArray(0);

        glUseProgram(programID);

        glUniform3f(glGetUniformLocation(programID,"c"),smokeColor.x,smokeColor.y,smokeColor.z);
        glUniform1f(glGetUniformLocation(programID,"transp"),transp);

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
        glPointSize(paticuleSize);


        glUniform1i(glGetUniformLocation(programID,"size_p"),paticuleSize);

    
        GLuint lifeAttributeLocation = glGetAttribLocation(programID, "life");
        glEnableVertexAttribArray(lifeAttributeLocation);
        glBindBuffer(GL_ARRAY_BUFFER, lifeBuffer);
        glVertexAttribPointer(lifeAttributeLocation, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_POINTS, 0, particles.position.size());

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
        glfwSwapBuffers(window);
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );
    if(vertexbuffer) glDeleteBuffers(1, &vertexbuffer);
    if(elementbuffer) glDeleteBuffers(1, &elementbuffer);
    if(particleBuffer) glDeleteBuffers(2, &particleBuffer);
    if(lifeBuffer) glDeleteBuffers(2, &lifeBuffer);
    if(programID) glDeleteProgram(programID);
    if(programID2) glDeleteProgram(programID2);
    if(VertexArrayID) glDeleteVertexArrays(1, &VertexArrayID);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera_position += cameraSpeed * camera_target;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera_position -= cameraSpeed * camera_target;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
