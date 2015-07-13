#include "main.h"
#include "cvar.h"

class TournamentStartListener: public IGameEventListener2 {
    public:
    virtual ~TournamentStartListener() {}
    virtual void FireGameEvent(IGameEvent *event) {
        ConNotifyf("%s", "Tournament event triggered");
        if (g_pCVar->FindVar("mp_tournament")->GetInt() != 1) return;
        if (!g_pEngineClient->IsRecordingDemo()) {
            prec_record(CCommand(0, nullptr));
        }
    }
};
static TournamentStartListener g_TournamentStartListener;

class PlayerSpawnListener: public IGameEventListener2 {
    public:
    virtual ~PlayerSpawnListener() {}
    virtual void FireGameEvent(IGameEvent *event) {
        if (static_cast<PrecMode>(prec_mode.GetInt()) == PrecMode::Always && g_pCVar->FindVar("mp_tournament")->GetInt() != 1) {
            if (!g_pEngineClient->IsRecordingDemo()) {
                prec_record(CCommand(0, nullptr));
            }
        }
    }
};
static PlayerSpawnListener g_PlayerSpawnListener;

bool register_eventlisteners() {
    g_pEventManager->AddListener(&g_TournamentStartListener, "teamplay_restart_round", false);
    g_pEventManager->AddListener(&g_PlayerSpawnListener, "localplayer_respawn", false);
    return true;
}

void unregister_eventlisteners() {
    g_pEventManager->RemoveListener(&g_TournamentStartListener);
    g_pEventManager->RemoveListener(&g_PlayerSpawnListener);
}
