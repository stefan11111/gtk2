#ifndef __GTKINTL_H__
#define __GTKINTL_H__

#define P_(String) (String)

/* not really I18N-related, but also a string marker macro */
#define I_(string) g_intern_static_string (string)

#define N_(String) (String)

#define _(String) (String)

#define NC_(String1, String2) (String2)

#define C_(String1, String2) (String2)

#define GETTEXT_PACKAGE ""

#endif
