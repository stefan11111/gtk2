Gtk 2

A fork of gtk2 without bloat, like atk and accessiblilty nonsense.

Further debloating is tricky to do because it would either break api, break actual features, or both.

Printing support is optional and disabled by default.

To enable, pass --enable-print to the configure script.

This is needed even for the backends file and lpr.

Here are some text files I removed from the main repo: https://github.com/stefan11111/gtk2-textfiles

Here is a list of some of the bloated "features" of gtk2 that I got rid of:

atk and other accessiblity stuff, like gail and friends.

dbus, gdbus, and everything else that matches `*dbus*` and is not a variable or comment.

win32 (windows) support

quartz (apple) support

ms-windows engine

engine modules (pixbuf)

immodules

gtk-builder-convert

gettext

translations (po and friends)

docs

examples

demos

tests
