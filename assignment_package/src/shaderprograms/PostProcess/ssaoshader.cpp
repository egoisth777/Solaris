#include "ssaoshader.h"

SSAOShader::SSAOShader(OpenGLContext* context) :
    PostProcessingShader(context)
{}


void SSAOShader::setupMemberVars()
{
    PostProcessingShader::setupMemberVars();
    unifSamplePoints= context->glGetUniformLocation(prog, "u_SamplePoints");
    unifNoise       = context->glGetUniformLocation(prog, "u_SampleNoise");
}


void SSAOShader::setUnifSamplePoints(std::vector<glm::vec3> &samples)
{
    useMe();

    if(unifSamplePoints != -1)
    {
        context->glUniform3fv(  unifSamplePoints,
                                // How many to pass
                                samples.size(),
                                // Pointer to the first element
                                &(samples[0][0]));
    }
}

void SSAOShader::setUnifNoise(std::vector<glm::vec2> &noise)
{
    useMe();

    if(unifNoise != -1)
    {
        context->glUniform2fv(  unifNoise,
                                // How many to pass
                                noise.size(),
                                // Pointer to the first element
                                &(noise[0][0]));
    }
}
