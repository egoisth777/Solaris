#ifndef GLOBALINSTANCE_H
#define GLOBALINSTANCE_H


#include "qglobal.h"
#include "scene/chunk.h"
#include "smartpointerhelp.h"
#include "settings.h"

#include <Interface/IUpdateComponent.h>

#include <MultiThread/threadpool.h>

#include <Frustum/frustum.h>

#include <cmath>

class MGlobalInstance : public IUpdateComponent
{
  // STATIC GlOBAL INSTANCE
private:
  static uPtr<MGlobalInstance> m_instance;

public:
  static MGlobalInstance* Get;
  static void Init(const Camera&);
  static void Destroy();
  MGlobalInstance(const Camera&);

  // IUpdateComponent interface
public:
    void update() override;
    void awake() override;

    /// Cache the game-start time in start()
    void start() override;


    // Delta Time
private:
    qint64  mPrevTime   = 0;
    qint64  mPrevRenderTime   = 0;
    qint64  mDeltaTime  = 0;
    qint64  mStartTime  = 0;
    int mUpdateCount = 0;

    float mRenderTimeScale = 1.f;
    float mRenderTimeScaleIndex = 3;
    bool  mIsReversed = false;
    const std::vector<float> renderTimeScalePresets =
        {0, 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0};

private:
    void updateTimer();
public:
    /// Get delta time, in secs
    float   deltaTime();
    /// Get delta time, in msecs
    qint64  deltaMSecsTime();
    /// Get current time elasped since game start in real world, in msecs
    /// Scaled by renderTimeScale;
    qint64  currentRenderMSecsTime();

    qint64  currentMSecsTime();

    void resetTime();

    glm::vec3 getLightAngle();

    void scaleRenderTimeUp();
    void scaleRenderTimeDown();
    void scaleRenderTimeReverse();


    // Profiler
private:
    std::unordered_map<QString, qint64> mProfilerMap;
public:
    qint64 getProfilerTime();
    void printProfiler(QString name);
    void startProfiler(QString name);

    // FPS
#define FPS_COUNTING_CYCLE 10       // making average based on multiple update cycle
private:
    int     mTotalFpsCountingCycle      = 0;
    qint64  mAccumulatedDeltaTimeInCycle= 0;
    float   mFPS        = 0;
    float   mAverageFPS = 0;
private:
    void updateFps();
public:
    float   framePerScreen();
    /// The average fps start from the game
    float   aveFramePerScreen();


    // Thread-Pool
// Use half hardware thread count as default worker count
private:
    ThreadPool m_threadPool;
public:
    ThreadPool& ThreadPool;


    // Frustum
private:
    Frustum m_frustum;
public:
    Frustum& Frustum = m_frustum;

    /// Check if a chunk is inside the camera's frustum
    bool isChunkInFrustum(const Chunk*) const;

    /// Check if a bounding box defined by a corner and its entends is inside the camera's frustum
    bool isBoxInFrustum(const glm::vec3& corner, const glm::vec3& extend) const;
};

#endif // GLOBALINSTANCE_H
