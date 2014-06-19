#pragma once

#include "StdHeader.h"
#include "GLHeader.h"
#include "vsShaderLib.h"

class Mesh {
public:
    Mesh();

    GLuint vao;
    GLuint vertex_coord_buff, texture_coord_buff, normal_buff, tri_index_buff;
    
    bool hasCoords, hasNormals;

    std::vector<float> vertices;    // x, y, z
    std::vector<float> texture_coords;      // x, y
    std::vector<float> normals;     // x, y, z
    std::vector<unsigned int> triangles;    // i, j, k


    void compile();
    
    // not used:
    // mesh.transform
    // mesh.computeNormals
    // mesh.computeWireframe
    // mesh.getAABB
    // mesh.getBoundingSphere

    static Mesh *plane(int detailX = 1, int detailY = 1);
    static Mesh *cube();
    static Mesh *sphere(int detail = 6);
protected:
private:
};


class Indexer {
public:
    typedef std::tuple<float,float,float> KeyType;
    Indexer(){}
    ~Indexer(){}
    int add(KeyType &v);
    std::vector<KeyType> unique;
    std::map<KeyType,int> map_;
private:
};

