#pragma once
#include <fcntl.h>

int file_lock_write(int fd, int start, off_t len);
int file_lock_read(int fd, int start, off_t len);
int file_unlock(int fd, int start, off_t len);
