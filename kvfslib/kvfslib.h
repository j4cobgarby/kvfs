#ifndef KVFSLIB_H
#define KVFSLIB_H

#include <stdio.h>

#define MKFILE "/_mk"
#define DELFILE "/_del"
#define INCFILE "/_inc"
#define DECFILE "/_dec"

struct kv_mnt {
    char *mntpoint;
    char *mkfile;
    char *delfile;
    char *incfile;
    char *decfile;
    size_t mntlen;
};

struct kv_mnt *kvfs_attach(const char *mntpoint);
void kvfs_free(struct kv_mnt *);

int key_exists(struct kv_mnt *, const char *key);
int key_create(struct kv_mnt *, const char *key);
int key_delete(struct kv_mnt *, const char *key);

int key_set_s(struct kv_mnt *, const char *key, const char *data);
int key_set_i(struct kv_mnt *, const char *key, int data);

int key_get_s(struct kv_mnt *, const char *key, char *dest, size_t n);
int key_get_i(struct kv_mnt *, const char *key, int *dest);

int key_inc(struct kv_mnt *, const char *key);
int key_dec(struct kv_mnt *, const char *key);

#endif