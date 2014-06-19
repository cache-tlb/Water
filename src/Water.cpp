#include "Water.h"
#include "GLHeader.h"

Water::Water() {
    plane = Mesh::plane();
    GLint filter = GL_LINEAR;

    Option options;
    options["type"] = toString(GL_FLOAT);
    options["filter"] = toString(filter);
    textureA = new Texture(256, 256, options);
    textureB = new Texture(256, 256, options);
    
    dropShader   = new Shader("./shaders/WaterVertex.vert", "./shaders/WaterDropShader.frag");
    updateShader = new Shader("./shaders/WaterVertex.vert", "./shaders/WaterUpdateShader.frag");
    normalShader = new Shader("./shaders/WaterVertex.vert", "./shaders/WaterNormalShader.frag");
    sphereShader = new Shader("./shaders/WaterVertex.vert", "./shaders/WaterSphereShader.frag");
}

Water::~Water() {
    if (plane) delete plane;
    if (textureA) delete textureA;
    if (textureB) delete textureB;

    if (dropShader) delete dropShader;
    if (updateShader) delete updateShader;
    if (sphereShader) delete sphereShader;
    if (normalShader) delete normalShader;
}

void Water::addDrop(float x, float y, float radius, float strength) {
    Texture *_textureA = textureA;
    Shader *_dropShader = dropShader;
    float xy[2]; xy[0] = x; xy[1] = y; float *_xy = xy;
    Mesh *_plane = plane;

    std::function<void(void)> callback = [_textureA, _dropShader, _xy, _plane, radius, strength](){
        _textureA->bind();
        _dropShader->uniforms("center", _xy);
        _dropShader->uniforms("radius", radius);
        _dropShader->uniforms("strength", strength);
        _dropShader->uniforms("texture", 0);
        _dropShader->draw(_plane);
    };

    textureB->drawTo(callback);
    textureB->swapWith(textureA);
}

void Water::moveSphere(const Vector &oldCenter, const Vector &newCenter, float radius) {
    Texture *_textureA = textureA;
    Shader *_sphereShader = sphereShader;
    float oldCenterArray[3]; oldCenter.toArray(oldCenterArray); float *_oldCenterArray = oldCenterArray;
    float newCenterArray[3]; newCenter.toArray(newCenterArray); float *_newCenterArray = newCenterArray;
    Mesh *_plane = plane;

    std::function<void(void)> callback = [_textureA, _sphereShader, _oldCenterArray, _newCenterArray, _plane, radius](){
        _textureA->bind();
        _sphereShader->uniforms("oldCenter", _oldCenterArray);
        _sphereShader->uniforms("newCenter", _newCenterArray);
        _sphereShader->uniforms("radius", radius);
        _sphereShader->uniforms("texture", 0);
        _sphereShader->draw(_plane);
    };

    textureB->drawTo(callback);
    textureB->swapWith(textureA);
}

void Water::stepSimulation() {
    Texture *_textureA = textureA;
    Shader *_updateShader = updateShader;
    float delta[2]; delta[0] = 1.f / float(textureA->width); delta[1] = 1.f / float(textureA->height);
    float *_delta = delta;
    Mesh *_plane = plane;

    std::function<void(void)> callback = [_textureA, _updateShader, _delta, _plane](){
        _textureA->bind();
        _updateShader->uniforms("delta", _delta);
        _updateShader->uniforms("texture", 0);
        _updateShader->draw(_plane);
    };

    textureB->drawTo(callback);
    textureB->swapWith(textureA);
}

void Water::updateNormals() {
    Texture *_textureA = textureA;
    Shader *_normalShader = normalShader;
    float delta[2]; delta[0] = 1.f / float(textureA->width); delta[1] = 1.f / float(textureA->height);
    float *_delta = delta;
    Mesh *_plane = plane;

    std::function<void(void)> callback = [_textureA, _normalShader, _delta, _plane](){
        _textureA->bind();
        _normalShader->uniforms("delta", _delta);
        _normalShader->uniforms("texture", 0);
        _normalShader->draw(_plane);
    };
    textureB->drawTo(callback);
    textureB->swapWith(textureA);
}

bool Water::checkShader() const {
    return dropShader->vsshader.isProgramValid()
        && updateShader->vsshader.isProgramValid()
        && sphereShader->vsshader.isProgramValid()
        && normalShader->vsshader.isProgramValid();
}
