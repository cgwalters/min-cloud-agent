Currently not actively developed
--------------------------------

Note: this repository is not under active development; the author
is working instead on https://cloudinit.readthedocs.org/en/latest/

min-cloud-agent
===============

In contrast to cloud-init, the vision of this project is to be an
ultra-small bootstrapping stage that one can use to start other
management tools.

At present, it implements a very small subset of the AWS EC2 metadata
API (which is also provided by the OpenStack hypervisor).

The list of requests implemented is:

 * `/2009-04-04/user-data`:

    Execute arbitrary script (must start with #!, e.g. #!/bin/bash).  The
    best practice is for this script to be idempotent, but min-cloud-agent
    will make a best effort to ensure this script runs exactly once when
    the system is first booted.

 * `/2009-04-04/meta-data/public-keys/0/openssh-key`:

    Provision a single ssh key to the `root` account's `.ssh/authorized_keys` file.
    If the file already exists, it will not be modified.

In contrast to cloud-init, the user-data must be an interpreted
program; it can not be the YAML cloud-config, have includes, or
anything like that.

For example, suppose you want to add a user to the system.  Rather
than using cloud init's "users" attribute in the cloud-config YAML,
you should simply invoke the operating system native tool,
e.g. `/usr/sbin/useradd` directly in a shell script.

It's strongly recommended that all operating system vendors shipping
min-cloud-agent include the `curl` utility.  This will allow user-data
script authors to download further code from a URL, or perform a POST
to registration/state server announcing that the system is online.

Another very useful thing to do in user-data is to call into the
system software management (e.g. yum/apt-get/rpm-ostree) to perform
upgrades or install further applications (such as Docker) or higher
level configuration management tools (such as Puppet).

min-cloud-agent will never grow an abstraction layer for any of these
things; if you are scripting multiple operating systems or versions,
your userdata scripts must carry the abstractions.

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
Python, and this agent is targeted at small host operating systems
that don't necessarily include Python by default.

Contributing
------------

Use Github pull requests.  You can discuss this program on the Fedora
cloud list: https://admin.fedoraproject.org/mailman/listinfo/cloud

License
-------

min-cloud-agent is licensed under the LGPLv2+.  See the COPYING
file for more information.
