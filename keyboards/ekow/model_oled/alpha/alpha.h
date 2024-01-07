/* Copyright 2022 eugenepy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "quantum.h"

#define XXX KC_NO

#define LAYOUT_f13_80(\
                                                                             K41,\
        K00,    K02,K03,K04,K05, K06,K07,K08,K09,  K0A,K0B,K0C,K0D,  K0E,K0F,K10,\
        K11,K12,K13,K14,K15,K16,K17,K18,K19,K1A,K1B,K1C,K1D,   K1E,  K1F,K20,K21,\
         K22, K23,K24,K25,K26,K27,K28,K29,K2A,K2B,K2C,K2D,K2E, K2F,  K30,K31,K32,\
         K33,  K34,K35,K36,K37,K38,K39,K3A,K3B,K3C,K3D,K3E,  K3F,                \
          K44,   K45,K46,K47,K48,K49,K4A,K4B,K4C,K4D,K4E,   K4F,         K50,    \
         K55, K56, K57,             K59,             K5C, K5D, K60,  K42,K53,K64 \
        )\
{\
        {K00, XXX, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, K0F, K10 }, \
        {K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, K1E, K1F, K20, K21 }, \
        {K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, K2E, K2F, K30, K31, K32 }, \
        {K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K3D, K3E, K3F, XXX, K41, K64, XXX }, \
        {K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, K4D, K4E, K4F, K50, XXX, XXX, K53, XXX }, \
        {K55, K56, K57, XXX, K59, XXX, XXX, K5C, XXX, K5D, XXX, K60, XXX, XXX, XXX, K42, XXX } \
}

#define LAYOUT_f13_ansi(\
                                                                             K41,\
        K00,    K02,K03,K04,K05,  K06,K07,K08,K09,  K0A,K0B,K0C,K0D, K0E,K0F,K10,\
        K11,K12,K13,K14,K15,K16,K17,K18,K19,K1A,K1B,K1C,K1D,    K1E, K1F,K20,K21,\
         K22, K23,K24,K25,K26,K27,K28,K29,K2A,K2B,K2C,K2D,K2E,  K2F, K30,K31,K32,\
         K33,  K34,K35,K36,K37,K38,K39,K3A,K3B,K3C,K3D,K3E,  K3F,                \
          K44,   K45,K46,K47,K48,K49,K4A,K4B,K4C,K4D,K4E,   K4F,         K50,    \
        K55, K56, K57,           K59,           K5C, K5D, K5F, K61,  K42,K53,K64 \
        )\
{\
        {K00, XXX, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, K0F, K10 }, \
        {K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, K1E, K1F, K20, K21 }, \
        {K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, K2E, K2F, K30, K31, K32 }, \
        {K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K3D, K3E, K3F, XXX, K41, K64, XXX }, \
        {K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, K4D, K4E, K4F, K50, XXX, XXX, K53, XXX }, \
        {K55, K56, K57, XXX, K59, XXX, XXX, K5C, K5D, XXX, K5F, XXX, K61, XXX, XXX, K42, XXX } \
}

#define LAYOUT_f13_mix(\
                                                                                  K41,\
        K00,    K02,K03,K04,K05, K06,K07,K08,K09,  K0A,K0B,K0C,K0D,       K0E,K0F,K10,\
        K11,K12,K13,K14,K15,K16,K17,K18,K19,K1A,K1B,K1C,K1D,   K1E,       K1F,K20,K21,\
         K22, K23,K24,K25,K26,K27,K28,K29,K2A,K2B,K2C,K2D,K2E, K2F,       K30,K31,K32,\
         K33,  K34,K35,K36,K37,K38,K39,K3A,K3B,K3C,K3D,K3E,  K3F,                     \
          K44,   K45,K46,K47,K48,K49,K4A,K4B,K4C,K4D,K4E,   K4F,              K50,    \
         K55, K56, K57,             K59,   K5C, K5D, K60, K61, K62, K63,  K42,K53,K64 \
        )\
{\
        {K00, XXX, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, K0F, K10 }, \
        {K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, K1E, K1F, K20, K21 }, \
        {K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, K2E, K2F, K30, K31, K32 }, \
        {K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K3D, K3E, K3F, XXX, K41, K64, XXX }, \
        {K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, K4D, K4E, K4F, K50, XXX, XXX, K53, XXX }, \
        {K55, K56, K57, XXX, K59, XXX, XXX, K5C, K5D, K60, K61, K62, K63, XXX, XXX, K42, XXX } \
}

