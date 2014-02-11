min-metadata-service
====================

This is a minimal implementation of the AWS metadata API in C; at
present, it only implements the
`/2009-04-04/meta-data/public-keys/0/openssh-key` request.

Dependencies
------------

 * GLib: https://git.gnome.org/browse/glib/
 * libgsystem: https://git.gnome.org/browse/libgsystem/

Planned features
----------------

 * hostname

Why not `cloud-init`?
---------------------

It pulls in a large pile of Python code; if one's operating system is
sufficiently mimimal to not have Python, then you very likely have at
least GLib.

Why not shell script?
---------------------

Shell is OK for about 10 lines of code.  Beyond that, one should
really turn to real programming languages.  C is fine for this, and
GLib makes it pleasant enough.
