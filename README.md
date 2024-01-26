Gtk 2

A fork of gtk2 without bloat, like atk and accessiblilty nonsense.

Further debloating is tricky to do because it would either break api, break actual features, or both.

IMPORTANT: Printing backends are disabled by default and need patching to work.
Grep for DISABLE_PRINTING_BACKENDS to enable, or revert https://github.com/stefan11111/gtk2/commit/5bf1e89aa6dc9c08d6c25c5e41389f86a3b1a7bd

Here is a list of some of the bloated "features" of gtk2 that I got rid of:

atk and other accessiblity stuff, like gail and friends.

dbus, gdbus, and everything else that matches *dbus* and is not a variable or comment.

win32 (windows) support

quartz (apple) support

ms-windows engine

engines modules (pixbuf)

immodules

gtk-builder-convert

translations (po and friends)

docs

examples

demos

tests
