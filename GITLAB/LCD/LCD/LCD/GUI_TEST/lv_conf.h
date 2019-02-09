/**
 * @file lv_conf.h
 * 
 */


#ifndef LV_CONF_H
#define LV_CONF_H

/*===================
   Graphical settings
 *=====================*/

/* Horizontal and vertical resolution of the library.
 * Screen resolution multiplied by LV_DOWN_SCALE*/
#define LV_HOR_RES          (240 * LV_DOWNSCALE)
#define LV_VER_RES          (320 * LV_DOWNSCALE)
#define LV_DPI              (80 * LV_DOWNSCALE)

/* Buffered rendering: >= LV_DOWNSCALE * LV_HOR_RES or 0 to disable buffering*/
#define LV_VDB_SIZE        (LV_HOR_RES * 10)

/* Enable antialaiassing
 * If enabled everything will half-sized
 * Use LV_DOWNSCALE to compensate
 * the down scaling effect of antialiassing*/
#define LV_ANTIALIAS        0

/*Set the downscaling value*/
#if LV_ANTIALIAS == 0
#define LV_DOWNSCALE        1
#else
#define LV_DOWNSCALE        2
#endif

#define LV_REFR_PERIOD      40    /*Screen refresh period in milliseconds*/
#define LV_INV_FIFO_SIZE    32    /*The average number of objects on a screen */

/*=================
   Misc. setting
 *=================*/
/*Display Input settings*/
#define LV_DISPI_READ_PERIOD      50     /*Input device read period milliseconds*/
#define LV_DISPI_TP_MARKER        0      /*Mark the pressed points (Value means marker point size)*/
#define LV_DISPI_DRAG_LIMIT       10     /*Drag threshold in pixels */
#define LV_DISPI_DRAG_THROW       20     /*Drag throw slow-down in [%]. Greater value means faster slow-down */
#define LV_DISPI_LONG_PRESS_TIME  400    /*Long press time in milliseconds*/
#define LV_DISPI_LONG_PRESS_REP_TIME 100 /*Repeated trigger period in long press [ms] */

/*lv_obj (base object) settings*/
#define LV_OBJ_FREE_NUM          1           /*Enable the free number attribute*/
#define LV_OBJ_FREE_P            1           /*Enable the free pointer attribute*/

/*Others*/
#define LV_COLOR_TRANSP     COLOR_LIME

/*==================
 *  LV OBJ X USAGE
 * ================*/

/*****************
 * Simple object
 *****************/

/*Label (dependencies: -*/
#define USE_LV_LABEL    1
#if USE_LV_LABEL != 0
#define LV_LABEL_SCROLL_SPEED       (25 * LV_DOWNSCALE) /*Hor, or ver. scroll speed (px/sec) in 'LV_LABEL_LONG_SCROLL' mode*/
#define LV_LABEL_SCROLL_SPEED_VER   (10 * LV_DOWNSCALE) /*Ver. scroll speed if hor. scroll is applied too*/
#define LV_LABEL_SCROLL_PLAYBACK_PAUSE  500 /*Wait before the scroll turns back in ms*/
#define LV_LABEL_SCROLL_REPEAT_PAUSE    500 /*Wait before the scroll begins again in ms*/
#endif

/*Image (dependencies: lv_label (if symbols are enabled) from misc: FSINT, UFS)*/
#define USE_LV_IMG      1
#if USE_LV_IMG != 0
//#define LV_IMG_DEF_WALLPAPER    img_square_x1  /*Comment this line to NOT use wallpaper*/
/* 1: enables to interpret the file names as symbol name
 * from symbol_def.h if they begin with a lower case letter.
 * (driver letters are always upper case)*/
#define LV_IMG_ENABLE_SYMBOLS   1
#if LV_IMG_ENABLE_SYMBOLS != 0
#define LV_IMG_DEF_SYMBOL_FONT       FONT_SYMBOL_30
#endif /*LV_IMG_ENABLE_SYMBOLS*/
#endif /*USE_LV_IMG*/

/*Line (dependencies: -*/
#define USE_LV_LINE     1

/*******************
 * Container object
 *******************/

/*Container (dependencies: -*/
#define USE_LV_CONT     1

/*Page (dependencies: lv_rect)*/
#define USE_LV_PAGE     1

/*Window (dependencies: lv_rect, lv_btn, lv_label, lv_img, lv_page)*/
#define USE_LV_WIN      1

/*************************
 * Data visualizer object
 *************************/

/*Bar (dependencies: -)*/
#define USE_LV_BAR       1

/*Line meter (dependencies: bar, misc: trigo)*/
#define USE_LV_LMETER   1

/*Gauge (dependencies: misc: trigo)*/
#define USE_LV_GAUGE    1

/*Chart (dependencies: -)*/
#define USE_LV_CHART    1

