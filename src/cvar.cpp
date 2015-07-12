#include <icvar.h>
#include "main.h"
#include "cvar.h"

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

ConVar prec_next_demoname("prec_next_demoname", "", FCVAR_ARCHIVE,
        "Name of the next demo (unimplemented)");

ConVar prec_notify("prec_notify", "1", FCVAR_ARCHIVE,
        "Where to log notifications (started/stopped recording, bookmarks, etc.)\n"
        "\t0 - Log to console (unimplemented)\n"
        "\t1 - Log to team chat (unimplemented)\n"
        "\t2 - Display on HUD (unimplemented)");

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

ConVar prec_dir("prec_dir", "/home/matt/", FCVAR_ARCHIVE,
        "Directory to store demos in (unimplemented)");

ConVar prec_sound("prec_sound", "1", FCVAR_ARCHIVE,
        "Play sounds when starting/stopping recording (unimplemented)\n"
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
        "Tag for demo file name (unimplemented)");

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
}

CON_COMMAND(prec_info, "List commands and cvars") {
}

CON_COMMAND_EXTERN(prec_record, prec_record, "Record a demo") {
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
