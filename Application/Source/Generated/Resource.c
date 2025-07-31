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
File        : Resource.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define _appDrawing      NULL
#define _NumDrawings     0
#define _aScrollerList   NULL
#define _NumScrollers    0
#define _CreateFlags     0

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _apRootList
*/
static APPW_ROOT_INFO * _apRootList[] = {
  &ID_SCREEN_WELCOME_RootInfo,
  &ID_SCREEN_PLAY_RootInfo,
  &ID_GAME_OVER_RootInfo,
  &ID_SCREEN_HIGH_SCORE_RootInfo,
};

/*********************************************************************
*
*       _NumScreens
*/
static unsigned _NumScreens = GUI_COUNTOF(_apRootList);

/*********************************************************************
*
*       _Calc_ID_VAR_01
*/
static GUI_CONST_STORAGE APPW_ATOM _aItem_ID_VAR_01[] = {
  { 0x00000001, 0, ATOM_CONSTANT, 0 },
  { 0x00001000, 0, ATOM_VARIABLE, 0 }
};

static I32 _aCalc_ID_VAR_01[GUI_COUNTOF(_aItem_ID_VAR_01)];

static GUI_CONST_STORAGE APPW_CALC _Calc_ID_VAR_01 = {
  "A+B", 2, _aItem_ID_VAR_01, _aCalc_ID_VAR_01
};

/*********************************************************************
*
*       _Calc_ID_VAR_02
*/
static GUI_CONST_STORAGE APPW_ATOM _aItem_ID_VAR_02[] = {
  { 0xffffffff, 0, ATOM_CONSTANT, 0 },
  { 0x00001000, -1, ATOM_VARIABLE, 0 }
};

static I32 _aCalc_ID_VAR_02[GUI_COUNTOF(_aItem_ID_VAR_02)];

static GUI_CONST_STORAGE APPW_CALC _Calc_ID_VAR_02 = {
  "A+B", 2, _aItem_ID_VAR_02, _aCalc_ID_VAR_02
};

/*********************************************************************
*
*       _aVarList
*/
static APPW_VAR_OBJECT _aVarList[] = {
  { ID_VAR_TIME, 0, 0, NULL },
  { ID_VAR_01, 0, 1, &_Calc_ID_VAR_01 },
  { ID_VAR_02, 0, -1, &_Calc_ID_VAR_02 },
};

/*********************************************************************
*
*       _NumVars
*/
static unsigned _NumVars = GUI_COUNTOF(_aVarList);

/*********************************************************************
*
*       _apLang
*/
static GUI_CONST_STORAGE char * _apLang[] = {
  (GUI_CONST_STORAGE char *)acAPPW_Language_0,
};

/*********************************************************************
*
*       _TextInit
*/
static GUI_CONST_STORAGE APPW_TEXT_INIT _TextInit = {
  (const char **)_apLang,
  GUI_COUNTOF(_apLang),
};

/*********************************************************************
*
*       Private data
*
**********************************************************************
*/
/*********************************************************************
*
*       Font data
*/
APPW_FONT APPW__aFont[2];

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       APPW__GetTextInit
*/
void APPW__GetTextInit(GUI_CONST_STORAGE APPW_TEXT_INIT ** ppTextInit) {
  *ppTextInit = &_TextInit;
}

/*********************************************************************
*
*       APPW__GetResource
*/
void APPW__GetResource(APPW_ROOT_INFO         *** pppRootInfo,    int * pNumScreens,
                       APPW_VAR_OBJECT         ** ppaVarList,     int * pNumVars,
                       const APPW_SCROLLER_DEF ** ppaScrollerDef, int * pNumScrollers,
                       APPW_DRAWING_ITEM      *** pppDrawingList, int * pNumDrawings,
                                                                  int * pCreateFlags) {
  *pppRootInfo    = _apRootList;
  *ppaVarList     = _aVarList;
  *ppaScrollerDef = _aScrollerList;
  *pppDrawingList = (APPW_DRAWING_ITEM **)_appDrawing;
  *pNumScreens    = _NumScreens;
  *pNumVars       = _NumVars;
  *pNumScrollers  = _NumScrollers;
  *pNumDrawings   = _NumDrawings;
  *pCreateFlags   = _CreateFlags;
}

/*************************** End of file ****************************/
