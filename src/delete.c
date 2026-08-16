#include <stdlib.h>
#include <string.h>
#include "edufs_node.h"
#include "edufs_errno.h"


static int __delete_node(node_t *node)
{
    /* TASK-1: Ignore NULL nodes, these are already deleted */
    if (node == NULL)
        return 0;
    /* END: TASK-1 */

    /* TASK-2: Recursively delete the node's children. If node is a dir,
     * child_count would be non-negative (>=0).
     */
    for (int i = 0; i < node->child_count; ++i)
        __delete_node(node->children[i]);
    /* END: TASK-2 */

    /* TASK-3: Free node data and children memory */
    free(node->data);
    free(node->children);
    /* END: TASK-3 */

    /* TASK-4: Reset the node if it is the sysroot, else
     * free the node memory (sysroot node can't be deleted
     * since it is not malloc-ed memory) and the node name
     * memory.
     */
    if (node == &edufs_sysroot)
    {
        node->children = NULL;
        node->child_count = 0;
    }
    else
    {
        free(node->name);
        free(node);
    }
    /* END: TASK-4 */

    return 0;
}


int edufs_rmtree(node_t *root)
{
    /* Search for the root's name in the parent's children and
     * remove it, decrease the parent's children buffer by 1 and
     * decrease the parent's child_count by 1.
     */
    node_t *parent = root->parent;

    if (parent == NULL) // parent = NULL in case of sysroot (ignored)
        goto end;

    int i, cmp, found_index = -1;

    /*
    for (i = low; i < high; ++i)
    {
        mid = (high - low + 1) / 2;
        cmp = strcmp(root->name, (parent->children[mid])->name);
        if (cmp == 0)
        {
            printf ("Found!!\n");
            break;
        }
        else if (cmp < 0)
            high = mid - 1;
        else
            low = mid + 1;
    }
    */

    // TODO: Implement binary search instead of linear search.
    for (i = 0; i < parent->child_count; ++i)
    {
        cmp = strcmp(root->name, (parent->children[i])->name);
        if (cmp == 0)
        {
            found_index = i;
            break;
        }
    }

    for (i = found_index; i < (parent->child_count - 1); ++i)
        parent->children[i] = parent->children[i + 1];

    parent->children[parent->child_count - 1] = NULL;

    node_t **old_children = parent->children;
    parent->children = realloc(parent->children,
    (parent->child_count - 1) * sizeof(node_t*));

    if (parent->children == NULL)
        parent->children = old_children;

    parent->child_count--;

end:
    return __delete_node(root);
}


int edufs_remove(node_t *node)
{
    /* Delete file nodes or empty dir nodes only */

    if (node->child_count > 0)
    {
        edufs_errno = ERR_NODE_HAS_CHILDREN;
        return -1;
    }
    return __delete_node(node);
}