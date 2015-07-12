#ifndef OPENPREC_MAIN_H
#define OPENPREC_MAIN_H

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

#endif
