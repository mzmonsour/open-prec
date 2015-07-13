#ifndef OPENPREC_CVAR_H
#define OPENPREC_CVAR_H

#include "colordef.h"

#define NOTIFY_COLOR    DEEP_ORANGE
#define MARK_COLOR      DEEP_ORANGE

// Use Msg/ColorMsg for raw prints
#define ColorMsg(...) g_pCVar->ConsoleColorPrintf(__VA_ARGS__)
#define ConNotifyf(X, ...) g_pCVar->ConsoleColorPrintf(NOTIFY_COLOR, "[%s]: " X "\n", OPENPREC_NAME, __VA_ARGS__)
#define ConNotify(X) ConNotifyf("%s", X)
#define ConMarkf(X, ...) g_pCVar->ConsoleColorPrintf(MARK_COLOR, "[%s]: " X "\n", OPENPREC_NAME, __VA_ARGS__)
#define ConMark(X) ConMarkf("%s", X)

bool register_cvars();
void unregister_cvars();

bool register_concommands();
void unregister_concommands();

extern ConVar   prec_sound,
                prec_mode,
                prec_screens;

enum struct PrecMode {
    Off,
    Named,
    Tournament,
    Always
};

void prec_record(const CCommand& args);
void prec_auto_record();

#endif
