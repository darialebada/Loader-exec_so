/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "exec_parser.h"

static so_exec_t *exec;
static struct sigaction default_handler;
static int fd;

/* my code */
static void segv_handler(int signum, siginfo_t *info, void *context)
{
	/* address at which fault occurred */
	uintptr_t fault_addr = (uintptr_t)info->si_addr;

	/* number of bytes in a page */
	int page_size = getpagesize();

	/* find the segment where fault occurred */
	for (int i = 0; i < exec->segments_no; i++) {
		if (fault_addr >= exec->segments[i].vaddr
			&& fault_addr < exec->segments[i].vaddr + exec->segments[i].mem_size) {
			/* the page fault is in the current segment */

			if (info->si_code != SEGV_MAPERR) {
				/* address is already mapped -> default handler */
				default_handler.sa_sigaction(signum, info, context);
				return;
			}

			/* find page index (where the page fault is) */
			int page_fault_idx = (fault_addr - exec->segments[i].vaddr) / page_size;

			/* where to mapp = address of page start */
			uintptr_t page_addr = exec->segments[i].vaddr + page_fault_idx * page_size;

			/* address not mapped */
			uintptr_t map_addr = mmap((void *)page_addr, page_size, PERM_W | PERM_R, MAP_FIXED | MAP_SHARED | MAP_ANONYMOUS, 0, 0);

			if (map_addr == MAP_FAILED) {
				perror("mmap failed");
				exit(EXIT_FAILURE);
			}

			int file_read;
			unsigned int file_size = exec->segments[i].file_size;

			/* size from vaddr to beginning of page with fault */
			int size = page_fault_idx * page_size;

			if (size < file_size && size + page_size <= file_size) {
				/* page is totally in file size - no zeros */
				int offset = exec->segments[i].offset + page_fault_idx * page_size;

				/* read data from file */
				lseek(fd, offset, SEEK_SET);
				file_read = read(fd, map_addr, page_size);
				if (file_read < 0) {
					perror("read failed");
					exit(EXIT_FAILURE);
				}

			} else if (file_size <= size) {
				/* page is totally in zero zone */
				memset((void *)map_addr, 0, page_size);

			} else if (size < file_size && size + page_size > file_size) {
				/* page is partly in zero zone */
				int offset = exec->segments[i].offset + page_fault_idx * page_size;

				/* read data from file */
				lseek(fd, offset, SEEK_SET);
				file_read = read(fd, map_addr, page_size);
				if (file_read < 0) {
					perror("read failed");
					exit(EXIT_FAILURE);
				}

				/* put zeros on remaining part of the page */
				int size_remained = size + page_size - file_size;

				memset((void *)exec->segments[i].vaddr + file_size, 0, size_remained);
			}
			/* change permisions for the file */
			file_read = mprotect((void *)map_addr, page_size, exec->segments[i].perm);
			return;
		}
	}
	/* cannot find page in a segment -> default handler */
	default_handler.sa_sigaction(signum, info, context);
}

/* skeleton provided by the Operating System team */
int so_init_loader(void)
{
	int rc;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = segv_handler;
	sa.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &sa, &default_handler);
	if (rc < 0) {
		perror("sigaction");
		return -1;
	}
	return 0;
}

int so_execute(char *path, char *argv[])
{
	exec = so_parse_exec(path);
	if (!exec)
		return -1;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		perror("open failed");
		exit(EXIT_FAILURE);
	}

	so_start_exec(exec, argv);

	return -1;
}