/*LED (dependencies: lv_rect)*/
#define USE_LV_LED      1

/*Message box (dependencies: lv_rect, lv_btn, lv_label)*/
#define USE_LV_MBOX     1

/*Text area (dependencies: lv_label, lv_page)*/
#define USE_LV_TA       1
#if USE_LV_TA != 0
#define LV_TA_MAX_LENGTH    256
#define LV_TA_CUR_BLINK_TIME 400    /*ms*/
#endif

/*************************
 * User input object
 *************************/

/*Button (dependencies: lv_cont*/
#define USE_LV_BTN      1

/*Button matrix (dependencies: -)*/
#define USE_LV_BTNM     1

/*Check box (dependencies: lv_btn, lv_label)*/
#define USE_LV_CB       1

/*List (dependencies: lv_btn, lv_label, lv_img)*/
#define USE_LV_LIST     1

/*Drop down list (dependencies: lv_page, lv_label)*/
#define USE_LV_DDLIST    1

/*Bar (dependencies: lv_bar)*/
#define USE_LV_SLIDER    1


/*==================
 *  LV APP SETTINGS
 * =================*/

/*Enable the application system*/
#define LV_APP_ENABLE       1

#if LV_APP_ENABLE != 0

#define LV_APP_DESKTOP 		1

#define LV_APP_SC_WIDTH     (LV_HOR_RES / 4)    /*Shortcut width*/
#define LV_APP_SC_HEIGHT    (LV_VER_RES / 3)    /*Shortcut height*/
#define LV_APP_FONT_SMALL   FONT_DEJAVU_20
#define LV_APP_FONT_MEDIUM  FONT_DEFAULT
#define LV_APP_FONT_LARGE   FONT_DEJAVU_40

/*Animation settings*/
#define LV_APP_ANIM_WIN      200      /*Animation time in milliseconds (0: turn off animation)*/
#define LV_APP_ANIM_DESKTOP  200      /*Animation on the desktop (0: turn off animation)*/

/* App. utility settings */
/*Notice*/
#define USE_LV_APP_NOTICE           1
#if USE_LV_APP_NOTICE != 0
#define LV_APP_NOTICE_SHOW_TIME         4000    /*Notices will be shown for this time [ms]*/
#define LV_APP_NOTICE_CLOSE_ANIM_TIME   300     /*Notice close animation time. [ms] 0: no animation */
#define LV_APP_NOTICE_MAX_NUM   6               /*Max. number of notices*/
#define LV_APP_NOTICE_MAX_LEN   256             /*Max. number of characters on a notice*/
#endif

/*File selector*/
#define USE_LV_APP_FSEL         1
#if USE_LV_APP_FSEL != 0
#define LV_APP_FSEL_PAGE_SIZE     8         /*Max. number of files/folder on a page*/
#define LV_APP_FSEL_FN_MAX_LEN    32        /*Max file name length*/
#define LV_APP_FSEL_PATH_MAX_LEN  256       /*Max path length*/
#endif

/*Keyboard*/
#define USE_LV_APP_KB           1
#if USE_LV_APP_KB != 0
#define LV_APP_KB_ANIM_TIME     300         /*ms*/
#endif
/*==================
 *  LV APP X USAGE
 * ================*/

#define USE_LV_APP_EXAMPLE  1

#define USE_LV_APP_SYSMON   1
#if USE_LV_APP_SYSMON != 0
#define LV_APP_SYSMON_REFR_TIME     500     /*[ms]*/
#define LV_APP_SYSMON_PNUM          64
#define LV_APP_SYSMON_MEM_WARN      (4 * 1024)
#define LV_APP_SYSMON_FRAG_WARN     (70)     /*[%]*/
#define LV_APP_SYSMON_DEFRAG_PERIOD (5000)   /*[%]*/
#endif /*USE_LV_APP_SYSMON != 0*/

#define USE_LV_APP_TERMINAL 1
#if USE_LV_APP_TERMINAL != 0
#define LV_APP_TERMINAL_LENGTH      512  /*Memory of the terminal*/
#endif /*USE_LV_APP_TERMINAL != 0*/

#define USE_LV_APP_FILES 1
#if USE_LV_APP_FILES != 0
#define LV_APP_FILES_PAGE_SIZE      8   /*Max. number of files/folder on a page*/
#define LV_APP_FILES_FN_MAX_LEN     128
#define LV_APP_FILES_PATH_MAX_LEN   256
#define LV_APP_FILES_CHUNK_DEF_SIZE 256
#define LV_APP_FILES_CHUNK_DEF_TIME 100
#define LV_APP_FILES_CHUNK_MAX_SIZE 1024
#endif /*USE_LV_APP_FILES != 0*/
#endif /*LV_APP_ENABLE != 0*/

#endif /*LV_CONF_H*/

