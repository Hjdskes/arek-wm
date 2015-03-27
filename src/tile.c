/* tile.c
 *
 * Copyright (C) 2015 Jente Hidskes <hjdskes@gmail.com>
 *
 * Tiling algorithms are taken from DWM, under the MIT/X Consortium License.
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

#include <meta/window.h>
#include <meta/workspace.h>

#include "arek-wm.h"
#include "meta-wrapper.h"
#include "tile.h"
#include "workspace.h"

static void
on_window_position_changed (MetaWindow *window, gpointer user_data)
{
	ArekWm *wm;

	wm = AREK_WM (user_data);

	/* This signal is only emitted when the user has moved the window while
	 * it is tiled, i.e. when the window should be made floating. */
	arek_wm_make_float (wm, window);
	arek_wm_retile (wm, meta_window_get_workspace (window));
}

static void
move_resize_frame (ArekWm *wm, MetaWindow *window, int x, int y, int w, int h)
{
	g_signal_handlers_block_by_func (window, G_CALLBACK (on_window_position_changed), wm);
	meta_window_move_resize_frame (window, FALSE, x, y, w, h);
	g_signal_handlers_unblock_by_func (window, G_CALLBACK (on_window_position_changed), wm);
}

static void
tile_monocle (ArekWm *wm, MetaRectangle *area, MetaWorkspace *space)
{
	for (GList *ws = arek_wm_nexttiled (wm->windows, space); ws; ws = arek_wm_nexttiled (ws->next, space)) {
		move_resize_frame (wm, ws->data, area->x, area->y, area->width, area->height);
	}
}

static void
tile_horizontal (ArekWm *wm, MetaRectangle *area, MetaWorkspace *space)
{
	GList *ws;
	MetaRectangle win_dim;
	guint nmaster, n, i, w, tx, mx, mh;
	gfloat mfact;

	for (n = 0, ws = arek_wm_nexttiled (wm->windows, space); ws; ws = arek_wm_nexttiled (ws->next, space), n++);
	if (n == 0) {
		return;
	}

	nmaster = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (space), "nmaster"));
	mfact = GPOINTER_TO_FLOAT (g_object_get_data (G_OBJECT (space), "mfact"));

	if (n > nmaster) {
		mh = nmaster != 0 ? area->height * mfact : 0;
	} else {
		mh = area->height;
	}

	i = tx = mx = 0;
	for (ws = arek_wm_nexttiled (wm->windows, space); ws; ws = arek_wm_nexttiled (ws->next, space), i++) {
		if (i < nmaster) {
			w = (area->width - mx) / (MIN (n, nmaster) - i);
			move_resize_frame (wm, ws->data, area->x + mx, area->y, w, mh);
			meta_window_get_frame_rect (ws->data, &win_dim);
			mx += win_dim.width;
		} else {
			w = (area->width - tx) / (n - i);
			move_resize_frame (wm, ws->data, area->x + tx, area->y + mh, w, area->height - mh);
			meta_window_get_frame_rect (ws->data, &win_dim);
			tx += win_dim.width;
		}
	}
}

static void
tile_vertical (ArekWm *wm, MetaRectangle *area, MetaWorkspace *space)
{
	GList *ws;
	MetaRectangle win_dim;
	guint nmaster, n, i, h, ty, my, mw;
	gfloat mfact;

	for (n = 0, ws = arek_wm_nexttiled (wm->windows, space); ws; ws = arek_wm_nexttiled (ws->next, space), n++);
	if (n == 0) {
		return;
	}

	nmaster = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (space), "nmaster"));
	mfact = GPOINTER_TO_FLOAT (g_object_get_data (G_OBJECT (space), "mfact"));

	if (n > nmaster) {
		mw = nmaster != 0 ? area->width * mfact : 0;
	} else {
		mw = area->width;
	}

	i = ty = my = 0;
	for (ws = arek_wm_nexttiled (wm->windows, space); ws; ws = arek_wm_nexttiled (ws->next, space), i++) {
		if (i < nmaster) {
			h = (area->height - my) / (MIN (n, nmaster) - i);
			move_resize_frame (wm, ws->data, area->x, area->y + my, mw, h);
			meta_window_get_frame_rect (ws->data, &win_dim);
			my += win_dim.height;
		} else {
			h = (area->height - ty) / (n - i);
			move_resize_frame (wm, ws->data, area->x + mw, area->y + ty, area->width - mw, h);
			meta_window_get_frame_rect (ws->data, &win_dim);
			ty += win_dim.height;
		}
	}
}

