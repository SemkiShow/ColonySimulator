// SPDX-FileCopyrightText: 2025 Jaraslau Zaitsau
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Sound.hpp"
#include "Settings.hpp"
#include "UI/GameMenu.hpp"
#include "UI/PauseMenu.hpp"
#include "UI/SettingsMenu.hpp"
#include <cstdlib>
#include <raylib.h>
#include <vector>

const float FADE_TIME = 0.5f;
const float DELAY_BETWEEN_TRACKS = 5.0f;

Sound victorySound;

std::vector<Music> soundtracks;
int soundtrackIdx = -1;
float fadeTimer = 0;
float delayTimer = DELAY_BETWEEN_TRACKS / 2.0f;

const char* soundtrackFiles[] = {
    "resources/sounds/main_theme.ogg",
};

void LoadSounds()
{
    if (!IsAudioDeviceReady()) InitAudioDevice();

    if (!IsSoundValid(victorySound))
        victorySound = LoadSound("resources/sounds/youve_got_mail.mp3");

    if (soundtracks.empty())
    {
        for (const char* file: soundtrackFiles)
        {
            Music track = LoadMusicStream(file);
            if (IsMusicValid(track)) soundtracks.push_back(track);
        }
    }
}

void UpdateSounds()
{
    if (soundtracks.empty()) return;

    if (soundtrackIdx == -1)
    {
        delayTimer += GetFrameTime();
        if (delayTimer >= DELAY_BETWEEN_TRACKS)
        {
            delayTimer = 0.0f;
            soundtrackIdx = rand() % soundtracks.size();
        }
    }

    if ((gameMenu->IsVisible() && !pauseMenu->IsVisible()) ||
        settingsMenu->IsMusicVolumeSliderActive())
    {
        fadeTimer += GetFrameTime();
        if (fadeTimer > FADE_TIME) fadeTimer = FADE_TIME;
    }
    else
    {
        fadeTimer -= GetFrameTime();
        if (fadeTimer < 0.0f) fadeTimer = 0.0f;
    }

    if (soundtrackIdx != -1)
    {
        Music& currentTrack = soundtracks[soundtrackIdx];
        SetMusicVolume(currentTrack, musicVolume * (fadeTimer / FADE_TIME));

        if (fadeTimer <= 0.0f)
        {
            PauseMusicStream(currentTrack);
        }
        else
        {
            if (!IsMusicStreamPlaying(currentTrack))
            {
                PlayMusicStream(currentTrack);
            }
        }

        UpdateMusicStream(currentTrack);

        if (GetMusicTimePlayed(currentTrack) >= GetMusicTimeLength(currentTrack))
        {
            StopMusicStream(currentTrack);
            soundtrackIdx = -1;
        }
    }

    SetSoundVolume(victorySound, sfxVolume * (fadeTimer / FADE_TIME));
}

void FreeSounds()
{
    UnloadSound(victorySound);
    for (Music& track: soundtracks)
    {
        UnloadMusicStream(track);
    }
    soundtracks.clear();

    CloseAudioDevice();
}
