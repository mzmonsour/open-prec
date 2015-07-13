#ifndef OPENPREC_MAIN_H
#define OPENPREC_MAIN_H

#include <string>
#include <memory>

// Source Engine Interfaces
#include <interface.h>
#include <eiface.h>
#include <filesystem.h>
#include <igameevents.h>
#include <cdll_int.h>
#include <vgui/ISystem.h>

#define OPENPREC_NAME "open-prec"
#define OPENPREC_DESC "An open source demo recording tool for TF2"
#define OPENPREC_AUTH "Matt Monsour"
#define OPENPREC_VERSION            OPENPREC_VERSION_MAJOR "." OPENPREC_VERSION_MINOR "." OPENPREC_VERSION_REVISION
#define OPENPREC_VERSION_MAJOR      "0"
#define OPENPREC_VERSION_MINOR      "1"
#define OPENPREC_VERSION_REVISION   "0"

#define OPENPREC_NOFEATURE_MSGF "Feature '%s' has not been implemented"

// Engine Interfaces
extern vgui::ISystem*       g_pSystem;
extern IVEngineClient*      g_pEngineClient;
extern IGameEventManager2*  g_pEventManager;
extern IFileSystem*         g_pFileSystem;
extern ICvar*               g_pCVar;

enum struct Class {
    Null,
    Scout,
    Soldier,
    Pyro,
    Demo,
    Heavy,
    Engi,
    Medic,
    Sniper,
    Spy
};

extern Class g_localPlayerClass;

class DemoInfo {
    public:
    DemoInfo(   const char *path, const char *file, const char *tag, const char *date,
                const char *map, const char *blu, const char *red);

    const std::string fullpath;
    const std::string filename;
    const std::string tag;
    const std::string date;
    const std::string mapname;
    const std::string bluteam;
    const std::string redteam;
};

extern std::unique_ptr<DemoInfo>    g_pDemoInfo,
                                    g_pPrevDemoInfo;

extern bool g_demoIsInternal;
extern bool g_roundIsActive;

#endif
