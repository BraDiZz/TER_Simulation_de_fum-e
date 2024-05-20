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
#include <map>


using namespace glm;

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
float time_global = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;
/*******************************************************************************/

std::vector<std::vector<std::vector<glm::vec3>>> gradientField;
std::vector<std::vector<std::vector<float>>> scalarField;
std::vector<std::vector<std::vector<float>>> pressureField;
std::vector<std::vector<std::vector<glm::vec3>>> velocityField;


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


void fillScalarField(std::vector<std::vector<std::vector<float>>>& scalarField, int resolution, float scalarForce) {
    // Réinitialisation du générateur de nombres aléatoires
    srand(time(NULL));

    scalarField.resize(resolution);
    for (int i = 0; i < resolution; ++i) {
        scalarField[i].resize(resolution);
        for (int j = 0; j < resolution; ++j) {
            scalarField[i][j].resize(resolution);
            for (int k = 0; k < resolution; ++k) {
                // Remplissage avec des valeurs aléatoires entre 0 et 10
                scalarField[i][j][k] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / scalarForce) - scalarForce;
            }
        }
    }
    
}
void calculateGradient(const std::vector<std::vector<std::vector<float>>>& scalarField, float cellSize, std::vector<std::vector<std::vector<glm::vec3>>>& gradientField) {
    int resolution = scalarField.size();
    // Calcul du gradient
    gradientField.resize(resolution);
    for (int i = 1; i < resolution - 1; ++i) {
        gradientField[i].resize(resolution);
        for (int j = 1; j < resolution - 1; ++j) {
            gradientField[i][j].resize(resolution);
            for (int k = 1; k < resolution - 1; ++k) {
                float gradientX = (scalarField[i + 1][j][k] - scalarField[i - 1][j][k]) / (2 * cellSize);
                float gradientY = (scalarField[i][j + 1][k] - scalarField[i][j - 1][k]) / (2 * cellSize);
                float gradientZ = (scalarField[i][j][k + 1] - scalarField[i][j][k - 1]) / (2 * cellSize);
                gradientField[i][j][k] = glm::vec3(gradientX, gradientY, gradientZ);
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
            glm::vec3 gradient = gradientField[i][j][k];
            deplacement[m] = deplacement[m] + gradient;
        }
    }
}

void initializeRandomPressureField(std::vector<std::vector<std::vector<float>>>& pressureField, int resolution) {
    pressureField.resize(resolution);
    srand(time(nullptr)); // Réinitialisation du générateur de nombres aléatoires
    for (int i = 0; i < resolution; ++i) {
        pressureField[i].resize(resolution);
        for (int j = 0; j < resolution; ++j) {
            pressureField[i][j].resize(resolution);
            for (int k = 0; k < resolution; ++k) {
                pressureField[i][j][k] = 0.0f;
            }
        }
    }
}

void initializeVelocityField(std::vector<std::vector<std::vector<glm::vec3>>>& velocityField, int resolution, float initialSpeed=4) {
    velocityField.resize(resolution);
    srand(time(NULL));  // Initialisation du générateur de nombres aléatoires

    for (int i = 0; i < resolution; ++i) {
        velocityField[i].resize(resolution);
        for (int j = 0; j < resolution; ++j) {
            velocityField[i][j].resize(resolution);
            for (int k = 0; k < resolution; ++k) {
                // Vitesse aléatoire dans une direction donnée
                float vx = (static_cast<float>(rand()) / RAND_MAX - static_cast<float>(rand()) / RAND_MAX) * initialSpeed;
                float vy = (static_cast<float>(rand()) / RAND_MAX - static_cast<float>(rand()) / RAND_MAX) * initialSpeed;
                float vz = (static_cast<float>(rand()) / RAND_MAX - static_cast<float>(rand()) / RAND_MAX) * initialSpeed;
                //float vz = static_cast<float>(rand()) / RAND_MAX * initialSpeed;

                velocityField[i][j][k] = glm::vec3(vx, vy, vz);
            }
        }
    }   
}


void advectParticles(const std::vector<std::vector<std::vector<glm::vec3>>>& velocityField,std::vector<glm::vec3>& particlePositions,float dt,float cellSize,int resolution) {
    for(int m=0;m<particlePositions.size();m++){
        int i = static_cast<int>((particlePositions[m][0] >= 0 ? particlePositions[m][0] : -particlePositions[m][0]) / cellSize); // Utilisation de la valeur absolue pour obtenir l'indice
        int j = static_cast<int>((particlePositions[m][1] >= 0 ? particlePositions[m][1] : -particlePositions[m][1]) / cellSize);
        int k = static_cast<int>((particlePositions[m][2] >= 0 ? particlePositions[m][2] : -particlePositions[m][2]) / cellSize);
        if(i >= 0 && i < velocityField.size() && j >= 0 && j < velocityField[i].size() && k >= 0 && k < velocityField[i][j].size()) {
            glm::vec3 velocity = velocityField[i][j][k];
            particlePositions[m]+= velocity * dt;  // Déplacement selon la vitesse du fluide
        }
    }
}
void correctVelocityWithPressure(std::vector<std::vector<std::vector<glm::vec3>>>& velocityField,const std::vector<std::vector<std::vector<float>>>& pressureField,float cellSize) {
    int resolution = pressureField.size();

    for (int i = 1; i < resolution - 1; ++i) {
        for (int j = 1; j < resolution - 1; ++j) {
            for (int k = 1; k < resolution - 1; ++k) {
                glm::vec3 pressureGradient = glm::vec3(
                    (pressureField[i+1][j][k] - pressureField[i-1][j][k]) / (10 * cellSize),
                    (pressureField[i][j+1][k] - pressureField[i][j-1][k]) / (10 * cellSize),
                    (pressureField[i][j][k+1] - pressureField[i][j][k-1]) / (10 * cellSize)
                );

                // Ajuster la vitesse en soustrayant le gradient de pression
                velocityField[i][j][k] -= pressureGradient;
            }
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

struct Particle{
    std::vector<glm::vec3> position;
    std::vector<float> life;
    std::vector<glm::vec3> deplacement;
    std::vector<float> baseVelocity;
    std::vector<float> densite;
};

void nettoyage(Particle &p){
    p.position.clear();
    p.life.clear();
    p.deplacement.clear();
    p.baseVelocity.clear();
}


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

    nettoyage(p);

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

void calculerTailleEtPositionCube(const vector<vec3>& points, float& taille, vec3& position) {
    // Initialiser les valeurs min/max pour chaque axe
    vec3 minCoord = points[0];
    vec3 maxCoord = points[0];

    // Trouver les valeurs min/max pour chaque axe
    for (const auto& point : points) {
        minCoord = min(minCoord, point);
        maxCoord = max(maxCoord, point);
    }
    taille = maxCoord.x - minCoord.x;

    position = (minCoord + maxCoord) / 2.0f;
}

vec3 estDansLeCube(const glm::vec3& point, const glm::vec3& cubePosition, float cubeSize, const glm::vec3& deplacement,int choix) {
    float demiTaille = cubeSize / 2.;
    float limiteMinX = cubePosition.x - demiTaille;
    float limiteMaxX = cubePosition.x + demiTaille;
    float limiteMinY = cubePosition.y - demiTaille;
    float limiteMaxY = cubePosition.y + demiTaille;
    float limiteMinZ = cubePosition.z - demiTaille;
    float limiteMaxZ = cubePosition.z + demiTaille;

    glm::vec3 pointApresDeplacement = point + deplacement;

    float offset=0.05;

    if(choix==0){
        bool test1=pointApresDeplacement.x >= limiteMinX && pointApresDeplacement.x <= limiteMaxX && pointApresDeplacement.y >= limiteMinY && pointApresDeplacement.y <= limiteMaxY && pointApresDeplacement.z >= limiteMinZ && pointApresDeplacement.z <= limiteMaxZ;
        bool test2=point.x >= limiteMinX-offset && point.x <= limiteMaxX+offset && point.y >= limiteMinY-offset && point.y <= limiteMaxY+offset && point.z >= limiteMinZ-offset && point.z <= limiteMaxZ+offset;
        if (test1 || test2){
            vec3 acc=vec3(deplacement[0],0.,deplacement[2]);
            vec3 res=estDansLeCube(point,cubePosition,cubeSize,acc,1);
            if(!(res[0]==1 && res[1]==1. && res[2]==1.) ){
                return acc;
            }
            acc=vec3(0.,deplacement[1],deplacement[2]);
            res=estDansLeCube(point,cubePosition,cubeSize,acc,1);
            if(!(res[0]==1 && res[1]==1. && res[2]==1.)){
                return acc;
            }
            acc=vec3(deplacement[0],deplacement[1],0.);
            res=estDansLeCube(point,cubePosition,cubeSize,acc,1);
            if(!(res[0]==1 && res[1]==1. && res[2]==1.)){
                return acc;
            }
        }else{
            return deplacement;
        }
    }
    if(choix==1){
        bool test1=pointApresDeplacement.x >= limiteMinX && pointApresDeplacement.x <= limiteMaxX
        && pointApresDeplacement.y >= limiteMinY && pointApresDeplacement.y <= limiteMaxY
        && pointApresDeplacement.z >= limiteMinZ && pointApresDeplacement.z <= limiteMaxZ;
        if (test1){
            return vec3(1.,1.,1.);
        }else{
            return deplacement;
        }
    }
    return deplacement;   
}


void calcul_normal(std::vector<vec3> &vertices, std::vector<unsigned short> &indices, std::vector<vec3> &normals){
    normals.resize(vertices.size());
    for(int i=0;i<indices.size();i+=3){
        unsigned int index0 = indices[i];
        unsigned int index1 = indices[i + 1];
        unsigned int index2 = indices[i + 2];

        glm::vec3 v0 = vertices[index0];
        glm::vec3 v1 = vertices[index1];
        glm::vec3 v2 = vertices[index2];

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        normals[index0] += normal;
        normals[index1] += normal;
        normals[index2] += normal;
    }

    for (size_t i = 0; i < normals.size(); ++i) {
        normals[i] = glm::normalize(normals[i]);
    }
}
int resolution_plan=16;    
int nx_square=10;
int ny_square=10;
void creation_plan(std::vector<unsigned short> &indices, std::vector<std::vector<unsigned short> > &triangles, std::vector<glm::vec3> &indexed_vertices, std::vector<glm::vec2> &indexed_texcoords) {
    indices.clear();
    indexed_vertices.clear();
    indexed_texcoords.clear();
    for (int i = 0; i <= resolution_plan; i++) {
        for (int j = 0; j <= resolution_plan; j++) {
            float x = static_cast<float>(i) / resolution_plan - 0.5f;
            float y = static_cast<float>(j) / resolution_plan - 0.5f;
            glm::vec3 val = glm::vec3((float)x * nx_square,0, (float)y * ny_square);
            indexed_vertices.push_back(val);
            glm::vec2 texCoord = glm::vec2(static_cast<float>(i) / resolution_plan, static_cast<float>(j) / resolution_plan);
            indexed_texcoords.push_back(texCoord);
        }
    }
    for (int i = 0; i <= resolution_plan; i++) {
        for (int j = 0; j <= resolution_plan; j++) {
            float x = static_cast<float>(i) / resolution_plan - 0.5f;
            float y = static_cast<float>(j) / resolution_plan - 0.5f;
            glm::vec3 val = glm::vec3((float)x * nx_square, (float)y * ny_square+(ny_square/2.),-3.);
            indexed_vertices.push_back(val);
            glm::vec2 texCoord = glm::vec2(static_cast<float>(i) / resolution_plan, static_cast<float>(j) / resolution_plan);
            indexed_texcoords.push_back(texCoord);
        }
    }
    for (int i = 0; i <= resolution_plan; i++) {
        for (int j = 0; j <= resolution_plan; j++) {
            float x = static_cast<float>(i) / resolution_plan - 0.5f;
            float y = static_cast<float>(j) / resolution_plan - 0.5f;
            glm::vec3 val = glm::vec3(-5.,(float)x * nx_square+(nx_square/2.), (float)y * ny_square+(ny_square/2.)-(ny_square/2.));
            indexed_vertices.push_back(val);
            glm::vec2 texCoord = glm::vec2(static_cast<float>(i) / resolution_plan, static_cast<float>(j) / resolution_plan);
            indexed_texcoords.push_back(texCoord);
        }
    }
    for (int i = 0; i <= resolution_plan; i++) {
        for (int j = 0; j <= resolution_plan; j++) {
            float x = static_cast<float>(i) / resolution_plan - 0.5f;
            float y = static_cast<float>(j) / resolution_plan - 0.5f;
            glm::vec3 val = glm::vec3(5.,(float)x * nx_square+(nx_square/2.), (float)y * ny_square+(ny_square/2.)-(ny_square/2.));
            indexed_vertices.push_back(val);
            glm::vec2 texCoord = glm::vec2(static_cast<float>(i) / resolution_plan, static_cast<float>(j) / resolution_plan);
            indexed_texcoords.push_back(texCoord);
        }
    }




    for (int i = 0; i < resolution_plan; i++) {
        for (int j = 0; j < resolution_plan; j++) {
            // floor
            indices.push_back(i * (resolution_plan + 1) + j);
            indices.push_back((i + 1) * (resolution_plan + 1) + j);
            indices.push_back(i * (resolution_plan + 1) + j + 1);
            indices.push_back(i * (resolution_plan + 1) + j + 1);
            indices.push_back((i + 1) * (resolution_plan + 1) + j);
            indices.push_back((i + 1) * (resolution_plan + 1) + j + 1);
            //back wall
            indices.push_back(((resolution_plan + 1) * (resolution_plan + 1)) + (i * (resolution_plan + 1)) + j);
            indices.push_back(((resolution_plan + 1) * (resolution_plan + 1)) + ((i + 1) * (resolution_plan + 1)) + j + 1);
            indices.push_back(((resolution_plan + 1) * (resolution_plan + 1)) + (i * (resolution_plan + 1)) + j + 1);
            indices.push_back(((resolution_plan + 1) * (resolution_plan + 1)) + (i * (resolution_plan + 1)) + j);
            indices.push_back(((resolution_plan + 1) * (resolution_plan + 1)) + ((i + 1) * (resolution_plan + 1)) + j);
            indices.push_back(((resolution_plan + 1) * (resolution_plan + 1)) + ((i + 1) * (resolution_plan + 1)) + j + 1);
            // left wall
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 2 + (i * (resolution_plan + 1) + j));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 2 + ((i + 1) * (resolution_plan + 1) + j));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 2 + (i * (resolution_plan + 1) + j + 1));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 2 + (i * (resolution_plan + 1) + j + 1));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 2 + ((i + 1) * (resolution_plan + 1) + j));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 2 + ((i + 1) * (resolution_plan + 1) + j + 1));

            // right wall
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 3 + (i * (resolution_plan + 1) + j));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 3 + ((i + 1) * (resolution_plan + 1) + j));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 3 + (i * (resolution_plan + 1) + j + 1));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 3 + (i * (resolution_plan + 1) + j + 1));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 3 + ((i + 1) * (resolution_plan + 1) + j));
            indices.push_back((resolution_plan + 1) * (resolution_plan + 1) * 3 + ((i + 1) * (resolution_plan + 1) + j + 1));

        }
    }
}

