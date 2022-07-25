#ifndef DATABASE_H
#define DATABASE_H

#define STR_LEN 30
#include <sys/types.h>

struct DB {
    char name[STR_LEN];
    u_int32_t value;
    struct DB *next;
};

 
void add_db(struct DB **head, char *name, size_t name_len, int value);
ssize_t del_db(struct DB *list, char *name);
ssize_t mod_db(struct DB *list, char *name, int value);
ssize_t ret_db(struct DB *list, char *name, char *retrive_name, int *retrive_value);
void free_db(struct DB *head);

#endif
