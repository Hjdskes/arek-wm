/* keybindings.c
 *
 * Copyright (C) 2015 Jente Hidskes <hjdskes@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <meta/keybindings.h>

#include "arek-wm.h"
#include "meta-wrapper.h"
#include "tile.h"
#include "windowlist.h"
#include "workspace.h"

#define AREK_WM_SCHEMA_KEYBINDINGS "com.unia.arek.wm.keybindings"

static gboolean
skip (MetaWindow *window, MetaWorkspace *space)
{
	return !meta_window_located_on_workspace (window, space) ||
		meta_window_is_minimized (window) ||
		meta_window_is_hidden (window);
}

static void
switch_windows (__attribute__ ((unused)) MetaDisplay *display,
		MetaScreen *screen,
		__attribute__ ((unused)) MetaWindow *event_window,
		ClutterKeyEvent *event,
		MetaKeyBinding *binding,
		gpointer user_data)
{
	ArekWm *wm;
	GList *w = NULL;
	MetaWorkspace *space;

	wm = AREK_WM (user_data);

	if (!wm->active_window) {
		return;
	}

	space = meta_screen_get_active_workspace (screen);
	if (meta_key_binding_is_reversed (binding)) {
		for (w = wm->active_window->prev; w && skip (w->data, space); w = w->prev);
		if (!w) {
			for (w = g_list_last (wm->windows); w && skip (w->data, space); w = w->prev);
		}
	} else {
		for (w = wm->active_window->next; w && skip (w->data, space); w = w->next);
		if (!w) {
			for (w = wm->windows; w && skip (w->data, space); w = w->next);
		}
	}

	if (w) {
		meta_window_activate (w->data, event->time);
	}
}

static void
on_run_dialog (__attribute__ ((unused)) MetaDisplay *display,
	       __attribute__ ((unused)) MetaScreen *screen,
	       __attribute__ ((unused)) MetaWindow *window,
	       __attribute__ ((unused)) ClutterKeyEvent *event,
	       __attribute__ ((unused)) MetaKeyBinding *binding,
	       __attribute__ ((unused)) gpointer user_data)
{
	// TODO: don't hardcode
	g_spawn_command_line_async ("dmenu_run -b", NULL);
}

static void
on_main_menu (__attribute__ ((unused)) MetaDisplay *display,
	      __attribute__ ((unused)) MetaScreen *screen,
	      __attribute__ ((unused)) MetaWindow *window,
	      __attribute__ ((unused)) ClutterKeyEvent *event,
	      __attribute__ ((unused)) MetaKeyBinding *binding,
	      __attribute__ ((unused)) gpointer user_data)
{
	// TODO: don't hardcode
	g_spawn_command_line_async ("dmenu_run -b", NULL);
}

static void
on_overlay_key (__attribute__ ((unused)) MetaDisplay *display,
		__attribute__ ((unused)) gpointer user_data)
{
	// TODO: don't hardcode
	g_spawn_command_line_async ("dmenu_run -b", NULL);
}

static void
set_mode (ArekWm *wm, TileMode mode)
{
	MetaWorkspace *space;

	space = meta_screen_get_active_workspace (wm->screen);

	g_object_set_data (G_OBJECT (space), "mode", GINT_TO_POINTER (mode));
	arek_wm_retile (wm, space);
}

static void
set_mode_vertical (__attribute__ ((unused)) MetaDisplay *display,
		   __attribute__ ((unused)) MetaScreen *screen,
		   __attribute__ ((unused)) MetaWindow *window,
		   __attribute__ ((unused)) ClutterKeyEvent *event,
		   __attribute__ ((unused)) MetaKeyBinding *binding,
		   gpointer user_data)
{
	set_mode (AREK_WM (user_data), TILE_MODE_VERTICAL);
}

static void
set_mode_horizontal (__attribute__ ((unused)) MetaDisplay *display,
		     __attribute__ ((unused)) MetaScreen *screen,
		     __attribute__ ((unused)) MetaWindow *window,
		     __attribute__ ((unused)) ClutterKeyEvent *event,
		     __attribute__ ((unused)) MetaKeyBinding *binding,
		     gpointer user_data)
{
	set_mode (AREK_WM (user_data), TILE_MODE_HORIZONTAL);
}

static void
set_mode_monocle (__attribute__ ((unused)) MetaDisplay *display,
		  __attribute__ ((unused)) MetaScreen *screen,
		  __attribute__ ((unused)) MetaWindow *window,
		  __attribute__ ((unused)) ClutterKeyEvent *event,
		  __attribute__ ((unused)) MetaKeyBinding *binding,
		  gpointer user_data)
{
	set_mode (AREK_WM (user_data), TILE_MODE_MONOCLE);
}

static void
set_mode_float (__attribute__ ((unused)) MetaDisplay *display,
		__attribute__ ((unused)) MetaScreen *screen,
		__attribute__ ((unused)) MetaWindow *window,
		__attribute__ ((unused)) ClutterKeyEvent *event,
		__attribute__ ((unused)) MetaKeyBinding *binding,
		gpointer user_data)
{
	set_mode (AREK_WM (user_data), TILE_MODE_FLOAT);
}

static void
set_nmaster (__attribute__ ((unused)) MetaDisplay *display,
	     MetaScreen *screen,
	     __attribute__ ((unused)) MetaWindow *window,
	     __attribute__ ((unused)) ClutterKeyEvent *event,
	     MetaKeyBinding *binding,
	     gpointer user_data)
{
	ArekWm *wm;
	MetaWorkspace *space;
	guint nmaster;

	wm = AREK_WM (user_data);
	space = meta_screen_get_active_workspace (screen);
	nmaster = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (space), "nmaster"));

	if (meta_key_binding_is_reversed (binding)) {
		if (nmaster == 0) {
			return;
		}
		nmaster--;
	} else {
		nmaster++;
	}


	g_object_set_data (G_OBJECT (space), "nmaster", GUINT_TO_POINTER (nmaster));
	arek_wm_retile (wm, space);
}

static void
set_mfact (__attribute__ ((unused)) MetaDisplay *display,
	   MetaScreen *screen,
	   __attribute__ ((unused)) MetaWindow *window,
	   __attribute__ ((unused)) ClutterKeyEvent *event,
	   MetaKeyBinding *binding,
	   gpointer user_data)
{
	ArekWm *wm;
	MetaWorkspace *space;
	float mfact;

	wm = AREK_WM (user_data);
	space = meta_screen_get_active_workspace (screen);
	mfact = GPOINTER_TO_FLOAT (g_object_get_data (G_OBJECT (space), "mfact"));

	if (meta_key_binding_is_reversed (binding)) {
		mfact -= wm->mfact_step;
	} else {
		mfact += wm->mfact_step;
	}

	if (mfact < 0.1 || mfact > 0.9) {
		return;
	}

	g_object_set_data (G_OBJECT (space), "mfact", GFLOAT_TO_POINTER (mfact));
	arek_wm_retile (wm, space);
}

/**
 * TODO: keep focus on w
 */
