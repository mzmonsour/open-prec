#include <icvar.h>
#include <string>
#include <sstream>
#include "main.h"
#include "cvar.h"
#include "sound.h"

/*
 * Define ConVars
 *
 * Note:
 *
 *  Notable constructors of ConVar
 *
        ConVar( const char *pName, const char *pDefaultValue, int flags = 0);

        ConVar( const char *pName, const char *pDefaultValue, int flags, const char *pHelpString );

        ConVar( const char *pName, const char *pDefaultValue, int flags, const char *pHelpString,
            bool bMin, float fMin, bool bMax, float fMax );

        ConVar( const char *pName, const char *pDefaultValue, int flags, const char *pHelpString,
            FnChangeCallback_t callback );

        ConVar( const char *pName, const char *pDefaultValue, int flags,
            const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax,
            FnChangeCallback_t callback );

 *
 *  FCVAR_* flags can be found in tier1/iconvar.h
 *  FCVAR_ARCHIVE will allow cvars to be archived
 *
 *  Definition of FnChangeCallback_t:
 *
        typedef void ( *FnChangeCallback_t )( IConVar *var, const char *pOldValue, float flOldValue );
 *
 */

static ConVar prec_version("prec_version", OPENPREC_VERSION, FCVAR_NONE,
        "The current version of open-prec");

ConVar prec_next_demoname("prec_next_demoname", "", FCVAR_NONE,
        "Name of the next demo");

ConVar prec_notify("prec_notify", "1", FCVAR_ARCHIVE,
        "Where to log notifications (started/stopped recording, bookmarks, etc.)\n"
        "\t0 - Log to console\n"
        "\t1 - Log to team chat\n"
        "\t2 - Display on HUD (unimplemented)");

enum struct PrecNotify {
    Console,
    Chat,
    Hud
};

ConVar prec_mode("prec_mode", "2", FCVAR_ARCHIVE,
        "\t0 - Turn off addon\n"
        "\t1 - Record only custom named demos\n"
        "\t2 - Record on servers with mp_tournament 1\n"
        "\t3 - Always record");

ConVar prec_min_streak("prec_min_streak", "4", FCVAR_ARCHIVE,
        "Minimum killstreak to log (unimplemented)",
        true, 2.0, false, 0);

ConVar prec_kill_delay("prec_kill_delay", "15", FCVAR_ARCHIVE,
        "Max delay between kills in a killstreak (unimplemented)",
        true, 5.0, false, 0);

ConVar prec_dir("prec_dir", "", FCVAR_ARCHIVE,
        "Directory to store demos in");

ConVar prec_sound("prec_sound", "1", FCVAR_ARCHIVE,
        "Play sounds when starting/stopping recording\n"
        "\t0 - Off\n"
        "\t1 - On");

ConVar prec_screens("prec_screens", "0", FCVAR_ARCHIVE,
        "Take screenshots of score and status (unimplemented)\n"
        "\t0 - Off"
        "\t1 - On");

ConVar prec_log("prec_log", "1", FCVAR_ARCHIVE,
        "\t0 - Don't log killstreaks or bookmarks (unimplemented)\n"
        "\t1 - Log killstreaks and bookmarks to killstreaks.txt (unimplemented)\n"
        "\t2 - Log separately for each demo (unimplemented)\n"
        "\t3 - Log separately for each map (unimplemented)");

ConVar prec_delete_useless_demo("prec_delete_useless_demo", "0", FCVAR_ARCHIVE,
        "Delete demo files without killstreaks or bookmarks (unimplemented)\n"
        "\t0 - Off \n"
        "\t1 - On");

ConVar prec_stv_status("prec_stv_status", "0", FCVAR_ARCHIVE,
        "Show SourceTV statistics after match (unimplemented)\n"
        "\t0 - Off\n"
        "\t1 - On");

ConVar prec_tag("prec_tag", "", FCVAR_ARCHIVE,
        "Tag for demo file name");

static const int g_numCvars = 13;
static ConVar *g_cvarList[] = {
    &prec_version,
    &prec_next_demoname,
    &prec_notify,
    &prec_mode,
    &prec_min_streak,
    &prec_kill_delay,
    &prec_dir,
    &prec_sound,
    &prec_screens,
    &prec_log,
    &prec_delete_useless_demo,
    &prec_stv_status,
    &prec_tag
};

/*
 * Define ConCommands
 *
 * Note:
 *
 *  CON_COMMAND(name, ...) expands to the following
 *
 *      static ConCommand name##_command(...) // Variable
 *      static void name(...) { ... } // Function
 *
 *  First argument passed can safely be assumed to be garbage
 *  Probably a legacy thing, because no built in convars read
 *  the first arg anyways
 */

static const int g_numCommands = 5;
extern ConCommand *g_commandList[g_numCommands];

CON_COMMAND(prec_about, "Display useful information about the plugin") {
    Msg("%s\n%s\n%s\n",
        OPENPREC_NAME " is an open-source demo recording tool for TF2",
        "Version: " OPENPREC_VERSION, "Author: " OPENPREC_AUTH
    );
    play_sound(Sound::About);
}

CON_COMMAND(prec_info, "List commands and cvars") {
    ConCommand *help = g_pCVar->FindCommand("help");
    const char *argv[2];
    argv[0] = "help";
    for (int i = 0; i < g_numCommands; ++i) {
        argv[1] = g_commandList[i]->GetName();
        CCommand args(2, argv);
        help->Dispatch(args);
    }
    for (int i = 0; i < g_numCvars; ++i) {
        argv[1] = g_cvarList[i]->GetName();
        CCommand args(2, argv);
        help->Dispatch(args);
    }
}

