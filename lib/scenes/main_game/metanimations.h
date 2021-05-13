/*
 * metanimations.h
 *
 *  Created on: May 12, 2021
 *      Author: patches
 */

#ifndef LIB_SCENES_MAIN_GAME_METANIMATIONS_H_
#define LIB_SCENES_MAIN_GAME_METANIMATIONS_H_

typedef struct Metanimation{
    unsigned int phase_id;
    char d[4][6][8];
} Metanimation;


#pragma PERSISTENT(SCENES_metanimations)
Metanimation SCENES_metanimations[] = {
                                {//begin entry for the egg phase
                                 .phase_id = 0x00,
                                 .d = {
                                       {
                                        "--------",
                                        "--------",
                                        "---00---",
                                        "---00---",
                                        "--------",
                                        "--------"
                                       },
                                       {
                                        "--------",
                                        "--------",
                                        "---00---",
                                        "---00---",
                                        "--------",
                                        "--------"
                                       },
                                       {
                                        "--------",
                                        "--------",
                                        "---00---",
                                        "---00---",
                                        "--------",
                                        "--------"
                                       },
                                       {
                                        "--------",
                                        "--------",
                                        "---00---",
                                        "---00---",
                                        "--------",
                                        "--------"
                                       }
                                    }
                                }
};


#endif /* LIB_SCENES_MAIN_GAME_METANIMATIONS_H_ */
