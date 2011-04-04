#ifndef _H_VWM_
#define _H_VWM_

#include <inttypes.h>

#include <glib.h>

#include <sys/types.h>

#ifdef _VIPER_WIDE
#include <ncursesw/curses.h>
#else
#include <curses.h>
#endif

#include <pseudo.h>
#include <viper.h>

#define VWM_VERSION					"2.1.3"

#ifndef _VWM_SCREENSAVER_TIMEOUT
#define _VWM_SCREENSAVER_TIMEOUT 5
#endif

#ifndef _VWM_SHARED_MODULES
#ifdef _VIPER_WIDE
#define _VWM_SHARED_MODULES      "/usr/lib/vwm/modules_wide/"
#else
#define _VWM_SHARED_MODULES      "/usr/lib/vwm/modules/"
#endif
#endif

#define VWM_SCREENSAVER			   "_:!_VWM_SCREENSAVER_!:_"
#define VWM_CLOCK_TICK           (0.1F)
#define VWM_CLOCK_TICKS_PER_SEC  ((short int)(1/VWM_CLOCK_TICK))

#define VWM_STATE_NORMAL         0
#define VMW_STATE_ASLEEP         (1<<1)      // indicates screensaver active
#define VWM_STATE_ACTIVE         (1<<2)      // indiates WM mode

enum
{
   VWM_PANEL_FREEZE=0x1,
   VWM_PANEL_THAW,
   VWM_PANEL_REWIND,
   VWM_PANEL_ADVANCE,
   VWM_PANEL_CLEAR
};

typedef WINDOW* (*VWM_MOD_MAIN)(gpointer anything);

typedef struct
{
	gchar				   *category;
	gchar				   *title;
	VWM_MOD_MAIN	   mod_main;
	gpointer			   anything;
	gchar				   libfilename[NAME_MAX];
}
VWM_MODULE;

typedef struct
{
   VWM_MODULE        *scrsaver_mod;
   gint              timeout;
   GTimer            *timer;
   ps_task_t         *ps_task;
   guint32           state;
}
VWM_SCRSAVER;

typedef struct
{
	GSList			   *category_list;
	GSList			   *module_list;
	GSList			   *key_list;
   VIPER_FUNC        wallpaper_agent; 
	VWM_SCRSAVER      screen_saver;
   guint32           state;
}
VWM;

typedef struct
{
   uid_t             user;
   gchar             *login;
   gchar             *passwd;
   gchar             *home;
   gchar             *rc_file;
   gchar             *mod_dir;
}
VWM_PROFILE;

/*	startup functions	*/
VWM*				vwm_init(void);
#define			vwm_get_instance()	            (vwm_init())
void				vwm_ui_accel(gint val);
void				vwm_ui_slow(gint val);
gint				vwm_ui_get_speed(void);
void				vwm_ui_set_speed(guint speed);
void 				vwm_hook_enter(VIPER_FUNC func,gpointer arg);
void				vwm_hook_leave(VIPER_FUNC func,gpointer arg);

/* panel facilities  */
WINDOW*        vwm_panel_init(void);
#define        vwm_panel_get_instance()         (vwm_panel_init())
gint16         vwm_panel_ctrl(guint32 ctrl,gint16 val);
#define        vwm_panel_freeze_set(timeout)    (vwm_panel_ctrl(VWM_PANEL_FREEZE,timeout))
#define        vwm_panel_freeze_get()           (vwm_panel_ctrl(VWM_PANEL_FREEZE,-1))
#define        vwm_panel_freeze_now()           (vwm_panel_ctrl(VWM_PANEL_FREEZE,0))
#define        vwm_panel_thaw_now()             (vwm_panel_ctrl(VWM_PANEL_THAW,0))
#define        vwm_panel_clear()                (vwm_panel_ctrl(VWM_PANEL_CLEAR,0))
uintmax_t      vwm_panel_message_add(gchar *msg,gint timeout);
void           vwm_panel_message_del(uintmax_t msg_id);
uintmax_t      vwm_panel_message_find(gchar *msg);
gint           vwm_panel_message_touch(uintmax_t msg_id);
gint           vwm_panel_message_promote(uintmax_t msg_id);

/*	extensibility functions	*/
gchar*	      vwm_modules_load(gchar *module_dir);
GSList*			vwm_modules_list(gchar *category);
GSList*			vwm_modules_list_categories(void);
VWM_MODULE*		vwm_module_add(gchar *category,gchar *title,
						VWM_MOD_MAIN,gpointer anything,gchar *libfilename);
VWM_MODULE*		vwm_module_find(gchar *title);
gint				vwm_category_sort(gpointer item1,gpointer item2);

/* profile functions */
VWM_PROFILE*   vwm_profile_init(void);
#define        vwm_profile_acquire()      vwm_profile_init()
gchar*         vwm_profile_mod_dir_get();
void           vwm_profile_mod_dir_set(gchar *module_dir);
#define        VWM_MOD_DIR                (vwm_profile_mod_dir_get())
gchar*         vwm_profile_login_get();
#define        VWM_LOGIN                  (vwm_profile_login_get())
gchar*         vwm_profile_rc_file_get();
#define        VWM_RC_FILE                (vwm_profile_rc_file_get())

/* screensaver functions   */
void           vwm_scrsaver_start(void);
void           vwm_scrsaver_stop(void);
void           vwm_scrsaver_set(gchar *title);
const gchar*   vwm_scrsaver_get(void);
void           vwm_scrsaver_timeout_set(gint timeout);
gint           vwm_scrsaver_timeout_get(void);

/*	helper functions	*/
void				vwm_post_help(gchar *msg);

/* helper macros  */


#endif