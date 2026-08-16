#ifndef EDUFS_ERRNO_H
#define EDUFS_ERRNO_H

#define ERR_MALLOC_FAIL         -2
#define ERR_PARENT_NOT_DIRNODE  -3
#define ERR_INVALID_NODE_NAME   -4
#define ERR_INVALID_NODE_TYPE   -5
#define ERR_NULLPTR_RECEIVED    -6
#define ERR_NODE_EXISTS         -7

extern int edufs_errno;

#endif
