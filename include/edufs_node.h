#ifndef EDUFS_NODE_H
#define EDUFS_NODE_H

#include <stdint.h>


enum NodeTypes {
    FILE_NODE,
    DIR_NODE
};

struct Node {
    /* Malloc-ed char array for Node name */
    char *name;
    size_t name_size;

    /* If Node is a file, then it will contain data. The data is a malloc-ed
     * array of bytes. If it is a directory, data will be NULL and data_size
     * will be -1.
     */
    uint8_t *data;
    ssize_t data_size;

    /* If Node is a directory, then it will contain list of files and folders,
     * all of which are the possible next nodes or the children of the given
     * Node here. children is a malloc-ed array of next node pointers. If it
     * is a file, children will be NULL and child_count will be -1.
     */
    struct Node **children;
    ssize_t child_count;

    /* Previous node for upward traversal. A Node can have many children but
     * only ONE parent.
     */
    struct Node *parent;
    
    /* Note: A node is a:
     * File when data_size >= 0 and child_count < 0,
     * Directory when data_size < 0 and child_count >= 0, and
     * Invalid otherwise.
     */
};
typedef struct Node node_t;

extern node_t edufs_sysroot;
extern int edufs_errno;

int edufs_node_type(node_t *node);
node_t *edufs_mkchild(node_t *parent, int type, const char *name);

#endif
