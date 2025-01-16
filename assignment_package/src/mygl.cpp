#include "mygl.h"
#include <glm_includes.h>

#include <QApplication>
#include <QKeyEvent>
#include "Enums/EnumUtil.h"
#include "globalinstance.h"
#include <Render/renderutils.h>
#include <iostream>
#include "settings.h"

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent)
    , m_terrain(this)
    , m_player(glm::vec3(-54.f, 166.f, 248.f), m_terrain)
    , m_inputHandler(this, &m_player)
    , m_geomQuad(this)
    , m_wireFrame(this)
    , m_progGBuffer(this)
    , m_progLambert(this)
    , m_progFlat(this)
    , m_progInstanced(this)
    , m_progLightDepth(this)
    , m_progDebug(this)
    , m_progUpsample(this)
    , m_skyShader(this)
    , m_progPostWater(this)
    , m_progPostLava(this)
    , m_progPassthrough(this)
    , m_postMotionBlur(this)
    , m_postSSAOShader(this)
    , m_postBilateralFilter(this)
    , m_progVolumeLight(this)
    , m_progComposite(this)
    , m_progWater(this)
    , m_postBloom(this)
    , m_postOutline(this)
    , m_postPosterization(this)
    , m_postHatching(this)
    , m_postFogIntensity(this)
    , m_postFog(this)
    , m_postVL(this)
    , m_postSky(this)
    , csm(this)
    , cameraPostFilter(EMPTY)
    , m_shaderFeatures(50, true)
{
    awake();    // All initialization goes into here

    onStateComplete(MGameState::GameInitStateEnums::GAME_MGR_CONSTRUCT);
}

MyGL::~MyGL() {
    MGlobalInstance::Destroy();
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}

void MyGL::initializeGL()
{
    // NOTE:
    // This function will be called before the first tick() but after the awake()

    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    printGLErrorLog();

    // laod textures
    m_albedoMap = mkU<Texture>(this);
    m_albedoMap->create("../textures/minecraft_textures_all.png");
    m_albedoMap->load(0);

    m_normalMap = mkU<Texture>(this);
    m_normalMap->create("../textures/minecraft_normals_all.png");
    m_normalMap->load(0);

    printGLErrorLog();

    // initialize framebuffers and textures
    createTmpBuffers();
    createSkyPassBuffers();
    initVolumeLightPass();
    initFogBuffer();
    initializeGBuffer();
    initializeSSAOBuffer();
    initializePostProcessBuffer();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of the world axes
    m_geomQuad.createVBOdata();

    csm.initialize();

    m_progGBuffer.create(":/glsl/gBuffer.vert.glsl", ":/glsl/gBuffer.frag.glsl");
    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    //m_progInstanced.create(":/glsl/instanced.vert.glsl", ":/glsl/lambert.frag.glsl");
    m_skyShader.create(":/glsl/passthrough.vert.glsl", ":/glsl/skyshader.frag.glsl");
    m_progUpsample.create(":/glsl/passthrough.vert.glsl", ":/glsl/upsampling.frag.glsl");
    m_progPassthrough.create(":/glsl/passthrough.vert.glsl", ":/glsl/passthrough.frag.glsl");
    m_progPostWater.create(":/glsl/passthrough.vert.glsl", ":/glsl/water.frag.glsl");
    m_progPostLava.create(":/glsl/passthrough.vert.glsl", ":/glsl/lava.frag.glsl");
    m_postMotionBlur.create(":/glsl/passthrough.vert.glsl", ":/glsl/motionBlur.frag.glsl");
    m_postSSAOShader.create(":/glsl/passthrough.vert.glsl", ":/glsl/postSSAO.frag.glsl");
    m_postBilateralFilter.create(":/glsl/passthrough.vert.glsl", ":/glsl/postBilateralFilter.frag.glsl");
    m_progVolumeLight.create(":/glsl/passthrough.vert.glsl", ":/glsl/volumelight.frag.glsl");
    m_progComposite.create(":/glsl/passthrough.vert.glsl", ":/glsl/composite.frag.glsl");
    m_progWater.create(":/glsl/watershader.vert.glsl", ":/glsl/watershader.frag.glsl");
    m_postBloom.create(":/glsl/passthrough.vert.glsl", ":/glsl/postBloom.frag.glsl");
    m_postOutline.create(":/glsl/passthrough.vert.glsl", ":/glsl/postOutLine.frag.glsl");
    m_postPosterization.create(":/glsl/passthrough.vert.glsl", ":/glsl/postPosterization.frag.glsl");
    m_postHatching.create(":/glsl/passthrough.vert.glsl", ":/glsl/postHatching.frag.glsl");
    m_postFogIntensity.create(":/glsl/passthrough.vert.glsl", ":/glsl/postFogIntensityMap.frag.glsl");
    m_postFog.create(":/glsl/passthrough.vert.glsl", ":/glsl/postFog.frag.glsl");
    m_postVL.create(":/glsl/passthrough.vert.glsl", ":/glsl/postVL.frag.glsl");
    m_postSky.create(":/glsl/passthrough.vert.glsl", ":/glsl/postSky.frag.glsl");

    m_progDebug.create(":/glsl/debug.vert.glsl", ":/glsl/debug.frag.glsl");
    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.

    glBindVertexArray(vao);
    m_terrain.CreateTestScene();

    printHardwareDebugInfo();
    printGLErrorLog();
    onStateComplete(MGameState::GameInitStateEnums::GL_INIT);
}



