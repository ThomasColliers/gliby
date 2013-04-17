#pragma once

#include <GL/glew.h>
#include "Math3D.h"

using namespace Math3D;

class Batch {
    public:
        Batch(void);
        ~Batch(void);

        // start & end batch
        void begin(GLenum primitive, GLuint nVerts, GLuint nTextureUnits = 0);
        void end(void);

        // block copy in vertex data
        void copyVertexData3f(Vector3f *verts_);
        void copyVertexData3f(GLfloat *verts_);
        void copyNormalDataf(Vector3f *normals_);
        void copyNormalDataf(GLfloat *normals_);
        void copyColorData4f(Vector4f *colors_);
        void copyColorData4f(GLfloat *colors_);
        void copyTexCoordData2f(Vector2f *texCoords_, GLuint texture);
        void copyTexCoordData2f(GLfloat *texCoords_, GLuint texture);
        
        // draw
        void draw(void);

        void reset(void);

    protected:
        // opengl buffer objects
        GLenum primitiveType;
        GLuint vertexArray;
        GLuint normalArray;
        GLuint colorArray;
        GLuint *texCoordArray;
        GLuint vertexArrayObject;

        // counters
        GLuint nNumVerts;
        GLuint nNumTextureUnits;

        bool done;
};
