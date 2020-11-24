#include "tiles.h"

#include <glm/gtx/string_cast.hpp> // for debugging

Tiles::Tiles(Shader* shader, int rows, int colums) :
    quadVertices{
                0.5f,  0.5f, 0.0f,  // top right
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  // bottom left
                -0.5f,  0.5f, 0.0f   // top left 
                },
    quadIndices{  // note that we start from 0!
                0, 1, 3,   // first triangle
                1, 2, 3    // second triangle
                },
    quadOffset{1},
    rotation{glm::rotate(glm::mat4{1.0f}, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f))}//,
    //tilesShader{shader}
{
    bool colorToggle = true;
    // Generate transformation matrices and add to positionsmatrices for rows and colums.
    for (int i = 0; i < rows; i++)
    {
        float zOffset = -(i + quadOffset);
        for (int j = 0; j < colums; j++)
        {
            // Multiply translate matrix with rotation. Translate in -z and +x direction.
            quadPositionMatrices.push_back(glm::translate(rotation, glm::vec3(j + quadOffset, zOffset, 0.0f)));

            if (colorToggle)
            {
                colors.push_back(glm::vec3(1.0, 1.0, 1.0));
            }
            else
            {
                colors.push_back(glm::vec3(0.5, 0.5, 1.0));
            }

            colorToggle = !colorToggle;
        }   
        colorToggle = !colorToggle;
    }

    //std::cout << quadPositionMatrices.size() << std::endl;

    // Init bind buffers
    //bindBuffers();

}

Tiles::~Tiles()
{
}

// Must re bind buffers if tiles should move positions.
void Tiles::bindBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
}

// Draw all tiles
void Tiles::drawTiles(Shader* tilesShader, glm::mat4 projMat, glm::mat4 viewMat)
{
    bindBuffers();
    int count = 0;    
    for (glm::mat4 tilePos : quadPositionMatrices)
    {

        tilesShader->useProgram();
        // Upload model matrix to shader
        tilesShader->uploadMat4("model", tilePos);

        tilesShader->uploadVec3("col", colors[count]);
        count ++;

        //std::cout << glm::to_string(tilePos) << "\n" << std::endl;
        // Upload model matrix to shader
        tilesShader->uploadMat4("projection", projMat);
        //std::cout << glGetError() << std::endl;
        tilesShader->uploadMat4("view", viewMat);
        //std::cout << glGetError() << std::endl;
        //tilesShader->uploadMat4("model", rotation);
        //std::cout << glGetError() << std::endl;

        tilesShader->useProgram();
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 
    }
    
   /*
    glUseProgram(tilesShader->shaderID);
    // Upload model matrix to shader
    tilesShader->uploadMat4("projection", projMat);
    std::cout << glGetError() << std::endl;
    tilesShader->uploadMat4("view", viewMat);
    std::cout << glGetError() << std::endl;
    tilesShader->uploadMat4("model", rotation);
    std::cout << glGetError() << std::endl;
    
    //std::cout << glm::to_string(rotation) << "\n" << std::endl;

    glUseProgram(tilesShader->shaderID);
    std::cout << glGetError() << std::endl;
    //std::cout << glGetError() << std::endl;
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    std::cout << glGetError() << std::endl;
    //std::cout << glGetError() << std::endl;
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    std::cout << glGetError() << std::endl;
    //std::cout << glGetError() << std::endl;
    // glBindVertexArray(0); // no need to unbind it every time 
    */
}