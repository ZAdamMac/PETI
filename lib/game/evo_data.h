/*
 * evo_data.h
 *
 *  Created on: May 12, 2021
 *      Author: patches
 */

#ifndef LIB_GAME_EVO_DATA_H_
#define LIB_GAME_EVO_DATA_H_


typedef struct Stage{
    int stage_id; // This is just an ID, it doesn't get used but it's there to make the file more readable.
    int phase; // Integer controls which metanim applies from the various metanimation sets. 0 for egg, 1 for baby, 2 for teen, 3 for adult, 4 for senior, 5 for final.
    int size; // 1 for a 1x1 character, 4 for a 2x2 character. If adding a new size the size of animation also needs to be adjusted
    const char animation[2][4];  // array, up to two frames, animating based on cells from the 16x16 font
    char *faceRight; // array, up to one frame, animating based on cells from the 16x16 font
    char *animationEating[2]; //// array, up to two frames, animating based on cells from the 16x16 font
    int rateHF; // Byte controls the hunger and fun degredation rates
    int highEvo; // If the high evolution condition is met, the new stage ID becomes whatever is set for this value.
    int lowEvo;  // If the high evolution condition is not met, we go here.
    int secretEvo; // If there is a secret evolution condition, we go here instead.
    int stageLength; // integer number of days before the next evolution.
} Stage;


#pragma PERSISTENT(EVO_metaStruct)
Stage EVO_metaStruct[] = {
                           {// Begin entry for egg. Arguments are in order isted in stage definition above
                                .stage_id = 0x00,
                                .phase = 0x00,
                                .size = 0x04,
                                .animation = {"\x04\x02\x08\x06",
                                 "\x01\x03\x05\x07"},
                                .faceRight = " ",
                                .animationEating = {" ", " "},
                                .rateHF = 0x00,
                                .highEvo = 0x00,
                                .lowEvo = 0x00,
                                .secretEvo = 0x00,
                                .stageLength = 0x00
                           }
};

#endif /* LIB_GAME_EVO_DATA_H_ */
