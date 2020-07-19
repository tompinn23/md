#pragma once

#include <stddef.h>

#define FP_ABI_VER 1

enum {
	FP_ENODIR,
	FP_EINVAL,
	FP_EACCESS,
	FP_ENOFILE
};

struct file_data;


struct file_plugin_ops {
	int (*init)();
	int (*deinit)();

	int (*enumerate_files)(char*** files_list, const char* uri);
	struct file_data* (*open_file)(const char* uri, const char* mode);
	size_t (*read)(struct file_data* fd, char* buffer, size_t count);
	size_t (*len)(struct file_data* fd);
	int (*seek)(struct file_data* fd, size_t offset);
	int (*close_file)(struct file_data* fd); 
	
	int (*error)(void);
};


extern const struct file_plugin_ops fp_ops;
extern const char* const file_plugin_uri[];
extern const unsigned fp_abi_ver;