std::string parse_demoname( const char *fmt, const char *tag, const char *date,
                            const char *map, const char *blu, const char *red) {
    char ch;
    bool escape = false;
    std::ostringstream fmtbuf, demoname;
    std::string fmtword;
    while ((ch = *fmt++) != 0) {
        if (ch == '%') {
            if (escape) {
                fmtword = fmtbuf.str();
                fmtbuf.str(std::string());
                if (fmtword == "tag") {
                    demoname << tag;
                } else if (fmtword == "date") {
                    demoname << date;
                } else if (fmtword == "map") {
                    demoname << map;
                } else if (fmtword == "blu") {
                    demoname << blu;
                } else if (fmtword == "red") {
                    demoname << red;
                }
            }
            escape = !escape;
        } else if (escape) {
            fmtbuf << ch;
        } else {
            demoname << ch;
        }
    }
    return demoname.str();
}

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif
std::string create_demo_path(const char *dir, const char *name) {
    std::ostringstream path;
    // Don't worry about multiple separators, most systems will happily ignore them
    path << dir << PATH_SEPARATOR << name;
    return path.str();
}

#define RECORD_NOTIFICATION "**RECORDING STARTED**"
CON_COMMAND_EXTERN(prec_record, prec_record, "Record a demo") {
    time_t posixtime;
    tm *timeinfo;
    ConCommand *record = g_pCVar->FindCommand("record");
    int mp_tournament = g_pCVar->FindVar("mp_tournament")->GetInt();
    int mode = prec_mode.GetInt();
    const char *argv[2];
    const char *nextdemoname = prec_next_demoname.GetString();
    const char *basedir;
    const char *tag;
    const char *bluteam;
    const char *redteam;
    char mapname[256];
    char datetime[256];
    std::string fallbackname;
    std::string filepath;
    argv[0] = "record";
    switch (static_cast<PrecMode>(mode)) {
        case PrecMode::Off: return;
        case PrecMode::Named: {
            if (strcmp(nextdemoname, "") == 0) return;
        } break;
        case PrecMode::Tournament: {
            if (mp_tournament != 1) return;
        } break;
        case PrecMode::Always: break;
        default: {
            ConNotify("Invalid recording mode, doing nothing");
            return;
        }
    }
    basedir = prec_dir.GetString();
    tag = prec_tag.GetString();
    bluteam = g_pCVar->FindVar("mp_tournament_blueteamname")->GetString();
    redteam = g_pCVar->FindVar("mp_tournament_redteamname")->GetString();
    time(&posixtime);
    timeinfo = localtime(&posixtime);
    strftime(datetime, sizeof(datetime), "%F", timeinfo);
    datetime[sizeof(datetime) - 1] = 0;
    g_pEngineClient->GetChapterName(mapname, sizeof(mapname));
    for (int i = 0; i < sizeof(mapname); ++i) {
        if (mapname[i] == ' ') {
            mapname[i] = 0;
            break;
        }
    }
    if (strcmp(nextdemoname, "") == 0) {
        // TODO: Read custom format string
        const char *demonamefmt = "%date%_%map%_%red%_%blu%_%tag%";
        fallbackname = parse_demoname(demonamefmt, tag, datetime, mapname, bluteam, redteam);
        nextdemoname = fallbackname.c_str();
    }
    filepath = create_demo_path(basedir, nextdemoname);
    argv[1] = filepath.c_str();
    record->Dispatch(CCommand(2, argv));
    if (g_pEngineClient->IsRecordingDemo()) {
        g_demoIsInternal = true;

        // Write demo info
        g_pPrevDemoInfo.swap(g_pDemoInfo);
        g_pDemoInfo = std::unique_ptr<DemoInfo>(
            new DemoInfo(filepath.c_str(), nextdemoname, tag, datetime, mapname, bluteam, redteam));

        // Write notifications
        int notify = prec_notify.GetInt();
        ConNotify(RECORD_NOTIFICATION);
        play_sound(Sound::Recording);
        if (mp_tournament == 1) {
            switch (static_cast<PrecNotify>(notify)) {
                default:
                case PrecNotify::Console:
                    break;
                case PrecNotify::Chat: {
                    g_pEngineClient->ClientCmd("say_team [" OPENPREC_NAME "]: " RECORD_NOTIFICATION);
                } break;
                case PrecNotify::Hud: {
                    ConNotifyf(OPENPREC_NOFEATURE_MSGF, "Hud notifications");
                } break;
            }
        }
    }
}

CON_COMMAND(prec_mark, "Make a bookmark at the current tick") {
}

CON_COMMAND(prec_delete_demo, "Delete previous demo") {
}

ConCommand *g_commandList[g_numCommands] = {
    &prec_about_command,
    &prec_info_command,
    &prec_record_command,
    &prec_mark_command,
    &prec_delete_demo_command
};

bool register_cvars() {
    for (int i = 0; i < g_numCvars; ++i) {
        g_pCVar->RegisterConCommand(g_cvarList[i]);
    }
    return true;
}

void unregister_cvars() {
    for (int i = 0; i < g_numCvars; ++i) {
        g_pCVar->UnregisterConCommand(g_cvarList[i]);
    }
}

bool register_concommands() {
    for (int i = 0; i < g_numCommands; ++i) {
        g_pCVar->RegisterConCommand(g_commandList[i]);
    }
    return true;
}

void unregister_concommands() {
    for (int i = 0; i < g_numCommands; ++i) {
        g_pCVar->UnregisterConCommand(g_commandList[i]);
    }
}
