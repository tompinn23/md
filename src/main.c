#include "file.h"

#include "utils.h"

#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <string.h>

int load_file_source() {
	// void* so;
	// void* symptr;
	// const unsigned* abi_version_ptr;

	// so = dlopen("./fp_stdio.so", RTLD_NOW);
	// if(so == NULL) {
	// 	printf("fp_stdio: %s\n", dlerror());
	// }

	// struct file_plugin_ops* fp_ops;
	// fp_ops = dlsym(so, "fp_ops");
	// abi_version_ptr = dlsym(so, "fp_abi_ver");
	
	// if(*abi_version_ptr != FP_ABI_VER) {
	// 	printf("Plugin ABI mismatch error\n");
	// }
	
	// fp_ops->init();
	
	// char** files;
	// int rc = fp_ops->enumerate_files(&files, "/home/tom");
	// for(int i = 0; i < rc; i++) {
	// 	printf("%d: %s\n", i, files[i]);
	// }
	
	// fp_ops->deinit();
	// dlclose(so);
	// return 0;
}


int main(int argc, char** argv) {
	// load_file_source();
	fp_load_plugins();
	FILE* fp = fopen("/home/tom/md/build/test.txt", "r");
	if(fp == NULL) {
		d_print("WTF!");
	}
	struct file* f = file_open("file:///home/tom/md/build/text.txt", "r");
	if(f == NULL) {
		printf("Failed to open file\n");
		return -1;
	}
	char buf[256];
	int bytesRead = file_read(f, buf, 256);
	printf("Bytes read: %d", bytesRead);
	printf("String: %s\n", buf);
	file_close(f);
	fp_close_plugins();
}
