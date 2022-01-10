#define _CTR_SECURE_NO_WARNINGS

#include <cstring>
#include <cstdio>
#include "include/vgl.h"
#include "include/vbm.h"

VBObject::VBObject(void) : mVAO(0), mAttributeBuffer(0),
    mIndexBuffer(0), mAttrib(0),
    mFrame(0), mMaterial(0)
{

}

VBObject::~VBObject(void)
{
    this->free();
}

bool VBObject::loadFromVBM(const char* filename, int vertexIndex, int normalIndex, int texCoord0Index)
{
    FILE* fp = NULL;
    fprintf(stderr, "%s load from VBM %s\n", __FUNCTION__, filename);
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "%s cant load from VBM %s\n", __FUNCTION__, filename);
        return false;
    }
    

    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp); //get the file size
    fseek(fp, 0, SEEK_SET);

    unsigned char* data = new unsigned char[fileSize];
    unsigned char* rawData;
    fread(data, fileSize, 1, fp);
    fclose(fp);

    VBM_HEADER_OLD* oldHeader = (VBM_HEADER_OLD*)data;
    VBM_HEADER* header = (VBM_HEADER*)data;
    rawData = data + header->size + header->numAttribs * sizeof(VBM_ATTRIB_HEADER) 
        + header->numFrames * sizeof(VBM_FRAME_HEADER);
    VBM_ATTRIB_HEADER* attribHeader = (VBM_ATTRIB_HEADER *)(data + header->size);
    VBM_FRAME_HEADER* frameHeader = (VBM_FRAME_HEADER*)(data + header->size + header->numAttribs * sizeof(VBM_ATTRIB_HEADER));
    unsigned int totalDataSize = 0;
    if (header->magic == 0x314d4253) {
        memset(&mHeader, 0, sizeof(mHeader));
        memcpy(&mHeader, header, header->size > sizeof(VBM_HEADER) ? sizeof(VBM_HEADER) : header->size);
    } else {
        memcpy(&mHeader, oldHeader, sizeof(VBM_HEADER));
        mHeader.numVertices = oldHeader->numVertices;
        mHeader.numIndices = oldHeader->numIndices;
        mHeader.indexType = oldHeader->indexType;
        mHeader.numMaterials = oldHeader->numMaterials;
        mHeader.flags = oldHeader->flags;
    }

    mAttrib = new VBM_ATTRIB_HEADER[mHeader.numAttribs];
    memcpy(mAttrib, attribHeader, mHeader.numAttribs * sizeof(VBM_ATTRIB_HEADER));
    mFrame = new VBM_FRAME_HEADER[mHeader.numFrames];
    memcpy(mFrame, frameHeader, mHeader.numFrames * sizeof(VBM_FRAME_HEADER));

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    glGenBuffers(1, &mAttributeBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffer);

    for (int i = 0; i < mHeader.numAttribs; i++) {
        totalDataSize += mAttrib[i].components * sizeof(GLfloat) * mHeader.numVertices;
    }
    glBufferData(GL_ARRAY_BUFFER, totalDataSize, rawData, GL_STATIC_DRAW);

    totalDataSize = 0;
    for (int i = 0; i < mHeader.numAttribs; i++) {
        int attribIndex = i;

        if (attribIndex == 0) {
            attribIndex = vertexIndex;
        } else if (attribIndex == 1) {
            attribIndex = normalIndex;
        } else if (attribIndex == 2) {
            attribIndex = texCoord0Index;
        }

        glVertexAttribPointer(attribIndex, mAttrib[i].components, mAttrib[i].type, GL_FALSE, 0, (GLvoid *)totalDataSize);
        glEnableVertexAttribArray(attribIndex);
        totalDataSize += mAttrib[i].components * sizeof(GLfloat) * header->numVertices;
    }

    if (mHeader.numIndices) {
        glGenBuffers(1, &mIndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        unsigned int elementSize;
        switch (header->indexType)
        {
        case GL_UNSIGNED_SHORT:
            elementSize = sizeof(GLushort);
            break;
        default:
            elementSize = sizeof(GLuint);
            break;
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mHeader.numIndices * elementSize, rawData + totalDataSize, GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    if (mHeader.numMaterials != 0) {
        mMaterial = new VBM_MATERIAL[mHeader.numMaterials];
        memcpy(mMaterial, rawData + totalDataSize, mHeader.numMaterials * sizeof(VBM_MATERIAL));
        totalDataSize += mHeader.numMaterials * sizeof(VBM_MATERIAL);
        mMaterialTextures = new VBObject::materialTexture[mHeader.numMaterials];
        memset(mMaterialTextures, 0, mHeader.numMaterials * sizeof(mMaterialTextures));
    }

    /*
    if (mHeader.numChunks != 0)
    {
        numChunks = new VBM_RENDER_CHUNK[mHeader.numChunks];
        memcpy(mChunks, rawData + totalDataSize, mHeader.numChunks * sizeof(VBM_RENDER_CHUNK));
        totalDataSize += mHeader.numChunks * sizeof(VBM_RENDER_CHUNK);
    }
    */

    delete [] data;

    return true;
}

bool VBObject::free(void)
{
    glDeleteBuffers(1, &mIndexBuffer);
    mIndexBuffer = 0;
    glDeleteBuffers(1, &mAttributeBuffer);
    mAttributeBuffer = 0;
    glDeleteVertexArrays(1, &mVAO);
	mVAO = 0;

    delete []mAttrib;
    mAttrib = NULL;
    delete []mFrame;
    mFrame = NULL;
    delete [] mMaterial;
    mMaterial = NULL;

    return true;
}

void VBObject::render(unsigned int frameIndex, unsigned int instances)
{
    if (frameIndex >= mHeader.numFrames) {
        return ;
    }

    glBindVertexArray(mVAO);

#if 0
    if (mHeader.numChunks)
    {
        unsigned int chunk = 6; // (t >> 1) % m_header.num_chunks;

        for (chunk = 0; chunk < mHeader.numChunks; chunk++)
        {
            unsigned int materialIndex = mChunks[chunk].materialIndex;
            // if (m_material_textures[material_index].normal != 0)
            {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, mMaterialTextures[materialIndex].normal);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mMaterialTextures[materialIndex].specular);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mMaterialTextures[materialIndex].diffuse);
                VBM_MATERIAL * material = &mMaterial[mChunks[chunk].materialIndex];
                glDrawArrays(GL_TRIANGLES, mChunks[chunk].first, mChunks[chunk].count);
            }
        }
    } 
    else
#else
    {
        if(instances) {
            if (mHeader.numIndices) {
                glDrawElementsInstanced(GL_TRIANGLES, mFrame[frameIndex].count, GL_UNSIGNED_INT, (GLvoid*)(mFrame[frameIndex].first * sizeof(GLuint)), instances);
            } else {
                glDrawArraysInstanced(GL_TRIANGLES, mFrame[frameIndex].first, mFrame[frameIndex].count, instances);
            }
        } else {
            if (mHeader.numIndices) {
                glDrawElements(GL_TRIANGLES, mFrame[frameIndex].count, GL_UNSIGNED_INT, (GLvoid*)(mFrame[frameIndex].first * sizeof(GLuint)));
            } else {
                glDrawArrays(GL_TRIANGLES, mFrame[frameIndex].first, mFrame[frameIndex].count);
            }
        }
        glBindVertexArray(0);
    }
#endif 

}

void VBObject::bindVertexArray(void)
{
    glBindVertexArray(mVAO);
}