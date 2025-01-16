#ifndef MYGL_H
#define MYGL_H

#include "settings.h"

#include "Enums/GameInitStateEnums.h"
#include "framebuffer.h"
#include "openglcontext.h"
#include "scene/camera.h"
#include "scene/player.h"
#include "scene/quad.h"
#include "scene/terrain.h"
#include "scene/wireframe.h"
#include "shaderprograms/postprocessingshader.h"
#include "shaderprograms/skyshader.h"
#include "shaderprograms/surfaceshader.h"
#include "shaderprograms/compositeshader.h"
#include "shaderprograms/watershader.h"
#include "texture.h"
#include "csm.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <smartpointerhelp.h>
#include <InputHandler/inputhandler.h>
#include <Render/GBufferModel.h>
#include <shaderprograms/PostProcess/motionblurshader.h>
#include <shaderprograms/PostProcess/postbilateralfilter.h>
#include <shaderprograms/PostProcess/ssaoshader.h>


class MyGL : public OpenGLContext
{
    Q_OBJECT
    // Game Variables
private:
    Terrain m_terrain; // All of the Chunks that currently comprise the world.
    Player m_player; // The entity controlled by the user. Contains a camera to display what it sees as well.
    QTimer m_timer; // Timer linked to tick(). Fires approximately 60 times per second.

    InputHandler m_inputHandler;

    // Rendering Variables
private:

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
        // Don't worry too much about this. Just know it is necessary in order to render geometry.
    Quad m_geomQuad;

    WireFrame m_wireFrame; // A wireframe representation of the world axes. It is hard-coded to sit centered at (32, 128, 32).

    SurfaceShader           m_progGBuffer;
    SurfaceShader           m_progLambert;
    SurfaceShader           m_progFlat;
    SurfaceShader           m_progInstanced;
    SurfaceShader           m_progLightDepth;
    PostProcessingShader    m_progDebug;
    PostProcessingShader    m_progUpsample;
    SkyShader               m_skyShader;
    PostProcessingShader    m_progPostWater;
    PostProcessingShader    m_progPostLava;
    PostProcessingShader    m_progPassthrough;
    MotionBlurShader        m_postMotionBlur;
    SSAOShader              m_postSSAOShader;
    postBilateralFilter     m_postBilateralFilter;
    ShadowMapShader         m_progVolumeLight;
    CompositeShader         m_progComposite;
    WaterShader             m_progWater;
    PostProcessingShader    m_postBloom;
    PostProcessingShader    m_postOutline;
    PostProcessingShader    m_postPosterization;
    PostProcessingShader    m_postHatching;
    PostProcessingShader    m_postFogIntensity;
    PostProcessingShader    m_postFog;
    PostProcessingShader    m_postVL;
    PostProcessingShader    m_postSky;

    CascadedShadowMap csm;

    uPtr<Texture> m_albedoMap;
    uPtr<Texture> m_normalMap;

    uPtr<FrameBuffer> m_skyOutput;
    uPtr<Texture> m_skyHalfMap;
    uPtr<Texture> m_skyMap;
    uPtr<Texture> m_pureSkyHalfMap;
    uPtr<Texture> m_pureSkyMap;

    uPtr<FrameBuffer> m_ssaoOutput;
    uPtr<Texture> m_ssaoMap;
    uPtr<Texture> m_ssaoBlurMap;

    uPtr<FrameBuffer> m_renderOutput;
    /// Available before post-process
    /// Currently generated in sky-pass
    uPtr<Texture> m_renderOutputTexture;

    uPtr<FrameBuffer> m_fogOutput;
    uPtr<Texture> m_fogIntensityMap;

    uPtr<FrameBuffer> m_postOutput;
    /// Achieve interleaving output in post-process
    uPtr<Texture> m_intermediateOutputTexture;

    uPtr<FrameBuffer> m_volumeLightBuffer;
    uPtr<Texture> m_volumeLightMap;

    uPtr<FrameBuffer> m_tmpHalfResBuffer;
    uPtr<Texture> m_tmpHalfResMap;

    GBuffer m_GBufferObject;

    BlockType cameraPostFilter;
    glm::mat4 cameraLastViewProj;
    std::vector<glm::vec3> ssaoSamplePoints;
    std::vector<glm::vec2> ssaoSampleNoise;

    /// Feature debugger
private:
    std::vector<bool> m_shaderFeatures;

private:
    void moveMouseToCenter(); // Forces the mouse position to the screen's center. You should call this
        // from within a mouse move event after reading the mouse movement so that
        // your mouse stays within the screen bounds and is always read.

