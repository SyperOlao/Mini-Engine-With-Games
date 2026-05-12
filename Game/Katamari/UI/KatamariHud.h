#ifndef PINGPONG_KATAMARIHUD_H
#define PINGPONG_KATAMARIHUD_H

struct AppContext;
struct KatamariWorldContext;

struct GBufferPickResult;

class KatamariHud final
{
public:
    static void Draw(
        AppContext &context,
        KatamariWorldContext const &world,
        int displayFps,
        float deltaTimeSeconds,
        bool gBufferDebugVisualizationEnabled,
        bool shadowCascadeDebugVisualizationEnabled,
        bool gBufferPickingInspectorEnabled,
        GBufferPickResult const &lastGBufferPickResult
    );
};

#endif
