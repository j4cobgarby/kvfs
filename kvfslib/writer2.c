#include <stdio.h>
#include "kvfslib.h"

int main() {
    struct kv_mnt *kv1 = kvfs_attach("/dev/kvs1");

    char keyname[5];

    printf("Waiting.\n");
    while (key_exists(kv1, "key9") != 0); // Wait for all the keys to exist
    printf("Going!\n");

    for (int i = 0; i < 1000; i++) {
        for (int k = 0; k < 10; k++) {
            sprintf(keyname, "key%d", k);
            key_inc(kv1, keyname);
        }
    }
}