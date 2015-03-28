/* workspace.c
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

#include <meta/workspace.h>

#include "arek-wm.h"
#include "workspace.h"

static void
init_workspace (gpointer data, gpointer user_data)
{
	ArekWm *wm;
	guint nmaster;
	gfloat mfact;
	TileMode mode;

	wm = AREK_WM (user_data);
	mode = g_settings_get_enum (wm->settings, "tile-mode");
	nmaster = g_settings_get_uint (wm->settings, "nmaster");
	// TODO: when GLib supports this, use floats in GSettings
	mfact = (gfloat) g_settings_get_double (wm->settings, "mfact");

	g_object_set_data (G_OBJECT (data), "mode", GINT_TO_POINTER (mode));
	g_object_set_data (G_OBJECT (data), "nmaster", GUINT_TO_POINTER (nmaster));
	g_object_set_data (G_OBJECT (data), "mfact", GFLOAT_TO_POINTER (mfact));
}

void
arek_wm_init_workspaces (ArekWm *wm)
{
	GList *spaces;

	spaces = meta_screen_get_workspaces (wm->screen);
	g_list_foreach (spaces, init_workspace, wm);
}