//variable du panneau de contrôle imgui
float side = 1.7; //taille du cube
int resolution=1;
float windDirection = 0; //direction du vent, angle?
float windStrength = 0; //force du vent
float gravity = 0;// force de la gravité
float lifeTime = 350.f; // durée de vie d'une particule en seconde
float nbParticule = 40.f;// nombre de particule
float paticuleSize = 18.f;
float cycle = 0.001; // cycle d'apparition des particules
float velocity = 0.01f;
float oscillationX = 0.f;
float oscillationY = 0.f;
float oscillationZ = 0.f;
bool isChecked = true;
const char *meshAvailable[] = { "Smoke","Sphère", "Chair", "Suzanne"};
int currentMesh = 0;
int acc=0;
bool collision=false;
bool gros_mesh=false;
float melange=0.5f;
float frequence=0.1f;
float amplitude=0.1f;

float scalarForce = 0.00003f;

glm::mat4 View;
glm::mat4 Model;
glm::mat4 Projection;
mat4 Model2 = glm::mat4(1.f);


bool start=false;
bool sphere_generate = false;
float transp=100.;
float lifeglobal = lifeTime*200.f;

bool show_menu_grille=false;
bool show_menu_force=false;
bool show_menu_para=false;

//pour stocker les infos du cube
float taille;
vec3 position_cube;
vec3 light_color=vec3(1.,1.,1.);


