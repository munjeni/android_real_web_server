#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char **argv) {
	Elf32_Ehdr header;
	Elf32_Phdr *phdr;
	int i, fld_cbck;
	char fld[256];
	struct stat bootimg_file;

	extern int __system(const char *command);

	printf("Sony kernel dumper by Munjeni @ XDA 2013\n\n");

	if (argc != 3) {
		printf("Syntax: kernel_dump OUTPUT_FOLDER /dev/block/BOOT_TPARTITION\n\n");
		return -1;
	}

	sprintf(fld, "%s/", argv[1]);
	if (0 != access(fld, F_OK)) {
		if (ENOENT == errno) {
			sprintf(fld, "mkdir %s", argv[1]);
			fld_cbck = __system(fld);
			if (fld_cbck == 0) {
				printf("\nCreated ouput folder %s\n\n", argv[1]);
			} else {
				printf("\nFAILURE to create output folder %s! Pllease try another folder!\n\n", argv[1]);
				return -1;
			}
		}

		if (ENOTDIR == errno) {
			printf("\nFAILURE to create output folder '%s' because there is file called '%s'!!!\n"
				"Try another one folder!\n\n", argv[1], argv[1]);
			return -1;
		}

	} else
		printf("\nUsing existing folder %s\n\n", argv[1]);

	sprintf(fld, "dd if=%s of=%s/boot.img", argv[2], argv[1]);
	fld_cbck = __system(fld);
	if (fld_cbck == 0) {
		printf("\nDumped boot.img(%s) to %s\n\n", argv[2], argv[1]);
	} else {
		printf("\nFAILURE to dump boot.img to %s! Pllease try another folder!\n\n", argv[1]);
		return -1;
	}

	sprintf(fld, "%s/boot.img", argv[1]);
	if (stat(fld, &bootimg_file) == -1) {
		fprintf(stderr, "Error, unable to access file %s\n", fld);
		return -1;
	}
	printf("opening %s\n\n", fld);

	FILE *fi = fopen(fld, "r");
	fread(&header, 1, sizeof(header), fi);
	if(header.e_ident[0] != ELFMAG0 || header.e_ident[1] != ELFMAG1 || header.e_ident[2] != ELFMAG2 || header.e_ident[3] != ELFMAG3) {
		printf("ELF magic not found, exiting\n");
		return 1;
	}

	printf("ELF magic found\n");
	printf("Entry point          : 0x%08X\n", header.e_entry);
	printf("Program Header start : 0x%x\n", header.e_phoff);
	printf("Program Header size  : %d\n", header.e_phentsize);
	printf("Program Header count : %d\n" , header.e_phnum);

	phdr = malloc(sizeof(Elf32_Phdr) * header.e_phnum);
	fseek(fi, header.e_phoff, SEEK_SET);
	for(i = 0; i < header.e_phnum ; i++) {
		fread(&phdr[i], 1, sizeof(Elf32_Phdr), fi);
		printf("-> PH[%d], type=%d, offset=%08X, virtual=%08X, phy=%08X, size=%d(0x%08X)\n",
					 i,
					 phdr[i].p_type,
					 phdr[i].p_offset,
					 phdr[i].p_vaddr,
					 phdr[i].p_paddr,
					 phdr[i].p_filesz,
					 phdr[i].p_filesz
		);
	}

	for(i = 0; i < header.e_phnum; i++) {
		char fname[256];
		FILE *fo;
		char *buff;

		buff = malloc(phdr[i].p_filesz);

		sprintf(fname, "%d", i);
		switch (i) {
			case 0:
				sprintf(fname, "%s/zImage", argv[1]);
				break;
			case 1:
				sprintf(fname, "%s/initrd.gz", argv[1]);
				break;
			case 2:
				sprintf(fname, "%s/cmdline", argv[1]);
				break;
			case 3:
				sprintf(fname, "%s/certificate", argv[1]);
				break;
			default:
				sprintf(fname, "%s/unknown.%d.bin", argv[1], i);
				break;
		}
		printf("...dumping to %s\n", fname);
		fo = fopen(fname, "w");
		fseek(fi, phdr[i].p_offset, SEEK_SET);
		fread(buff, 1, phdr[i].p_filesz, fi);
		fwrite(buff, 1, phdr[i].p_filesz, fo);
		fclose(fo);
		free(buff);
	}

	return 0;
}
