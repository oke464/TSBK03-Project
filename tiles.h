#ifndef _TILES_H_
#define _TILES_H_

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "shader.h"

class Tiles
{
public:
    Tiles(Shader* shader, int rows, int colums);
    ~Tiles();

    void bindBuffers();
    void drawTiles(Shader* tilesShader, glm::mat4 projMat, glm::mat4 viewMat);

private:
    float quadVertices[12];
    unsigned int quadIndices[6];
    float quadOffset;

    glm::mat4 rotation{};
    std::vector<glm::mat4> quadPositionMatrices{};

    std::vector<glm::vec3> colors{};

    //Shader* tilesShader{};

    
    unsigned int VAO{};
    unsigned int VBO{};
    unsigned int EBO{};
    
};

#endif