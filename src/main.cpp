#include "main.h"
#include "cvar.h"
#include "sound.h"
#include "event.h"

vgui::ISystem*       g_pSystem;
IVEngineClient*      g_pEngineClient;
IGameEventManager2*  g_pEventManager;
IFileSystem*         g_pFileSystem;

Class g_localPlayerClass = Class::Null;

DemoInfo::DemoInfo( const char *path, const char *file, const char *tag, const char *date,
                    const char *map, const char *blu, const char *red) :
    fullpath(path),
    filename(file),
    tag(tag),
    date(date),
    mapname(map),
    bluteam(blu),
    redteam(red),
    hasMarks(false) {
}

void DemoInfo::Mark() {
    hasMarks = true;
}

bool DemoInfo::HasMarks() const {
    return hasMarks;
}

std::unique_ptr<DemoInfo> g_pDemoInfo(nullptr);
std::unique_ptr<DemoInfo> g_pPrevDemoInfo(nullptr);;

bool g_demoIsInternal = false;
bool g_roundIsActive = false;

class PluginImpl: public IServerPluginCallbacks {
    public:

                 PluginImpl();
        virtual ~PluginImpl();

        /*
         * Declare virtual functions from IServerPluginCallbacks
         */

        virtual bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
        virtual void Unload();
        virtual void Pause();
        virtual void UnPause();
        virtual const char* GetPluginDescription();

        virtual void LevelInit(char const *pMapName);
        virtual void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
        virtual void GameFrame(bool simulating);
        virtual void LevelShutdown();
        virtual void OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue);
        virtual void OnEdictAllocated(edict_t *edict);
        virtual void OnEdictFreed(const edict_t *edict);

        virtual PLUGIN_RESULT ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen);
        virtual void ClientPutInServer(edict_t *pEntity, char const *playername);
        virtual void ClientActive(edict_t *pEntity);
        virtual void ClientDisconnect(edict_t *pEntity);
        virtual void SetCommandClient(int index);
        virtual void ClientSettingsChanged(edict_t *pEdict);
        virtual PLUGIN_RESULT ClientCommand(edict_t *pEntity, const CCommand &args);
        virtual PLUGIN_RESULT NetworkIDValidated(const char *pszUserName, const char *pszNetworkID);
};

PluginImpl g_PluginObj;
//EXPOSE_SINGLE_INTERFACE_GL0BALVAR(PluginImpl, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_PluginObj)
// Manual expansion of EXPOSE_SINGLE_INTERFACE_GLOBALVAR
// TODO: Determine why macro causes build to fail under GCC 5.1.0
static void* __CreatePluginImplIServerPluginCallbacks_interface() {return static_cast<IServerPluginCallbacks*>( &g_PluginObj );}
static InterfaceReg __g_CreatePluginImplIServerPluginCallbacks_reg(__CreatePluginImplIServerPluginCallbacks_interface, INTERFACEVERSION_ISERVERPLUGINCALLBACKS);

PluginImpl::PluginImpl() {
    // Do nothing!
}

PluginImpl::~PluginImpl() {
    // Do nothing!
}

