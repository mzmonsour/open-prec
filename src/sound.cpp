#include "main.h"
#include "sound.h"
#include "cvar.h"

static const char* g_soundTable[3][10] = {
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Sound::About
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Sound::Recording
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr} // Sound::Killstreak
};

bool load_sound_table() {
    g_soundTable[(int)Sound::About][(int)Class::Null] = "vo/sniper_item_birdhead_kill_scoped02.mp3";
    g_soundTable[(int)Sound::Recording][(int)Class::Null] = "items/cart_explode_trigger";
    g_soundTable[(int)Sound::Killstreak][(int)Class::Null] = "vo/heavy_generic01.mp3";
    return true;
}

void play_sound(Sound sfx) {
    if (prec_sound.GetInt() == 1) {
        const char **table;
        const char *argv[2];
        ConCommand *play;
        table = g_soundTable[(int)sfx];
        argv[0] = "play";
        play = g_pCVar->FindCommand("play");
        if (table[(int)Class::Null] != nullptr) {
            argv[1] = table[(int)Class::Null];
        } else if (table[(int)g_localPlayerClass] != nullptr) {
            argv[1] = table[(int)g_localPlayerClass];
        } else return;
        play->Dispatch(CCommand(2, argv));
    }
}
