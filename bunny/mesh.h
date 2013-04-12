#ifndef __MESH_H__
#define __MESH_H__

#include "../ogl_es.h"

struct VERTEX
{
    float Pos[3];
    float Normal[3];
};

// Poly mesh

class CMesh
{
public:
    CMesh(const VERTEX *pVertices, GLsizeiptr VBSize, const GLushort *pIndices, GLuint IndexCount);
    ~CMesh();

    void Draw();

private:
    GLuint m_VB;
    GLuint m_IB;
    GLuint m_IndexCount;
};

#endif // __MESH_H__
