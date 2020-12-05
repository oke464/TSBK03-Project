#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <glm/glm.hpp>

#include "learnOpenGL/model.h"
#include "learnOpenGL/shader.h"

class Object
{
public:
    ~Object();

    virtual void draw(Shader shader);


    
    glm::vec3 getPosition() {return pos; }
    glm::mat4 getTransformation() {return transformation; }
    void setPosition(glm::vec3 position) { pos = position; }
    void setScale(glm::vec3 inpScale) { scale = inpScale; }
    void setRotation(glm::mat4 inpRotation) { rotation = inpRotation; }

    void updateTransformation();
protected:

    Object(glm::vec3 position, std::string modelRelativePath);
    glm::vec3 pos;
    Model* objectModel;
    glm::vec3 scale;
    glm::mat4 rotation;
    glm::mat4 transformation;

};


class Sphere : public Object
{
public:
    Sphere(glm::vec3 pos, std::string modelRelativePath);


private:
};

#endif