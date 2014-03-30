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

#include <glib/gi18n.h>

#define INITIAL_SETUP_DONE_MSGID "73de127e872e6c6e1a46160a6cc286b5"

typedef struct {
  GFile *initial_setup_done_file;
} InitialSetupAppData;

static GHashTable *
parse_os_release (const char *contents,
                  const char *split)
{
  char **lines = g_strsplit (contents, split, -1);
  char **iter;
  GHashTable *ret = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

  for (iter = lines; *iter; iter++)
    {
      char *line = *iter;
      char *eq;
      const char *quotedval;
      char *val;

      if (g_str_has_prefix (line, "#"))
        continue;
      
      eq = strchr (line, '=');
      if (!eq)
        continue;
      
      *eq = '\0';
      quotedval = eq + 1;
      val = g_shell_unquote (quotedval, NULL);
      if (!val)
        continue;
      
      g_hash_table_insert (ret, line, val);
    }

  return ret;
}

static void
do_initial_setup_thread (GTask           *task,
                         gpointer         source_object,
                         gpointer         task_data,
                         GCancellable    *cancellable)
{
  gboolean ret = FALSE;
  GError *local_error = NULL;
  GError **error = &local_error;
  gs_unref_object GFile *etc_os_release =
    g_file_new_for_path ("/etc/os-release");
  gs_free char *contents = NULL;
  gs_unref_hashtable GHashTable *osrelease_values = NULL;
  const char *pretty_name;
  const char *passwd_root_argv[] = { "passwd", "root", NULL };
  gsize len;
  guint i;
  const guint max_passwd_attempts = 5;

  if (!g_file_load_contents (etc_os_release, cancellable,
                             &contents, &len, NULL, error))
    {
      g_prefix_error (error, "Reading /etc/os-release: ");
      goto out;
    }

  osrelease_values = parse_os_release (contents, "\n");
  pretty_name = g_hash_table_lookup (osrelease_values, "PRETTY_NAME");
  if (!pretty_name)
    pretty_name = g_hash_table_lookup (osrelease_values, "ID");
  if (!pretty_name)
    pretty_name = " (/etc/os-release not found)";

  g_print (_("Welcome to %s"), pretty_name);
  g_print ("\n");

  (void) readline (_("Press Enter to begin setup"));

  g_print (_("No user accounts found, and root password is locked"));
  g_print ("\n");

  for (i = 0; i < max_passwd_attempts; i++)
    {
      int estatus;

      if (!g_spawn_sync (NULL, (char**)passwd_root_argv, NULL,
                         G_SPAWN_CHILD_INHERITS_STDIN,
                         NULL, NULL, NULL, NULL,
                         &estatus, error))
        goto out;

      if (g_spawn_check_exit_status (estatus, NULL))
        break;
    }
  if (i == max_passwd_attempts)
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                   "Failed to run \"passwd root\" after %u attempts",
                   max_passwd_attempts);
      goto out;
    }

  ret = TRUE;
 out:
  if (ret)
    g_task_return_boolean (task, ret);
  else
    g_task_return_error (task, local_error);
}

static void
initial_setup_async (GApplication        *app,
                     GCancellable        *cancellable,
                     GAsyncReadyCallback  callback,
                     gpointer             user_data)
{
  GTask *task;

  task = g_task_new (app, cancellable, callback, user_data);
  g_task_run_in_thread (task, do_initial_setup_thread);
}

static gboolean
initial_setup_finish (GApplication         *app,
                      GAsyncResult         *result,
                      GError              **error)
{
  g_return_val_if_fail (g_task_is_valid (result, app), FALSE);
  return g_task_propagate_boolean ((GTask*)result, error);
}

static void
on_initial_setup_complete (GObject          *src,
                           GAsyncResult     *result,
                           gpointer          user_data)
{


}

static gboolean
write_initial_setup_done (GFile             *path,
                          GCancellable      *cancellable,
                          GError           **error)
{
  gboolean ret = FALSE;

  if (!g_file_replace_contents (path, "", 0, NULL, FALSE,
                                G_FILE_CREATE_REPLACE_DESTINATION, NULL,
                                cancellable, error))
    goto out;

  gs_log_structured_print_id_v (INITIAL_SETUP_DONE_MSGID,
                                "Initial setup complete");

  ret = TRUE;
 out:
  return ret;
}

