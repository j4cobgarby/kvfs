#include "kvfslib.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct kv_mnt kvfs_attach(const char *mntpoint) {
    struct kv_mnt ret;
    size_t len_mntpnt = strlen(mntpoint);

    ret.mntpoint = calloc(len_mntpnt, sizeof(char));

    ret.mkfile  = calloc(len_mntpnt + strlen(MKFILE), sizeof(char));
    ret.delfile = calloc(len_mntpnt + strlen(DELFILE), sizeof(char));
    ret.incfile = calloc(len_mntpnt + strlen(INCFILE), sizeof(char));
    ret.decfile = calloc(len_mntpnt + strlen(DECFILE), sizeof(char));

    strcpy(ret.mntpoint, mntpoint);
    sprintf(ret.mkfile, "%s%s", mntpoint, MKFILE);
    sprintf(ret.delfile, "%s%s", mntpoint, DELFILE);
    sprintf(ret.incfile, "%s%s", mntpoint, INCFILE);
    sprintf(ret.decfile, "%s%s", mntpoint, DECFILE);

    ret.mntlen = len_mntpnt;
}

int key_exists(struct kv_mnt *mnt, const char *key) {
    char *keypath = calloc(strlen(key) + mnt->mntlen, sizeof(char));
    sprintf(keypath, "%s/%s", mnt->mntpoint, key);
    return access(keypath, F_OK);
}

int key_create(struct kv_mnt *mnt, const char *key) {
    FILE *f = fopen(mnt->mkfile, "w");
    fwrite(key, sizeof(char), strlen(key), f);
    fclose(f);

    return 0;
}

int key_delete(struct kv_mnt *mnt, const char *key) {
    FILE *f = fopen(mnt->delfile, "w");
    fwrite(key, sizeof(char), strlen(key), f);
    fclose(f);

    return 0;
}

int key_inc(struct kv_mnt *mnt, const char *key) {
    FILE *f = fopen(mnt->incfile, "w");
    fwrite(key, sizeof(char), strlen(key), f);
    fclose(f);

    return 0;
}

int key_dec(struct kv_mnt *mnt, const char *key) {
    FILE *f = fopen(mnt->decfile, "w");
    fwrite(key, sizeof(char), strlen(key), f);
    fclose(f);

    return 0;
}

int key_set_s(struct kv_mnt *mnt, const char *key, const char *data) {
    FILE *f;
    char *keypath = calloc(strlen(key) + mnt->mntlen, sizeof(char));
    sprintf(keypath, "%s/%s", mnt->mntpoint, key);
    
    if (!(f = fopen(keypath, "w"))) return -1;
    fwrite(data, sizeof(char), strlen(data), f);
    fclose(f);

    return 0;
}

int key_get_s(struct kv_mnt *mnt, const char *key, char *dest, size_t n) {
    FILE *f;
    int n_read;

    char *keypath = calloc(strlen(key) + mnt->mntlen, sizeof(char));
    sprintf(keypath, "%s/%s", mnt->mntpoint, key);

    if (!(f = fopen(keypath, "r"))) return -1;
    n_read = fread(dest, sizeof(char), n, f);
    fclose(f);

    return n_read == n;
}

int key_set_i(struct kv_mnt *mnt, const char *key, int data) {
    FILE *f;
    char *keypath = calloc(strlen(key) + mnt->mntlen, sizeof(char));
    sprintf(keypath, "%s/%s", mnt->mntpoint, key);
    
    if (!(f = fopen(keypath, "w"))) return -1;
    fprintf(f, "%d", data);
    fclose(f);

    return 0;
}

int key_get_i(struct kv_mnt *mnt, const char *key, int *n) {
    FILE *f;
    int n_read;
    
    char *keypath = calloc(strlen(key) + mnt->mntlen, sizeof(char));
    sprintf(keypath, "%s/%s", mnt->mntpoint, key);

    if (!(f = fopen(keypath, "r"))) return -1;
    n_read = fscanf(f, "%d", n);
    fclose(f);

    return n_read == 1;
}