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
File        : ID_SCREEN_WELCOME.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_WELCOME.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _aCreate
*/
static APPW_CREATE_ITEM _aCreate[] = {
  { WM_OBJECT_WINDOW_Create,
    ID_SCREEN_WELCOME, 0,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_00, ID_SCREEN_WELCOME,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00, ID_SCREEN_WELCOME,
    { { { DISPOSE_MODE_REL_PARENT, 23, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 144, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      195, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_01, ID_SCREEN_WELCOME,
    { { { DISPOSE_MODE_REL_PARENT, 70, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 181, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_BOX_00,         APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_TEXT_00,        APPW_SET_PROP_COLOR,        { ARG_V(0xffaa0000) } },
  { ID_TEXT_00,        APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_00,        APPW_SET_PROP_TEXTID,       { ARG_V(ID_Text1) } },
  { ID_TEXT_00,        APPW_SET_PROP_BKCOLOR,      { ARG_V(0xff005500) } },
  { ID_TEXT_00,        APPW_SET_PROP_FONT,         { ARG_VPF(0, acNettoOT_40_Normal_EXT_AA4, &APPW__aFont[0]) } },
  { ID_TEXT_01,        APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLUE) } },
  { ID_TEXT_01,        APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                     ARG_V(0),
                                                     ARG_V(0) } },
  { ID_TEXT_01,        APPW_SET_PROP_TEXTID,       { ARG_V(ID_Text2) } },
};

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       ID_SCREEN_WELCOME_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_WELCOME_RootInfo = {
  ID_SCREEN_WELCOME,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  NULL,  0,
  cbID_SCREEN_WELCOME,
  0
};

/*************************** End of file ****************************/