int
main (int    argc,
      char **argv)
{
  GError *local_error = NULL;
  GError **error = NULL;
  GCancellable *cancellable = NULL;
  gs_unref_object GApplication *app = NULL;
  InitialSetupAppData *appdata = NULL;
  gs_unref_variant GVariant *user_list = NULL;
  gs_unref_variant GVariant *root_account_reply = NULL;
  gs_unref_object GDBusProxy *accountsservice = NULL;
  gs_unref_object GDBusProxy *root_account_properties = NULL;
  gs_unref_variant GVariant *root_account_locked_reply = NULL;
  gs_unref_variant GVariant *root_account_locked_property = NULL;
  gs_unref_variant GVariant *root_account_locked_value = NULL;
  gs_unref_object GFile *root_authorized_keys_path = NULL;
  const char *root_account_path = NULL;
  gboolean have_user_accounts;
  gboolean root_is_locked;

#if 0
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

  g_setenv ("GIO_USE_VFS", "local", TRUE);

  app = g_application_new (NULL, G_APPLICATION_NON_UNIQUE);
  appdata = g_new0 (InitialSetupAppData, 1);
  appdata->initial_setup_done_file = g_file_new_for_path ("/var/lib/initial-setup-accounts/done");

  if (g_file_query_exists (appdata->initial_setup_done_file, NULL))
    goto out;
  
  accountsservice = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM, 0, NULL,
                                                   "org.freedesktop.Accounts",
                                                   "/org/freedesktop/Accounts",
                                                   "org.freedesktop.Accounts",
                                                   cancellable,
                                                   error);
  if (!accountsservice)
    goto out;

  user_list = g_dbus_proxy_call_sync (accountsservice, "ListCachedUsers",
                                      NULL, 0, -1, cancellable, error);
  if (!user_list)
    goto out;
  
  have_user_accounts = g_variant_n_children (user_list) > 0;
  
  root_account_reply = g_dbus_proxy_call_sync (accountsservice,
                                               "FindUserById",
                                               g_variant_new ("(t)", (guint64)0),
                                               0, -1,
                                               cancellable, error);
  if (!root_account_reply)
    goto out;
  if (!g_variant_is_of_type (root_account_reply, G_VARIANT_TYPE ("(o)")))
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                   "Unexpected return type from FindUserById");
      goto out;
    }
  g_variant_get (root_account_reply, "(&o)", &root_account_path);

  root_account_properties =
    g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM, 0, NULL,
                                   g_dbus_proxy_get_name_owner (accountsservice),
                                   root_account_path,
                                   "org.freedesktop.DBus.Properties",
                                   cancellable,
                                   error);
  if (!root_account_properties)
    goto out;
  
  root_account_locked_reply =
    g_dbus_proxy_call_sync (root_account_properties,
                            "Get", g_variant_new ("(ss)", "org.freedesktop.Accounts.User", "Locked"),
                            0, -1, cancellable, error);
  if (!root_account_locked_reply)
    goto out;
  if (!g_variant_is_of_type (root_account_locked_reply, G_VARIANT_TYPE ("(v)")))
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                   "Unexpected return type from property Get");
      goto out;
    }
  g_variant_get (root_account_locked_reply, "(v)", &root_account_locked_value);
  root_account_locked_property = g_variant_get_variant (root_account_locked_value);
  if (!g_variant_is_of_type (root_account_locked_property, G_VARIANT_TYPE ("b")))
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                   "Unexpected non-boolean value for Locked");
      goto out;
    }
  
  root_is_locked = g_variant_get_boolean (root_account_locked_property);

  root_authorized_keys_path = g_file_new_for_path ("/root/.ssh/authorized_keys");

  if (!(have_user_accounts && root_is_locked &&
        !g_file_query_exists (root_authorized_keys_path, NULL)))
    {
      initial_setup_async (app, cancellable, on_initial_setup_complete, NULL);
    }
  else
    {
      if (!write_initial_setup_done (app, cancellable, error))
        goto out;
    }
  
 out:
  if (appdata)
    {
      g_clear_object (&appdata->initial_setup_done_file);
      g_free (appdata);
    }
  if (local_error != NULL)
    {
      g_printerr ("error: %s\n", local_error->message);
      g_error_free (local_error);
      return 1;
    }
  return 0;
}
