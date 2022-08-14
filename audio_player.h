//
// Created by sharu on 14-08-2022.
//

/**
  @file
  audio_player.h

  @brief
  This file provides the definitions for running the audio in the device.

*/

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

typedef enum
{
    PLAYING,
    PAUSED,
    COMPLETED,
    FORCE_STOPPED
}AUDIO_PLAYER_STATE;

typedef struct audio_player_cfg_tag {
    int (*cb_fn)(int);
    int speed;
    int current_file;
    audio_player_status status;
}audio_player_cfg, *paudio_player_cfg;
int audio_player_init(audio_player_cfg *);
int audio_player_play(audio_player_cfg *, char *path);
int audio_player_pause(audio_player_cfg *);
int audio_player_stop(audio_player_cfg *);
int audio_player_speed(audio_player_cfg *, float speed);
int audio_player_deinit(audio_player_cfg *);
int audio_player_status(audio_player_cfg *);

#endif // AUDIO_PLAYER_H
