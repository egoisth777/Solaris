#ifndef POSTBILATERALFILTER_H
#define POSTBILATERALFILTER_H

#include <shaderprograms/postprocessingshader.h>



class postBilateralFilter : public PostProcessingShader
{
public:
    int unifSpatialParameter = -1;
    int unifRangeParameter = -1;

public:
    postBilateralFilter(OpenGLContext* context);

    void setUnifSpatialPara(float);
    void setUnifRangePara(float);

    // ShaderProgram interface
public:
    void setupMemberVars() override;
};

#endif // POSTBILATERALFILTER_H
