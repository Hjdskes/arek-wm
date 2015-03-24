/* background.h
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

#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include <glib.h>
#include <meta/meta-background-group.h>

G_BEGIN_DECLS

typedef struct _ArekBackground        ArekBackground;
typedef struct _ArekBackgroundClass   ArekBackgroundClass;
typedef struct _ArekBackgroundPrivate ArekBackgroundPrivate;

#define AREK_BACKGROUND_TYPE            (arek_background_get_type ())
#define AREK_BACKGROUND(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), AREK_BACKGROUND_TYPE, ArekBackground))
#define AREK_BACKGROUND_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  AREK_BACKGROUND_TYPE, ArekBackgroundClass))
#define IS_AREK_BACKGROUND(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), AREK_BACKGROUND_TYPE))
#define IS_AREK_BACKGROUND_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  AREK_BACKGROUND_TYPE))
#define AREK_BACKGROUND_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  AREK_BACKGROUND_TYPE, ArekBackgroundClass))

struct _ArekBackground {
	MetaBackgroundGroup parent;

	ArekBackgroundPrivate *priv;
};

struct _ArekBackgroundClass {
	ClutterActorClass parent_class;
};

GType arek_background_get_type (void);

ClutterActor *arek_background_new (MetaScreen *screen, int index);

G_END_DECLS

#endif /* __BACKGROUND_H__ */

