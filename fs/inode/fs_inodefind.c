/****************************************************************************
 * fs/inode/fs_inodefind.c
 *
 *   Copyright (C) 2007-2009 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <string.h>
#include <assert.h>
#include <errno.h>

#include <nuttx/fs/fs.h>

#include "inode/inode.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: inode_find and indode_find_nofollow
 *
 * Description:
 *   This is called from the open() logic to get a reference to the inode
 *   associated with a path.
 *
 *   Both versions will follow soft links in path leading up to the terminal
 *   node.  inode_find() will deference that terminal node,
 *   indode_find_nofollow no follow will not.
 *
 ****************************************************************************/

FAR struct inode *inode_find(FAR const char *path, FAR const char **relpath)
{
  struct inode_search_s desc;
  FAR struct inode *node = NULL;
  int ret;

  if (path == NULL || *path != '/')
    {
      return NULL;
    }

  /* Find the node matching the path.  If found, increment the count of
   * references on the node.
   */

  memset(&desc, 0, sizeof(struct inode_search_s));
  desc.path = path;

  inode_semtake();
  ret = inode_search(&desc);
  if (ret >= 0)
    {
      /* Found it */

      node = desc.node;
      DEBUGASSERT(node != NULL);

      /* Increment the reference count on the inode */

      node->i_crefs++;

      /* Return any remaining relative path fragment */

      if (relpath != NULL)
        {
          *relpath = desc.relpath;
        }
    }

  inode_semgive();
  return node;
}

#ifdef CONFIG_PSEUDOFS_SOFTLINKS
FAR struct inode *inode_find_nofollow(FAR const char *path,
                                      FAR const char **relpath)
{
  struct inode_search_s desc;
  FAR struct inode *node = NULL;
  int ret;

  if (path == NULL || *path != '/')
    {
      return NULL;
    }

  /* Find the node matching the path.  If found, increment the count of
   * references on the node.
   */

  memset(&desc, 0, sizeof(struct inode_search_s));
  desc.path = path;

  inode_semtake();
  ret = inode_search_nofollow(&desc);
  if (ret >= 0)
    {
      /* Found it */

      node = desc.node;
      DEBUGASSERT(node != NULL);

      /* Increment the reference count on the inode */

      node->i_crefs++;

      /* Return any remaining relative path fragment */

      if (relpath != NULL)
        {
          *relpath = desc.relpath;
        }
    }

  inode_semgive();
  return node;
}
#endif
