#include "globalinstance.h"
#include <QDateTime>
#include <iostream>
#include "macros/debugMacro.h"

uPtr<MGlobalInstance> MGlobalInstance::m_instance = nullptr;
MGlobalInstance* MGlobalInstance::Get = nullptr;

void MGlobalInstance::Init(const Camera& camera)
{
    if(Get == nullptr){
        m_instance = mkU<MGlobalInstance>(camera);
    }
    else{
        return;
    }

    Get = m_instance.get();
    Get->awake();
}

void MGlobalInstance::Destroy()
{
    m_instance.release();
    Get = nullptr;
    m_instance = nullptr;
}

MGlobalInstance::MGlobalInstance(const Camera& camera) :
    m_threadPool(THREAD_COUNT),
    ThreadPool(m_threadPool),
    m_frustum(&camera)
{}

void MGlobalInstance::update()
{
    ++mUpdateCount;
    updateTimer(); // Everything goes after this
    updateFps();
    m_frustum.update();
}


void MGlobalInstance::awake()
{

}

void MGlobalInstance::start()
{
    mStartTime = QDateTime::currentMSecsSinceEpoch();
    mPrevTime = 0;
    mDeltaTime = 0;
}


void MGlobalInstance::updateTimer()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch() - mStartTime;
    mDeltaTime = currentTime - mPrevTime;
    mPrevTime = currentTime;
    mPrevRenderTime += mDeltaTime * mRenderTimeScale;
    //std::cout << mUpdateCount << ' ' << mDeltaTime << std::endl;
}

void MGlobalInstance::resetTime()
{
    mStartTime = QDateTime::currentMSecsSinceEpoch();
}

float MGlobalInstance::deltaTime()
{
    return (float) mDeltaTime / 1000.0f;
}

qint64 MGlobalInstance::deltaMSecsTime()
{
    return mDeltaTime;
}

qint64 MGlobalInstance::currentRenderMSecsTime()
{
    return mPrevRenderTime;
}

qint64 MGlobalInstance::currentMSecsTime()
{
    return mPrevTime;
}

glm::vec3 MGlobalInstance::getLightAngle()
{
    float angle = glm::mod(currentRenderMSecsTime() * 0.00005, 2.0 * M_PI);
    float dayTime = angle / M_PI;
    glm::vec3 dir = glm::vec3(0.f, sin(angle), cos(angle));
    // moon dir
    if (dayTime > 1.0) dir = -dir;

    //dir.x = 0.2f;
    return glm::normalize(dir);
}

void MGlobalInstance::scaleRenderTimeUp()
{
    if(mRenderTimeScaleIndex >= renderTimeScalePresets.size() - 1){
        return;
    }

    mRenderTimeScale = (mIsReversed ? -1 : 1) * renderTimeScalePresets[++mRenderTimeScaleIndex];
    std::cout << "Scale Render time to: " << mRenderTimeScale << std::endl;
}

void MGlobalInstance::scaleRenderTimeDown()
{
    if(mRenderTimeScaleIndex <= 0){
        return;
    }

    mRenderTimeScale = (mIsReversed ? -1 : 1) * renderTimeScalePresets[--mRenderTimeScaleIndex];
    std::cout << "Scale Render time to: " << mRenderTimeScale << std::endl;
}

void MGlobalInstance::scaleRenderTimeReverse()
{
    mIsReversed = !mIsReversed;
    mRenderTimeScale *= -1;
}

qint64 MGlobalInstance::getProfilerTime()
{
    return QDateTime::currentMSecsSinceEpoch();
}

void MGlobalInstance::printProfiler(QString name)
{
#ifdef DEBUG
    auto elapsed = getProfilerTime() - mProfilerMap[name];
    std::cout << name.toStdString() << " elapsed time: " << elapsed << std::endl;
#endif
}

void MGlobalInstance::startProfiler(QString name)
{
#ifdef DEBUG
    mProfilerMap[name] = getProfilerTime();
#endif
}


void MGlobalInstance::updateFps()
{
    mAccumulatedDeltaTimeInCycle += mDeltaTime;
    if(++mTotalFpsCountingCycle % FPS_COUNTING_CYCLE == 0){
        mFPS = 1000.f / mAccumulatedDeltaTimeInCycle * FPS_COUNTING_CYCLE;
        mAccumulatedDeltaTimeInCycle = 0;
        mAverageFPS = 1000.f / currentMSecsTime() * mTotalFpsCountingCycle;
    }
}

float MGlobalInstance::framePerScreen()
{
    return mFPS;
}

float MGlobalInstance::aveFramePerScreen()
{
    return mAverageFPS;
}

bool MGlobalInstance::isChunkInFrustum(const Chunk * chunk) const
{
    glm::vec3 chunkCorner = {chunk->getOrigin().x, 0, chunk->getOrigin().y};
    return isBoxInFrustum(chunkCorner, Chunk::ChunkSize);
}

bool MGlobalInstance::isBoxInFrustum(const glm::vec3 &corner, const glm::vec3 &extend) const
{
    return this->m_frustum.isOnFrustumBoxCorner(corner, extend);
}
