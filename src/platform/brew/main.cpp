typedef signed int int32;
typedef unsigned int uint32;
#define _UINT32_DEFINED
#define _INT32_DEFINED
extern "C"
{
#include <AEEAppGen.h>
#include <AEEStdLib.h>
#include <AEEFile.h>
#include <AEEPointerHelpers.h>
#include <AEEMedia.h>
#include <AEEMediaFormats.h>
#include "OpenLara.bid"
#include "brewFileIO.h"
}
#include "core.h"
#include "game.h"
#include <cstddef>

extern const AEEVTBL(ISource) gsCOpenLaraAudioSourceFuncs;

class OpenLara_AudioSource {
public:
    const AEEVTBL(ISource) *m_vtbl;
    uint32 m_refCount;

    OpenLara_AudioSource() : m_refCount(1) {
        m_vtbl = &gsCOpenLaraAudioSourceFuncs;
    }

    ~OpenLara_AudioSource() {

    }

    static uint32 sti_AddRef(ISource *po) {
        return ((OpenLara_AudioSource *) po)->AddRef();
    }

    static uint32 sti_Release(ISource *po) {
        return ((OpenLara_AudioSource *) po)->Release();
    }

    static int sti_QueryInterface(ISource *po, AEEIID cls, void** ppVoid) {
        return ((OpenLara_AudioSource *) po)->QueryInterface(cls, ppVoid);
    }

    static int32 sti_Read(ISource *po, char *pcBuf, int32 cbBuf) {
        return ((OpenLara_AudioSource *) po)->Read(pcBuf, cbBuf);
    }

    static void sti_Readable(ISource *po, AEECallback *pcb) {
        return ((OpenLara_AudioSource *) po)->Readable(pcb);
    }

    uint32 AddRef() {
        return ++m_refCount;
    }

    uint32 Release() {
        uint32 newRc = --m_refCount;
        if(newRc == 0) {
            delete this;
        }
        return newRc;
    }

    int QueryInterface(AEEIID cls, void** ppo) {
        switch (cls) {
            case AEECLSID_QUERYINTERFACE:
            case AEEIID_IMedia:
            case AEECLSID_MEDIAPMD:
                *ppo = (void *)this;
                AddRef();
                return SUCCESS;
            default:
                *ppo = NULL;
                return ECLASSNOTSUPPORT;
        }
    }

    int32 Read(char *pcBuf, int32 cbBuf) {
        Sound::fill((Sound::Frame *) pcBuf, cbBuf / sizeof(Sound::Frame));
        return cbBuf;
    }

    void Readable(AEECallback *pcb) {
        
    }
} gsOLAudSrc;

IMedia *gsOLAudStrm = NULL;

const AEEVTBL(ISource) gsCOpenLaraAudioSourceFuncs = {
    OpenLara_AudioSource::sti_AddRef,
    OpenLara_AudioSource::sti_Release,
    OpenLara_AudioSource::sti_QueryInterface,
    OpenLara_AudioSource::sti_Read,
    OpenLara_AudioSource::sti_Readable,
};

InputKey getInputKey(uint16 key) {
    switch(key) {
        case AVK_STAR   : return ikPhoneStar;
        case AVK_POUND  : return ikPhonePound;

        case AVK_PUNC1  : return ikDot;
        case AVK_PUNC2  : return ikComma;

        case AVK_LSHIFT :
        case AVK_RSHIFT : return ikShift;

        case AVK_LCTRL  :
        case AVK_RCTRL  : return ikCtrl;

        case AVK_LALT   :
        case AVK_RALT   : return ikAlt;

        case AVK_ENTER  : return ikEnter;
        case AVK_SPACE  : return ikSpace;

        // no arrows - they are handled as joykeys

        case AVK_TXPGUP     : return ikPrev;
        case AVK_TXPGDOWN   : return ikNext;
        case AVK_TXHOME     : return ikHome;
        case AVK_TXEND      : return ikEnd;
        case AVK_TXINSERT   : return ikIns;
        case AVK_TXDELETE   : return ikDel;

        // no numpad on BREW
        case AVK_FUNCTION1		 : return ikF1;
        case AVK_FUNCTION2		 : return ikF2;
        case AVK_FUNCTION3		 : return ikF3;
        case AVK_FUNCTION4		 : return ikF4;
        case AVK_FUNCTION5		 : return ikF5;
        case AVK_FUNCTION6		 : return ikF6;
        case AVK_FUNCTION7		 : return ikF7;
        case AVK_FUNCTION8		 : return ikF8;
        case AVK_FUNCTION9		 : return ikF9;
        case AVK_FUNCTION10		 : return ikF10;
        case AVK_FUNCTION11		 : return ikF11;
        case AVK_FUNCTION12		 : return ikF12;

        case AVK_0		 : return ik0;
        case AVK_1		 : return ik1;
        case AVK_2		 : return ik2;
        case AVK_3		 : return ik3;
        case AVK_4		 : return ik4;
        case AVK_5		 : return ik5;
        case AVK_6		 : return ik6;
        case AVK_7		 : return ik7;
        case AVK_8		 : return ik8;
        case AVK_9		 : return ik9;

        case AVK_A		 : return ikA;
        case AVK_B		 : return ikB;
        case AVK_C		 : return ikC;
        case AVK_D		 : return ikD;
        case AVK_E		 : return ikE;
        case AVK_F		 : return ikF;
        case AVK_G		 : return ikG;
        case AVK_H		 : return ikH;
        case AVK_I		 : return ikI;
        case AVK_J		 : return ikJ;
        case AVK_K		 : return ikK;
        case AVK_L		 : return ikL;
        case AVK_M		 : return ikM;
        case AVK_N		 : return ikN;
        case AVK_O		 : return ikO;
        case AVK_P		 : return ikP;
        case AVK_Q		 : return ikQ;
        case AVK_R		 : return ikR;
        case AVK_S		 : return ikS;
        case AVK_T		 : return ikT;
        case AVK_U		 : return ikU;
        case AVK_V		 : return ikV;
        case AVK_W		 : return ikW;
        case AVK_X		 : return ikX;
        case AVK_Y		 : return ikY;
        case AVK_Z		 : return ikZ;

        default: return ikNone;
    }
}

