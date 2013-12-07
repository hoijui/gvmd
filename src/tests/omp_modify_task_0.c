/* Test 0 of OMP MODIFY_TASK.
 * $Id$
 * Description: Test the OMP MODIFY_TASK command.
 *
 * Authors:
 * Matthew Mundell <matthew.mundell@greenbone.net>
 *
 * Copyright:
 * Copyright (C) 2009 Greenbone Networks GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2,
 * or, at your option, any later version as published by the Free
 * Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define TRACE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "../tracef.h"

int
main ()
{
  int socket, ret;
  gnutls_session_t session;
  char* id;

  setup_test ();

  socket = connect_to_manager (&session);
  if (socket == -1) return EXIT_FAILURE;

  if (omp_authenticate_env (&session))
    {
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Create a task. */

  if (omp_create_task_rc_file (&session,
                               "new_task_empty_rc",
                               "Test for omp_modify_task_0",
                               "Comment.",
                               &id))
    {
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Send a modify_task request. */

  if (openvas_server_sendf (&session,
                            "<modify_task"
                            " task_id=\"%s\">"
                            "<parameter id=\"comment\">"
                            "Modified comment."
                            "</parameter>"
                            "</modify_task>",
                            id)
      == -1)
    {
      omp_delete_task (&session, id);
      close_manager_connection (socket, session);
      free (id);
      return EXIT_FAILURE;
    }

  /* Read the response. */

  entity_t entity = NULL;
  read_entity (&session, &entity);

  /* Compare. */

  entity_t expected = add_entity (NULL, "modify_task_response", NULL);
  add_attribute (expected, "status", "200");
  add_attribute (expected, "status_text", "OK");

  if (compare_entities (entity, expected))
    ret = EXIT_FAILURE;
  else
    ret = EXIT_SUCCESS;

  /* Cleanup. */

  omp_delete_task (&session, id);
  close_manager_connection (socket, session);
  free_entity (entity);
  free_entity (expected);
  free (id);

  return ret;
}