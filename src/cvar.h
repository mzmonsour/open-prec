#ifndef OPENPREC_CVAR_H
#define OPENPREC_CVAR_H

bool register_cvars();
void unregister_cvars();

bool register_concommands();
void unregister_concommands();

extern ConVar prec_sound;

#endif
