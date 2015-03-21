/* main.c
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

#include <stdlib.h>
#include <meta/main.h>

#include "arek-wm.h"

#define GAIL_OPT "NO_GAIL"
#define AT_BRIDGE_OPT "NO_AT_BRIDGE"
#define GNOME_KBDS "Mutter,GNOME Shell"
#define WMNAME "Mutter (Arek)"

static gboolean
print_version_and_exit (__attribute__ ((unused)) const gchar *name,
			__attribute__ ((unused)) const gchar *value,
			__attribute__ ((unused)) gpointer user_data,
			__attribute__ ((unused)) GError **error)
{
	g_print ("%s %s\n", "Arek window manager", VERSION);
	exit (EXIT_SUCCESS);
	return TRUE;
}

static const GOptionEntry options[] = {
	{ "version", 'v', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
		print_version_and_exit, "Show the application's version and exit", NULL },
	{ NULL }
};

int
main (int argc, char **argv)
{
	GOptionContext *context;
	GError *error = NULL;

	context = meta_get_option_context ();
	g_option_context_add_main_entries (context, options, NULL);

	if (!g_option_context_parse (context, &argc, &argv, &error)) {
		gchar *help;

		help = g_strdup_printf ("Run '%s --help' to see a full "
					"list of available command line "
					"options", argv[0]);
		g_printerr ("%s\n%s\n", error->message, help);

		g_clear_error (&error);
		g_free (help);
		g_option_context_free (context);

		return 1;
	}
	g_option_context_free (context);

	meta_plugin_manager_set_plugin_type (arek_wm_get_type ());
	meta_set_gnome_wm_keybindings (GNOME_KBDS);
	meta_set_wm_name (WMNAME);

	g_setenv (GAIL_OPT, "1", TRUE);
	g_setenv (AT_BRIDGE_OPT, "1", TRUE);

	meta_init ();

	g_unsetenv (GAIL_OPT);
	g_unsetenv (AT_BRIDGE_OPT);

	meta_register_with_session ();

	return meta_run ();
}