static void
move_in_stack (__attribute__ ((unused)) MetaDisplay *display,
	       __attribute__ ((unused)) MetaScreen *screen,
	       __attribute__ ((unused)) MetaWindow *window,
	       ClutterKeyEvent *event,
	       MetaKeyBinding *binding,
	       gpointer user_data)
{
	ArekWm *wm;
	GList *w = NULL, *v = NULL;
	MetaWorkspace *space;
	gint mode;

	wm = AREK_WM (user_data);

	if (!(w = wm->active_window) ||
	    meta_window_is_floating (w->data) ||
	    !arek_wm_can_tile (w->data))
	{
		return;
	}

	space = meta_window_get_workspace (w->data);
	if ((mode = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (space), "mode"))) == TILE_MODE_FLOAT) {
		return;
	}

	if (meta_key_binding_is_reversed (binding)) {
		if (w != arek_wm_nexttiled (wm->windows, space) && (v = arek_wm_nexttiled (w->next, space))) {
			arek_wm_switch_elems_in_list (wm, w, v);
		}
	} else {
		if ((v = arek_wm_prevtiled (w->prev, space)) && v != arek_wm_nexttiled (wm->windows, space)) {
			arek_wm_switch_elems_in_list (wm, v, w);
		}
	}

	meta_window_activate (w->data, event->time);
	arek_wm_retile (wm, space);
}

static void
move_to_master (__attribute__ ((unused)) MetaDisplay *display,
		__attribute__ ((unused)) MetaScreen *screen,
		__attribute__ ((unused)) MetaWindow *window,
		ClutterKeyEvent *event,
		__attribute__ ((unused)) MetaKeyBinding *binding,
		gpointer user_data)
{
	ArekWm *wm;
	GList *w = NULL;
	MetaWorkspace *space;
	gint mode;

	wm = AREK_WM (user_data);

	if (!(w = wm->active_window) ||
	    meta_window_is_floating (w->data) ||
	    !arek_wm_can_tile (w->data))
	{
		return;
	}

	space = meta_window_get_workspace (w->data);
	if ((mode = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (space), "mode"))) == TILE_MODE_FLOAT) {
		return;
	}

	if (w == arek_wm_nexttiled (wm->windows, space)) {
		if (!(w = arek_wm_nexttiled (w->next, space))) {
			return;
		}
	}

	arek_wm_remove_link_from_list (wm, w);
	arek_wm_prepend_link_to_list (wm, w);
	meta_window_activate (w->data, event->time);
	arek_wm_retile (wm, space);
}

