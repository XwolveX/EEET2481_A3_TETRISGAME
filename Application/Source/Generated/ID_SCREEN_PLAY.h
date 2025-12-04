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
File        : ID_SCREEN_PLAY.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef ID_SCREEN_PLAY_H
#define ID_SCREEN_PLAY_H

#include "AppWizard.h"

/*********************************************************************
*
*       Objects
*/
#define ID_IMAGE_00         (GUI_ID_USER + 18)
#define ID_BOX_GAME         (GUI_ID_USER + 1)
#define ID_BOX_NEXTBOX      (GUI_ID_USER + 2)
#define ID_BOX_STATUS       (GUI_ID_USER + 3)
#define ID_BOX_PAUSEINFO    (GUI_ID_USER + 4)
#define ID_TEXT_NCODE       (GUI_ID_USER + 11)
#define ID_TEXT_NCODE_VALUE (GUI_ID_USER + 5)
#define ID_TEXT_TIME        (GUI_ID_USER + 6)
#define ID_TEXT_LEVEL       (GUI_ID_USER + 7)
#define ID_TEXT_SCORE       (GUI_ID_USER + 8)
#define ID_TEXT_NCODE_Copy  (GUI_ID_USER + 9)
#define ID_TEXT_LINE        (GUI_ID_USER + 10)

/*********************************************************************
*
*       Slots
*/

/*********************************************************************
*
*       Callback
*/
void cbID_SCREEN_PLAY(WM_MESSAGE * pMsg);

#endif  // ID_SCREEN_PLAY_H

/*************************** End of file ****************************/
