include $(top_srcdir)/build/modmake.rulesam
MODNAME=mod_rasa

mod_LTLIBRARIES = mod_rasa.la
mod_rasa_la_SOURCES  = mod_rasa.c
mod_rasa_la_CFLAGS   = $(AM_CFLAGS)
mod_rasa_la_LIBADD   = $(switch_builddir)/libfreeswitch.la
mod_rasa_la_LDFLAGS  = -avoid-version -module -no-undefined -shared
