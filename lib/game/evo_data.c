/*
 * evo_data.h
 *
 *  Created on: May 12, 2021
 *      Author: patches
 */

#include "evo_data.h"

unsigned int EVO_count_stages = 16;

#pragma PERSISTENT(EVO_metaStruct)
extern Stage EVO_metaStruct[] = {
                           {// Begin entry for egg. Arguments are in order listed in stage definition above
                                .stage_id = 0x00,
                                .phase = 0x00,
                                .size = 0x04,
                                .animation = {"\x04\x02\x08\x06",
                                              "\x01\x03\x05\x07"},
                                .faceRight = " ",
                                .animationEating = {" ", " "},
                                .rateHF = 0x00,
                                .highEvo = 0x01,
                                .lowEvo = 0x01,
                                .secretEvo = 0x01,
                                .stageLength = 0x00 // In the case of the egg this value is not actually used
                           },
                           {// Begin entry for the baby. Arguments are in order listed in stage definition above
                               .stage_id = 0x01,
                               .phase = 0x01,
                               .size = 0x01,
                               .animation = {"\x09",
                                             "\x0A"},
                               .faceRight = "\x0B",
                               .animationEating = {"\x0B", "\x0B"},
                               .rateHF = 0xFF,
                               .highEvo = 0x02,
                               .lowEvo = 0x03,
                               .secretEvo = 0x02,
                               .stageLength = 0x01
                          },
                          {// Begin entry for Hungryboi. Arguments are in order listed in stage definition above
                              .stage_id = 0x02,
                              .phase = 0x02,
                              .size = 0x04,
                              .animation = {" \x0F\x0D\x0E",
                                            " \x12\x10\x11"},
                              .faceRight = " \x0F\x0D\x0E",
                              .animationEating = {" \x0F\x0D\x0E", " \x0F\x0D\x13"},
                              .rateHF = 0x00,
                              .highEvo = 0x04,
                              .lowEvo = 0x05,
                              .secretEvo = 0x00,    // TODO Set
                              .stageLength = 0x00   // TODO Set
                         },
                         {// Begin entry for SicklySlim. Arguments are in order listed in stage definition above
                             .stage_id = 0x03,
                             .phase = 0x02,
                             .size = 0x04,
                             .animation = {"  \x14\x15",
                                           "  \x16\x17"},
                             .faceRight = "  \x16\x17",
                             .animationEating = {"  \x14\x15", "  \x16\x18"},
                             .rateHF = 0x00,
                             .highEvo = 0x00,      // TODO Set
                             .lowEvo = 0x00,       // TODO Set
                             .secretEvo = 0x00,    // TODO Set
                             .stageLength = 0x00   // TODO Set
                        },
                        {// Begin entry for Jellyfloat. Arguments are in order listed in stage definition above
                            .stage_id = 0x04,
                            .phase = 0x03,
                            .size = 0x04,
                            .animation = {"\x19\x1A\x1B\x1C",
                                          "\x1D\x1E\x1F\x0C"},
                            .faceRight = "\x19\x1A\x1B\x1C",
                            .animationEating = {"\x19\x1A\x1B\x1C", "\x21\x22\xC6\xC7"},
                            .rateHF = 0x00,
                            .highEvo = 0x00,      // TODO Set
                            .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                            .secretEvo = 0x00,    // TODO Set
                            .stageLength = 0x00   // TODO Set
                       },
                       {// Begin entry for Bughound. Arguments are in order listed in stage definition above
                           .stage_id = 0x05,
                           .phase = 0x03,
                           .size = 0x04,
                           .animation = {"\x24\x25\x23\x26",
                                         "\x28\x25\x27\x29"},
                           .faceRight = " \x25\x2B\x26",
                           .animationEating = {"\x24\x25\x23\x26", "\x28\x25\x23\x2A"},
                           .rateHF = 0x00,
                           .highEvo = 0x00,      // TODO Set
                           .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                           .secretEvo = 0x00,    // TODO Set
                           .stageLength = 0x00   // TODO Set
                      },
                      {// Begin entry for Zazenkuchi. Arguments are in order listed in stage definition above
                          .stage_id = 0x06,
                          .phase = 0x03,
                          .size = 0x04,
                          .animation = {"\x2C\x36\x2D\x37",
                                        "\x2E\x2F\x30\x31"},
                          .faceRight = "\x38\x36\x39\x37",
                          .animationEating = {"\x2C\x36\x2D\x37", "\x32\x33\x34\x35"},
                          .rateHF = 0x00,
                          .highEvo = 0x00,      // TODO Set
                          .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                          .secretEvo = 0x00,    // TODO Set
                          .stageLength = 0x00   // TODO Set
                     },
                     {// Begin entry for Pointyboi. Arguments are in order listed in stage definition above
                         .stage_id = 0x07,
                         .phase = 0x03,
                         .size = 0x04,
                         .animation = {"\x3A\x3B\x3C\x3D",
                                       "\x3E\x3F\x40\x41"},
                         .faceRight = "\x3A\x3B\x46\x47",
                         .animationEating = {"\x3A\x3B\x3C\x3D", "\x42\x43\x44\x45"},
                         .rateHF = 0x00,
                         .highEvo = 0x00,      // TODO Set
                         .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                         .secretEvo = 0x00,    // TODO Set
                         .stageLength = 0x00   // TODO Set
                    },
                    {// Begin entry for Skullcrab. Arguments are in order listed in stage definition above
                        .stage_id = 0x08,
                        .phase = 0x03,
                        .size = 0x04,
                        .animation = {"\x48\x49\x4A\x4B",
                                      "\x48\x49\x4C\x4D"},
                        .faceRight = "\x48\x49\x4A\x4B",
                        .animationEating = {"\x48\x49\x4A\x4B", "\x48\x49\x4E\x4F"},
                        .rateHF = 0x00,
                        .highEvo = 0x00,      // TODO Set
                        .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                        .secretEvo = 0x00,    // TODO Set
                        .stageLength = 0x00   // TODO Set
                   },
                   {// Begin entry for Darumite. Arguments are in order listed in stage definition above
                       .stage_id = 0x09,
                       .phase = 0x04,
                       .size = 0x09,
                       .animation = {"\x50\x51\x52\x53\x54\x55\x56\x5B\x5C",
                                     "\x50\x51\x52\x53\x54\x55\x59\x5A\x58"},
                       .faceRight = "\x5E\xFF\x5F\x53\x54\x55\x56\x57\x58",
                       .animationEating = {"\x50\x51\x52\x53\x54\x55\x56\x57\x58", "\x50\x51\x52\x53\x5D\x55\x56\x57\x58"},
                       .rateHF = 0x00,
                       .highEvo = 0x00,      // TODO Set
                       .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                       .secretEvo = 0x00,    // TODO Set
                       .stageLength = 0x00   // TODO Set
                  },
                  {// Begin entry for Squid. Arguments are in order listed in stage definition above
                      .stage_id = 0x0A,
                      .phase = 0x04,
                      .size = 0x09,
                      .animation = {"\x60\x61\x62\x63\x64\x65\x66\x67\x68",
                                    "\x60\x61\x62\x63\x64\x65\x69\x67\x6A"},
                      .faceRight = "\x60\x61\x62\x6C\x6D\x6E\x66\x67\x6F",
                      .animationEating = {"\x60\x61\x62\x63\x64\x65\x66\x67\x68", "\x60\x61\x62\x63\x6B\x65\x69\x67\x6A"},
                      .rateHF = 0x00,
                      .highEvo = 0x00,      // TODO Set
                      .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                      .secretEvo = 0x00,    // TODO Set
                      .stageLength = 0x00   // TODO Set
                 },
                 {// Begin entry for Cthulhorse. Arguments are in order listed in stage definition above
                     .stage_id = 0x0B,
                     .phase = 0x04,
                     .size = 0x09,
                     .animation = {"\x70\x71\x72\x73 \x74\x75\x76\x77",
                                   "\x70\x71\x72\x73 \x74\x78\x79\x7A"},
                     .faceRight = "\x70\x71\x72\x73 \x74\x75\x76\x77",
                     .animationEating = {"\x70\x71\x72\x73 \x74\x75\x76\x77", "\x70\x71\x72\x73 \x7B\x75\x76\x77"},
                     .rateHF = 0x00,
                     .highEvo = 0x00,      // TODO Set
                     .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                     .secretEvo = 0x00,    // TODO Set
                     .stageLength = 0x00   // TODO Set
                },
                {// Begin entry for Ham-Monster. Arguments are in order listed in stage definition above
                    .stage_id = 0x0C,
                    .phase = 0x04,
                    .size = 0x09,
                    .animation = {"\x80\x81 \x82\x83 \x84\x85 ",
                                  "\x80\x81 \x86\x83 \x87\x85 "},
                    .faceRight = "\x80\x81 \x82\x83 \x84\x85 ",
                    .animationEating = {"\x80\x81 \x82\x83 \x84\x85 ", "\x80\x81 \x88\x89 \x84\x85 "},
                    .rateHF = 0x00,
                    .highEvo = 0x00,      // TODO Set
                    .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                    .secretEvo = 0x00,    // TODO Set
                    .stageLength = 0x00   // TODO Set
               },
               {// Begin entry for Nautalus. Arguments are in order listed in stage definition above
                   .stage_id = 0x0D,
                   .phase = 0x04,
                   .size = 0x09,
                   .animation = {"\x90\x91\x92\x93\x94\x95\x96\x97 ",
                                 "\x90\x98\x99\x93\x9A\x9B\x96\x97 "},
                   .faceRight = "\x90\x91\x92\x93\x94\x95\x96\x97 ",
                   .animationEating = {"\x90\x91\x92\x93\x94\x95\x96\x97 ", "\x90\x98\x99\x93\x9C\x9D \x9E\x9F"},
                   .rateHF = 0x00,
                   .highEvo = 0x00,      // TODO Set
                   .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                   .secretEvo = 0x00,    // TODO Set
                   .stageLength = 0x00   // TODO Set
              },
              {// Begin entry for Ax-a-lot'l. Arguments are in order listed in stage definition above
                  .stage_id = 0x0E,
                  .phase = 0x04,
                  .size = 0x09,
                  .animation = {"\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8",
                                "\xA0\xA1\xA2\xA9\xAA\xA5\xAB\xAC\xA8"},
                  .faceRight = " \xAF\xB0\xB1\xB2\xB3\xB4\xB5\xB6",
                  .animationEating = {"\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8", "\xA0\xA1\xA2\xA9\xAD\xAE\xAB\xAC\xA8"},
                  .rateHF = 0x00,
                  .highEvo = 0x00,      // TODO Set
                  .lowEvo = 0xFF,       // 0xFF is the reserved state value for death, since it's unlikely to ever be addressable.
                  .secretEvo = 0x00,    // TODO Set
                  .stageLength = 0x00   // TODO Set
             }
    };
