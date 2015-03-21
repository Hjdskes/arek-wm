/* windowlist.c
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

#include <glib.h>

#include "arek-wm.h"
#include "tile.h"

void
arek_wm_remove_from_list (ArekWm *wm, MetaWindow *window)
{
	wm->windows = g_list_remove (wm->windows, window);
}

void
arek_wm_add_to_list (ArekWm *wm, MetaWindow *window)
{
	wm->windows = g_list_prepend (wm->windows, window);
}

void
arek_wm_remove_link_from_list (ArekWm *wm, GList *window)
{
	wm->windows = g_list_remove_link (wm->windows, window);
}

void
arek_wm_prepend_link_to_list (ArekWm *wm, GList *window)
{
	wm->windows = g_list_concat (window, wm->windows);
}

void
arek_wm_switch_elems_in_list (ArekWm *wm, GList *window, GList *sibling)
{
	arek_wm_remove_link_from_list (wm, sibling);
	sibling->prev = window->prev;
	sibling->next = window;
	window->prev = sibling;
	if (sibling->prev) {
		sibling->prev->next = sibling;
	}
}

