/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : Resource.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef RESOURCE_H
#define RESOURCE_H

#include "AppWizard.h"

/*********************************************************************
*
*       Text
*/
#define ID_Text1 0
#define ID_Text2 1
#define ID_TEXT_NCODED 2
#define ID_TEXT_NCODED_VALUE 3
#define ID_RTEXT_0 4
#define ID_RTEXT_1 5
#define ID_RTEXT_2 6
#define ID_RTEXT_3 7
#define ID_RTEXT_4 8
#define ID_TEXT_TIME_VALUE 9

extern GUI_CONST_STORAGE unsigned char acAPPW_Language_0[];

/*********************************************************************
*
*       Font data
*/
extern APPW_FONT APPW__aFont[2];

/*********************************************************************
*
*       Fonts
*/
extern GUI_CONST_STORAGE unsigned char acNettoOT_40_Normal_EXT_AA4[];
extern GUI_CONST_STORAGE unsigned char acNettoOT_32_Normal_EXT_AA4[];

/*********************************************************************
*
*       Images
*/
extern GUI_CONST_STORAGE unsigned char acpixel[];

/*********************************************************************
*
*       Variables
*/
#define ID_VAR_TIME (GUI_ID_USER + 2048)
#define ID_VAR_01 (GUI_ID_USER + 2049)
#define ID_VAR_02 (GUI_ID_USER + 2050)

/*********************************************************************
*
*       Screens
*/
#define ID_SCREEN_WELCOME (GUI_ID_USER + 4096)
#define ID_SCREEN_PLAY (GUI_ID_USER + 4097)
#define ID_GAME_OVER (GUI_ID_USER + 4098)
#define ID_SCREEN_HIGH_SCORE (GUI_ID_USER + 4099)

extern APPW_ROOT_INFO ID_SCREEN_WELCOME_RootInfo;
extern APPW_ROOT_INFO ID_SCREEN_PLAY_RootInfo;
extern APPW_ROOT_INFO ID_GAME_OVER_RootInfo;
extern APPW_ROOT_INFO ID_SCREEN_HIGH_SCORE_RootInfo;

#define APPW_INITIAL_SCREEN &ID_SCREEN_WELCOME_RootInfo

/*********************************************************************
*
*       Project path
*/
#define APPW_PROJECT_PATH "C:/Users/dinhl/AppData/Local/AppWizard/Project/tetrisgame"

#endif  // RESOURCE_H

/*************************** End of file ****************************/
