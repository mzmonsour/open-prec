#ifndef OPENPREC_SOUND_H
#define OPENPREC_SOUND_H

enum struct Sound {
    About,
    Recording,
    Killstreak,
};

bool load_sound_table();

void play_sound(Sound sfx);

#endif
