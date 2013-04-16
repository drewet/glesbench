#include "mesh.h"

//
// CMesh
//
CMesh::CMesh(const void *pVertices, GLsizeiptr VertexBufferSize, const GLushort *pIndices, GLuint IndexCount, bool bHasNormals):
    m_VB(0),
    m_IB(0),
    m_IndexCount(IndexCount),
    m_bHasNormals(bHasNormals)
{
    glGenBuffers(1, &m_VB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferData(GL_ARRAY_BUFFER, VertexBufferSize, pVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(GLushort), pIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//
// ~CMesh
//
CMesh::~CMesh()
{
    glDeleteBuffers(1, &m_IB);
    glDeleteBuffers(1, &m_VB);
}

//
// Draw
//
void CMesh::Draw()
{
    GLsizei Stride;

    if (m_bHasNormals)
    {
        Stride = sizeof(float) * 6;

        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(0)); // Vertices
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(sizeof(GL_FLOAT) * 3)); // Normals
        glEnableVertexAttribArray(1);
    }
    else
    {
        Stride = 0; // Tightly packed

        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(0)); // Vertices
        glEnableVertexAttribArray(0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);

    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_SHORT, 0);

    if (m_bHasNormals)
        glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}
