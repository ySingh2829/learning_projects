#include "database.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* add to database, function requires name, name's length and value */
void
add_db(struct DB **head, char *name, size_t name_len, int value)
{
    struct DB *new;
    new = malloc(sizeof(struct DB));
    if (new == NULL)
        exit(1);
    strncpy(new->name, name, name_len);
    new->value = value;
    new->next = *head;

    *head = new;
}

/* On success returns 0; on failure return -1 (possibly due to not finding record */
ssize_t
del_db(struct DB *list, char *name)
{
    struct DB *curr, *prev;

    for (curr = list, prev = NULL;
            curr != NULL;
            prev = curr, curr = curr->next) {
        if (strcmp(curr->name, name) == 0)
            break;
    }
    if (curr != NULL) {
        if (prev == NULL)
            list = list->next;
        else 
            prev->next = curr->next;
        free(curr);
        return 0;
    }
    else {
    return -1;
    }
}

/* Return -1 when record cannot be found, else 0 on success */
ssize_t
mod_db(struct DB *list, char *name, int value)
{
    struct DB *curr;

    for(curr = list; curr != NULL; curr = curr->next) {
        if (strcmp(curr->name, name) == 0)
            break;
    }

    if (curr != NULL) {
        curr->value = value;
        return 0;
    } else {
    return -1;
    }
}

/* Returns -1 when value couldn't be found */
ssize_t
ret_db(struct DB *list, char *name, char *ret_name, int *ret_value)
{   
    struct DB *curr;
    if (list == NULL) {
        return -1;
    }

    for(curr = list; curr != NULL; curr = curr->next) {
        if (strcmp(curr->name, name) == 0)
            break;
    }
        
    if (curr != NULL) { 
        *ret_value = curr->value;
        strcpy(ret_name, curr->name);
        return 0;
    }
    else {
     return -1;
    }
}

/* This functions clears the list and frees up any resources used by it */
void
free_db(struct DB *list)
{
    struct DB *curr;

    while (list != NULL) {
        curr = list;
        list = list->next;
        free(curr);
    }
}
