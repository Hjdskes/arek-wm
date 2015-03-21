/* meta-wrapper.h
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

#ifndef __META_WRAPPER_H__
#define __META_WRAPPER_H__

#include <meta/window.h>

G_BEGIN_DECLS

gboolean meta_window_is_floating (MetaWindow *window);
gboolean meta_window_is_minimized (MetaWindow *window);
gboolean meta_window_is_resizeable (MetaWindow *window);

G_END_DECLS

#endif /* __META_WRAPPER_H__ */