void
arek_wm_retile (ArekWm *wm, MetaWorkspace *space)
{
	MetaRectangle area;
	gint monitor;
	gint mode;

	monitor = meta_screen_get_current_monitor (wm->screen);
	meta_workspace_get_work_area_for_monitor (space, monitor, &area);
	mode = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (space), "mode"));

	switch (mode) {
		case TILE_MODE_VERTICAL:
			tile_vertical (wm, &area, space);
			break;
		case TILE_MODE_HORIZONTAL:
			tile_horizontal (wm, &area, space);
			break;
		case TILE_MODE_MONOCLE:
			tile_monocle (wm, &area, space);
			break;
		default: /* TILE_MODE_FLOAT: do nothing. */
			break;
	}
}

/**
 * TODO: we can keep a list of window classes in GSettings to iterate over
 * and see if the user wants a window to not be tiled. Example: conky.
 */
gboolean
arek_wm_can_tile (MetaWindow *window)
{
	g_return_val_if_fail (META_IS_WINDOW (window), FALSE);

	if (meta_window_get_window_type (window) != META_WINDOW_NORMAL) {
		return FALSE;
	} else if (meta_window_is_override_redirect (window)) {
		return FALSE;
	} else if (meta_window_is_floating (window)) {
		return FALSE;
	} else if (meta_window_is_minimized (window)) {
		return FALSE;
	} else if (!meta_window_is_resizeable (window)) {
		return FALSE;
	} else if (meta_window_is_hidden (window)) { /* What is a hidden window? */
		return FALSE;
	} else if (meta_window_is_fullscreen (window)) {
		return FALSE;
	} else if (meta_window_is_skip_taskbar (window)) {
		return FALSE;
	} else if (meta_window_is_on_all_workspaces (window)) {
		return FALSE;
	}

	return TRUE;
}

void
arek_wm_make_float (ArekWm *wm, MetaWindow *window)
{
	MetaWindowActor *actor;

	actor = META_WINDOW_ACTOR (meta_window_get_compositor_private (window));
	g_object_set (actor, "no-shadow", FALSE, NULL);

	meta_window_make_above (window);

	/* Don't retile when a floating window is moved around. */
	g_signal_handlers_disconnect_by_func (window, G_CALLBACK (on_window_position_changed), wm);
}

/**
 * TODO: Once Mutter exposes MetaWindow's "below" property,
 * set it here and remove arek_wm_make_float.
 */
void
arek_wm_make_tile (ArekWm *wm, MetaWindow *window)
{
	MetaWindowActor *actor;

	actor = META_WINDOW_ACTOR (meta_window_get_compositor_private (window));
	g_object_set (actor, "no-shadow", TRUE, NULL);

	meta_window_unmake_above (window);

	/* Connect signal handlers to the newly tiled window, so we can
	 * retile ourselves when windows move around. */
	g_signal_connect (window, "position-changed",
			  G_CALLBACK (on_window_position_changed), wm);
}

GList *
arek_wm_nexttiled (GList *ws, MetaWorkspace *space)
{
	for (; ws && (!meta_window_located_on_workspace (ws->data, space) ||
		       meta_window_is_floating (ws->data) ||
		      !arek_wm_can_tile (ws->data)); ws = ws->next);
	return ws;
}

GList *
arek_wm_prevtiled (GList *ws, MetaWorkspace *space)
{
	for (; ws && (!meta_window_located_on_workspace (ws->data, space) ||
		       meta_window_is_floating (ws->data) ||
		      !arek_wm_can_tile (ws->data)); ws = ws->prev);
	return ws;
}

