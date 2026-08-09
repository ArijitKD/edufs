#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

enum ErrorList {
    ERR_MALLOC_FAIL = -2,
    ERR_PARENT_NOT_DIRNODE = -3,
    ERR_INVALID_NODE_NAME = -4,
    ERR_INVALID_NODE_TYPE = -5,
    ERR_NULLPTR_RECEIVED = -6,
    ERR_NODE_EXISTS = -7
};

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

node_t rootdir = {
    .name = NULL,
    .name_size = 0,
    .data = NULL,
    .data_size = -1,
    .children = NULL,
    .child_count = 0,
    .parent = NULL
};
int edufs_errno = 0;


static int __get_node_type(node_t *node)
{   
    if (node == NULL)
        return -1;

    if (node->data_size >= 0 && node->child_count < 0)
        return FILE_NODE;

    if (node->data_size < 0 && node->child_count >= 0)
        return DIR_NODE;

    return -1;
}


node_t *create_child(node_t *parent, int type, const char *name)
{
    /* TASK-1: Check if parent is a valid directory node */
    if (__get_node_type(parent) != DIR_NODE)
    {
        edufs_errno = ERR_PARENT_NOT_DIRNODE;
        return NULL;
    }
    /* END: TASK-1 */

    /* TASK-2: Check if child node type is valid */
    if (type != FILE_NODE && type != DIR_NODE)
    {
        edufs_errno = ERR_INVALID_NODE_TYPE;
        return NULL;
    }
    /* END: TASK-2 */

    /* TASK-3: Validate child name received for naming */
    if (name == NULL)
    {
        edufs_errno = ERR_NULLPTR_RECEIVED;
        return NULL;
    }
    if (strcmp(name, "..") == 0 || strcmp(name, ".") == 0 ||
    strcmp(name, "") == 0)
    {
        edufs_errno = ERR_INVALID_NODE_NAME;
        return NULL;
    }
    size_t name_size = strlen(name);
    int i, forbidden_char_found = 0, same_name_found = 0;
    for (i = 0; i < name_size; ++i)
    {
        if (!(('A' <= name[i]  && 'Z' >= name[i]) ||
        ('a' <= name[i]  && 'z' >= name[i]) ||
        ('0' <= name[i]  && '9' >= name[i]) ||
        (name[i] == '#') || (name[i] == '$') || (name[i] == '%') ||
        (name[i] == '&') || (name[i] == '(') || (name[i] == ')') ||
        (name[i] == '*') || (name[i] == '+') || (name[i] == ',') ||
        (name[i] == '-') || (name[i] == '.') || (name[i] == ';') ||
        (name[i] == '=') || (name[i] == '@') || (name[i] == '[') ||
        (name[i] == ']') || (name[i] == '^') || (name[i] == '_') ||
        (name[i] == '{') || (name[i] == '}') ))
        {
            forbidden_char_found = 1;
            break;
        }
    }
    if (forbidden_char_found)
    {
        edufs_errno = ERR_INVALID_NODE_NAME;
        return NULL;
    }
    for (i = 0; i < parent->child_count; ++i)
    {
        if (strcmp((parent->children[i])->name, name) == 0)
        {
            same_name_found = 1;
            break;
        }
    }
    if (same_name_found)
    {
        edufs_errno = ERR_NODE_EXISTS;
        return NULL;
    }
    /* END: TASK-3 */

    /* TASK-4: Create the child */
    node_t *child = calloc(1, sizeof(node_t));
    if (child == NULL)
    {
        edufs_errno = ERR_MALLOC_FAIL;
        return NULL;
    }
    /* END: TASK-4 */
    
    /* TASK-5: Assign the child its name */
    char *child_name = malloc((name_size + 1) * sizeof(char));
    if (child_name == NULL)
    {
        edufs_errno = ERR_MALLOC_FAIL;
        free(child);
        return NULL;
    }
    strncpy(child_name, name, name_size);
    child_name[name_size] = '\0';
    child->name = child_name;
    child->name_size = name_size;
    /* END: TASK-5 */

    /* TASK-6: Update child depending on the given node type */
    if (type == DIR_NODE)
        child->data_size = -1;
    else if (type == FILE_NODE)
        child->child_count = -1;
    /* END: TASK-6 */

    /* TASK-7: Update child with parent info */
    child->parent = parent;
    /* END: TASK-7 */

    /* TASK-8: Make room for the new child */
    node_t **old_children = parent->children;
    parent->children = realloc(parent->children,
    (parent->child_count + 1) * sizeof(node_t*));
    if (parent->children == NULL)
    {
        parent->children = old_children;
        edufs_errno = ERR_MALLOC_FAIL;
        free(child->name);
        free(child);
        return NULL;
    }
    parent->children[parent->child_count] = NULL; // set new last elem to NULL
    parent->child_count += 1;
    /* END: TASK-8 */

    /* TASK-9: Put the new child in lexographical order of the parent's
     * children names.
     */
    int insert_index = parent->child_count - 1;
    for (i = 0; i < parent->child_count; ++i)
    {
        if ((parent->children[i] != NULL) &&
        strcmp((parent->children[i])->name, child->name) > 0)
        {
            insert_index = i;
            break;
        }
    }
    for (i = parent->child_count - 1; i > insert_index; --i)
    {
        parent->children[i] = parent->children[i - 1];
    }
    parent->children[insert_index] = child;
    /* END: TASK-9 */

    return child;
}
    

int main(void)
{
    node_t *new_node = create_child(&rootdir, DIR_NODE, "bin");
    int node_type = __get_node_type(new_node);

    if (node_type == DIR_NODE)
        printf("Created directory node.\n");
    else if (node_type == FILE_NODE)
        printf("Created file node.\n");
    else
        printf("Node not created.\n");

    return 0;
}
