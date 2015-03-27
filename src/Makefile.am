-include $(top_srcdir)/common.mk

bin_PROGRAMS = \
	arek-wm

arek_wm_SOURCES = \
	arek-wm.c \
	background.c \
	keybindings.c \
	meta-wrapper.c \
	tile.c \
	windowlist.c \
	workspace.c \
	main.c

arek_wm_CFLAGS = \
	$(MUTTER_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(AM_CFLAGS)

arek_wm_LDADD = \
	$(MUTTER_LIBS) \
	$(GLIB_LIBS)

arek_wm_LDFLAGS = \
	-export-dynamic -no-undefined

