typedef signed int int32;
typedef unsigned int uint32;
#define _UINT32_DEFINED
#define _INT32_DEFINED
extern "C"
{
#include <AEEAppGen.h>
#include <AEEStdLib.h>
#include <AEEFile.h>
#include "OpenLara.bid"
#include "brewFileIO.h"
}
#include "core.h"
#include "game.h"
#include <cstddef>
extern "C"
{
#include <gles/egl.h>
#include <GLES_1x.h>
#include <EGL_1x.h>
}

JoyKey getJoyKey(uint16 key) {
    switch (key) {
        case AVK_SELECT :
        case AVK_GP_1   :
        case AVK_5      : return jkA;
        case AVK_GP_2   :
        case AVK_0      : return jkB;
        case AVK_GP_3   :
        case AVK_1      : return jkX;
        case AVK_GP_4   :
        case AVK_3      : return jkY;
        case AVK_7      : return jkLB;
        case AVK_9      : return jkRB;
        case AVK_GP_SL  :
        case AVK_STAR   : return jkLT;
        case AVK_GP_SR  :
        case AVK_POUND  : return jkRT;
        case AVK_SOFT1  : return jkStart;
        case AVK_SOFT2  : return jkSelect;
        case AVK_2 :
        case AVK_UP     : return jkUp;
        case AVK_8 :
        case AVK_DOWN   : return jkDown;
        case AVK_4 :
        case AVK_LEFT   : return jkLeft;
        case AVK_6 :
        case AVK_RIGHT  : return jkRight;
        default         : return jkNone;
    }
}

void* operator new(size_t count) {
    return MALLOC(count | ALLOC_NO_ZMEM);
}

void* operator new[](std::size_t count) {
    return operator new(count);
}

void operator delete(void* ptr) noexcept {
    return FREE(ptr);
}

void operator delete(void* ptr, std::size_t count) noexcept {
    return operator delete(ptr);
}

void operator delete[](void* ptr) noexcept {
    return operator delete(ptr);
}

void operator delete[](void* ptr, std::size_t count) noexcept {
    return operator delete(ptr);
}

void* osMutexInit() {
    return NULL;
}

void osMutexFree(void *obj) {
    
}

void osMutexLock(void *obj) {
    
}

void osMutexUnlock(void *obj) {
    
}

uint32 osStartTime = 0;

int osGetTimeMS() {
    return int(GETUPTIMEMS() - osStartTime);
}

void osJoyVibrate(int index, float L, float R) {
    //
}

static bool laraIsRunning = false;

IDIB *displayWindow;
EGLDisplay display;
EGLSurface surface;
EGLContext context;

bool eglInit(IDIB *window, const char **ppszError) {
    LOG("EGL init context...\n");

    const EGLint eglAttr[] = {
        // EGL_RENDERABLE_TYPE,        EGL_OPENGL_ES_BIT,
        // EGL_SURFACE_TYPE,           EGL_WINDOW_BIT,
        EGL_BLUE_SIZE,              5,
        EGL_GREEN_SIZE,             6,
        EGL_RED_SIZE,               5,
        // EGL_DEPTH_SIZE,             16,
        EGL_NONE
    };

    const EGLint sfcAttr[] = {
        EGL_NONE
    };

    const EGLint ctxAttr[] = {
        EGL_NONE
    };

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        *ppszError = ("eglGetDisplay = EGL_NO_DISPLAY");
        return false;
    }

    if (eglInitialize(display, NULL, NULL) == EGL_FALSE) {
        *ppszError = ("eglInitialize = EGL_FALSE");
        return false;
    }

    EGLConfig config;
    EGLint configCount;

    if (eglChooseConfig(display, eglAttr, &config, 1, &configCount) == EGL_FALSE || configCount == 0) {
        *ppszError = ("eglChooseConfig = EGL_FALSE");
        return false;
    }

    //if(eglGetConfigs(display, &config, 1, &configCount) == EGL_FALSE || configCount == 0) {
    //    *ppszError = ("eglGetConfigs = EGL_FALSE");
    //    return false;
    //}

    surface = eglCreateWindowSurface(display, config, window, sfcAttr);
    if (surface == EGL_NO_SURFACE) {
        *ppszError = ("eglCreateWindowSurface = EGL_NO_SURFACE");
        return false;
    }

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
    if (context == EGL_NO_CONTEXT) {
        *ppszError = ("eglCreateContext = EGL_NO_CONTEXT");
        return false;
    }

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        *ppszError = ("eglMakeCurrent = EGL_FALSE");
        return false;
    }

    *ppszError = NULL;

    return true;
}

