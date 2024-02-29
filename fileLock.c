#include "fileLock.h"


int file_lock_write(int fd, int start, off_t len)
{
    struct flock fl;
    fl.l_type = F_WRLCK; //several can acquire simultaneously
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = len;
    return fcntl(fd, F_SETLK, &fl);
}

int file_lock_read(int fd, int start, off_t len)
{
    struct flock fl;
    fl.l_type = F_RDLCK; //several can acquire simultaneously
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = len;
    return fcntl(fd, F_SETLK, &fl);
}

int file_unlock(int fd, int start, off_t len)
{
    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = len;
    return fcntl(fd, F_SETLK, &fl);
}
