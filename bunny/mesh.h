#ifndef __MESH_H__
#define __MESH_H__

#include "../ogl_es.h"

// Poly mesh

class CMesh
{
public:
    CMesh(const void *pVertices, GLsizeiptr VBSize, const GLushort *pIndices, GLuint IndexCount, bool bHasNormals);
    ~CMesh();

    void Draw();

private:
    GLuint m_VB;
    GLuint m_IB;
    GLuint m_IndexCount;
    bool m_bHasNormals;
};

#endif // __MESH_H__
