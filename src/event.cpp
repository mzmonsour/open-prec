#include <queue>

#include "main.h"
#include "cvar.h"
#include "sound.h"

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

class GameEndListener: public IGameEventListener2 {
    public:
    virtual ~GameEndListener() {}
    virtual void FireGameEvent(IGameEvent *event) {
        g_roundIsActive = false;
        if (prec_screens.GetInt() == 1
            && g_demoIsInternal
            && g_pEngineClient->IsRecordingDemo()) {
            g_pEngineClient->ClientCmd("+score; screenshot; -score");
        }
    }
};
static GameEndListener g_GameEndListener;

class Killstreak {
    public:
    Killstreak(int tick, long kills) :
        tick(tick),
        kills(kills) {
    }

    const int tick;
    const long kills;
};

static std::queue<Killstreak> g_ksQueue;
static int ks_startTick = 0;
static int ks_lastKill = 0;
static long ks_killCount = 0;

void reset_killstreak_counter() {
    ks_startTick = 0;
    ks_lastKill = 0;
    ks_killCount = 0;
}

static void log_all_streaks() {
    int logtick;
    int logkills;
    if (!g_ksQueue.empty()) {
        play_sound(Sound::Killstreak);
        while (!g_ksQueue.empty()) {
            Killstreak& ks = g_ksQueue.front();
            prec_log_mark(Mark::Killstreak, ks.tick, ks.kills);
            g_ksQueue.pop();
        }
    }
}

static void try_queue_killstreak() {
    int minstreak = prec_min_streak.GetInt();
    if (ks_killCount >= minstreak) {
        g_ksQueue.emplace(ks_startTick, ks_killCount);
    }
    reset_killstreak_counter();
}

class KillfeedListener: public IGameEventListener2 {
    public:
    virtual ~KillfeedListener() {}
    virtual void FireGameEvent(IGameEvent *event) {
        long maxtime = prec_kill_delay.GetFloat() * 1000;
        long curtime = g_pSystem->GetTimeMillis();
        int entindex;
        player_info_t playerinfo;
        if (g_roundIsActive && g_pEngineClient->IsRecordingDemo() && g_demoIsInternal) {
            if ((curtime - ks_lastKill) > maxtime) {
                try_queue_killstreak();
            }
            entindex = g_pEngineClient->GetLocalPlayer();
            if (!g_pEngineClient->GetPlayerInfo(entindex, &playerinfo)) return;
            if (event->GetInt("userid") != playerinfo.userID) {
                if (event->GetInt("attacker") == playerinfo.userID) {
                    if (ks_killCount <= 0) {
                        ks_startTick = g_pEngineClient->GetDemoRecordingTick();
                    }
                    ks_killCount++;
                    ks_lastKill = curtime;
                }
            } else {
                try_queue_killstreak();
                log_all_streaks();
            }
        }
    }
};
static KillfeedListener g_KillfeedListener;

class RoundStartListener: public IGameEventListener2 {
    public:
    virtual ~RoundStartListener() {}
    virtual void FireGameEvent(IGameEvent *event) {
        g_roundIsActive = true;
    }
};
static RoundStartListener g_RoundStartListener;

class RoundEndListener: public IGameEventListener2 {
    public:
    virtual ~RoundEndListener() {}
    virtual void FireGameEvent(IGameEvent *event) {
        g_roundIsActive = false;
        try_queue_killstreak();
        log_all_streaks();
    }
};
static RoundEndListener g_RoundEndListener;

bool register_eventlisteners() {
    g_pEventManager->AddListener(&g_TournamentStartListener, "teamplay_restart_round", false);
    g_pEventManager->AddListener(&g_PlayerSpawnListener, "localplayer_respawn", false);
    g_pEventManager->AddListener(&g_ClassTrackListener, "player_changeclass", false);
    g_pEventManager->AddListener(&g_GameEndListener, "teamplay_game_over", false);
    g_pEventManager->AddListener(&g_GameEndListener, "tf_game_over", false);
    g_pEventManager->AddListener(&g_KillfeedListener, "player_death", false);
    g_pEventManager->AddListener(&g_RoundStartListener, "teamplay_round_start", false);
    g_pEventManager->AddListener(&g_RoundEndListener, "teamplay_round_win", false);
    g_pEventManager->AddListener(&g_RoundEndListener, "teamplay_round_stalemate", false);
    return true;
}

void unregister_eventlisteners() {
    g_pEventManager->RemoveListener(&g_TournamentStartListener);
    g_pEventManager->RemoveListener(&g_PlayerSpawnListener);
    g_pEventManager->RemoveListener(&g_ClassTrackListener);
    g_pEventManager->RemoveListener(&g_GameEndListener);
    g_pEventManager->RemoveListener(&g_KillfeedListener);
    g_pEventManager->RemoveListener(&g_RoundStartListener);
    g_pEventManager->RemoveListener(&g_RoundEndListener);
}