JoyKey getJoyKey(uint16 key) {
    switch (key) {
        case AVK_CLR    : return jkSelect;
        case AVK_SELECT : return jkStart;

        case AVK_GP_1   : return jkA;
        case AVK_GP_2   : return jkB;
        case AVK_GP_3   : return jkX;
        case AVK_GP_4   : return jkY;
        case AVK_GP_5   : return jkLT;
        case AVK_GP_6   : return jkRT;
        case AVK_GP_SL  : return jkLB;
        case AVK_GP_SR  : return jkRB;
        case AVK_UP     : return jkUp;
        case AVK_DOWN   : return jkDown;
        case AVK_LEFT   : return jkLeft;
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

static void parseTouchData(uint32 dwParam, int &x, int &y, int &id) {
    char* pszdwParam = (char *) dwParam;
    AEE_POINTER_GET_XY(pszdwParam, &x, &y);
    char ptrIDStr[AEE_POINTER_PTRID_SIZE + 1] {0};
    id = 0;
    if(AEE_POINTER_GET_VALUE(pszdwParam, (char *) "ptrID", ptrIDStr, AEE_POINTER_PTRID_SIZE) == SUCCESS) {
        id = AEE_POINTER_PARSE_HEX_STR(ptrIDStr, AEE_POINTER_PTRID_VALUE_SIZE);
    }
}

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

        // create audio player
        if(SUCCESS == ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MEDIAPCM, (void **) &gsOLAudStrm)) {
            AEEMediaWaveSpec wspec;
            MEMSET(&wspec, 0, sizeof(wspec));
            wspec.wSize = sizeof(wspec);
            wspec.clsMedia = AEECLSID_MEDIAPCM;
            wspec.wChannels = 2;
            wspec.dwSamplesPerSec = 44100;
            wspec.wBitsPerSample = 16;
            wspec.bUnsigned = FALSE;

            AEEMediaDataEx md;
            MEMSET(&md, 0, sizeof(md));
            md.clsData = MMD_ISOURCE;
            md.pData = &gsOLAudSrc;
            md.dwSize = 0;

            md.dwStructSize = sizeof(md);
            md.dwCaps = 0;
            md.bRaw = TRUE;
            md.pSpec = &wspec;
            md.dwSpecSize = sizeof(wspec);
            md.dwBufferSize = 0;

            IMEDIA_SetMediaDataEx(gsOLAudStrm, &md, 1);
            IMEDIA_Play(gsOLAudStrm);
        } else {
            gsOLAudStrm = NULL;
        }

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

        if(Core::isQuit) {
            ISHELL_CloseApplet(pMe->m_pIShell, FALSE);
        } else {
            ISHELL_PostEvent(pMe->m_pIShell, AEECLSID_OPENLARA, EVT_USER, 0, 0);
        }

        return TRUE;
    } else if(eCode == EVT_APP_STOP) {
        laraIsRunning = false;

        if (gsOLAudStrm) {
            IMEDIA_Stop(gsOLAudStrm);
            IMEDIA_Release(gsOLAudStrm);
            gsOLAudStrm = NULL;
        }

        Game::deinit();

        bfio_dispose();

        return TRUE;
    } else if(eCode == EVT_KEY_PRESS) {
        Input::setJoyDown(0, getJoyKey(wParam), true);
        Input::setDown(getInputKey(wParam), true);
        return TRUE;
    } else if(eCode == EVT_KEY_RELEASE) {
        Input::setJoyDown(0, getJoyKey(wParam), false);
        Input::setDown(getInputKey(wParam), false);
        return TRUE;
    }/* else if(eCode == EVT_POINTER_DOWN || eCode == EVT_POINTER_UP || eCode == EVT_POINTER_MOVE) {
        int x, y, id;
        parseTouchData(dwParam, x, y, id);
        InputKey key = Input::getTouch(id);
        if (key == ikNone) return FALSE;
        Input::setPos(key, vec2(x, y));
        if (eCode == EVT_POINTER_DOWN) {
            Input::setDown(key, true);
        } else if(eCode == EVT_POINTER_UP) {
            Input::setDown(key, false);
        }
        return TRUE;
    }*/
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
