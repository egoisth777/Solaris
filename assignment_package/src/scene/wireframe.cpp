#include "wireframe.h"
#include "blocks.h"

WireFrame::~WireFrame()
{}

void WireFrame::createVBOdata()
{
    glm::vec4 pos[24];
    for (int i = 0; i < 24; ++i)
    {
        pos[i] = offset + CubeVert[i] + 0.0005f * glm::vec4(DirVec[i / 4], 0);
    }

    glm::vec4 col[24];
    std::fill(std::begin(col), std::begin(col) + 24, glm::vec4(0,0,0,1));

    GLuint idx[48];
    for (int i = 0; i < 6; ++i)
    {
        int j = 8 * i;
        idx[j + 0] = i * 4 + 0; idx[j + 1] = i * 4 + 1;
        idx[j + 2] = i * 4 + 1; idx[j + 3] = i * 4 + 2;
        idx[j + 4] = i * 4 + 2; idx[j + 5] = i * 4 + 3;
        idx[j + 6] = i * 4 + 3; idx[j + 7] = i * 4 + 0;
    }

    m_count = 48;

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 48 * sizeof(GLuint), idx, GL_DYNAMIC_DRAW);
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), pos, GL_DYNAMIC_DRAW);
    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), col, GL_DYNAMIC_DRAW);
}

GLenum WireFrame::drawMode()
{
    return GL_LINES;
}
