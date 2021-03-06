// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include "Types.h"
#include "Graphics.h"

struct WindowsNativeState;

class GlDisplayService
{
public:
    GlDisplayService();
    ~GlDisplayService();

    bool TryBindDisplay(const WindowsNativeState& state);
    void ReleaseDisplay(bool destroyEGL);
    void ReleaseMainRenderSurface();

    bool IsDisplayAvailable() const;
    int GetDisplayWidth() const;
    int GetDisplayHeight() const;
    EGLDisplay GetDisplay() const;
    EGLSurface GetSurface() const;
    EGLSurface GetSharedSurface() const;
    EGLContext GetContext() const;
    EGLContext GetResourceBuildSharedContext() const;
    void* GetMainRenderSurfaceSharePointer();
private:
    EGLDisplay m_display;
    EGLSurface m_surface;
    EGLSurface m_sharedSurface;
    EGLContext m_context;
    EGLContext m_resourceBuildSharedContext;
    int m_displayWidth;
    int m_displayHeight;

    bool m_displayBound;
    
};