void eglFree() {
    LOG("EGL release context\n");
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    eglTerminate(display);
}


static boolean OpenLara_HandleEvent(AEEApplet *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    if(eCode == EVT_APP_START) {
        bfio_init(pMe->m_pIShell);

        if(EGL_Init(pMe->m_pIShell) != SUCCESS) {
            IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, (const AECHAR *) L"EGL_Init failed", -1, 0, 0, NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
            IDISPLAY_Update  (pMe->m_pIDisplay);
            return TRUE;
        }

        IBitmap *pDispBitmap = NULL;
        IDISPLAY_GetDeviceBitmap(pMe->m_pIDisplay, &pDispBitmap);

        IDIB *pDispDib = NULL;  
        IBITMAP_QueryInterface(pDispBitmap, AEEIID_IDIB, (void**) &pDispDib);
        IBITMAP_Release(pDispBitmap);

        displayWindow = pDispDib;

        const char *eglInitError;
        if(!eglInit(displayWindow, &eglInitError)) {
            EGLint eglerr = eglGetError();
            AECHAR errMsg[50];
            AECHAR eglErrorInfo[50];
            STRTOWSTR(eglInitError, errMsg, 50 * 2);
            int fontHeight = IDisplay_GetFontMetrics(pMe->m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL);
            IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, errMsg, -1, 0, 0, NULL, 0);
            WSPRINTF(eglErrorInfo, 50 * sizeof(AECHAR), (const AECHAR *)L"eglGetError() = 0x%08x", eglerr);
            IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, eglErrorInfo, -1, 0, fontHeight, NULL, 0);
            
            IDISPLAY_Update  (pMe->m_pIDisplay);
            return TRUE;
        }

        laraIsRunning = true;

        osStartTime = GETUPTIMEMS();
        
        STRCPY(cacheDir, AEEFS_HOME_DIR);
        STRCPY(saveDir, AEEFS_HOME_DIR);
        STRCPY(contentDir, AEEFS_HOME_DIR);

        GLES_Init(pMe->m_pIShell);

        Core::width = displayWindow->cx;
        Core::height = displayWindow->cy;

        Core::defLang = 0; // TODO: get system language

        Game::init((const char *) NULL);

        ISHELL_PostEvent(pMe->m_pIShell, AEECLSID_OPENLARA, EVT_USER, 0, 0);

        return TRUE;
    } else if(eCode == EVT_USER) {
        if(!laraIsRunning) return TRUE;

        if(Game::update()) {
            //AEERect clearRect {0, 0, (int16) Core::width, (int16) Core::height};
            //IDISPLAY_FillRect(pMe->m_pIDisplay, &clearRect, 0);

            //IBitmap *pDispBitmap = NULL;
            //IDISPLAY_GetDeviceBitmap(pMe->m_pIDisplay, &pDispBitmap);

            //IDIB *pDispDib = NULL;  
            //IBITMAP_QueryInterface(pDispBitmap, AEEIID_IDIB, (void**) &pDispDib);
            //IBITMAP_Release(pDispBitmap);

            Game::render();

            //IDIB_Release(pDispDib);
            //IDISPLAY_Update(pMe->m_pIDisplay);
            eglSwapBuffers(display, surface); 
        }

        ISHELL_PostEvent(pMe->m_pIShell, AEECLSID_OPENLARA, EVT_USER, 0, 0);

        return TRUE;
    } else if(eCode == EVT_APP_STOP) {
        if(laraIsRunning) {
            laraIsRunning = false;

            Game::deinit();
            GLES_Release();
            eglFree();
            EGL_Release();
        }

        IDIB_Release(displayWindow);

        bfio_dispose();

        return TRUE;
    } else if(eCode == EVT_KEY_PRESS) {
        Input::setJoyDown(0, getJoyKey(wParam), true);
        return TRUE;
    } else if(eCode == EVT_KEY_RELEASE) {
        Input::setJoyDown(0, getJoyKey(wParam), false);
        return TRUE;
    }

    return FALSE;
}

int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod, void **ppObj)
{
    *ppObj = NULL;

    if (AEEApplet_New(sizeof(AEEApplet),                // Size of our private class
                      ClsId,                            // Our class ID
                      pIShell,                          // Shell interface
                      pMod,                             // Module instance
                      (IApplet **)ppObj,                // Return object
                      (AEEHANDLER)OpenLara_HandleEvent, // Our event handler
                      NULL))                            // No special "cleanup" function
        return (AEE_SUCCESS);

    return (EFAILED);
}
