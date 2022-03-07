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

JoyKey getJoyKey(uint16 key) {
    switch (key) {
        case AVK_SELECT :
        case AVK_5      : return jkA;
        case AVK_0      : return jkB;
        case AVK_1      : return jkX;
        case AVK_3      : return jkY;
        case AVK_7      : return jkLB;
        case AVK_9      : return jkRB;
        case AVK_STAR   : return jkLT;
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

static boolean OpenLara_HandleEvent(AEEApplet *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    if(eCode == EVT_APP_START) {
        bfio_init(pMe->m_pIShell);

        laraIsRunning = true;

        osStartTime = GETUPTIMEMS();
        
        STRCPY(cacheDir, AEEFS_HOME_DIR);
        STRCPY(saveDir, AEEFS_HOME_DIR);
        STRCPY(contentDir, AEEFS_HOME_DIR);

        IBitmap *pDispBitmap = NULL;
        IDISPLAY_GetDeviceBitmap(pMe->m_pIDisplay, &pDispBitmap);

        IDIB *pDispDib = NULL;  
        IBITMAP_QueryInterface(pDispBitmap, AEEIID_IDIB, (void**) &pDispDib);
        IBITMAP_Release(pDispBitmap);

        Core::width = pDispDib->cx;
        Core::height = pDispDib->cy;

        IDIB_Release(pDispDib);

        Core::defLang = 0; // TODO: get system language

        Game::init((const char *) NULL);

        GAPI::resize();

        ISHELL_PostEvent(pMe->m_pIShell, AEECLSID_OPENLARA, EVT_USER, 0, 0);

        return TRUE;
    } else if(eCode == EVT_USER) {
        if(!laraIsRunning) return TRUE;

        if(Game::update()) {
            AEERect clearRect {0, 0, (int16) Core::width, (int16) Core::height};
            IDISPLAY_FillRect(pMe->m_pIDisplay, &clearRect, 0);

            IBitmap *pDispBitmap = NULL;
            IDISPLAY_GetDeviceBitmap(pMe->m_pIDisplay, &pDispBitmap);

            IDIB *pDispDib = NULL;  
            IBITMAP_QueryInterface(pDispBitmap, AEEIID_IDIB, (void**) &pDispDib);
            IBITMAP_Release(pDispBitmap);

            GAPI::swColor = (uint16*) pDispDib->pBmp;
            Game::render();

            IDIB_Release(pDispDib);
            IDISPLAY_Update(pMe->m_pIDisplay);
        }

        ISHELL_PostEvent(pMe->m_pIShell, AEECLSID_OPENLARA, EVT_USER, 0, 0);

        return TRUE;
    } else if(eCode == EVT_APP_STOP) {
        laraIsRunning = false;

        Game::deinit();

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
