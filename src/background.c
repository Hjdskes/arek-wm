/* background.c
 *
 * Copyright (C) 2015 Jente Hidskes <hjdskes@gmail.com>
 * Copyright (C) 2014 Ikey Doherty <ikey.doherty@gmail.com>
 * Copyright (c) 2014 Emanuel Fernandes <efernandes@tektorque.com> (color/modes handling)
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

#include <meta/meta-background.h>
#include <meta/meta-background-actor.h>
#include <meta/meta-version.h>

#include "background.h"

#define BACKGROUND_SCHEMA  "org.gnome.desktop.background"
#define PICTURE_URI        "picture-uri"
#define PRIMARY_COLOR      "primary-color"
#define SECONDARY_COLOR    "secondary-color"
#define COLOR_SHADING_TYPE "color-shading-type"
#define BACKGROUND_STYLE   "picture-options"
#define GNOME_COLOR_HACK   "gnome-control-center/pixmaps/noise-texture-light.png"

#define BACKGROUND_TIMEOUT 850

enum {
	PROP_0,
	PROP_SCREEN,
	PROP_MONITOR,
	N_PROPERTIES
};

struct _ArekBackgroundPrivate {
	MetaScreen *screen;
	int monitor;

	GSettings *settings;
	ClutterActor *current_background;
	ClutterActor *previous_background;
};

static GParamSpec *g_obj_props[N_PROPERTIES] = { NULL };

G_DEFINE_TYPE_WITH_PRIVATE (ArekBackground, arek_background, META_TYPE_BACKGROUND_GROUP);

static void remove_previous (ClutterActor *actor, gpointer user_data)
{
	ArekBackgroundPrivate *priv;

	priv = AREK_BACKGROUND (user_data)->priv;

	clutter_actor_destroy (actor);
	priv->previous_background = NULL;
}

static void begin_remove_previous (__attribute__ ((unused)) ClutterActor *actor,
				   gpointer user_data)
{
	ArekBackgroundPrivate *priv;

	priv = AREK_BACKGROUND (user_data)->priv;

	/* Animate-out the previous background. */
	if (priv->previous_background && priv->previous_background != priv->current_background) {
		g_signal_connect (priv->previous_background, "transitions-completed",
				  G_CALLBACK (remove_previous), user_data);

		clutter_actor_save_easing_state (priv->previous_background);
		clutter_actor_set_easing_mode (priv->previous_background, CLUTTER_EASE_OUT_QUAD);
		clutter_actor_set_easing_duration (priv->previous_background, BACKGROUND_TIMEOUT);
		g_object_set (priv->previous_background, "opacity", 0, NULL);
		clutter_actor_restore_easing_state (priv->previous_background);
	}
}

static void on_changed (__attribute__ ((unused)) MetaBackground *background,
			gpointer user_data)
{
	ArekBackgroundPrivate *priv;

	priv = AREK_BACKGROUND (user_data)->priv;

	/* Animate-in the new background. */
	clutter_actor_save_easing_state (priv->current_background);
	g_signal_connect (priv->current_background, "transitions-completed",
			  G_CALLBACK (begin_remove_previous), user_data);

	clutter_actor_set_easing_mode (priv->current_background, CLUTTER_EASE_IN_EXPO);
	clutter_actor_set_easing_duration (priv->current_background, BACKGROUND_TIMEOUT);
	g_object_set (priv->current_background, "opacity", 255, NULL);
	clutter_actor_restore_easing_state (priv->current_background);
}

static void
set_image_background (gchar                   *current_background_filename,
		      MetaBackground          *background,
		      GDesktopBackgroundStyle *style)
{
	GFile *current_background_file = NULL;

	/* Load up the new wallpaper. */
	current_background_file = g_file_new_for_uri (current_background_filename);
	g_free (current_background_filename);

#if META_MINOR_VERSION > 14
	meta_background_set_file (background, current_background_file, *style);
#else
	char *filename = g_file_get_path (current_background_file);
	if (filename) {
		meta_background_set_filename (background, filename, *style);
		g_free (filename);
	} else {
		g_message ("Note: File does not exist...");
	}
#endif

	g_object_unref (current_background_file);
}

static void
set_color_background (GSettings *settings, MetaBackground *background)
{
	GDesktopBackgroundShading shading_direction;
	ClutterColor primary_color;
	ClutterColor secondary_color;
	gchar *color_str = NULL;

	shading_direction = g_settings_get_enum (settings, COLOR_SHADING_TYPE);

	/* Primary color. */
	color_str = g_settings_get_string (settings, PRIMARY_COLOR);
	if (color_str) {
		clutter_color_from_string (&primary_color, color_str);
		g_free (color_str);
		color_str = NULL;
	}

	/* Secondary color. */
	color_str = g_settings_get_string (settings, SECONDARY_COLOR);
	if (color_str) {
		clutter_color_from_string (&secondary_color, color_str);
		g_free (color_str);
		color_str = NULL;
	}

	if (shading_direction == G_DESKTOP_BACKGROUND_SHADING_SOLID) {
		meta_background_set_color (background, &primary_color);
	} else {
		meta_background_set_gradient (background, shading_direction,
					      &primary_color, &secondary_color);
	}
}

