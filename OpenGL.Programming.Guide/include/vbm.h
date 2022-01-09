#ifndef __VBM_H__
#define __VBM_H__

//Define VBM_FILE_TYPES_ONLY before including this file to only include definitions
//of types used in VBM files. This can be used to create loaders/converts/exportes that
//have no dependencies outside this file. not that in that case. gl.h dosen't get 
//include and so in order to include some os the tokens required by files
//(GL_UNSIGNED_INT, for example). you will need to defined them youself
#ifndef VBM_FILE_TYPES_ONLY
//#include "vgl.h"
#include "vmath.h"
#endif

#define VBM_FLAG_HAS_VERTICES       0x00000001
#define VBM_FLAG_HAS_INDICES        0x00000002
#define VBM_FLAG_HAS_FRAMES         0x00000004
#define VBM_FLAG_HAS_MATERIALS      0x00000008

typedef struct VBM_HEADER_t
{
    unsigned int magic;
    unsigned int size;
    char name[64];
    unsigned int numAttribs;
    unsigned int numFrames;
    // unsigned int numChunks;
    unsigned int numVertices;
    unsigned int numIndices;
    unsigned int indexType;
    unsigned int numMaterials;
    unsigned int flags;
} VBM_HEADER;

typedef struct VBM_HEADER_OLD_t
{
    unsigned int magic;
    unsigned int size;
    char name[64];
    unsigned int numAttribs;
    unsigned int numFrames;
    unsigned int numChunks;
    unsigned int numVertices;
    unsigned int numIndices;
    unsigned int indexType;
    unsigned int numMaterials;
    unsigned int flags;
} VBM_HEADER_OLD;

typedef struct VBM_ATTRIB_HEADER_t
{
    char name[64];
    unsigned int type;
    unsigned int components;
    unsigned int flags;
} VBM_ATTRIB_HEADER;

typedef struct VBM_FRAME_HEADER_t
{
    unsigned int first;
    unsigned int count;
    unsigned int flags;
} VBM_FRAME_HEADER;

typedef struct VBM_RENDER_CHUNK_t
{
    unsigned int materialIndex;
    unsigned int first;
    unsigned int count;
} VBM_RENDER_CHUNK;

typedef struct VBM_VEC4F_t
{
    float x;
    float y;
    float z;
    float w;
} VBM_VEC4F;

typedef struct VBM_VEC3F_t
{
    float x;
    float y;
    float z;
}VBM_VEC3F;

typedef struct VBM_VEC2F_t
{
    float x;
    float y;
}VBM_VEC2F;

typedef struct VBM_MATERIAL_t
{
    char name[32]; //name of the material
    VBM_VEC3F ambient; //ambient color;
    VBM_VEC3F diffuse; //diffuse color;
    VBM_VEC3F specular; //specular color
    VBM_VEC3F specularExp; //specular exponent
    float shininess; //shiness
    float alpha; //alpha value (transparency)
    VBM_VEC3F transmission; //transmissivity
    float ior; //index of refraction (optical density)
    char ambientMap[64]; //ambient map (texture)
    char diffuseMap[64]; //diffuse map (texture)
    char specularMap[64]; //specular map (texture)
    char normalMap[64]; //normal map (texture)
} VBM_MATERIAL;

#ifndef VBM_FILE_TYPES_ONLY

class VBObject
{
public:
    VBObject(void);
    virtual ~VBObject(void);

    bool loadFromVBM(const char *filename, int vertexIndex, int normalIndex, int texCoord0Index);
    void render(unsigned int frameIndex = 0, unsigned int instances = 0);
    bool free(void);

    unsigned int getVertexCount(unsigned int frame = 0)
    {
        return frame < mHeader.numFrames ? mFrame[frame].count : 0;
    }

    unsigned int getAttributeCount(void)
    {
        return mHeader.numAttribs;
    }

    const char* getAttributename(unsigned int index) const
    {
        return index < mHeader.numAttribs ? mAttrib[index].name : NULL;
    }

    unsigned int getFrameCount(void) const
    {
        return mHeader.numFrames;
    }

    unsigned int getMaterialCount(void) const
    {
        return mHeader.numMaterials;
    }

    const char* getMaterialName(unsigned int index) const
    {
        return mMaterial[index].name;
    }

    const vmath::vec3 getMaterialAmbient(unsigned int index) const
    {
        return vmath::vec3(mMaterial[index].ambient.x, mMaterial[index].ambient.y, mMaterial[index].ambient.z);
    }

    const vmath::vec3 getMaterialDiffuse(unsigned int index) const
    {
        return vmath::vec3(mMaterial[index].diffuse.x, mMaterial[index].diffuse.y, mMaterial[index].diffuse.z);
    }

    const char* getMaterialDiffuseMapName(unsigned int index) const
    {
        return mMaterial[index].diffuseMap;
    }

    const char* getMaterialSpecularMapName(unsigned int index) const
    {
        return mMaterial[index].specularMap;
    }

    const char* getMaterialNormalMapName(unsigned int index) const
    {
        return mMaterial[index].normalMap;
    }

    void setMaterialDiffuseTexture(unsigned int index, GLuint texName)
    {
        mMaterialTextures[index].diffuse = texName;
    }

    void setMaterialSpecularexture(unsigned int index, GLuint texName)
    {
        mMaterialTextures[index].specular = texName;
    }

    void setMaterialNormalTexture(unsigned int index, GLuint texName)
    {
        mMaterialTextures[index].normal = texName;
    }

    void bindVertexArray()
    {
        glBindVertexArray(mVAO);
    }
    

protected:
    GLuint mVAO;
    GLuint mAttributeBuffer;
    GLuint mIndexBuffer;

    VBM_HEADER mHeader;
    VBM_ATTRIB_HEADER* mAttrib;
    VBM_FRAME_HEADER* mFrame;
    VBM_MATERIAL* mMaterial;
    VBM_RENDER_CHUNK* mChunks;

    struct materialTexture
    {
        GLuint diffuse;
        GLuint specular;
        GLuint normal;
    };

    materialTexture* mMaterialTextures;
};
#endif /* VBM_FILE_TYPES_ONLY */

#endif /*__VBM_H__*/