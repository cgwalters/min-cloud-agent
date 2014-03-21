/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*-
 *
 * Copyright (C) 2014 Colin Walters <walters@verbum.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <libgsystem.h>
#include <glib-unix.h>
#include <unistd.h>
#include <gio/gio.h>

GSource *
libinit_setup_on_have_root_ssh_keys (GFunc           func,
				     gpointer        user_data)
{
  gs_unref_object GFile *authorized_keys =
    g_file_new_for_path (LIBINIT_SETUP_ROOT_SSH_KEY_PATH);
  gs_unr

}