static void
update (ArekBackground *self)
{
	ArekBackgroundPrivate *priv;
	ClutterActor *actor = NULL;
	MetaBackground *background = NULL;
	MetaRectangle rect;
	gchar *current_background_filename = NULL;
	GDesktopBackgroundStyle style;

	priv = self->priv;

	/* Create new background actor. */
	actor = meta_background_actor_new (priv->screen, priv->monitor);
	background = meta_background_new (priv->screen);
	meta_background_actor_set_background (META_BACKGROUND_ACTOR (actor), background);
	g_object_unref (background);

	/* Configure new background actor. */
	meta_screen_get_monitor_geometry (priv->screen, priv->monitor, &rect);
	clutter_actor_set_size (actor, rect.width, rect.height);
	clutter_actor_insert_child_at_index (CLUTTER_ACTOR (self), actor, -1);
	g_object_set (actor, "opacity", 0, NULL);
	clutter_actor_show (actor);
	g_signal_connect (background, "changed", G_CALLBACK (on_changed), self);

	if (priv->current_background) {
		priv->previous_background = priv->current_background;
	}
	priv->current_background = actor;

	/* Apply. */
	style = g_settings_get_enum (priv->settings, BACKGROUND_STYLE);
	current_background_filename = g_settings_get_string (priv->settings, PICTURE_URI);
	if (style == G_DESKTOP_BACKGROUND_STYLE_NONE ||
	    /* See: https://github.com/evolve-os/budgie-desktop/commit/cf29965389a9e76566d8f469b89831e1dad0b890. */
	    g_str_has_suffix (current_background_filename, GNOME_COLOR_HACK))
	{
		set_color_background (priv->settings, background);
	} else {
		set_image_background (current_background_filename, background, &style);
	}
}

static void
on_settings_changed (__attribute__ ((unused)) GSettings *settings,
		     __attribute__ ((unused)) const gchar *key,
		     gpointer user_data)
{
	update (AREK_BACKGROUND (user_data));
}

static void
arek_background_set_property (GObject      *object,
			      guint	 prop_id,
			      const GValue *value,
			      GParamSpec   *pspec)
{
	ArekBackgroundPrivate *priv;

	priv = AREK_BACKGROUND (object)->priv;

	switch (prop_id) {
		case PROP_SCREEN:
			priv->screen = g_value_get_pointer (value);
			break;
		case PROP_MONITOR:
			priv->monitor = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
arek_background_get_property (GObject    *object,
			      guint       prop_id,
			      GValue     *value,
			      GParamSpec *pspec)
{
	ArekBackgroundPrivate *priv;

	priv = AREK_BACKGROUND (object)->priv;

	switch (prop_id) {
		case PROP_SCREEN:
			g_value_set_pointer (value, priv->screen);
			break;
		case PROP_MONITOR:
			g_value_set_int (value, priv->monitor);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
arek_background_dispose (GObject *object)
{
	ArekBackgroundPrivate *priv;

	priv = AREK_BACKGROUND (object)->priv;

	if (priv->settings) {
		g_object_unref (priv->settings);
		priv->settings = NULL;
	}

	G_OBJECT_CLASS (arek_background_parent_class)->dispose (object);
}

static GObject *
arek_background_constructor (GType type, guint n_props, GObjectConstructParam *props)
{
	GObject *object;
	ArekBackground *self;
	ArekBackgroundPrivate *priv;
	MetaRectangle rectangle;

	object = G_OBJECT_CLASS (arek_background_parent_class)->constructor (type, n_props, props);
	self = AREK_BACKGROUND (object);
	priv = self->priv;

	meta_screen_get_monitor_geometry (priv->screen, priv->monitor, &rectangle);
	clutter_actor_set_position (CLUTTER_ACTOR (self), rectangle.x, rectangle.y);
	clutter_actor_set_size (CLUTTER_ACTOR (self), rectangle.width, rectangle.height);

	g_signal_connect (priv->settings, "changed", G_CALLBACK (on_settings_changed), self);
	update (self);

	return object;
}

static void
arek_background_class_init (ArekBackgroundClass *klass)
{
	GObjectClass *g_object_class;

	g_object_class = G_OBJECT_CLASS (klass);
	g_object_class->constructor = arek_background_constructor;
	g_object_class->dispose = arek_background_dispose;
	g_object_class->get_property = arek_background_get_property;
	g_object_class->set_property = arek_background_set_property;

	g_obj_props[PROP_SCREEN] =
		g_param_spec_pointer ("screen",
				      "Screen",
				      "The screen of this background",
				      G_PARAM_CONSTRUCT | G_PARAM_WRITABLE);

	g_obj_props[PROP_MONITOR] =
		g_param_spec_int ("monitor",
				  "Monitor",
				  "The index of this background's monitor",
				  0, 100, 0, G_PARAM_CONSTRUCT | G_PARAM_WRITABLE);

	g_object_class_install_properties (g_object_class, N_PROPERTIES, g_obj_props);
}

static void
arek_background_init (ArekBackground *self)
{
	ArekBackgroundPrivate *priv;

	self->priv = priv = arek_background_get_instance_private (self);
	priv->settings = g_settings_new (BACKGROUND_SCHEMA);

	clutter_actor_set_background_color (CLUTTER_ACTOR (self), CLUTTER_COLOR_DarkRed);
}

ClutterActor *
arek_background_new (MetaScreen *screen, int monitor)
{
	return g_object_new (AREK_BACKGROUND_TYPE,
			     "screen", screen,
			     "monitor", monitor,
			     NULL);
}

