# Copyright (C) 2014 Colin Walters <walters@verbum.org>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/>.

include Makefile-decls.am

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

if HAVE_SYSTEMD
systemdservicedir = $(systemdsystemunitdir)
systemdservice_DATA =
endif

include Makefile-min-metadata-service.am
include Makefile-initial-setup-accounts.am
