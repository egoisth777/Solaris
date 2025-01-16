#ifndef SSAOSHADER_H
#define SSAOSHADER_H

#include <shaderprograms/postprocessingshader.h>



class SSAOShader : public PostProcessingShader
{
private:
    int unifSamplePoints;
    int unifNoise;
    int unifProj;

public:
    SSAOShader(OpenGLContext* context);

    void setUnifSamplePoints(std::vector<glm::vec3>&);
    void setUnifNoise(std::vector<glm::vec2>&);

    // ShaderProgram interface
public:
    void setupMemberVars() override;
};

#endif // SSAOSHADER_H
