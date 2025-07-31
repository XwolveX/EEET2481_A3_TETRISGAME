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
File        : ID_SCREEN_PLAY.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_PLAY.h"

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
    ID_SCREEN_PLAY, 0,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_00, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      240, 326, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_GAME, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_NEXTBOX, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 260, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_STATUS, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 70, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 70, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_PAUSEINFO, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 260, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_NCODE, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 80, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      70, 10, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_NCODE_VALUE, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      70, 15, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_TIME, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 112, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      70, 21, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_TIME_VALUE, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 133, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      70, 21, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_GAUGE_Create,
    ID_GAUGE_00, ID_SCREEN_PLAY,
    { { { DISPOSE_MODE_REL_PARENT, -17, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 112, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      195, 100, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_IMAGE_00,         APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_IMAGE_00,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acpixel),
                                                       ARG_V(52529), } },
  { ID_BOX_GAME,         APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_BOX_NEXTBOX,      APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_BOX_STATUS,       APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_BOX_PAUSEINFO,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_BLACK) } },
  { ID_TEXT_NCODE,       APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_TEXT_NCODE,       APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                       ARG_V(0),
                                                       ARG_V(0) } },
  { ID_TEXT_NCODE,       APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_NCODED) } },
  { ID_TEXT_NCODE,       APPW_SET_PROP_BKCOLOR,      { ARG_V(GUI_INVALID_COLOR) } },
  { ID_TEXT_NCODE_VALUE, APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_TEXT_NCODE_VALUE, APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                       ARG_V(0),
                                                       ARG_V(0) } },
  { ID_TEXT_NCODE_VALUE, APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_NCODED_VALUE) } },
  { ID_TEXT_TIME,        APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_TEXT_TIME,        APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                       ARG_V(0),
                                                       ARG_V(0) } },
  { ID_TEXT_TIME,        APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_3) } },
  { ID_TEXT_TIME_VALUE,  APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_TEXT_TIME_VALUE,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                       ARG_V(0),
                                                       ARG_V(0) } },
  { ID_TEXT_TIME_VALUE,  APPW_SET_PROP_TEXTID,       { ARG_V(ID_TEXT_TIME_VALUE) } },
  { ID_GAUGE_00,         APPW_SET_PROP_ALIGNBITMAP,  { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_BOTTOM),
                                                       ARG_V(0),
                                                       ARG_V(4294967273) } },
  { ID_GAUGE_00,         APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                       ARG_V(1800) } },
  { ID_GAUGE_00,         APPW_SET_PROP_SPAN,         { ARG_V(0),
                                                       ARG_V(10) } },
  { ID_GAUGE_00,         APPW_SET_PROP_RADIUS,       { ARG_V(50) } },
  { ID_GAUGE_00,         APPW_SET_PROP_COLORS,       { ARG_V(0xff2c2c30),
                                                       ARG_V(0xff2777b3),
                                                       ARG_V(GUI_INVALID_COLOR) } },
  { ID_GAUGE_00,         APPW_SET_PROP_VALUES,       { ARG_V(19),
                                                       ARG_V(13) } },
  { ID_GAUGE_00,         APPW_SET_PROP_ROUNDEDVAL,   { ARG_V(0) } },
  { ID_GAUGE_00,         APPW_SET_PROP_ROUNDEDEND,   { ARG_V(0) } },
  { ID_GAUGE_00,         APPW_SET_PROP_VALUE,        { ARG_V(0) } },
};

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_VAR_TIME,         WM_NOTIFICATION_VALUE_CHANGED,    ID_GAUGE_00,         APPW_JOB_SETVALUE,       ID_SCREEN_PLAY__WM_NOTIFICATION_VALUE_CHANGED__ID_GAUGE_00__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_TIME,         WM_NOTIFICATION_VALUE_CHANGED,    ID_TEXT_TIME_VALUE,  APPW_JOB_SETTEXT,        ID_SCREEN_PLAY__WM_NOTIFICATION_VALUE_CHANGED__ID_TEXT_TIME_VALUE__APPW_JOB_SETTEXT,
    { ARG_V(-1),
    }, 0, NULL
  },
};

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       ID_SCREEN_PLAY_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_PLAY_RootInfo = {
  ID_SCREEN_PLAY,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  _aAction, GUI_COUNTOF(_aAction),
  cbID_SCREEN_PLAY,
  0
};

/*************************** End of file ****************************/