static void
toggle_float (__attribute__ ((unused)) MetaDisplay *display,
	      __attribute__ ((unused)) MetaScreen *screen,
	      MetaWindow *window,
	      __attribute__ ((unused)) ClutterKeyEvent *event,
	      __attribute__ ((unused)) MetaKeyBinding *binding,
	      gpointer user_data)
{
	ArekWm *wm;

	wm = AREK_WM (user_data);

	if (meta_window_is_floating (window)) {
		arek_wm_make_tile (wm, window);
	} else {
		arek_wm_make_float (wm, window);
	}

	arek_wm_retile (wm, meta_window_get_workspace (window));
}

void
arek_wm_init_keybindings (ArekWm *wm)
{
	GSettings *arek_keybindings;

	/* Set up some default keybindings. */
	arek_keybindings = g_settings_new (AREK_WM_SCHEMA_KEYBINDINGS);

	/* Tile modes. */
	meta_display_add_keybinding (wm->display, "tile-vertical", arek_keybindings,
				     META_KEY_BINDING_NONE,
				     set_mode_vertical, wm, NULL);

	meta_display_add_keybinding (wm->display, "tile-horizontal", arek_keybindings,
				     META_KEY_BINDING_NONE,
				     set_mode_horizontal, wm, NULL);

	meta_display_add_keybinding (wm->display, "tile-monocle", arek_keybindings,
				     META_KEY_BINDING_NONE,
				     set_mode_monocle, wm, NULL);

	meta_display_add_keybinding (wm->display, "tile-float", arek_keybindings,
				     META_KEY_BINDING_NONE,
				     set_mode_float, wm, NULL);

	/* Arek Window Manager control. */
	meta_display_add_keybinding (wm->display, "increase-nmaster", arek_keybindings,
				     META_KEY_BINDING_NONE,
				     set_nmaster, wm, NULL);
	meta_display_add_keybinding (wm->display, "decrease-nmaster", arek_keybindings,
				     META_KEY_BINDING_IS_REVERSED,
				     set_nmaster, wm, NULL);

	meta_display_add_keybinding (wm->display, "increase-mfact", arek_keybindings,
				     META_KEY_BINDING_NONE,
				     set_mfact, wm, NULL);
	meta_display_add_keybinding (wm->display, "decrease-mfact", arek_keybindings,
				     META_KEY_BINDING_IS_REVERSED,
				     set_mfact, wm, NULL);

	/* Window management. */
	meta_display_add_keybinding (wm->display, "move-to-master", arek_keybindings,
				     META_KEY_BINDING_PER_WINDOW,
				     move_to_master, wm, NULL);

	meta_display_add_keybinding (wm->display, "move-up", arek_keybindings,
				     META_KEY_BINDING_PER_WINDOW,
				     move_in_stack, wm, NULL);
	meta_display_add_keybinding (wm->display, "move-down", arek_keybindings,
				     META_KEY_BINDING_PER_WINDOW | META_KEY_BINDING_IS_REVERSED,
				     move_in_stack, wm, NULL);

	meta_display_add_keybinding (wm->display, "toggle-floating", arek_keybindings,
				     META_KEY_BINDING_PER_WINDOW,
				     toggle_float, wm, NULL);

	g_object_unref (arek_keybindings);

	meta_keybindings_set_custom_handler ("switch-windows", switch_windows, wm, NULL);
	meta_keybindings_set_custom_handler ("switch-applications", switch_windows, wm, NULL);
	meta_keybindings_set_custom_handler ("switch-windows-backward", switch_windows, wm, NULL);
	meta_keybindings_set_custom_handler ("switch-applications-backward", switch_windows, wm, NULL);

	g_signal_connect (wm->display, "overlay-key", G_CALLBACK (on_overlay_key), NULL);
	meta_keybindings_set_custom_handler ("panel-main-menu", on_main_menu, NULL, NULL);
	meta_keybindings_set_custom_handler ("panel-run-dialog", on_run_dialog, NULL, NULL);
}

