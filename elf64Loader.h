#ifndef _ELF64LOADER_H_
#define _ELF64LOADER_H_

#include <elf.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>

typedef struct {
  Elf64_Ehdr *e_hdr;
  Elf64_Phdr *p_hdr;
  Elf64_Shdr *s_hdr;
  uint8_t *base;
}ELF64;

void mmap_file(const char* file, ELF64 *elf);
void init_elf64(ELF64 *elf);
Elf64_Shdr* get_section(const char* section, ELF64 *elf);
uint64_t page_align(uint64_t n, uint64_t page_size);
void *get_function(const char* func_name, ELF64 *elf);
bool checkMagic(Elf64_Ehdr* elf);
void print_elf_type(ELF64* elf);
uint8_t* get_shstrtab(const ELF64* elf);
bool is_section_valid(const char* s, ELF64 *elf);
Elf64_Sym * get_symbol(ELF64* elf);

#endif