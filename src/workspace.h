/* workspace.h
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

#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

#include "arek-wm.h"

G_BEGIN_DECLS

inline static gpointer
GFLOAT_TO_POINTER (gfloat f)
{
	union {
		gfloat f;
		gpointer p;
	} c;

	c.f = f;
	return c.p;
}

inline static gfloat
GPOINTER_TO_FLOAT (gpointer p)
{
	union {
		gpointer p;
		gfloat f;
	} c;

	c.p = p;
	return c.f;
}

void arek_wm_init_workspaces (ArekWm *wm);

G_END_DECLS

#endif /* __WORKSPACE_H__ */

