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

#define LIBINIT_SETUP_MONITORING_FAILED_ID "ffd4acb785f0976aeefca95d6433abb8"


gboolean
libinit_setup_query_ssh_authorized_keys (gboolean      *out_exists,
                                         GFileMonitor **out_monitor,
                                         GCancellable  *cancellable,
                                         GError       **error)
{
  gboolean ret = FALSE;
  gs_unref_object GFile *authorized_keys =
    g_file_new_for_path (LIBINIT_SETUP_ROOT_SSH_KEY_PATH);
  gs_unref_object GFileMonitor *ret_monitor = NULL;
  GFileMonitor *monitor;

  if (g_file_query_exists (authorized_keys, NULL))
    {
      *out_exists = TRUE;
    }
  else
    {
      gs_unref_object GFile *parentdir = g_file_get_parent (authorized_keys);
      
      if (g_file_query_exists (parentdir, NULL))
        {
          monitor = g_file_monitor_file (authorized_keys, 0, NULL, &local_error);
        }
      else
        {
        }

  if (!monitor)
    {
      gs_log_structured_print_id_v (LIBINIT_SETUP_MONITORING_FAILED_ID,
                                    "Failed to monitor '%s': %s",
                                    gs_file_get_path_cached (authorized_keys),
                                    local_error->message);
      g_error_free (local_error);
      return g_timeout_source_new (0);
    }
  g_signal_connect (monitor, "changed", G_CALLBACK (on_authorized_keys_changed), 
}
