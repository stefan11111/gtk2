Gtk 2

A fork of gtk2 without bloat, like atk and accessiblilty nonsense.

Further debloating is tricky to do because it would either break api, break actual features, or both.

There is also a patch to remove printing backends in the print-patches directory named remove-printing.patch.

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

translations (po and friends)

docs

examples

demos

tests
