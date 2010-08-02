/*
	This file is part of Warzone 2100.
	Copyright (C) 1999-2004  Eidos Interactive
	Copyright (C) 2005-2009  Warzone Resurrection Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
/** @file
 *  Image ID definitions
 */

#ifndef __INCLUDED_SRC_FREND_H__
#define __INCLUDED_SRC_FREND_H__

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

enum {
	IMAGE_FE_LOGO,
	IMAGE_COM1,
	IMAGE_COM2,
	IMAGE_COM3,
	IMAGE_COM4,
	IMAGE_COM1_HI,
	IMAGE_COM2_HI,
	IMAGE_14400_HI,
	IMAGE_19200_HI,
	IMAGE_56000_HI,
	IMAGE_115200_HI,
	IMAGE_ARENA_HI,
	IMAGE_CAMPAIGN_HI,
	IMAGE_14400,
	IMAGE_19200,
	IMAGE_56000,
	IMAGE_115200,
	IMAGE_ARENA,
	IMAGE_CAMPAIGN,
	IMAGE_REFRESH,
	IMAGE_PLAYER0,
	IMAGE_PLAYER1,
	IMAGE_PLAYER2,
	IMAGE_PLAYER3,
	IMAGE_PLAYER4,
	IMAGE_PLAYER5,
	IMAGE_PLAYER6,
	IMAGE_PLAYER7,
	IMAGE_HOST,
	IMAGE_NO,
	IMAGE_OK,
	IMAGE_QUESTION,
	IMAGE_POWMED,
	IMAGE_POWHI,
	IMAGE_POWLO_HI,
	IMAGE_POWMED_HI,
	IMAGE_POWHI_HI,
	IMAGE_NOALLI,
	IMAGE_OFFALLI,
	IMAGE_ALLI,
	IMAGE_NOALLI_HI,
	IMAGE_OFFALLI_HI,
	IMAGE_ALLI_HI,
	IMAGE_COM3_HI,
	IMAGE_COM4_HI,
	IMAGE_HI34,
	IMAGE_HI31,
	IMAGE_HI23,
	IMAGE_HI39,
	IMAGE_HI41,
	IMAGE_HI64,
	IMAGE_ARENA_OVER,
	IMAGE_CAMPAIGN_OVER,
	IMAGE_MEDAL_GOLD,
	IMAGE_MEDAL_SILVER,
	IMAGE_MEDAL_BRONZE,
	IMAGE_POWLO,
	IMAGE_MULTIRANK1,
	IMAGE_RETURN,
	IMAGE_EDIT_PLAYER,
	IMAGE_EDIT_FORCE,
	IMAGE_EDIT_MAP,
	IMAGE_EDIT_GAME,
	IMAGE_TECHLO,
	IMAGE_TECHMED,
	IMAGE_TECHHI,
	IMAGE_TECHLO_HI,
	IMAGE_TECHMED_HI,
	IMAGE_TECHHI_HI,
	IMAGE_NOBASE,
	IMAGE_SBASE,
	IMAGE_LBASE,
	IMAGE_NOBASE_HI,
	IMAGE_SBASE_HI,
	IMAGE_LBASE_HI,
	IMAGE_NOLIMIT,
	IMAGE_TIMELIMIT,
	IMAGE_FRAGLIMIT,
	IMAGE_NOLIMIT_HI,
	IMAGE_TIMELIMIT_HI,
	IMAGE_FRAGLIMIT_HI,
	IMAGE_RETURN_HI,
	IMAGE_SLIM,
	IMAGE_SLIM_HI,
	IMAGE_LOADFORCE,
	IMAGE_SAVEFORCE,
	IMAGE_CLEARFORCE,
	IMAGE_DEFAULTFORCE,
	IMAGE_HI56,
	IMAGE_COMPUTER_N_HI,
	IMAGE_COMPUTER_N,
	IMAGE_COMPUTER_Y,
	IMAGE_COMPUTER_Y_HI,
	IMAGE_LAMP_RED,
	IMAGE_LAMP_AMBER,
	IMAGE_LAMP_GREEN,
	IMAGE_SKIRMISH_OVER,
	IMAGE_TEAM_OVER,
	IMAGE_SKIRMISH,
	IMAGE_TEAM,
	IMAGE_SKIRMISH_HI,
	IMAGE_TEAM_HI,
	IMAGE_PLAYER_PC,
	IMAGE_MULTIRANK2,
	IMAGE_MEDAL_DUMMY,
	IMAGE_PLAYERX,
	IMAGE_FOG_OFF,
	IMAGE_FOG_ON,
	IMAGE_FOG_OFF_HI,
	IMAGE_FOG_ON_HI,
	IMAGE_WEE_GUY,
	IMAGE_TFONT_189,
	IMAGE_TFONT_225,
	IMAGE_TFONT_224,
	IMAGE_TFONT_188,
	IMAGE_TFONT_226,
	IMAGE_TFONT_227,
	IMAGE_TFONT_228,
	IMAGE_TFONT_229,
	IMAGE_TFONT_230,
	IMAGE_TFONT_199,
	IMAGE_TFONT_232,
	IMAGE_TFONT_233,
	IMAGE_TFONT_234,
	IMAGE_TFONT_235,
	IMAGE_TFONT_236,
	IMAGE_TFONT_237,
	IMAGE_TFONT_238,
	IMAGE_TFONT_239,
	IMAGE_TFONT_241,
	IMAGE_TFONT_209,
	IMAGE_TFONT_242,
	IMAGE_TFONT_243,
	IMAGE_TFONT_244,
	IMAGE_TFONT_246,
	IMAGE_TFONT_248,
	IMAGE_TFONT_249,
	IMAGE_TFONT_250,
	IMAGE_TFONT_251,
	IMAGE_TFONT_252,
	IMAGE_TFONT_253,
	IMAGE_TFONT_128,
	IMAGE_TFONT_198,
	IMAGE_TFONT_196,
	IMAGE_TFONT_197,
	IMAGE_TFONT_201,
	IMAGE_TFONT_214,
	IMAGE_TFONT_220,
	IMAGE_TFONT_216,
	IMAGE_TFONT_215,
	IMAGE_TFONT_131,
	IMAGE_TFONT_176,
	IMAGE_TFONT_191,
	IMAGE_TFONT_161,
	IMAGE_TFONT_174,
	IMAGE_TFONT_172,
	IMAGE_TFONT_171,
	IMAGE_TFONT_187,
	IMAGE_TFONT_193,
	IMAGE_TFONT_194,
	IMAGE_TFONT_192,
	IMAGE_TFONT_195,
	IMAGE_TFONT_240,
	IMAGE_TFONT_208,
	IMAGE_TFONT_203,
	IMAGE_TFONT_200,
	IMAGE_TFONT_212,
	IMAGE_TFONT_210,
	IMAGE_TFONT_213,
	IMAGE_TFONT_218,
	IMAGE_TFONT_219,
	IMAGE_TFONT_217,
	IMAGE_TFONT_221,
	IMAGE_TFONT_245,
	IMAGE_PENCIL,
	IMAGE_NOPENCIL,
	IMAGE_KEYMAP_DEFAULT,
	IMAGE_MULTIRANK3,
	IMAGE_TFONT_223,
	IMAGE_TFONT_163,
	IMAGE_TFONT_170,
	IMAGE_TFONT_202,
	IMAGE_TFONT_211,
	IMAGE_TFONT_206,
	IMAGE_TFONT_207,
	IMAGE_TFONT_204,
	IMAGE_TFONT_205,
	IMAGE_TFONT_63,
	IMAGE_TFONT_45,
	IMAGE_NOJOIN,
	IMAGE_ALLI_TEAMS_HI,
	IMAGE_ALLI_TEAMS,
	IMAGE_TEAM0,
	IMAGE_TEAM1,
	IMAGE_TEAM2,
	IMAGE_TEAM3,
	IMAGE_TEAM4,
	IMAGE_TEAM5,
	IMAGE_TEAM6,
	IMAGE_TEAM7,
	IMAGE_HOST_HI,
	IMAGE_KEYMAP_DEFAULT_HI,
	IMAGE_EDIT_MAP_HI,
	IMAGE_EDIT_GAME_HI,
	IMAGE_EDIT_PLAYER_HI,
	IMAGE_PLAYER0_HI,
	IMAGE_PLAYER1_HI,
	IMAGE_PLAYER2_HI,
	IMAGE_PLAYER3_HI,
	IMAGE_PLAYER4_HI,
	IMAGE_PLAYER5_HI,
	IMAGE_PLAYER6_HI,
	IMAGE_PLAYER7_HI,
	IMAGE_TEAM0_HI,
	IMAGE_TEAM1_HI,
	IMAGE_TEAM2_HI,
	IMAGE_TEAM3_HI,
	IMAGE_TEAM4_HI,
	IMAGE_TEAM5_HI,
	IMAGE_TEAM6_HI,
	IMAGE_TEAM7_HI,
	IMAGE_CHECK_OFF,		// checkbox unchecked
	IMAGE_CHECK_ON,			// checkbox checked
	IMAGE_CHECK_OFF_HI,		// checkbox unchecked highlighted
	IMAGE_CHECK_ON_HI,		// checkbox checked highlighted
	IMAGE_DARK_LOCKED,		// dark backround 'locked'
	IMAGE_DARK_UNLOCKED,	//	 "     "      'unlocked'
	IMAGE_LOCKED_NOBG,		// No background 'locked'
	IMAGE_UNLOCKED_NOBG,	// "       "     'unlocked'
	IMAGE_TRANS_LOCKED,		// clear background 'locked'
	IMAGE_TRANS_UNLOCKED,	//  "       "       'unlocked'
	IMAGE_UNLOCK_BLUE,		// Custom image for the host screen to fix the blend issue
	IMAGE_LOCK_BLUE,		//
	IMAGE_SCAVENGERS_ON,
	IMAGE_SCAVENGERS_OFF,
	IMAGE_SCAVENGERS_ON_HI,
	IMAGE_SCAVENGERS_OFF_HI,
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __INCLUDED_SRC_FREND_H__