#include <sys/stat.h>
#include <fcntl.h>
#include "syscall.h"
#include "libc.h"

int lstat(const char *restrict path, struct stat *restrict buf)
{
#if defined(SYS_lstat)
	return syscall(SYS_lstat, path, buf);
#elif defined(SYS_fstatat)
	return syscall(SYS_fstatat, AT_FDCWD, path, buf, AT_SYMLINK_NOFOLLOW);
#else
	return syscall(SYS_statx, AT_FDCWD, path,
			AT_SYMLINK_NOFOLLOW | AT_STATX_SYNC_AS_STAT,
			STATX_BASIC_STATS, buf);
#endif
}

LFS64(lstat);
