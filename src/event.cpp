#include "main.h"
#include "cvar.h"

class TournamentStartListener: public IGameEventListener2 {
    public:
    virtual ~TournamentStartListener() {}
    virtual void FireGameEvent(IGameEvent *event) {
        if (g_pCVar->FindVar("mp_tournament")->GetInt() != 1) return;
        if (!g_pEngineClient->IsRecordingDemo()) {
            prec_auto_record();
        }
    }
};
static TournamentStartListener g_TournamentStartListener;

class PlayerSpawnListener: public IGameEventListener2 {
    public:
    virtual ~PlayerSpawnListener() {}
    virtual void FireGameEvent(IGameEvent *event) {
        if (static_cast<PrecMode>(prec_mode.GetInt()) == PrecMode::Always && g_pCVar->FindVar("mp_tournament")->GetInt() != 1) {
            prec_auto_record();
        }
    }
};
static PlayerSpawnListener g_PlayerSpawnListener;

class ClassTrackListener: public IGameEventListener2 {
    public:
    virtual ~ClassTrackListener() {}
    virtual void FireGameEvent(IGameEvent *event) {
        int entindex = g_pEngineClient->GetLocalPlayer();
        player_info_t playerinfo;
        if (g_pEngineClient->GetPlayerInfo(entindex, &playerinfo)) {
            if (event->GetInt("userid") == playerinfo.userID) {
                switch (event->GetInt("class")) {
                    case 1: g_localPlayerClass = Class::Scout; break;
                    case 2: g_localPlayerClass = Class::Sniper; break;
                    case 3: g_localPlayerClass = Class::Soldier; break;
                    case 4: g_localPlayerClass = Class::Demo; break;
                    case 5: g_localPlayerClass = Class::Medic; break;
                    case 6: g_localPlayerClass = Class::Heavy; break;
                    case 7: g_localPlayerClass = Class::Pyro; break;
                    case 8: g_localPlayerClass = Class::Spy; break;
                    case 9: g_localPlayerClass = Class::Engi; break;
                    default: g_localPlayerClass = Class::Null;
                }
            }
        }
    }
};
static ClassTrackListener g_ClassTrackListener;

bool register_eventlisteners() {
    g_pEventManager->AddListener(&g_TournamentStartListener, "teamplay_restart_round", false);
    g_pEventManager->AddListener(&g_PlayerSpawnListener, "localplayer_respawn", false);
    g_pEventManager->AddListener(&g_ClassTrackListener, "player_changeclass", false);
    return true;
}

void unregister_eventlisteners() {
    g_pEventManager->RemoveListener(&g_TournamentStartListener);
    g_pEventManager->RemoveListener(&g_PlayerSpawnListener);
    g_pEventManager->RemoveListener(&g_ClassTrackListener);
}
