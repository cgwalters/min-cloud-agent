min-metadata-service
====================

In contrast to cloud-init, the vision of this project is to be an
ultra-small bootstrapping stage that one can use to start other
management tools.

At present, it implements a very small subset of the AWS EC2 metadata API.
The list of requests implemented is:

	`/2009-04-04/meta-data/public-keys/0/openssh-key`: Provision a single ssh key to the `root` account

In the near future, it will also implement the `user-data` request,
just for shell script.  This will allow you to directly call into the
operating system tools (like package managers) to install e.g.  Ruby
or Python for a configuration management tool, or to use `curl` to
download further code.

Dependencies
------------

 * GLib: https://git.gnome.org/browse/glib/
 * libgsystem: https://git.gnome.org/browse/libgsystem/

Planned features
----------------

 * user-data
 * configurable account for ssh keys
 * hostname

Why create this instead of just using `cloud-init`?
---------------------------------------------------

cloud-init is a very featureful program - it has many data providers
and backends.  This leads to a fuzzy line between what it does and
what other operating system management and configuration tools like
Puppet should do.  It means cloud-init is likely to keep growing.

Beyond simply being a large program in itself, cloud-init is written
Python, and `min-metadata-service` is targeted at small host operating
systems that don't necessarily include Python by default.

Contributing
------------

Use Github pull requests.  You can discuss this program on the Fedora
cloud list: https://admin.fedoraproject.org/mailman/listinfo/cloud

License
-------

min-metadata-service is licensed under the LGPLv2+.  See the COPYING
file for more information.
