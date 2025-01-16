#pragma once
#include <openglcontext.h>
#include <glm_includes.h>

//This defines a class which can be rendered by our shader program.
//Make any geometry a subclass of ShaderProgram::Drawable in order to render it with the ShaderProgram class.
class Drawable
{
public:
    struct VertexData
    {
        glm::vec4 pos;
        glm::vec4 nor;
        glm::vec2 uv;
        int block;

        VertexData(glm::vec4 p, glm::vec4 n, glm::vec2 u, int b);
    };
    bool ready = true;
protected:
    int m_count;     // The number of indices stored in bufIdx.
    int m_traCnt;    // The number of indices stored in transparent index buffer
    GLuint m_bufIdx; // A Vertex Buffer Object that we will use to store triangle indices (GLuints)
    GLuint m_bufTraIdx; // A Vertex Buffer Object that we will use to store triangle indices (GLuints)

    GLuint m_bufPos; // A Vertex Buffer Object that we will use to store mesh vertices (vec4s)
    GLuint m_bufNor; // A Vertex Buffer Object that we will use to store mesh normals (vec4s)
    GLuint m_bufUV;// A Vertex Buffer Object that we will use to store mesh uv2 (vec2s)
    GLuint m_bufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.
                   // Instead, we use a uniform vec4 in the shader to set an overall color for the geometry
    GLuint m_bufVert; // Interleaved opaque buffer
    GLuint m_bufTransp; // Interleaved transparent buffer

    bool m_idxGenerated; // Set to TRUE by generateIdx(), returned by bindIdx().
    bool m_traIdxGenerated;
    bool m_posGenerated;
    bool m_norGenerated;
    bool m_colGenerated;
    bool m_uvGenerated;
    bool m_vertGenerated;
    bool m_transpGenerated;

    OpenGLContext* mp_context; // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                          // we need to pass our OpenGL context to the Drawable in order to call GL functions
                          // from within this class.


public:
    Drawable(OpenGLContext* mp_context);
    virtual ~Drawable();

    virtual void createVBOdata() = 0; // To be implemented by subclasses. Populates the VBOs of the Drawable.
    void destroyVBOdata(); // Frees the VBOs of the Drawable.

    // Getter functions for various GL data
    virtual GLenum drawMode();
    int elemCount();
    int traElemCount();
    bool initialized();

    // Call these functions when you want to call glGenBuffers on the buffers stored in the Drawable
    // These will properly set the values of idxBound etc. which need to be checked in ShaderProgram::draw()
    void generateIdx();
    void generateTraIdx();
    void generatePos();
    void generateNor();
    void generateCol();
    void generateUV();
    void generateVert();
    void generateTransp();

    bool bindIdx();
    bool bindTraIdx();
    bool bindPos();
    bool bindNor();
    bool bindCol();
    bool bindUV();
    bool bindVert();
    bool bindTransp();
};

// A subclass of Drawable that enables the base code to render duplicates of
// the Terrain class's Cube member variable via OpenGL's instanced rendering.
// You will not have need for this class when completing the base requirements
// for Mini Minecraft, but you might consider using instanced rendering for
// some of the milestone 3 ideas.
class InstancedDrawable : public Drawable {
protected:
    int m_numInstances;
    GLuint m_bufPosOffset;

    bool m_offsetGenerated;

public:
    InstancedDrawable(OpenGLContext* mp_context);
    virtual ~InstancedDrawable();
    int instanceCount() const;

    void generateOffsetBuf();
    bool bindOffsetBuf();
    void clearOffsetBuf();
    void clearColorBuf();

    virtual void createInstancedVBOdata(std::vector<glm::vec3> &offsets, std::vector<glm::vec3> &colors) = 0;
};
