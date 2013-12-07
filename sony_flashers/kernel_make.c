#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>

int main(int argc, char **argv) {
	int f_krn, f_cpio, f_cmdline, f_out;
	struct stat st_new_kernel, st_new_cpio, st_new_cmdline, target_file;
	unsigned char *kernel_data, *cpio_data, *cmdline_data, *null_data;
	unsigned int krn_size, cpio_start, cpio_size, cmdline_start, cmdline_size, null_start=0, null_size=0;
	unsigned int krn_start = 0x00001000;

	Elf32_Ehdr header;
	Elf32_Phdr *phdr;

	int i;

	printf("Sony dual kernel flasher v 0.1 by LeTama\n\n");
	printf("Sony dual kernel flasher v 0.1 modified by munjeni\n\n");

	if (argc != 5) {
		printf("Syntax: make_kernel PATH_TO/zImage PATH_TO/initrd.gz PATH_TO/cmdline PATH_TO_EXISTING/boot.img\n\n");
		return -1;
	}

	if (stat(argv[4], &target_file) == -1) {
		fprintf(stderr, "Error, unable to access file %s\n", argv[4]);
		return -1;
	}
       printf("opening %s\n%s size = 0x%08X\n\n", argv[4], argv[4], (unsigned int)target_file.st_size);

	if (stat(argv[1], &st_new_kernel) == -1) {
		fprintf(stderr, "Error, unable to access kernel file %s\n", argv[1]);
		return -1;
	}

	if (st_new_kernel.st_size < 500*1024) {
		fprintf(stderr, "Error, kernel size (0x%08X) < 500 KB, invalid file ?\n", (unsigned int)st_new_kernel.st_size);
		return -1;
	}

	if (stat(argv[2], &st_new_cpio) == -1) {
		fprintf(stderr, "Error, unable to access cpio file %s\n", argv[2]);
		return -1;
	}
                         
	if (st_new_cpio.st_size == 0 ) {
		fprintf(stderr, "Error, empty cpio, invalid file ?\n");
		return -1;
	}

	if (stat(argv[3], &st_new_cmdline) == -1) {
		fprintf(stderr, "Error, unable to access cmdline file %s\n", argv[3]);
		return -1;
	}
                         
	if (st_new_cmdline.st_size == 0 ) {
		fprintf(stderr, "Error, empty cmdline, invalid file ?\n");
		return -1;
	}

	f_krn = open(argv[1], O_RDONLY);

	if (f_krn == -1) {
		fprintf(stderr, "Error, unable to open kernel file %s\n", argv[1]);
		return -1;
	}

	f_cpio  = open(argv[2], O_RDONLY);

	if (f_cpio == -1) {
		fprintf(stderr, "Error, unable to open cpio file %s\n", argv[2]);
		return -1;
	}

	f_cmdline  = open(argv[3], O_RDONLY);

	if (f_cmdline == -1) {
		fprintf(stderr, "Error, unable to open cmdline file %s\n", argv[3]);
		return -1;
	}

	printf("New kernel size = 0x%08X\n", (unsigned int)st_new_kernel.st_size);
	printf("New cpio size = 0x%08X\n", (unsigned int)st_new_cpio.st_size);
	printf("New cmdline size = 0x%08X\n\n", (unsigned int)st_new_cmdline.st_size);

	printf("Reading new kernel\n");
	kernel_data = malloc(st_new_kernel.st_size);

	if (kernel_data == NULL) {
		fprintf(stderr, "Error, not enough memory to read kernel\n");
		return -1;
	}

	if (read(f_krn, kernel_data, st_new_kernel.st_size) != st_new_kernel.st_size) {
		fprintf(stderr, "Error, unable to read input kernel\n");
		return -1;
	}
	close(f_krn);

	printf("Reading new cpio\n");

	cpio_data = malloc(st_new_cpio.st_size);

	if (cpio_data == NULL) {
		fprintf(stderr, "Error, not enough memory to read cpio\n");
		return -1;
	}

	if (read(f_cpio, cpio_data, st_new_cpio.st_size) != st_new_cpio.st_size) {
		fprintf(stderr, "Error, unable to read input cpio\n");
		return -1;
	}
	close(f_cpio);

	printf("Reading new cmdline\n");

	cmdline_data = malloc(st_new_cmdline.st_size);

	if (cmdline_data == NULL) {
		fprintf(stderr, "Error, not enough memory to read cmdline\n");
		return -1;
	}

	if (read(f_cmdline, cmdline_data, st_new_cmdline.st_size) != st_new_cmdline.st_size) {
		fprintf(stderr, "Error, unable to read input cmdline\n");
		return -1;
	}
	close(f_cmdline);

	printf("\nOpening target %s\n", argv[4]);

	f_out = open(argv[4], O_RDWR | O_SYNC);

	if (f_out  == -1){
		fprintf(stderr, "Error, unable to open %s for writing\n", argv[4]);
		return -1;
	}

	read(f_out, &header, sizeof(header));
	if (header.e_ident[0] != ELFMAG0 || header.e_ident[1] != ELFMAG1 || header.e_ident[2] != ELFMAG2 || header.e_ident[3] != ELFMAG3) {
		printf("ELF magic not found, exiting\n");
		return -1;
	}

	printf("ELF magic found\n\n");
	printf("Entry point          : 0x%08X\n", header.e_entry);
	printf("Program Header start : 0x%x\n", header.e_phoff);
	printf("Program Header size  : %d\n", header.e_phentsize);
	printf("Program Header count : %d\n\n" , header.e_phnum);
  
	phdr = malloc(sizeof(Elf32_Phdr) * header.e_phnum);
	lseek(f_out, header.e_phoff, SEEK_SET);
	for (i = 0; i < header.e_phnum ; i++) {
		read(f_out, &phdr[i], sizeof(Elf32_Phdr));
		printf("PH[%d], type=%d, offset=%08X, virtual=%08X, phy=%08X, size=0x%08X\n", i,
				 phdr[i].p_type,
				 phdr[i].p_offset,
				 phdr[i].p_vaddr,
				 phdr[i].p_paddr,
				 phdr[i].p_filesz
		);
	}
	printf("\n");

	printf("Kernel section found\n\n");

	krn_size = (unsigned int) st_new_kernel.st_size;

	lseek(f_out, 0x44, SEEK_SET);
	if (write(f_out, &krn_size, sizeof(unsigned int)) != sizeof(unsigned int)) {
		fprintf(stderr, "Error, unable to write kernel size\n");
		return -1;
	}

	lseek(f_out, 0x48, SEEK_SET);
	if (write(f_out, &krn_size, sizeof(unsigned int)) != sizeof(unsigned int)) {
		fprintf(stderr, "Error, unable to write kernel size\n");
		return -1;
	}

	printf("kernel       start: 0x%08X\n", krn_start);
	printf("             size : 0x%08X\n\n", krn_size);

	cpio_start = krn_start + krn_size;

	lseek(f_out, 0x58, SEEK_SET);
	if (write(f_out, &cpio_start, sizeof(unsigned int)) != sizeof(unsigned int)) {
		fprintf(stderr, "Error, unable to write cpio start\n");
		return -1;
	}

	cpio_size = (unsigned int) st_new_cpio.st_size;

	lseek(f_out, 0x64, SEEK_SET);
	if (write(f_out, &cpio_size, sizeof(unsigned int)) != sizeof(unsigned int)) {
		fprintf(stderr, "Error, unable to write cpio size\n");
		return -1;
	}

	lseek(f_out, 0x68, SEEK_SET);
	if (write(f_out, &cpio_size, sizeof(unsigned int)) != sizeof(unsigned int)) {
		fprintf(stderr, "Error, unable to write cpio size\n");
		return -1;
	}

	printf("cpio         start: 0x%08X\n", cpio_start);
	printf("             size : 0x%08X\n\n",  cpio_size);

	cmdline_start = krn_start + krn_size + cpio_size;

	lseek(f_out, 0x78, SEEK_SET);
	if (write(f_out, &cmdline_start, sizeof(unsigned int)) != sizeof(unsigned int)) {
		fprintf(stderr, "Error, unable to write cmdline start\n");
		return -1;
	}

 	cmdline_size = (unsigned int) st_new_cmdline.st_size;

	lseek(f_out, 0x84, SEEK_SET);
	if (write(f_out, &cmdline_size, sizeof(unsigned int)) != sizeof(unsigned int)) {
		fprintf(stderr, "Error, unable to write cmdline size\n");
		return -1;
	}

	lseek(f_out, 0x88, SEEK_SET);
	if (write(f_out, &cmdline_size, sizeof(unsigned int)) != sizeof(unsigned int)) {
		fprintf(stderr, "Error, unable to write cmdline size\n");
		return -1;
	}

	// now, write data
	lseek(f_out, krn_start, SEEK_SET);

	if (write(f_out, kernel_data, krn_size) != (ssize_t)krn_size) {
		fprintf(stderr, "Error, unable to write kernel\n");
		return -1;
	}

	if (lseek(f_out,  cpio_start, SEEK_SET) != (off_t)(cpio_start)) {
		fprintf(stderr, "Error, unable to move to cpio location\n");
		return -1;
	}

	if (write(f_out, cpio_data, cpio_size) != (ssize_t)cpio_size) {
		fprintf(stderr, "Error, unable to write cpio\n");
		return -1;
	}

	printf("cmdline      start: 0x%08X\n", cmdline_start);
	printf("             size : 0x%08X\n\n", cmdline_size);

	if (lseek(f_out,  cmdline_start, SEEK_SET) != (off_t)(cmdline_start)) {
		fprintf(stderr, "Error, unable to move to cmdline location\n");
		return -1;
	}

	if (write(f_out, cmdline_data, cmdline_size) != (ssize_t)cmdline_size) {
		fprintf(stderr, "Error, unable to write cmdline\n");
		return -1;
	}

	free(cmdline_data);

	null_start += cmdline_start + cmdline_size;

	if (null_start > target_file.st_size) {
		fprintf(stderr, "Error, cmdline out of range!\n");
		return -1;
	}

	null_size += target_file.st_size - null_start;

	printf("null         start: 0x%08X\n", null_start);
	printf("             size : 0x%08X\n\n",  null_size);

	if (lseek(f_out,  null_start, SEEK_SET) != (off_t)(null_start)) {
		fprintf(stderr, "Error, unable to move to null_start location\n");
		return -1;
	}

	null_data = malloc(null_size);

	if (null_data == NULL) {
		fprintf(stderr, "Error, not enough memory for null_data\n");
		return -1;
	}

	null_data[null_size] = '\x00';

	if (write(f_out, null_data, null_size) != (ssize_t)null_size) {
		fprintf(stderr, "Error, unable to write null_data\n");
		return -1;
	}
	
	printf("targed file %s is patched sucesfully, enjoy new kernel!\n", argv[4]);

	return 0;
}
