#include "object.h"


//######################################
//########## Abstract Object class #####
//######################################

Object::Object(glm::vec3 position, std::string modelRelativePath) : 
    pos{position},
    scale{glm::vec3(1.0f, 1.0f, 1.0f)},
    rotation{glm::mat4(1.0f)},
    transformation{glm::translate(rotation, position)}
{
    // Set model 
    std::string sourcePath = __FILE__;
    std::string dirPath = sourcePath.substr(0, sourcePath.rfind("/"));
    std::string modelPath = std::string(dirPath);
    modelPath.append("/").append(modelRelativePath);
    objectModel = new Model(modelPath);

}

Object::~Object()
{
    delete objectModel;
}

// Updates the transformation matrix based on position, rotation, scale.
void Object::updateTransformation()
{
    transformation = glm::translate(rotation, pos);
    transformation = glm::scale(transformation, scale);
}

void Object::draw(Shader shader)
{
    objectModel->Draw(shader);
}

//######################################
//########## Sphere class ##############
//######################################

Sphere::Sphere(glm::vec3 position, std::string modelRelativePath) :
    Object(position, modelRelativePath)
{

}
