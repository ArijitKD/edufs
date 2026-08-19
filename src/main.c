#include <stdio.h>
#include "edufs.h"

int edufs_errno = 0;

int main(void)
{
    printf("edufs initialized.\n\n");

    printf("Contents of /:\n");
    for (int i = 0; i < edufs_sysroot.child_count; ++i)
    {
        printf("%s\tType: %s\n", (edufs_sysroot.children[i])->name,
        (edufs_node_type(edufs_sysroot.children[i]) == FILE_NODE)? "File" : "Dir");
    }
    if (edufs_sysroot.child_count == 0)
    {
        printf("[Empty directory]\n");
    }
    putchar('\n');

    node_t *bindir = edufs_mkchild(&edufs_sysroot, DIR_NODE, "bin");
    printf("Created directory /bin.\n");

    edufs_mkchild(&edufs_sysroot, FILE_NODE, "swap");
    printf("Created file /swap.\n");

    edufs_mkchild(&edufs_sysroot, DIR_NODE, "etc");
    printf("Created directory /etc.\n");

    edufs_mkchild(&edufs_sysroot, DIR_NODE, "var");
    printf("Created directory /var.\n");

    putchar('\n');

    edufs_mkchild(bindir, DIR_NODE, "hack");
    printf("Created directory /bin/hack.\n");

    edufs_mkchild(bindir, FILE_NODE, "cc");
    printf("Created file /bin/cc.\n");

    edufs_mkchild(bindir, FILE_NODE, "bash");
    printf("Created file /bin/bash.\n");

    putchar('\n');

    printf("Contents of /:\n");
    for (int i = 0; i < edufs_sysroot.child_count; ++i)
    {
        printf("%s\tType: %s\n", (edufs_sysroot.children[i])->name,
        (edufs_node_type(edufs_sysroot.children[i]) == FILE_NODE)? "File" : "Dir");
    }

    putchar('\n');

    for (int j = 0; j < edufs_sysroot.child_count; ++j)
    {
        node_t *node = edufs_sysroot.children[j];

        if (edufs_node_type(node) == DIR_NODE)
        {
            printf("Contents of /%s (directory):\n", node->name);
            if (node->child_count == 0)
            {
                printf("[Empty directory]\n");
            }
            else
            {
                for (int i = 0; i < node->child_count; ++i)
                {
                    printf("%s\tType: %s\n", (node->children[i])->name,
                    (edufs_node_type(node->children[i]) == FILE_NODE)? "File" : "Dir");
                }
            }
        }
        else
        {
            printf ("/%s is a file.\n", node->name);
        }
        putchar('\n');
    }

    if (edufs_rmtree(bindir) == 0)
    {
        printf("Directory /bin deleted successfully.\n");
    }
    else
    {
        printf("edufs_rmtree() failed with error code: %d\n", edufs_errno);
        goto err;
    }
    putchar('\n');

    printf("Contents of /:\n");
    for (int i = 0; i < edufs_sysroot.child_count; ++i)
    {
        printf("%s\tType: %s\n", (edufs_sysroot.children[i])->name,
        (edufs_node_type(edufs_sysroot.children[i]) == FILE_NODE)? "File" : "Dir");
    }
    putchar('\n');

    if (edufs_rmtree(&edufs_sysroot) == 0)
    {
        printf("System root has been wiped.\n");
    }
    else
    {
        printf("edufs_rmtree() failed with error code: %d\n", edufs_errno);
        goto err;
    }
    putchar('\n');

    printf("Contents of /:\n");
    for (int i = 0; i < edufs_sysroot.child_count; ++i)
    {
        printf("%s\tType: %s\n", (edufs_sysroot.children[i])->name,
        (edufs_node_type(edufs_sysroot.children[i]) == FILE_NODE)? "File" : "Dir");
    }
    if (edufs_sysroot.child_count == 0)
    {
        printf("[Empty directory]\n");
    }
    putchar('\n');

    return 0;

err:
    return -1;
}
