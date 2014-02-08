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
-------------------

It pulls in a large pile of Python code; if one's operating system is
sufficiently mimimal to not have Python, then you very likely have at
least GLib.
