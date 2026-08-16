#ifndef EDUFS_H
#define EDUFS_H

#include "edufs_errno.h"
#include "edufs_node.h"

int edufs_rmtree(node_t *root);
int edufs_remove(node_t *node);

#endif
