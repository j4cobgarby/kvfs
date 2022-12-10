#ifndef KVFSLIB_H
#define KVFSLIB_H

#include <stdio.h>

struct kv_mnt {
    const char *mntpoint;
};

int key_exists(struct kv_mnt *, const char *key);
int key_create(struct kv_mnt *, const char *key);
int key_delete(struct kv_mnt *, const char *key);

int key_set_s(struct kv_mnt *, const char *key, const char *data);
int key_set_i(struct kv_mnt *, const char *key, int data);

int key_get_s(struct kv_mnt *, const char *key, const char *dest);
int key_get_i(struct kv_mnt *, const char *key, int &dest);

int key_inc(struct kv_mnt *, const char *key, int &dest);
int key_dec(struct kv_mnt *, const char *key, int &dest);

#endif