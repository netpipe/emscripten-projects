#ifndef isomaster_h
#define isomaster_h

#include <gtk/gtk.h>

#include "bk/bk.h"
#include "window.h"
#include "browser.h"
#include "fsbrowser.h"
#include "isobrowser.h"
#include "settings.h"
#include "error.h"
#include "boot.h"
#include "about.h"
#include "editfile.h"

#ifdef ENABLE_NLS
#  include <libintl.h>
#  define _(str) gettext(str)
#  ifdef gettext_noop
#    define N_(str) gettext_noop (str)
#  else
#    define N_(str) (str)
#  endif
#else
#  define _(str) (str)
#  define N_(str) (str)
#endif

/* c99 doesn't define the following (posix file types) */
#ifndef S_IFDIR
#  define S_IFDIR  0040000
#endif
#ifndef S_IFREG
#  define S_IFREG  0100000
#endif

#endif
