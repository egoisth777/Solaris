#ifndef SETTINGS_H
#define SETTINGS_H

#define SHADOW_MAP_RES 4096

#define FAR_CLIP  400.f
#define NEAR_CLIP 0.1f

#define TERRAIN_RENDER_RADIUS   3
#define TERRAIN_LOAD_RADIUS     4

#define THREAD_COUNT std::thread::hardware_concurrency()


#define PLAYER_INIT_POS  glm::vec3(48.f, 130.f, 48.f)

#define BUILD_RIVER
#define BUILD_GIANT_TREE
#define BUILD_L_TREE
#define BUILD_GRASS
#define BUILD_FLOWER
#define BUILD_LAKE

/// Render features
/// Start with ss_
/// which measns static-shader

enum ShaderFeatureEnums : int {

    // Shadow Pass
    PassShadow          = 0,

    // Water Pass
    PassWater           = 1,
    PassWaterFresnel    = 2,
    PassWaterReflection = 3,

    // Geo Pass
    PlantWave           = 4,
    DayNightAmbient     = 5,

    // Post-process
    PostVolumeLight     = 6,
    PostSky             = 7,
    PostSSAO            = 8,
    PostFog             = 9,
    PostDistFog         = 10,
    PostHeightFog       = 11,
    PostFogSSC          = 12,
    PostMotionBlur      = 13
};

#endif // SETTINGS_H
