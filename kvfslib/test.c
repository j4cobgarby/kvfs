#include <stdio.h>
#include "kvfslib.h"

int main() {
	struct kv_mnt kv1 = kvfs_attach("/dev/kvs1");
}
