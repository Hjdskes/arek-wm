/* arek-wm.h
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

#ifndef __AREK_WM_H__
#define __AREK_WM_H__

#include <glib.h>
#include <meta/meta-plugin.h>

G_BEGIN_DECLS

typedef enum {
	TILE_MODE_VERTICAL,
	TILE_MODE_HORIZONTAL,
	TILE_MODE_MONOCLE,
	TILE_MODE_FLOAT
} TileMode;

typedef struct _ArekWm      ArekWm;
typedef struct _ArekWmClass ArekWmClass;

#define AREK_WM_TYPE            (arek_wm_get_type ())
#define AREK_WM(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), AREK_WM_TYPE, ArekWm))
#define AREK_WM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  AREK_WM_TYPE, ArekWmClass))
#define IS_AREK_WM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), AREK_WM_TYPE))
#define IS_AREK_WM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  AREK_WM_TYPE))
#define AREK_WM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  AREK_WM_TYPE, ArekWmClass))

struct _ArekWm {
	MetaPlugin parent;

	MetaScreen *screen;
	MetaDisplay *display;

	GList *windows;
	GList *active_window;
	TileMode mode;
	guint nmaster;
	gdouble mfact;
	gdouble mfact_step;
};

struct _ArekWmClass {
	MetaPluginClass parent_class;
};

GType arek_wm_get_type (void);

G_END_DECLS

#endif /* __AREK_WM_H__ */

