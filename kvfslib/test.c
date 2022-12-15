#include <stdio.h>
#include "kvfslib.h"

int main() {
	struct kv_mnt *kv1 = kvfs_attach("/dev/kvs1");

	char keyname[5];

	for (int i = 0; i < 10; i++) {
		sprintf(keyname, "key%d", i);
		printf("Making key %s\n", keyname);
		key_create(kv1, keyname);
		key_set_i(kv1, keyname, 1);
	}

	kvfs_free(kv1);
}
