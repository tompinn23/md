#pragma once

struct file;

struct file* file_open(const char* uri, const char* mode);
int file_read(struct file* f, char* buffer, int count);
int file_write(struct file* f, char* buffer, int count);
int file_len(struct file* f);
int file_seek(struct file* f, int offset);
int file_close(struct file* f);



void fp_load_plugins();
void fp_close_plugins();

void fp_select(const char* name);
