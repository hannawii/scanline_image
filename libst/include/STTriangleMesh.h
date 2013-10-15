// STTriangleMesh.h
#ifndef __STTRIANGLEMESH_H__
#define __STTRIANGLEMESH_H__

#include "STPoint2.h"
#include "STPoint3.h"

#include <string>
#include <vector>
#include <iostream>

struct STFace;

struct STVertex{
    STVertex(float x, float y, float z, float u=0, float v=0){
        pt=STPoint3(x,y,z);
        texPos=STPoint2(u,v);
    }
    STVertex(const STPoint3& pt, const STPoint2& texPos=STPoint2(0, 0)){
        this->pt=pt;
        this->texPos=texPos;
        f=0;
    }
    STPoint3 pt;
    STVector3 normal;
    STPoint2 texPos;
    STFace *f;
};
inline std::ostream& operator <<(std::ostream& stream, const STVertex& v);

struct STFace{
    STFace(STVertex* v0, STVertex* v1, STVertex* v2){
        v[0]=v0;
        v[1]=v1;
        v[2]=v2;
        texPos[0] = v0->texPos;
        texPos[1] = v1->texPos;
        texPos[2] = v2->texPos;
    }
    STVertex *v[3];
    STFace *adjF[3];
    STVector3 normal;
    STPoint2 texPos[3];
};
inline std::ostream& operator <<(std::ostream& stream, const STFace& f);

/**
* STTriangleMesh use a simple data structure to represent a triangle mesh.
*/
class STTriangleMesh
{
public:
    //
    // Initialization
    //
    STTriangleMesh();
    STTriangleMesh(const std::string& filename);

    //
    // Delete and clean up.
    //
    ~STTriangleMesh();


    //
    // Draw the triangle mesh to the OpenGL window using GL_TRIANGLES.
    //
    void Draw(bool smooth) const;

    //
    // Read the triangle mesh from files.
    //
    bool Read(const std::string& filename);

    unsigned int AddVertex(float x, float y, float z, float u=0, float v=0);

    unsigned int AddVertex(const STPoint3& pt, const STPoint2& texPos=STPoint2(0, 0));

    unsigned int AddFace(unsigned int id0,unsigned int id1,unsigned int id2);
    
    //
    // Build topology and calculate normals for the triangle mesh.
    //
    bool Build();
    bool BuildTopology();
    bool CalculateNormal();
	bool CalculateTextureCoordinates();

    STFace* NextAdjFace(STVertex *v, STFace *f);
    STFace* NextAdjFaceReverse(STVertex *v, STFace *f);

    void LoopSubdivide();
    //
    // Local members
    //
    std::vector<STVertex*> mVertices;
    std::vector<STFace*> mFaces;
};

#endif  // __STTRIANGLEMESH_H__

