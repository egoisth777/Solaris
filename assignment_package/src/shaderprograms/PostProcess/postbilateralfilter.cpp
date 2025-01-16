#include "postbilateralfilter.h"


postBilateralFilter::postBilateralFilter(OpenGLContext *context) :
    PostProcessingShader(context)
{}

void postBilateralFilter::setUnifSpatialPara(float val)
{
    useMe();

    if(unifSpatialParameter != -1)
    {
        context->glUniform1f(unifSpatialParameter, val);
    }
}

void postBilateralFilter::setUnifRangePara(float val)
{
    useMe();

    if(unifRangeParameter != -1)
    {
        context->glUniform1f(unifRangeParameter, val);
    }
}

void postBilateralFilter::setupMemberVars()
{
    PostProcessingShader::setupMemberVars();
    unifSpatialParameter = context->glGetUniformLocation(prog, "spatialParameter");
    unifRangeParameter = context->glGetUniformLocation(prog, "rangeParameter");
}