//light 
float intensity=0.142f;
vec3 light_pos=vec3(0.,3.8,1.);

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

/*
    glEnable(GL_LIGHTING);
    GLfloat lightDirection[] = {1.0f, 1.0f, 1.0f, 0.0f}; // Direction de la lumière (dans l'espace du monde)
    glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);*/

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
    GLuint smoke2 = loadTexture2DFromFilePath("../textures/smoke3.png");
    GLuint plan_text = loadTexture2DFromFilePath("../textures/pavement.jpg");
    //GLuint plan_text = loadTexture2DFromFilePath("../textures/ciel.png");


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

    std::vector<unsigned short> indices_grid; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles_grid;
    std::vector<glm::vec3> indexed_vertices_grid; // sommets
    std::vector<glm::vec3> normal_grid;


    std::vector<unsigned short> indices_gen; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles_gen;
    std::vector<glm::vec3> indexed_vertices_gen;
    std::vector<glm::vec3> normal_gen;

    std::vector<unsigned short> indices_piece; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles_piece;
    std::vector<glm::vec3> indexed_vertices_piece;
    std::vector<glm::vec3> normal_piece;
    std::vector<glm::vec2> textcoord_piece;

    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    Grid grid;
    grid.resolution = resolution;
    grid.minPos=vec3(-side,-side,-side);
    grid.maxPos=vec3(side,side,side);
    grid.cells.resize(pow(grid.resolution,3));

    //buffer cube
    GLuint vertexbuffer;
    GLuint elementbuffer;
    GLuint normalbuffer;
    //buffer piece
    GLuint vertexbuffer_piece;
    GLuint elementbuffer_piece;
    GLuint normalbuffer_piece;  
    GLuint textcoordbuffer_piece;
    //buffer turbine  
    GLuint vertexbuffer_gen;
    GLuint elementbuffer_gen;
    GLuint normalbuffer_gen;
    //buffer particule
    GLuint particleBuffer;
    GLuint lifeBuffer;
    
    Particle particles;

    
    //pré calucle suzanne    
    std::vector<glm::vec3> suzanne;
    std::vector<glm::vec3> suzanne2;

    indexed_vertices_mesh.clear();
    indices_mesh.clear();
    loadOFF("./data/suzanne.off",indexed_vertices_mesh,indices_mesh,triangles_mesh);
    
    for(int i=0;i<indices_mesh.size();i+=3){
        vec3 x=indexed_vertices_mesh[indices_mesh[i]];
        vec3 y=indexed_vertices_mesh[indices_mesh[i+1]];
        vec3 z=indexed_vertices_mesh[indices_mesh[i+2]];
        for(int j=0;j<10;j++){
            vec3 acc=generate_triangle(x,y,z);
            suzanne.push_back(acc);
        }
    }
    for(int i=0;i<indexed_vertices_mesh.size();++i){
        vec3 acc=indexed_vertices_mesh[i];
        suzanne.push_back(acc);
    }

    for(int i=0;i<indexed_vertices_mesh.size();++i){
        vec3 acc=indexed_vertices_mesh[i];
        suzanne2.push_back(acc);
    }

    //pre calcul singe
    std::vector<glm::vec3> monkey;
    std::vector<glm::vec3> monkey2;

    indexed_vertices_mesh.clear();
    indices_mesh.clear();
    loadOFF("./data/sphere.off",indexed_vertices_mesh,indices_mesh,triangles_mesh);
    
    for(int i=0;i<indices_mesh.size();i+=3){
        vec3 x=indexed_vertices_mesh[indices_mesh[i]];
        vec3 y=indexed_vertices_mesh[indices_mesh[i+1]];
        vec3 z=indexed_vertices_mesh[indices_mesh[i+2]];
        for(int j=0;j<10;j++){
            vec3 acc=generate_triangle(x,y,z);
            monkey.push_back(acc);
        }
    }
    for(int i=0;i<indexed_vertices_mesh.size();++i){
        vec3 acc=indexed_vertices_mesh[i];
        monkey.push_back(acc);
    }

    for(int i=0;i<indexed_vertices_mesh.size();++i){
        vec3 acc=indexed_vertices_mesh[i];
        monkey2.push_back(acc);
    }

    //-------

    //calcul taille du cube 
    loadOFF("./data/cube.off",indexed_vertices_grid,indices_grid,triangles_mesh);
    calculerTailleEtPositionCube(indexed_vertices_grid,taille,position_cube);
    calcul_normal(indexed_vertices_grid,indices_grid,normal_grid);
    /*
    for(int i=0;i<normal_grid.size();++i){
        cout<<normal_grid[i][0]<<endl;
    }*/
    indexed_vertices_grid.clear();
    indices_grid.clear();
    //----------

    loadOFF("./data/turbine.off",indexed_vertices_gen,indices_gen,triangles_gen);
    calcul_normal(indexed_vertices_gen,indices_gen,normal_gen);



     creation_plan(indices_piece,triangles_piece,indexed_vertices_piece,textcoord_piece);
     calcul_normal(indexed_vertices_piece,indices_piece,normal_piece);
     for(int i=0;i<normal_piece.size()/4.;++i){
         normal_piece[i]=vec3(-normal_piece[i][0],-normal_piece[i][1],-normal_piece[i][2]);
     }
     for(int i=(normal_piece.size()/4.)*3.;i<normal_piece.size();++i){
         normal_piece[i]=vec3(-normal_piece[i][0],-normal_piece[i][1],-normal_piece[i][2]);
    }




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


        ImGui::Begin("panneau de controle");
        if (ImGui::CollapsingHeader("Grille", show_menu_grille))
        {
            ImGui::SliderFloat("Taille de", &side, 0.01f, 4.f);
            ImGui::SliderInt("Resolution de la grille", &resolution,1.f,30.f);
            ImGui::SliderFloat("Force du vecteur scalaire", &scalarForce, 0.001f, 0.01f); // à déterminer
            grid.resolution = resolution;
        }
        if (ImGui::CollapsingHeader("Environnement", show_menu_grille))
        {
            ImGui::SliderFloat("Intensité de la lumière", &intensity, 0.01f, 0.5f);
            ImGui::SliderFloat("Positon lumière sur x", &light_pos[0], -5.f, 5.f);
            ImGui::SliderFloat("Positon lumière sur y", &light_pos[1], -5.f, 10.f);
            ImGui::SliderFloat("Positon lumière sur z", &light_pos[2], -5.f, 5.f);
            static float color_light[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit4("Color", color_light);
            light_color=vec3(color_light[0],color_light[1],color_light[2]);


        }
        static float oldResolution = resolution;
        if (oldResolution != resolution) {

            oldResolution= resolution;
        }
        grid.resolution = resolution;
        static float oldScalarForce = scalarForce;
        if (oldScalarForce != scalarForce) {
            // seulement lorsque la resolution est modifié.
            fillScalarField(scalarField,resolution,scalarForce);
            calculateGradient(scalarField,side/resolution,gradientField);
            oldScalarForce = scalarForce;
        }
        static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        vec3 gravityVector = {0,-gravity,0};
        float arrowDirection = fmod(windDirection, 360.0f) * (3.14159265358979323846f / 180.0f); // Convertir en radians
        ImVec4 smokeColor = ImVec4(color[0], color[1], color[2], 1.0f);
        glm::vec3 windVector; 
        initializeVelocityField(velocityField,resolution, 1);
        initializeRandomPressureField(pressureField, resolution);
        correctVelocityWithPressure(velocityField,pressureField,side/resolution);
        advectParticles(velocityField,particles.position,scalarForce,side/resolution,resolution);
        fillScalarField(scalarField,resolution,scalarForce/300);
        calculateGradient(scalarField,side/resolution,gradientField);
        applyGradientToParticles(gradientField,side/resolution,particles.position,particles.deplacement);
        if (ImGui::CollapsingHeader("Force Externe", show_menu_force))
        {
             // Initialiser à blanc
            ImGui::ColorEdit4("Color", color);
            ImGui::SliderFloat("Angle du vent", &windDirection, 0.0f, 360.0f);
            ImGui::SliderFloat("Force du vent", &windStrength, 0.0f, 40.0f,"%1.f");
            ImGui::SliderFloat("Force de la gravité", &gravity, 0.0f, 0.0004f,"%.6f");   
            windVector=DrawWindArrow(windStrength, arrowDirection,smokeColor);         
            ImGui::Dummy(ImVec2(0.0f, 200.0f));
            ImGui::Checkbox("Collision avec le cube", &collision);
            if(collision){
                loadOFF("./data/cube.off",indexed_vertices_grid,indices_grid,triangles_mesh);
            }else{
                indexed_vertices_grid.clear();
                indices_grid.clear();
            }
            ImGui::SliderFloat("Hauteur du cube", &position_cube[1], -10.0f, 5.f);  
            ImGui::SliderFloat("Positon du cube", &position_cube[0], -5.0f, 5.f);
            ImGui::SliderFloat("Profondeur du cube", &position_cube[2], -5.0f, 5.f);

        }


        if (ImGui::CollapsingHeader("Paramètre de la fumée", show_menu_para))
        {
            ImGui::SliderFloat("Nombre de particule", &nbParticule, 1.0f, 200.0f,"%2.f"); // à déterminer
            ImGui::SliderFloat("Vitesse d'expulsion des particules", &velocity, 0.0f, 0.03f,"%.3f"); // à déterminer
            ImGui::SliderFloat("Durée de vie des particules", &lifeTime, 1.0f, 1000.0f); // à déterminer
            ImGui::SliderFloat("Taille des particules", &paticuleSize, 1.0f, 40.0f); 
            ImGui::SliderFloat("Transparence", &transp, 20.0f, 300.0f);// à déterminer
            ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 2.0f);
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


            ImGui::Checkbox("Gros mesh", &gros_mesh); 
            if (ImGui::Button("Lancer la simulation")){
                start=!start;
            }
            if(start){
                ImGui::Text("en cours"); 
            }else{
                ImGui::Text("arretez");
            }
            ImGui::Text("nbParticule: %d", particles.position.size());
        }
        ImGui::PopStyleColor(); // Restaurer la couleur de fond par défaut après la fin de la fenêtre important!!!!
        ImGui::End();


        if(currentMesh==0){
            std::vector<Particle> acc_stock;
            if(start){
                for(int i=0;i<nbParticule;i++){
                    particles.position.push_back(generate_position());
                    particles.life.push_back(lifeTime);
                    particles.baseVelocity.push_back(velocity);
                    particles.deplacement.push_back(generate_deplacement());
                }            
            }
            int taille_prov=particles.position.size();
            if(taille_prov>0){
                for (int i = 0; i < taille_prov; ++i) {
                    particles.life[i]--;
                    if(particles.life[i]>0){
                        if(collision){ 
                                vec3 pos_acc=vec3(position_cube[0]*0.3,position_cube[1]*0.3,position_cube[2]*0.3);
                                vec3 acc=estDansLeCube(particles.position[i],pos_acc,taille*0.3, particles.deplacement[i],0);
                                
                                
                                if(acc[0]==particles.deplacement[i][0]  && acc[1]==particles.deplacement[i][1] && acc[2]==particles.deplacement[i][2]){
                                    particles.position[i] += acc;
                                    particles.position[i] += vec3(0,particles.baseVelocity[i],0);
                                    particles.baseVelocity[i] += gravityVector.y; 
                                }else{
                                    particles.position[i] += vec3(10.,10.,10.)*acc;
                                }

                                particles.position[i] += windVector;
                                
                        }else{    

                                particles.position[i] += particles.deplacement[i]; 
                                particles.position[i] += windVector;
                                particles.position[i] += vec3(0,particles.baseVelocity[i],0);
                                particles.baseVelocity[i] += gravityVector.y;                                
                             
                        }
                        
                        
                        
                    }else{
                            
                        particles.position.erase(particles.position.begin()+i);
                        particles.deplacement.erase(particles.deplacement.begin()+i);
                        particles.life.erase(particles.life.begin()+i);
                        particles.baseVelocity.erase(particles.baseVelocity.begin()+i);
                    }
                }

            }
            
            
            
        }
        if(currentMesh==1){
            if(gros_mesh){
                paticuleSize=20.;
                int t=monkey.size();
                if(!sphere_generate){
                    vec3 dep=generate_deplacement();
                    dep=vec3(dep.x/2.,dep.y/2.,dep.z/2.);
                    nettoyage(particles);
                    for(int i=0;i<t;++i){
                        particles.position.push_back(monkey[i]);
                        particles.life.push_back(lifeTime);
                        particles.baseVelocity.push_back(velocity/10.);
                        particles.deplacement.push_back(dep);
                        float d=rand()%10;
                        d/=10;
                        particles.densite.push_back(d);
                    }
                    sphere_generate=true;
                }



                if(start){
                    int taille_prov=particles.position.size();
                    if(taille_prov>0){
                        for (int i = 0; i < taille_prov; ++i) {
                            //particles.life[i]=particles.life[i]-1;
                            if(particles.life[i]>0){
                                particles.position[i] += particles.deplacement[i]; 
                                particles.position[i] += windVector;
                                particles.position[i] += vec3(0,particles.baseVelocity[i],0);
                                particles.baseVelocity[i] += gravityVector.y;
                                float acc = 1. * (1.225-particles.densite[i]) * 9.81 * deltaTime / 100.;
                                particles.baseVelocity[i] = -acc;
                                
                            }else{
                                
                                particles.position.erase(particles.position.begin()+i);
                                particles.deplacement.erase(particles.deplacement.begin()+i);
                                particles.life.erase(particles.life.begin()+i);
                                particles.baseVelocity.erase(particles.baseVelocity.begin()+i);
                                particles.densite.erase(particles.densite.begin()+i);
                            }
                        }
                    }
                }else{
                    nettoyage(particles);
                    sphere_generate=false;
                }
            }else{
                paticuleSize=40.;
                int t=monkey2.size();
                if(!sphere_generate){
                    vec3 dep=generate_deplacement();
                    dep=vec3(dep.x/2.,dep.y/2.,dep.z/2.);
                    nettoyage(particles);
                    for(int i=0;i<t;++i){
                        particles.position.push_back(monkey2[i]);
                        particles.life.push_back(lifeTime);
                        particles.baseVelocity.push_back(velocity/10.);
                        particles.deplacement.push_back(dep);
                        float d=rand()%10;
                        d/=10;
                        particles.densite.push_back(d);
                    }
                    sphere_generate=true;
                }



                if(start){
                    int taille_prov=particles.position.size();
                    if(taille_prov>0){
                        for (int i = 0; i < taille_prov; ++i) {
                            //particles.life[i]=particles.life[i]-1;
                            if(particles.life[i]>0){
                                particles.position[i] += particles.deplacement[i]; 
                                particles.position[i] += windVector;
                                particles.position[i] += vec3(0,particles.baseVelocity[i],0);
                                particles.baseVelocity[i] += gravityVector.y;
                                float acc = 1. * (1.225-particles.densite[i]) * 9.81 * deltaTime / 100.;
                                particles.baseVelocity[i] = -acc;
                                
                            }else{
                                
                                particles.position.erase(particles.position.begin()+i);
                                particles.deplacement.erase(particles.deplacement.begin()+i);
                                particles.life.erase(particles.life.begin()+i);
                                particles.baseVelocity.erase(particles.baseVelocity.begin()+i);
                                particles.densite.erase(particles.densite.begin()+i);
                            }
                        }
                    }
                }else{
                    nettoyage(particles);
                    sphere_generate=false;
                }

            }
        }
        if(currentMesh==2){
            paticuleSize=60.;
            particles.position.push_back(vec3(0.,0.,0));
            particles.life.push_back(lifeTime);
            particles.baseVelocity.push_back(velocity/10.);
            particles.deplacement.push_back(generate_deplacement());
}
        if(currentMesh==3){
            if(gros_mesh){
                paticuleSize=20.;
                int t=suzanne.size();
                if(!sphere_generate){
                    vec3 dep=generate_deplacement();
                    dep=vec3(dep.x/2.,dep.y/2.,dep.z/2.);
                    nettoyage(particles);
                    for(int i=0;i<t;++i){
                        particles.position.push_back(suzanne[i]);
                        particles.life.push_back(lifeTime);
                        particles.baseVelocity.push_back(velocity/10.);
                        particles.deplacement.push_back(dep);
                        float d=rand()%10;
                        d/=10;
                        particles.densite.push_back(d);
                    }
                    sphere_generate=true;
                }



                if(start){
                    int taille_prov=particles.position.size();
                    if(taille_prov>0){
                        for (int i = 0; i < taille_prov; ++i) {
                            //particles.life[i]=particles.life[i]-1;
                            if(particles.life[i]>0){
                                particles.position[i] += particles.deplacement[i]; 
                                particles.position[i] += windVector;
                                particles.position[i] += vec3(0,particles.baseVelocity[i],0);
                                particles.baseVelocity[i] += gravityVector.y;
                                float acc = 1. * (1.225-particles.densite[i]) * 9.81 * deltaTime / 100.;
                                particles.baseVelocity[i] = -acc;
                            
                            }else{
                                    
                                particles.position.erase(particles.position.begin()+i);
                                particles.deplacement.erase(particles.deplacement.begin()+i);
                                particles.life.erase(particles.life.begin()+i);
                                particles.baseVelocity.erase(particles.baseVelocity.begin()+i);
                                particles.densite.erase(particles.densite.begin()+i);
                            }
                        }
                    }
                }else{
                    nettoyage(particles);
                    sphere_generate=false;
                }
            }else{
                paticuleSize=40.;
                int t=suzanne2.size();
                if(!sphere_generate){
                    vec3 dep=generate_deplacement();
                    dep=vec3(dep.x/2.,dep.y/2.,dep.z/2.);
                    nettoyage(particles);
                    for(int i=0;i<t;++i){
                        particles.position.push_back(suzanne2[i]);
                        particles.life.push_back(lifeTime);
                        particles.baseVelocity.push_back(velocity/10.);
                        particles.deplacement.push_back(dep);
                        float d=rand()%10;
                        d/=10;
                        particles.densite.push_back(d);
                    }
                    sphere_generate=true;
                }



                if(start){
                    int taille_prov=particles.position.size();
                    if(taille_prov>0){
                        for (int i = 0; i < taille_prov; ++i) {
                            //particles.life[i]=particles.life[i]-1;
                            if(particles.life[i]>0){
                                particles.position[i] += particles.deplacement[i]; 
                                particles.position[i] += windVector;
                                particles.position[i] += vec3(0,particles.baseVelocity[i],0);
                                particles.baseVelocity[i] += gravityVector.y;
                                float acc = 1. * (1.225-particles.densite[i]) * 9.81 * deltaTime / 100.;
                                particles.baseVelocity[i] = -acc;
                            
                            }else{
                                    
                                particles.position.erase(particles.position.begin()+i);
                                particles.deplacement.erase(particles.deplacement.begin()+i);
                                particles.life.erase(particles.life.begin()+i);
                                particles.baseVelocity.erase(particles.baseVelocity.begin()+i);
                                particles.densite.erase(particles.densite.begin()+i);
                            }
                        }
                    }
                }else{
                    nettoyage(particles);
                    sphere_generate=false;
                }

            }


            
        }    
       

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, smoke);

        GLuint textureLocation = glGetUniformLocation(programID, "particleTexture");
        glUniform1i(textureLocation, 0); 

        glActiveTexture(GL_TEXTURE0+1);
        glBindTexture(GL_TEXTURE_2D, smoke2);

        GLuint textureLocation2 = glGetUniformLocation(programID, "particleTexture2");
        glUniform1i(textureLocation2, 1); 

        glUseProgram(programID2);
        glActiveTexture(GL_TEXTURE0+2);
        glBindTexture(GL_TEXTURE_2D, plan_text);

        GLuint textureLocation3 = glGetUniformLocation(programID2, "texture_plan");
        glUniform1i(textureLocation3, 2); 
            


        //permet d'ordonner en fonction de la profondeur     
        if(particles.position.size()>0) {
            tri_profondeur(particles);
        }


        //bind de la turbine
        glGenBuffers(1, &vertexbuffer_gen);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_gen);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices_gen.size() * sizeof(glm::vec3), &indexed_vertices_gen[0], GL_DYNAMIC_DRAW);

        glGenBuffers(1, &normalbuffer_gen);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_gen);
        glBufferData(GL_ARRAY_BUFFER, normal_gen.size() * sizeof(glm::vec3), &normal_gen[0], GL_DYNAMIC_DRAW);

        glGenBuffers(1, &elementbuffer_gen);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer_gen);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_gen.size() * sizeof(unsigned short), &indices_gen[0], GL_DYNAMIC_DRAW);

        //bind piece
        glGenBuffers(1, &vertexbuffer_piece);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_piece);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices_piece.size() * sizeof(glm::vec3), &indexed_vertices_piece[0], GL_DYNAMIC_DRAW);

        glGenBuffers(1, &normalbuffer_piece);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_piece);
        glBufferData(GL_ARRAY_BUFFER, normal_piece.size() * sizeof(glm::vec3), &normal_piece[0], GL_DYNAMIC_DRAW);

        glGenBuffers(1, &textcoordbuffer_piece);
        glBindBuffer(GL_ARRAY_BUFFER, textcoordbuffer_piece);
        glBufferData(GL_ARRAY_BUFFER, textcoord_piece.size() * sizeof(glm::vec3), &textcoord_piece[0], GL_DYNAMIC_DRAW);

        glGenBuffers(1, &elementbuffer_piece);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer_piece);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_piece.size() * sizeof(unsigned short), &indices_piece[0], GL_DYNAMIC_DRAW);

        //bind du cube
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices_grid.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, indexed_vertices_grid.size() * sizeof(glm::vec3), &indexed_vertices_grid[0]);

        glGenBuffers(1, &normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, normal_grid.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_grid.size() * sizeof(glm::vec3), &normal_grid[0]);

        glGenBuffers(1, &elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_grid.size() * sizeof(unsigned short), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices_grid.size() * sizeof(unsigned short), &indices_grid[0]);

        //bind des particules
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


        glUseProgram(programID2);

        Model = glm::mat4(1.f);
        Model2 = glm::scale(Model,vec3(0.3));
        Model2 = glm::translate(Model2,position_cube);
        View = glm::lookAt(camera_position, camera_position + camera_target, camera_up);
        Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        mat4 Model3 = glm::mat4(1.f);
        Model3 = glm::translate(Model3,vec3(0.,-2.,0.));
        Model3 = glm::rotate(Model3,radians(-90.f),vec3(0.,0.,1.));
        Model3 = glm::scale(Model3,vec3(1.));

        mat4 Model4 = glm::mat4(1.f);
        Model4 = glm::translate(Model4,vec3(0.,-2.2,-2.));
        Model4 = glm::scale(Model4,vec3(1.));


        glUniformMatrix4fv(M, 1, GL_FALSE, &Model2[0][0]);
        glUniformMatrix4fv(V, 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(P, 1, GL_FALSE, &Projection[0][0]);

        
        glUniform3f(glGetUniformLocation(programID2,"c"),smokeColor.x,smokeColor.y,smokeColor.z);
        glUniform1f(glGetUniformLocation(programID2,"intensity"),intensity);
        glUniform3f(glGetUniformLocation(programID2,"lightPos"),light_pos.x,light_pos.y,light_pos.z);
        glUniform3f(glGetUniformLocation(programID2,"lightColor"),light_color.x,light_color.y,light_color.z);
        glUniform1f(glGetUniformLocation(programID2,"deltaTime"),time_global);

        // envoie du cube
        glEnableVertexAttribArray(0);
        glUniform1i(glGetUniformLocation(programID2,"plan"),0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
                    1,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    indices_grid.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );
        //envoie de la piece
        glUniform1i(glGetUniformLocation(programID2,"plan"),1);
        glUniformMatrix4fv(M, 1, GL_FALSE, &Model4[0][0]);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_piece);
        glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_piece);
        glVertexAttribPointer(
                    1,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, textcoordbuffer_piece);
        glVertexAttribPointer(
                    2,                  // attribute
                    2,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer_piece);
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    indices_piece.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );


        //envoie de turbine
        glUniform1i(glGetUniformLocation(programID2,"plan"),0);
        glEnableVertexAttribArray(0);
        glUniformMatrix4fv(M, 1, GL_FALSE, &Model3[0][0]);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_gen);
        glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_gen);
        glVertexAttribPointer(
                    1,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer_gen);
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    indices_gen.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );

        glDisableVertexAttribArray(0);


        //envoie des particules
        glUseProgram(programID);

        glUniformMatrix4fv(M, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(V, 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(P, 1, GL_FALSE, &Projection[0][0]);

        glUniform3f(glGetUniformLocation(programID,"c"),smokeColor.x,smokeColor.y,smokeColor.z);
        glUniform1f(glGetUniformLocation(programID,"transp"),transp);
        glUniform1f(glGetUniformLocation(programID,"deltaTime"),time_global);
        glUniform1f(glGetUniformLocation(programID,"melange"),melange);
        glUniform1f(glGetUniformLocation(programID,"amplitude"),amplitude);
        glUniform1f(glGetUniformLocation(programID,"intensity"),intensity);
        glUniform3f(glGetUniformLocation(programID,"lightPos"),light_pos.x,light_pos.y,light_pos.z);
        glUniform3f(glGetUniformLocation(programID,"lightColor"),light_color.x,light_color.y,light_color.z);

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

        //--------------------------

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
    if(normalbuffer) glDeleteBuffers(1,&normalbuffer);
    if(vertexbuffer_gen) glDeleteBuffers(1, &vertexbuffer_gen);
    if(elementbuffer_gen) glDeleteBuffers(1, &elementbuffer_gen);
    if(normalbuffer_gen) glDeleteBuffers(1,&normalbuffer_gen);
    if(vertexbuffer_piece) glDeleteBuffers(1, &vertexbuffer_piece);
    if(elementbuffer_piece) glDeleteBuffers(1, &elementbuffer_piece);
    if(normalbuffer_piece) glDeleteBuffers(1,&normalbuffer_piece);
    if(textcoordbuffer_piece) glDeleteBuffers(1,&textcoordbuffer_piece);
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
