#include "fp.h"


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "mem.h"
#include "utils.h"

static struct file_plugin_data {
	unsigned int remote : 1;
		
	char** file_list;
	int sz_list;

	int errnum;	
} data;

struct file_data {
	char* uri;
	FILE* filep;
};



int init() {
	memset(&data, 0, sizeof(struct file_plugin_data));
	data.remote = 0;
	return 0;
}

int deinit() {
	for(int i =0 ; i < data.sz_list; i++) {
		mem_free(data.file_list[i]);
	}
	mem_free(data.file_list);
	return 0;
}

int enumerate_files(char*** file_list, const char* uri) {
	struct dirent* entry;
	int files = 0;
	DIR* dirp = opendir(uri);
	if(!dirp) {
		return FP_ENODIR;
	}
	while((entry = readdir(dirp))) {
		files++;
	}
	rewinddir(dirp);
	if(!data.file_list) {
		data.file_list = mem_malloc(files * sizeof(char*));
		data.sz_list = files;
	}
	else {
		if(files > data.sz_list) {
			data.file_list = mem_realloc(data.file_list, files);
			memset(data.file_list + data.sz_list, 0, files - data.sz_list);
			data.sz_list = files;
		}
	}
	int i = 0;
	while((entry = readdir(dirp))) {
		if(!data.file_list[i]) {
			data.file_list[i] = mem_malloc((NAME_MAX + 1) * sizeof(char));
		}
		strcpy(data.file_list[i], entry->d_name);	
		i++;
	}
	(*file_list) = data.file_list;
	return files;
}

struct file_data* open_file(const char* uri, const char* mode){
	struct file_data* fd = mem_zalloc(sizeof(struct file_data));
	d_print("opening file: %s", uri);
	fd->filep = fopen(uri, mode);
	fd->uri = string_make(uri);
	if(fd->filep == NULL) {
		d_print("Failed to open file: %s", strerror(errno));	
		mem_free(fd);
		switch(errno) {
			case EACCES:
				data.errnum = FP_EACCESS;
				break;
			case EINVAL:
				data.errnum = FP_EINVAL;
				break;
			case ENOENT:
				data.errnum = FP_ENOFILE;
				break;
		}
		return NULL;
	}
	return fd;
}

size_t read(struct file_data* fd, char* buffer, size_t count) {
	return fread(buffer, count, 1, fd->filep);	
}

size_t len(struct file_data* fd) {
	struct stat st;
	stat(fd->uri, &st);
	return st.st_size;
}

int seek(struct file_data* fd, size_t offset) {
	return fseek(fd->filep, offset, SEEK_SET);
}

int close_file(struct file_data* fd) {
	mem_free(fd->uri);
	int rc = fclose(fd->filep);
	if(rc == EOF) {
		rc = -1;
	}
	mem_free(fd);
	return rc;
}

int get_error() {
	return data.errnum;
}

const struct file_plugin_ops fp_ops = {
	.init = init,
	.deinit = deinit,

	.enumerate_files = enumerate_files,
	.open_file = open_file,
	.read = read,
	.len = len,
	.seek = seek,
	.close_file = close_file,
	.error = get_error
};

const char* const file_plugin_uri[] = { "file", NULL };
const unsigned fp_abi_ver = FP_ABI_VER;