    void sendPlayerDataToGUI() const;


public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    // Called once when MyGL is initialized.
    // Once this is called, all OpenGL function
    // invocations are valid (before this, they
    // will cause segfaults)
    void initializeGL() override;

    // Called whenever MyGL is resized.
    void resizeGL(int w, int h) override;
    // Called whenever MyGL::update() is called.
    // In the base code, update() is called from tick().
    void paintGL() override;

protected:
    // Automatically invoked when the user
    // presses a key on the keyboard
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    // Automatically invoked when the user
    // moves the mouse
    void mouseMoveEvent(QMouseEvent *e) override;
    // Automatically invoked when the user
    // presses a mouse button
    void mousePressEvent(QMouseEvent *e) override;

private slots:
    void tick(); // Slot that gets called ~60 times per second by m_timer firing.

signals:
    void sig_sendPlayerPos(QString)         const;
    void sig_sendPlayerVel(QString)         const;
    void sig_sendPlayerAcc(QString)         const;
    void sig_sendPlayerLook(QString)        const;
    void sig_sendPlayerChunk(QString)       const;
    void sig_sendPlayerTerrainZone(QString) const;
    void sig_sendFPS(QString)               const;
    void sig_sendAveFPS(QString)            const;

    // Manu signals
signals:
    void sig_enableLoadingBanner(bool)      const;
    void sig_sendLoadingStatus(QString)     const;

    // Getter & Setters
public:
    Player *getPlayer();
    Terrain *getTerrain();

    // Life Time
private:
    MGameState::GameInitStateEnums m_initStateFlag = MGameState::GameInitStateEnums::NULL_STATE;

    /// All initialization goes into here
    void awake();

    /// Engine level start, engine start ticking
    /// All post init stuff goes into here
    /// Every component should be well instantiated when calling start
    void start();

    /// Gameplay level start
    /// Player should be able to control now
    void startGame();

    void updateEngine();
    void updateGui();

    /// Calls in the end of every tick
    void postTick();

    void onStateComplete(GameInitState);

    /// Return TRUE iff engine is ready for call start
    bool isEngineReady();

    bool isEngineStart();

    /// Return TRUE iff gameplay is ready to start
    bool isReadyToStartPlay();

    bool isGamePlayStart();

    // Life Time Helpers
private:
    void registerCallbacks();

    // OpenGLContext interface
protected:
    void onGLGetError(GLenum) override;

    // Call back functions
private:
    void onTerrainGenerateComplete();
    void onPlayerCameraOverlayTypeChanged(BlockType);

    // Render PipeLine helpers
private:
    void createSkyPassBuffers();
    void createTmpBuffers();
    void initializeGBuffer();
    void initializeSSAOBuffer();
    void initVolumeLightPass();
    void initFogBuffer();
    void initializePostProcessBuffer();
    void printHardwareDebugInfo();
    /// Change the texture assignment of a framebuffer
    void redirectFbOutput(FrameBuffer*, Texture*,
                          unsigned int assignPosition = 0,
                          bool doClear = true);

    void dynamicEnableFeature(const InputBundle&);

    // Render PipeLine
private:
    // Called from paintGL().
    void generateShadowMap();
    void renderTerrain();
    void renderSkyPass();
    void renderSSAOPass();
    void renderCompositePass();
    void renderWaterPass();
    void renderPostProcess();
    void renderCameraEnvFilter();
    void renderVolumeLightPass();


    // Post-Process shading
private:
    void renderPostProcessMotionBlur();
    void renderPostProcessBloom();
    void renderPostProcessOutline();
    void renderPostProcessPosterization();
    void renderPostProcessHatching();
    void renderPostProcessFog();
    void renderPostProcessVL();
    void renderPostProcessSky();


    void startPostProcessSetUp();
    /// Call this every end of render process to make ping-pong buffer output
    void onOnePostProcessPassComplete();
    void onPostProcessPassComplete();

    // SSAO shading helper
private:
    void renderPostSSAO();
    void renderBlurSSAO();

    // Ui shading
private:
    void renderHeadUpDisplay();

    // QT UI helper
private:
    void enableLoadingView(bool);
    void setLoadingTxt(QString);

    // Video recording helper
private:
    float m_featureDisplayTimer = 0.0f;
    std::vector<ShaderFeatureEnums> m_featureDisplayList;

    void switchFeature(ShaderFeatureEnums);
    void switchFeature(ShaderFeatureEnums, bool target);
    bool getFeature(ShaderFeatureEnums) const;
    void startAutomaticFeatureDisplay();
    void updateFeatureDisplay();
};


#endif // MYGL_H
