min-metadata-service
====================

This is a minimal implementation of the AWS metadata API in C; at
present, it only implements the
`/2009-04-04/meta-data/public-keys/0/openssh-key` request.

Near term future goals:

 * hostname


Why not cloud init?
-------------------

It pulls in an enormous pile of Python code; if one's operating system
is sufficiently mimimal to not have Python, then you very likely have
at least GLib.