bool PluginImpl::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) {
    g_pSystem = static_cast<vgui::ISystem*>(interfaceFactory(VGUI_SYSTEM_INTERFACE_VERSION, NULL));
    if (g_pSystem == nullptr) {
        Msg("Failed to initialize VGUI System interface\n");
        return false;
    }
    g_pEngineClient = static_cast<IVEngineClient*>(interfaceFactory(VENGINE_CLIENT_INTERFACE_VERSION, NULL));
    if (g_pEngineClient == nullptr) {
        Msg("Failed to initialize Client Engine interface\n");
        return false;
    }
    g_pEventManager = static_cast<IGameEventManager2*>(interfaceFactory(INTERFACEVERSION_GAMEEVENTSMANAGER2, NULL));
    if (g_pEventManager == nullptr) {
        Msg("Failed to initialize Game Event interface\n");
        return false;
    }
    g_pFileSystem = static_cast<IFileSystem*>(interfaceFactory(FILESYSTEM_INTERFACE_VERSION, NULL));
    if (g_pFileSystem == nullptr) {
        Msg("Failed to initialize Filesystem interface\n");
        return false;
    }
    g_pCVar = static_cast<ICvar*>(interfaceFactory(CVAR_INTERFACE_VERSION, NULL));
    if (g_pCVar == nullptr) {
        Msg("Failed to initialize Cvar interface\n");
        return false;
    }
    if (!register_cvars()) {
        Msg("Failed to register cvars\n");
        return false;
    }
    if (!register_concommands()) {
        Msg("Failed to register ConCommands\n");
        return false;
    }
    if (!load_sound_table()) {
        Msg("Failed to load sound table\n");
        return false;
    }
    if (!register_eventlisteners()) {
        Msg("Failed to load event listeners\n");
        return false;
    }
    KeyValues *cfg;
    const char *cfg_path = "addons" PATH_SEP_STR "prec.cfg";
    if ((cfg = g_pFileSystem->LoadKeyValues(IFileSystem::TYPE_VMT, cfg_path)) == nullptr) {
        Msg("Failed to load prec.cfg\n");
        return false;
    }
    if (!load_demo_formats(cfg)) {
        Msg("Failed to load demo formatting options\n");
        return false;
    }
    cfg->deleteThis();
    Msg(OPENPREC_NAME " loaded!\n");
    return true;
}

void PluginImpl::Unload() {
    unregister_cvars();
    unregister_concommands();
    unregister_eventlisteners();
}

void PluginImpl::Pause() {}
void PluginImpl::UnPause() {}
const char *PluginImpl::GetPluginDescription() {
    return OPENPREC_NAME " v" OPENPREC_VERSION ": " OPENPREC_DESC ", by " OPENPREC_AUTH;
}

void PluginImpl::LevelInit(char const *pMapName) {
    g_localPlayerClass = Class::Null;
}

void PluginImpl::ServerActivate(edict_t *pEdictList, int edictCount, int clientMax) {}

void PluginImpl::GameFrame(bool simulating) {
    if (simulating && g_demoIsInternal) {
        g_demoIsInternal = g_pEngineClient->IsRecordingDemo();
        if (!g_demoIsInternal && !g_pDemoInfo->HasMarks() && prec_delete_useless_demo.GetInt() == 1) {
            prec_delete_demo(CCommand(0, nullptr));
        }
    }
}

void PluginImpl::LevelShutdown() {
    g_roundIsActive = false;
    if (g_pEngineClient->IsRecordingDemo() && g_demoIsInternal) {
        ConCommand *stop = g_pCVar->FindCommand("stop");
        const char *argv[1] = {"stop"};
        stop->Dispatch(CCommand(1, argv));
        g_demoIsInternal = false;
        if (!g_pDemoInfo->HasMarks() && prec_delete_useless_demo.GetInt() == 1) {
            prec_delete_demo(CCommand(0, nullptr));
        }
    }
}

void PluginImpl::OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue) {}
void PluginImpl::OnEdictAllocated(edict_t *edict) {}
void PluginImpl::OnEdictFreed(const edict_t *edict) {}

PLUGIN_RESULT PluginImpl::ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen) {
    return PLUGIN_CONTINUE;
}

void PluginImpl::ClientPutInServer(edict_t *pEntity, char const *playername) {}
void PluginImpl::ClientActive(edict_t *pEntity) {}
void PluginImpl::ClientDisconnect(edict_t *pEntity) {}
void PluginImpl::SetCommandClient(int index) {}
void PluginImpl::ClientSettingsChanged(edict_t *pEdict) {}

PLUGIN_RESULT PluginImpl::ClientCommand(edict_t *pEntity, const CCommand &args) {
    return PLUGIN_CONTINUE;
}
PLUGIN_RESULT PluginImpl::NetworkIDValidated(const char *pszUserName, const char *pszNetworkID) {
    return PLUGIN_CONTINUE;
}
