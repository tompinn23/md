#include "file.h"

#include "fp.h"
#include "mem.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <dlfcn.h>

struct file_plugin {
	const char* name;

	const struct file_plugin_ops* fops;
	const char* const* uris;

	void* _so_handle;
};

static struct file_plugin** plugins;





static struct file_plugin* determine_plugin(const char* scheme) {
	for(int i = 0; plugins[i] != NULL; i++) {
		struct file_plugin* plug;
		int j = 0;
		while(plugins[i]->uris[j] != NULL) {	
			if(strcmp(plugins[i]->uris[j], scheme) == 0) {
				return plugins[i];
			}
			j++;
		}
	}
	d_print("Unknown uri scheme or parser fail: %s", scheme);
	return NULL;
}

struct file {
	const char* uri;
	
	struct file_plugin_ops* fops;
	struct file_data* private; 
};

void fp_load_plugins() {
	struct dirent* entry;
	//FIXME: Fix plugin directory.
	DIR* dirp = opendir(".");
	if(!dirp) {
		printf("Failed to open plugin dir: %s\n", strerror(errno));
		return;
	}
	int file_count = 1;
	while((entry = readdir(dirp))) {
			if(entry->d_type == DT_REG)
				file_count++;
	}
	rewinddir(dirp);
	plugins = mem_zalloc(file_count * sizeof(struct file_plugin*));
	int i = 0;
	while((entry = readdir(dirp))) {
		bool err = false;
		struct file_plugin* plug; 
		char* ext;
		char filename[512];
		if(entry->d_name[0] == '.') {
			continue;
		}
		ext = strrchr(entry->d_name, '.');
		if(ext == NULL) {
			continue;
		}
		if(strcmp(ext, ".so")) 
			continue;

		snprintf(filename, sizeof(filename), "%s/%s", ".", entry->d_name);
		void* so;
		const unsigned* abi_version_ptr;
		so = dlopen(filename, RTLD_NOW);
		if(so == NULL) {
			printf("%s: %s\n", filename, dlerror());
			continue;
		}	
		plug = mem_zalloc(sizeof(struct file_plugin));
		plug->name = string_split(entry->d_name, ".");
		plug->fops = dlsym(so, "fp_ops");
		plug->uris = dlsym(so, "file_plugin_uri");
		abi_version_ptr = dlsym(so, "fp_abi_ver");
		d_print("name: %s, fops: %x, uris: %x\n", plug->name, plug->fops, plug->uris);
		if(!plug->fops || !plug->uris) {
			err = true;
			printf("Failed to load plugin missing features.\n");
		}
		if(abi_version_ptr == NULL || *abi_version_ptr != FP_ABI_VER) {
			err = true;
			printf("Plugin ABI version mismatch.\n");
		}
		if(err) {
			printf("Plugin fail: %s\n", plug->name); 
			dlclose(so);
			mem_free(plug->name);
			mem_free(plug);
			continue;
		}
		plug->_so_handle = so;
		plugins[i] = plug;
		i++;
	}

}

void fp_close_plugins() {
	struct file_plugin* plug;
	for(int i = 0; (plug = plugins[i]) != NULL; i++) {
		mem_free(plug->name);
		dlclose(plug->_so_handle);
		mem_free(plug);
	}
	mem_free(plugins);
}






struct file* file_open(const char* uri, const char* mode) {
	bool err = false;
	struct file* file = mem_zalloc(sizeof(struct file_plugin));
	char* uri_copy = string_make(uri);
	char* scheme = strtok(uri_copy, ":");
	struct file_plugin* plug = determine_plugin(scheme);
	if(plug == NULL) {
		err = true;
	}
	else {
		char *pos = strstr(uri, "://") + 3;
		file->uri = string_make(uri);
		file->private = plug->fops->open_file(pos, mode);
		if(file->private == NULL) {
			err = true;	
		}
	}
	mem_free(uri_copy);
	if(err) {
		mem_free(file);
		switch(plug->fops->error()) {
			case FP_EINVAL:
				d_print("Failed to open file: invalid mode");
				break;
			case FP_EACCESS:
				d_print("Failed to open file: lack of perms");
				break;
			case FP_ENOFILE:
				d_print("Failed to open file: No file exists");
				break;
		}
		return NULL;
	}
	return file;
}

int file_read(struct file* f, char* buffer, int count) {
	return f->fops->read(f->private, buffer, count);
}

int file_write(struct file* f, char* buffer, int count) {
	d_print("Error not implemented.");
	return 0;
}

int file_len(struct file* f) {
	return f->fops->len(f->private);
}

int file_seek(struct file* f, int offset) {
	return f->fops->seek(f->private, offset);
}

int file_close(struct file* f) {
	return f->fops->close_file(f->private);
}
