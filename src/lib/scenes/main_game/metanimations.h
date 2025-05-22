/*
 * metanimations.h
 *
 *  Created on: May 12, 2021
 *      Author: patches
 */

#ifndef LIB_SCENES_MAIN_GAME_METANIMATIONS_H_
#define LIB_SCENES_MAIN_GAME_METANIMATIONS_H_

typedef struct Metanimation{    // A special structure for defining a metanimation, which is the animation that moves characters around the screen.
    unsigned int phase_id;      // Convenience property, made to make it easier to determine which phase ID to reference when constructing a stage in the game/evo data.
    char d[4][6][9];            // the actual animation itself. Four arrays, each of six eight-character strings, allow you to define up to four frames of animation.
} Metanimation;                 // Refer to the wiki for the construction of these strings.

#define METANIMATION_STATUS_ROOT '@'  // Defines the character used to encode the root position for drawing status icons


#pragma PERSISTENT(SCENES_metanimations)
Metanimation SCENES_metanimations[] = { // Parent array of metanimations exposed to other functions. Should contain one for each phase-of-life.
      {//begin entry for the egg phase
            .phase_id = 0x00,
            .d = {
                  {
                        "--------",
                        "---@----",
                        "---00---",
                        "---00---",
                        "--------",
                        "--------"
                  },
                  {
                        "--------",
                        "---@----",
                        "---00---",
                        "---00---",
                        "--------",
                        "--------"
                  },
                  {
                        "--------",
                        "---@----",
                        "---00---",
                        "---00---",
                        "--------",
                        "--------"
                  },
                  {
                        "--------",
                        "---@----",
                        "---00---",
                        "---00---",
                        "--------",
                        "--------"
                  }
            }
      },
      {//begin entry for the baby phase
            .phase_id = 0x01,
            .d = {
                  {
                        "--------",
                        "--------",
                        "----@---",
                        "----0---",
                        "--------",
                        "--------"
                  },
                  {
                        "--------",
                        "--------",
                        "---@----",
                        "---0----",
                        "--------",
                        "--------"
                  },
                  {
                        "--------",
                        "--------",
                        "---@----",
                        "---1----",
                        "--------",
                        "--------"
                  },
                  {
                        "--------",
                        "--------",
                        "----@---",
                        "----1---",
                        "--------",
                        "--------"
                  }
            }
      },
      {//begin entry for the kid phase
            .phase_id = 0x02,
            .d = {
                  {
                        "--------",
                        "--------",
                        "-@------",
                        "-00-----",
                        "-00-----",
                        "--------"
                  },
                  {
                        "--------",
                        "--------",
                        "---@----",
                        "---00---",
                        "---00---",
                        "--------"
                  },
                  {
                        "--------",
                        "--------",
                        "----@---",
                        "----00--",
                        "----00--",
                        "--------"
                  },
                  {
                        "--------",
                        "--------",
                        "---@----",
                        "---11---",
                        "---11---",
                        "--------"
                  }
            }
      },
      {//begin entry for the teen phase
      .phase_id = 0x03,
      .d = {
            {
                  "--------",
                  "--------",
                  "@-------",
                  "00------",
                  "00------",
                  "--------"
            },
            {
                  "---@----",
                  "---00---",
                  "---00---",
                  "--------",
                  "--------",
                  "--------"
            },
            {
                  "--------",
                  "--------",
                  "------@-",
                  "------11",
                  "------11",
                  "--------"
            },
            {
                  "--------",
                  "---@----",
                  "---11---",
                  "---11---",
                  "--------",
                  "--------"
            }
      }
},
{//begin entry for the adult phase
      .phase_id = 0x04,
      .d = {
            {
                  "-@------",
                  "000-----",
                  "000-----",
                  "000-----",
                  "--------",
                  "--------"
            },
            {
                  "----@---",
                  "---000--",
                  "---000--",
                  "---000--",
                  "--------",
                  "--------"
            },
            {
                  "------@-",
                  "-----111",
                  "-----111",
                  "-----111",
                  "--------",
                  "--------"
            },
            {
                  "---@----",
                  "--111---",
                  "--111---",
                  "--111---",
                  "--------",
                  "--------"
            }
      }
},
{//begin entry for the special sentinel animation
      .phase_id = 0x05,
      .d = {
            {
                  "--------",
                  "----@---",
                  "--000---",
                  "--000---",
                  "--000---",
                  "--------"
            },
            {
                  "--------",
                  "----@---",
                  "--000---",
                  "--000---",
                  "--000---",
                  "--------"
            },
            {
                  "--------",
                  "----@---",
                  "--000---",
                  "--000---",
                  "--000---",
                  "--------"
            },
            {
                  "--------",
                  "----@---",
                  "--000---",
                  "--010---",
                  "--000---",
                  "--------"
            }
      }
}
};


#endif /* LIB_SCENES_MAIN_GAME_METANIMATIONS_H_ */