void MyGL::initializeGBuffer()
{
    //  TODO: Optimize pipeline

    m_renderOutput = mkU<FrameBuffer>(this, width(), height());
    m_renderOutput->genBuffer();
    m_renderOutput->createDepthBuffer();

    m_renderOutputTexture = mkU<Texture>(this);
    m_renderOutputTexture->create(width(), height(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);
    m_renderOutput->bindTexture(m_renderOutputTexture->getTextureHandle(), 0);
    m_renderOutput->create();


    m_GBufferObject.gBuffer = mkU<FrameBuffer>(this, width(), height());
    m_GBufferObject.gBuffer->genBuffer();
    m_GBufferObject.gBuffer->createDepthBuffer();

    m_GBufferObject.gTexAlbedo = mkU<Texture>(this);
    m_GBufferObject.gTexAlbedo->create(width(), height(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
    m_GBufferObject.gBuffer->bindTexture(m_GBufferObject.gTexAlbedo->getTextureHandle(), 0);

    m_GBufferObject.gTexPosition = mkU<Texture>(this);

    m_GBufferObject.gTexPosition->create(width(), height(), GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST);

    m_GBufferObject.gBuffer->bindTexture(m_GBufferObject.gTexPosition->getTextureHandle(), 1);

    m_GBufferObject.gTexVelocity = mkU<Texture>(this);
    m_GBufferObject.gTexVelocity->create(width(), height(), GL_RG, GL_RG, GL_UNSIGNED_BYTE, GL_LINEAR);
    m_GBufferObject.gBuffer->bindTexture(m_GBufferObject.gTexVelocity->getTextureHandle(), 2);

    m_GBufferObject.gTexViewNormal = mkU<Texture>(this);
    m_GBufferObject.gTexViewNormal->create(width(), height(), GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR);
    m_GBufferObject.gBuffer->bindTexture(m_GBufferObject.gTexViewNormal->getTextureHandle(), 3);

    m_GBufferObject.gTexNormal = mkU<Texture>(this);
    m_GBufferObject.gTexNormal->create(width(), height(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
    m_GBufferObject.gBuffer->bindTexture(m_GBufferObject.gTexNormal->getTextureHandle(), 4);

/// Other necessary g-buffer attributes goes below here

    m_GBufferObject.gBuffer->create();
    printGLErrorLog();
}


void MyGL::initializeSSAOBuffer()
{
    m_ssaoOutput = mkU<FrameBuffer>(this, width(), height());
    m_ssaoOutput->genBuffer();

    m_ssaoMap = mkU<Texture>(this);
    m_ssaoMap->create(width(), height(), GL_RED, GL_RED, GL_UNSIGNED_BYTE, GL_NEAREST);
    m_ssaoOutput->bindTexture(m_ssaoMap->getTextureHandle(), 0);

    m_ssaoBlurMap = mkU<Texture>(this);
    m_ssaoBlurMap->create(width(), height(), GL_RED, GL_RED, GL_UNSIGNED_BYTE, GL_NEAREST);
    //m_ssaoBlurOutPut->bindTexture(m_ssaoBlurMap->getTextureHandle(), 0);

    m_ssaoOutput->create();
    printGLErrorLog();
}

void MyGL::printHardwareDebugInfo()
{
    // Print-out max-attachment information
    GLint maxDrawBuffers;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
    std::cout << "The hardware max-draw-buffers is: " << maxDrawBuffers << std::endl;
}

void MyGL::redirectFbOutput(FrameBuffer *fb, Texture *tex,
                            unsigned int assignPosition,
                            bool doClear)
{
    fb->bindFrameBuffer();
    fb->updateBindTexture(tex->getTextureHandle(), assignPosition);
    if(doClear){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void MyGL::dynamicEnableFeature(const InputBundle & inp)
{
    if(!isGamePlayStart()){
        return;
    }
    /// Change value here to debug different feature
    if(inp.EnableShaderFeature1){
        startAutomaticFeatureDisplay();
        //switchFeature(ShaderFeatureEnums::PassShadow);
    }

    if(inp.EnableShaderFeature2){
        switchFeature(ShaderFeatureEnums::PostVolumeLight);
    }

    if(inp.EnableShaderFeature3){
        switchFeature(ShaderFeatureEnums::PostSSAO);
    }


    if(inp.RenderTimeSpeedUp){
        MGlobalInstance::Get->scaleRenderTimeUp();
    }

    if(inp.RenderTimeSpeedDown){
        MGlobalInstance::Get->scaleRenderTimeDown();
    }

    if(inp.RenderTimeSpeedReverse){
        MGlobalInstance::Get->scaleRenderTimeReverse();
    }
}

void MyGL::initializePostProcessBuffer()
{
    /// Post-process buffer
    m_intermediateOutputTexture = mkU<Texture>(this);
    m_intermediateOutputTexture->create(width(), height(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);

    m_postOutput = mkU<FrameBuffer>(this, width(), height());
    m_postOutput->genBuffer();
    m_postOutput->bindTexture(m_intermediateOutputTexture->getTextureHandle(), 0);
    m_postOutput->create();
    printGLErrorLog();
}


void MyGL::awake()
{
    MGlobalInstance::Init(this->m_player.mcr_camera);
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(7);
    setFocusPolicy(Qt::ClickFocus);
}

void MyGL::start()
{
    MGlobalInstance::Get->start();
    m_player.start();

    registerCallbacks();

    onStateComplete(MGameState::GameInitStateEnums::ENGINE_START);
}

void MyGL::startGame()
{
    enableLoadingView(false);
    m_player.onGameStart();
    m_inputHandler.onGameStart();
    MGlobalInstance::Get->resetTime();
    onStateComplete(MGameState::GameInitStateEnums::GAME_START);

    return;

    /// Disable features manually here
    std::vector<ShaderFeatureEnums> disableFeatures{
        PostMotionBlur
        , PostFogSSC
        , PostDistFog
        , PostHeightFog
        , PostSSAO
        , PostVolumeLight
        , PassShadow
        , PassWaterReflection
        , PassWaterFresnel
        , PlantWave
    };

    /// Turn off the features
    /// They will be turn on in future updates
    for(auto feature : disableFeatures){
        switchFeature(feature, false);
    }
}

void MyGL::updateEngine()
{
    MGlobalInstance::Get->startProfiler("\tLogic");

    InputBundle userInput = this->m_inputHandler.getInputs();
    dynamicEnableFeature(userInput);
    this->m_player.tick(MGlobalInstance::Get->deltaTime(), userInput);
    m_terrain.updatePlayerPos(m_player.mcr_position);
    m_terrain.loadNewChunkFromPlayer();

    MGlobalInstance::Get->printProfiler("\tLogic");
}

void MyGL::updateGui()
{

    MGlobalInstance::Get->startProfiler("\tDraw");

    updateFeatureDisplay();
    // Refresh Screen & Draw
    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data

    MGlobalInstance::Get->printProfiler("\tDraw");
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    MGlobalInstance::Get->startProfiler("Update");

    // Update all global instances (calculating delta time)
    // Do nothing if tick triggered twice in one frame
    MGlobalInstance::Get->update();
    if(MGlobalInstance::Get->deltaTime() == 0){
        return;
    }

    // Perform an error check every tick to get unintended GL error in time
    printGLErrorLog();

    cameraLastViewProj = m_player.mcr_camera.getViewProj();

    // Logic layer
    updateEngine();

    // Draw layer
    updateGui();

    // Post update
    postTick();

    MGlobalInstance::Get->printProfiler("Update");
}

void MyGL::postTick()
{
    MGlobalInstance::Get->startProfiler("\tPostTick");
    m_terrain.postUpdate();
    MGlobalInstance::Get->printProfiler("\tPostTick");
}

void MyGL::onStateComplete(MGameState::GameInitStateEnums initState)
{
    std::cout << "State " << (int)initState << " flagged" << std::endl;
    m_initStateFlag = m_initStateFlag | initState;

    // Once all state is fulfilled, start
    if(isEngineReady() && !isEngineStart()){
        start();
        std::cout << "Engine Start..." << std::endl;
    }
    if(isReadyToStartPlay() && !isGamePlayStart()){
        startGame();
        std::cout << "Game Start..." << std::endl;
    }


    // Set loading text to next stage
    switch (initState){
    case GameInitState::GAME_MGR_CONSTRUCT:
        setLoadingTxt(" -- Initing GL...");
        break;
    case GameInitState::GL_INIT:
        setLoadingTxt(" -- Engine ready. Planting trees...");
        break;
    case GameInitState::TERRAIN_GENERATE:
        setLoadingTxt("");
        break;
    default:
        break;
    }
}

bool MyGL::isEngineReady()
{
    return isUnderState<MGameState::GameInitStateEnums>(
        m_initStateFlag,
        MGameState::GameInitStateEnums::INIT_COMPLETE);
}

bool MyGL::isEngineStart()
{
    return isUnderState<MGameState::GameInitStateEnums>(
        m_initStateFlag,
        MGameState::GameInitStateEnums::ENGINE_START);
}

bool MyGL::isReadyToStartPlay()
{
    return isUnderState<MGameState::GameInitStateEnums>(
        m_initStateFlag,
        MGameState::GameInitStateEnums::READY_FOR_PLAY);
}

bool MyGL::isGamePlayStart()
{
    return isUnderState<MGameState::GameInitStateEnums>(
        m_initStateFlag,
        MGameState::GameInitStateEnums::GAME_START);
}

void MyGL::registerCallbacks()
{
    /// Register a callback to terrain for its generation
    /// This callback will be triggered
    /// once the nearby chunks of the player is ready
    ///
    /// We can hence set the height of the player based on this
    std::function<void()> terrainCompleteCallBack = std::bind(&MyGL::onTerrainGenerateComplete, this);
    m_terrain.RegisterTerrainTaskCompleteAction(terrainCompleteCallBack);


    /// Register callback to invoke extra post-possess when
    /// the camera is got overlayed by some cube
    std::function<void(BlockType)> playerCameraCallBack =
        std::bind(&MyGL::onPlayerCameraOverlayTypeChanged, this, std::placeholders::_1);
    m_player.registerCameraOverlayTypeChangeAction(playerCameraCallBack);
}

void MyGL::onTerrainGenerateComplete()
{
    // Disable the terrain callback
    this->m_terrain.UnregisterTerrainTaskCompleteAction();
    this->onStateComplete(MGameState::GameInitStateEnums::TERRAIN_GENERATE);
}

void MyGL::onPlayerCameraOverlayTypeChanged(BlockType blockType)
{
    cameraPostFilter = blockType;
    //std::cout << "Overlay changed to: " << (int)blockType << std::endl;
}

void MyGL::moveMouseToCenter()
{
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

Player *MyGL::getPlayer()
{
    return &m_player;
}

Terrain *MyGL::getTerrain()
{
    return &m_terrain;
}

void MyGL::sendPlayerDataToGUI() const
{
    //std::cout << "the height is" << TerrainGenerator::getHeight(m_player.get_pos().x, m_player.get_pos().z) << std::endl;
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));

    emit sig_sendFPS(QString::fromStdString(std::to_string(MGlobalInstance::Get->framePerScreen())));
    emit sig_sendAveFPS(QString::fromStdString(std::to_string(MGlobalInstance::Get->aveFramePerScreen())));
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    // Clear the screen so that we only see newly drawn images
    ShaderProgram::DrawCalls = 0;


    generateShadowMap();
    renderTerrain();                // Geometry G-Buffer generation
    renderSSAOPass();
    renderVolumeLightPass();
    renderSkyPass();
    renderCompositePass();
    renderWaterPass();
    renderPostProcess();            // Render into post-process buffer; Under-develeping
    renderCameraEnvFilter();        // Render into screen buffer
    renderHeadUpDisplay();


    //
    //glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    //m_progFlat.setModelMatrix(glm::mat4());
    //m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    //m_progFlat.draw(m_worldAxes);
    //glEnable(GL_DEPTH_TEST);
    //std::cout << "Draw Calls: " << ShaderProgram::DrawCalls << std::endl;
}

void MyGL::generateShadowMap()
{
    auto lightMats = MGlobalInstance::Get->Frustum.getCascadedLightViewProj();
    m_progVolumeLight.setLight(lightMats, &MGlobalInstance::Get->Frustum);

    glViewport(0, 0, SHADOW_MAP_RES, SHADOW_MAP_RES);
    csm.progLightDepth.setModelMatrix(glm::mat4(1.f));
    for (int i = 0; i < 3; ++i)
    {
        csm.progLightDepth.setViewProjMatrix(lightMats[i]);

        csm.bindLightDepthBuffer(i);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_albedoMap->bind(0);
        m_terrain.drawShadow(&csm.progLightDepth);
    }

    //glCullFace(GL_BACK);

    glm::ivec2 dims = m_player.mcr_camera.getDims() / 2;
    glViewport(0,0,dims.x * devicePixelRatio(), dims.y * devicePixelRatio());

    csm.progShadowMap.setLight(lightMats, &MGlobalInstance::Get->Frustum);
    csm.progShadowMap.setModelMatrix(glm::mat4(1.f));
    csm.progShadowMap.setViewProjMatrix(m_player.mcr_camera.getViewProj());

    //csm.bindShadowFBO();
    m_tmpHalfResBuffer->bindFrameBuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    csm.bindLightDepthTexture(0);
    m_albedoMap->bind(1);
    m_terrain.drawShadow(&csm.progShadowMap);

    glViewport(0,0,this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());

    csm.bindShadowFBO();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_tmpHalfResMap->bind(0);
    m_postBilateralFilter.setUnifSpatialPara(2);
    m_postBilateralFilter.setUnifRangePara(4);
    //m_progUpsample.draw(m_geomQuad, 0);
    m_postBilateralFilter.draw(m_geomQuad, 0);

}

void MyGL::renderVolumeLightPass()
{
    m_progVolumeLight.setModelMatrix(glm::mat4(1.f));
    m_progVolumeLight.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progVolumeLight.setTime(MGlobalInstance::Get->currentRenderMSecsTime());
    m_progVolumeLight.setCamera(&m_player.mcr_camera);

    m_volumeLightBuffer->bindFrameBuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    csm.bindLightDepthTexture(0);
    m_GBufferObject.gTexPosition->bind(1);
    m_progVolumeLight.drawFlat(m_geomQuad, 0);

}

void MyGL::renderSkyPass()
{
    glDisable(GL_DEPTH_TEST);
    m_skyOutput->bindFrameBuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 dims = m_player.mcr_camera.getDims() / 2;
    glViewport(0,0,dims.x * devicePixelRatio(), dims.y * devicePixelRatio());
    m_skyShader.setCamera(&m_player.mcr_camera);
    m_skyShader.setDimensions(dims);
    m_skyShader.setTime(MGlobalInstance::Get->currentRenderMSecsTime());
    m_skyShader.draw(m_geomQuad);

    redirectFbOutput(m_postOutput.get(), m_skyMap.get(), 0);
    glViewport(0,0,this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    m_progUpsample.setDimensions(dims);
    m_skyHalfMap->bind(0);
    m_progUpsample.draw(m_geomQuad, 0);

    redirectFbOutput(m_postOutput.get(), m_pureSkyMap.get(), 0);
    m_progUpsample.setDimensions(dims);
    m_pureSkyHalfMap->bind(0);
    m_progUpsample.draw(m_geomQuad, 0);
}

void MyGL::renderSSAOPass()
{

    glDisable(GL_DEPTH_TEST);

    /// Generate SSAO map before blur
    redirectFbOutput(m_ssaoOutput.get(), m_ssaoMap.get());
    renderPostSSAO();

    /// Blur SSAO map
    redirectFbOutput(m_ssaoOutput.get(), m_ssaoBlurMap.get());
    renderBlurSSAO();

    glEnable(GL_DEPTH_TEST);
}


void MyGL::renderPostSSAO()
{
    m_postSSAOShader.setUnifProjMatrix(m_player.mcr_camera.getProj());
    m_postSSAOShader.setUnifViewMatrix(m_player.mcr_camera.getView());

    /// Generate random point on-demand
    if(ssaoSamplePoints.empty()){
        ssaoSamplePoints = RenderUtils::generateSSAOSamplePoints(64);
        ssaoSampleNoise = RenderUtils::generateSSAONoise(16);
    }

    m_postSSAOShader.setUnifEnableShader(getFeature(PostSSAO));
    m_postSSAOShader.setUnifSamplePoints(ssaoSamplePoints);
    m_postSSAOShader.setUnifNoise(ssaoSampleNoise);

    m_GBufferObject.gTexAlbedo->bind(0);
    m_GBufferObject.gTexViewNormal->bind(1);
    m_GBufferObject.gTexPosition->bind(2);
    m_postSSAOShader.setUnifViewNormTex(1);
    m_postSSAOShader.setUnifPositionTex(2);

    m_postSSAOShader.draw(m_geomQuad, 0);
}

void MyGL::renderBlurSSAO()
{
    m_ssaoMap->bind(0);
    m_postBilateralFilter.setUnifSpatialPara(2);
    m_postBilateralFilter.setUnifRangePara(5);
    m_postBilateralFilter.draw(m_geomQuad, 0);
}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generatedTerrain
// for more info)
void MyGL::renderTerrain() {
    /// Clear G-Buffer
    m_GBufferObject.gBuffer->bindFrameBuffer();
    //m_renderOutput->bindFrameBuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //auto lightMats = MGlobalInstance::Get->Frustum.getCascadedLightViewProj();
    m_progGBuffer.setUnifEnableShaderFeature1(getFeature(PlantWave));
    m_progGBuffer.setViewMatrix(m_player.mcr_camera.getView());
    m_progGBuffer.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    //m_progGBuffer.setViewProjMatrix(lightMats[0]);
    m_progGBuffer.setViewProjMatrixLastFrame(cameraLastViewProj);
    m_progGBuffer.setModelMatrix(glm::mat4(1.f));
    m_progGBuffer.setTime(MGlobalInstance::Get->currentRenderMSecsTime());

    m_albedoMap->bind(0);
    m_normalMap->bind(1);
    m_terrain.draw(&m_progGBuffer);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //m_terrain.drawTransp(&m_progGBuffer);
    //glDisable(GL_BLEND);
}

void MyGL::renderCompositePass()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    m_renderOutput->bindFrameBuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_GBufferObject.gTexAlbedo->bind(0);
    m_GBufferObject.gTexNormal->bind(1);
    m_GBufferObject.gTexPosition->bind(2);
    csm.bindShadowMap(3);
    m_ssaoBlurMap->bind(4);
    m_pureSkyMap->bind(5);

    m_progComposite.setUnifEnableShaderFeature1(getFeature(PassShadow));
    m_progComposite.setUnifEnableShaderFeature2(getFeature(DayNightAmbient));
    m_progComposite.setUnifEnableShaderFeature3(getFeature(PostDistFog));

    m_progComposite.setViewMatrix(m_player.mcr_camera.getView());
    m_progComposite.setLightDir();
    m_progComposite.setTime(MGlobalInstance::Get->currentRenderMSecsTime());

    m_progComposite.draw(m_geomQuad, 0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void MyGL::renderWaterPass()
{
    // copy composite output to intermediateOutputTexture
    startPostProcessSetUp();    // Switch intermediateOutputTexture as the output of post-process
    m_renderOutputTexture->bind(0);
    m_progPassthrough.draw(m_geomQuad, 0);

    m_renderOutput->bindFrameBuffer();

    // copy depth buffer of gbuffer to render output
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBufferObject.gBuffer->getFrameBufferHandle());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_renderOutput->getFrameBufferHandle());
    glBlitFramebuffer(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio(),
                      0, 0, width() * devicePixelRatio(), height() * devicePixelRatio(),
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    m_intermediateOutputTexture->bind(0);
    m_GBufferObject.gTexPosition->bind(1);
    m_pureSkyMap->bind(2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    m_progWater.setUnifEnableShader(getFeature(PassWater));
    m_progWater.setUnifEnableShaderFeature1(getFeature(PassWaterFresnel));
    m_progWater.setUnifEnableShaderFeature2(getFeature(PassWaterReflection));

    m_progWater.setCamera(&m_player.mcr_camera);
    m_progWater.setLightDir();
    m_progWater.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progWater.setModelMatrix(glm::mat4(1.f));
    m_progWater.setTime(MGlobalInstance::Get->currentRenderMSecsTime());
    m_terrain.drawTransp(&m_progWater);

    glDisable(GL_BLEND);

    //m_intermediateOutputTexture->bind(0);
}


void MyGL::renderPostProcess()
{
    startPostProcessSetUp();

    glDisable(GL_DEPTH_TEST);
    // More Post Process goes below

    renderPostProcessSky();
    renderPostProcessFog();
    renderPostProcessVL();
//  renderPostProcessHatching();
//  renderPostProcessOutline();

//  renderPostProcessBloom();
//  renderPostProcessPosterization();
    renderPostProcessMotionBlur();

    glEnable(GL_DEPTH_TEST);

    onPostProcessPassComplete();
}

void MyGL::startPostProcessSetUp()
{
    redirectFbOutput(m_postOutput.get(), m_intermediateOutputTexture.get());
}

void MyGL::onOnePostProcessPassComplete()
{
    /// Interleaving Update
    std::swap(m_intermediateOutputTexture, m_renderOutputTexture);
    m_postOutput->updateBindTexture(m_intermediateOutputTexture->getTextureHandle());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MyGL::onPostProcessPassComplete()
{
    /// Rebind the output texture to render-output
    /// This is necessary because we swap the content of this texture at the end of every post-process
    m_renderOutput->updateBindTexture(m_renderOutputTexture->getTextureHandle());
}


void MyGL::renderPostProcessMotionBlur()
{
    m_postMotionBlur.setUnifEnableShader(getFeature(PostMotionBlur));
    m_postMotionBlur.setUnifViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_postMotionBlur.setViewProjMatrixLastFrame(cameraLastViewProj);
    m_postMotionBlur.setModelMatrix(glm::mat4(1.f));
    m_renderOutputTexture->bind(0);
    m_GBufferObject.gTexVelocity->bind(1);
    m_postMotionBlur.setUnifVelocityTex(1);
    m_postMotionBlur.draw(m_geomQuad, 0);
    onOnePostProcessPassComplete();
}

void MyGL::renderPostProcessBloom()
{
    m_renderOutputTexture->bind(0);
    m_postBloom.draw(m_geomQuad, 0);
    onOnePostProcessPassComplete();
}

void MyGL::renderPostProcessOutline()
{
    m_postOutline.setUnifViewMatrix(m_player.mcr_camera.getView());
    m_postOutline.setUnifNearFarClip(m_player.mcr_camera.getNearFarClip());

    m_GBufferObject.gTexPosition->bind(1);
    m_postOutline.setUnifPositionTex(1);

    m_GBufferObject.gTexViewNormal->bind(2);
    m_postOutline.setUnifViewNormTex(2);

    m_renderOutputTexture->bind(0);
    m_postOutline.draw(m_geomQuad, 0);
    onOnePostProcessPassComplete();
}

void MyGL::renderPostProcessPosterization()
{
    m_renderOutputTexture->bind(0);
    m_postPosterization.draw(m_geomQuad, 0);
    onOnePostProcessPassComplete();
}

void MyGL::renderPostProcessHatching()
{
    m_postHatching.setTime(MGlobalInstance::Get->currentRenderMSecsTime());
    m_postHatching.setUnifViewMatrix(m_player.mcr_camera.getView());
    m_postHatching.setUnifNearFarClip(m_player.mcr_camera.getNearFarClip());

    m_GBufferObject.gTexPosition->bind(1);
    m_postHatching.setUnifPositionTex(1);

    m_renderOutputTexture->bind(0);
    m_postHatching.draw(m_geomQuad, 0);
    onOnePostProcessPassComplete();
}

void MyGL::renderPostProcessFog()
{
    redirectFbOutput(m_fogOutput.get(), m_fogIntensityMap.get());

    m_postFogIntensity.setUnifViewMatrix(m_player.mcr_camera.getView());
    m_postFogIntensity.setUnifNearFarClip(m_player.mcr_camera.getNearFarClip());
    m_postFogIntensity.setUnifCameraPos(m_player.mcr_camera.getEye());
    m_postFogIntensity.setTime(MGlobalInstance::Get->currentRenderMSecsTime());
    m_postFogIntensity.setUnifEnableShaderFeature1(getFeature(PostDistFog));
    m_postFogIntensity.setUnifEnableShaderFeature2(getFeature(PostHeightFog));


    m_GBufferObject.gTexPosition->bind(1);
    m_postFogIntensity.setUnifPositionTex(1);

    m_renderOutputTexture->bind(0);
    m_postFogIntensity.draw(m_geomQuad, 0);

    redirectFbOutput(m_postOutput.get(), m_intermediateOutputTexture.get());

    m_postFog.setUnifEnableShader(getFeature(PostFog));
    m_postFog.setUnifEnableShaderFeature1(getFeature(PostFogSSC));

    m_fogIntensityMap->bind(1);
    m_postFog.setUnifFogIntensityTex(1);

    this->m_pureSkyMap->bind(2);
    m_postFog.setUnifSkyPureTex(2);

    m_renderOutputTexture->bind(0);
    m_postFog.draw(m_geomQuad, 0);

    onOnePostProcessPassComplete();
}

void MyGL::renderPostProcessVL()
{
    m_postVL.setUnifEnableShader(getFeature(PostVolumeLight));
    m_postVL.setTime(MGlobalInstance::Get->currentRenderMSecsTime());
    m_renderOutputTexture->bind(0);
    m_volumeLightMap->bind(1);
    m_postVL.setUnifPositionTex(1);

    m_postVL.draw(m_geomQuad, 0);
    onOnePostProcessPassComplete();
}

void MyGL::renderPostProcessSky()
{
    m_postSky.setUnifEnableShader(getFeature(PostSky));

    this->m_skyMap->bind(1);
    m_postSky.setUnifSkyTex(1);

    m_renderOutputTexture->bind(0);
    m_postSky.draw(m_geomQuad, 0);
    onOnePostProcessPassComplete();
}

void MyGL::renderCameraEnvFilter()
{
    glDisable(GL_DEPTH_TEST);

    PostProcessingShader* camFilter;
    switch (cameraPostFilter)
    {
    case WATER:
        camFilter = &m_progPostWater;
        break;
    case LAVA:
        camFilter = &m_progPostLava;
        break;
    default:
        camFilter = &m_progPassthrough;
        break;
    }
    camFilter->setDimensions(m_player.mcr_camera.getDims());
    camFilter->setTime(MGlobalInstance::Get->currentRenderMSecsTime());
    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
    m_renderOutputTexture->bind(0);
    camFilter->draw(m_geomQuad);
    glEnable(GL_DEPTH_TEST);
}

void MyGL::renderHeadUpDisplay()
{
    std::optional<HitInfo> target = m_player.getInteractivableCubeHit();
    if(target)
    {
        m_wireFrame.offset = glm::vec4(target.value().hitCubeIndex, 0);
        m_wireFrame.createVBOdata();
        m_progFlat.setModelMatrix(glm::mat4());
        m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
        m_progFlat.draw(m_wireFrame);
    }
}

void MyGL::enableLoadingView(bool val)
{
    emit sig_enableLoadingBanner(val);
}

void MyGL::setLoadingTxt(QString txt)
{
    emit sig_sendLoadingStatus(txt);
}

void MyGL::switchFeature(ShaderFeatureEnums feature)
{
    m_shaderFeatures[feature] = !m_shaderFeatures[feature];
    std::cout << "Feature " << feature << " -> " << m_shaderFeatures[feature] << std::endl;
}

void MyGL::switchFeature(ShaderFeatureEnums feature, bool target)
{
    if(m_shaderFeatures[feature] == target){
        return;
    }
    switchFeature(feature);
}

bool MyGL::getFeature(ShaderFeatureEnums feature) const
{
    return m_shaderFeatures[feature];
}

void MyGL::startAutomaticFeatureDisplay()
{
    m_featureDisplayTimer = 0.0f;

    /// Set up the feature you want to display here
    /// They will be turn on in REVERSE order
    m_featureDisplayList = std::vector<ShaderFeatureEnums>{
        PostMotionBlur
        , PostFogSSC
        , PostDistFog
        , PostHeightFog
        , PostVolumeLight
        , PassShadow
        , PostSSAO
        , PassWaterReflection
        , PassWaterFresnel
        , PassWater
        , DayNightAmbient
        , PostSky
    };

    /// Turn off the features
    /// They will be turn on in future updates
    for(auto feature : m_featureDisplayList){
        switchFeature(feature, false);
    }
}

void MyGL::updateFeatureDisplay()
{
    if(m_featureDisplayList.empty()){
        return;
    }

    /// Set the update inverval as 1s per feature
    const float INTERVAL = 0.5f;
    m_featureDisplayTimer += MGlobalInstance::Get->deltaTime();
    if(m_featureDisplayTimer < INTERVAL){
        return;
    }
    m_featureDisplayTimer = 0;

    switchFeature(m_featureDisplayList.back(), true);
    m_featureDisplayList.pop_back();
}

void MyGL::onGLGetError(GLenum error)
{
    OpenGLContext::onGLGetError(error);

#if 0
    if(isGamePlayStart()){
        OpenGLContext::onGLGetError(error);
    }   // When playing, terminate the program
    else{
        OpenGLContext::onGLGetError(error);
        std::cerr << "An error occurs in prepare state ..." << std::endl;
    }
#endif
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    this->m_inputHandler.onKeyPress(e);
    if (e->key() == Qt::Key_G)
    {
        m_terrain.CreateVLTestScene(m_player.get_pos());
    }
    if (e->key() == Qt::Key_O)
    {
        m_terrain.CreateLakeSword(m_player.get_pos());
    }
}

void MyGL::keyReleaseEvent(QKeyEvent *e) {
    this->m_inputHandler.onKeyRelease(e);
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    this->m_inputHandler.mouseMoveEvent(e);
}

void MyGL::mousePressEvent(QMouseEvent *e) {

    std::optional<HitInfo> target = m_player.getInteractivableCubeHit();
    if(!target){
        return;
    }

    if(e->buttons() == Qt::LeftButton){
        glm::ivec3 targetPos = target.value().hitCubeIndex;
        m_terrain.playerBreakBlock(targetPos.x, targetPos.y, targetPos.z);
    }

    if(e->button() == Qt::RightButton){
        glm::vec3 targetPos = target.value().hitPosition;
        if(m_player.getCollider().isInCubePosition(glm::ivec3(glm::floor(targetPos)))){
            return;
        }
        m_terrain.playerPlaceBlock(glm::floor(targetPos.x),
                                   glm::floor(targetPos.y),
                                   glm::floor(targetPos.z),
                                   BlockType::STONE);
    }

#if 0
    glm::vec3 hp = glm::floor(target.value().hitPosition);
    glm::vec3 hi = glm::vec3(target.value().hitCubeIndex);
    std::cout
        << hp.x << ' '
        << hp.y << ' '
        << hp.z << '\t'
        << hi.x << ' '
        << hi.y << ' '
        << hi.z << std::endl;
#endif
}


void MyGL::createSkyPassBuffers()
{
    m_skyOutput = mkU<FrameBuffer>(this, width() / 2, height() / 2);
    m_skyOutput->genBuffer();
    m_skyHalfMap = mkU<Texture>(this);
    m_skyHalfMap->create(width() / 2, height() / 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);

    m_skyMap = mkU<Texture>(this);
    m_skyMap->create(width(), height(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);

    m_pureSkyHalfMap = mkU<Texture>(this);
    m_pureSkyHalfMap->create(width() / 2, height() / 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);

    m_pureSkyMap = mkU<Texture>(this);
    m_pureSkyMap->create(width(), height(), GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);


    m_skyOutput->bindTexture(m_skyHalfMap->getTextureHandle(), 0);
    m_skyOutput->bindTexture(m_pureSkyHalfMap->getTextureHandle(), 1);
    m_skyOutput->create();
}

void MyGL::createTmpBuffers()
{
    m_tmpHalfResBuffer = mkU<FrameBuffer>(this, width() / 2, height() / 2);
    m_tmpHalfResBuffer->genBuffer();
    m_tmpHalfResBuffer->createDepthBuffer();
    m_tmpHalfResMap = mkU<Texture>(this);
    m_tmpHalfResMap->create(width() / 2, height() / 2, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR);

    m_tmpHalfResBuffer->bindTexture(m_tmpHalfResMap->getTextureHandle(), 0);
    m_tmpHalfResBuffer->create();
}

void MyGL::initVolumeLightPass()
{
    m_volumeLightBuffer = mkU<FrameBuffer>(this, width(), height());
    m_volumeLightBuffer->genBuffer();
    m_volumeLightMap = mkU<Texture>(this);
    m_volumeLightMap->create(width(), height(), GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR);

    m_volumeLightBuffer->bindTexture(m_volumeLightMap->getTextureHandle(), 0);
    m_volumeLightBuffer->create();
}

void MyGL::initFogBuffer()
{
    m_fogOutput = mkU<FrameBuffer>(this, width(), height());
    m_fogOutput->genBuffer();
    m_fogIntensityMap = mkU<Texture>(this);
    m_fogIntensityMap->create(width(), height(), GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR);

    m_fogOutput->bindTexture(m_fogIntensityMap->getTextureHandle(), 0);
    m_fogOutput->create();
}
