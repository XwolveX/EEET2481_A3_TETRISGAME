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
File        : ID_SCREEN_PLAY_Slots.c
Purpose     : AppWizard managed file, function content could be changed
---------------------------END-OF-HEADER------------------------------
*/

#include "Application.h"
#include "../Generated/Resource.h"
#include "../Generated/ID_SCREEN_PLAY.h"

/*** Begin of user code area ***/
/*** End of user code area ***/

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       cbID_SCREEN_PLAY
*/
void cbID_SCREEN_PLAY(WM_MESSAGE * pMsg) {
  GUI_USE_PARA(pMsg);
}

/*********************************************************************
*
*       ID_SCREEN_PLAY__WM_NOTIFICATION_VALUE_CHANGED__ID_GAUGE_00__APPW_JOB_SETVALUE
*/
void ID_SCREEN_PLAY__WM_NOTIFICATION_VALUE_CHANGED__ID_GAUGE_00__APPW_JOB_SETVALUE(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
}

/*********************************************************************
*
*       ID_SCREEN_PLAY__WM_NOTIFICATION_VALUE_CHANGED__ID_TEXT_TIME_VALUE__APPW_JOB_SETTEXT
*/
void ID_SCREEN_PLAY__WM_NOTIFICATION_VALUE_CHANGED__ID_TEXT_TIME_VALUE__APPW_JOB_SETTEXT(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
}

/*************************** End of file ****************************/
