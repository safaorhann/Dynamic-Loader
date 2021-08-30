#include "elf64Loader.h"
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

//checks the file if it is valid elf.
bool checkMagic(Elf64_Ehdr* elf) {
  char ElfMagic[] = {0x7f, 'E', 'L', 'F', '\0'};
  return (memcmp(elf->e_ident, ElfMagic, strlen(ElfMagic))) == 0;
}

//This func prints the ELF file type.
void print_elf_type(ELF64* elf){
  switch(elf->e_hdr->e_type)
  {
    case ET_NONE:
      printf("ELF TYPE : No File Type\n");
    case ET_REL:
      printf("ELF TYPE : Relocatable File Type\n");
    case ET_EXEC:
      printf("ELF TYPE : Executable File Type\n");
    case ET_DYN:
      printf("ELF TYPE : Shared Object File Type\n");
    case ET_CORE:
      printf("ELF TYPE : Core File Type\n");
    default:
      printf("ELF TYPE : Processor-specific File Type\n");
  }
}

//this func opens the file given the name, finds the size of it and then maps it to the memory. After mapping the file, it sets the base address of the mapped file to the ELF32 structs base member.
void mmap_file(const char* file, ELF64 *elf){
  int fd = open(file, O_RDONLY);
  if (fd <= 0) {
      perror("Cannot open the ELF file.");
      exit(errno);
  }
  struct stat stat;
  fstat(fd, &stat);

  elf->base = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

  if (elf->base == MAP_FAILED){
      perror("Maping the ELF file failed.");
      exit(errno);
  }
  close(fd);
}

//using the ELF64 structs base member, it sets the elf header, section header and program header member of the struct.
void init_elf64(ELF64 *elf){
  elf->e_hdr = (Elf64_Ehdr *)elf->base;
  if(elf->e_hdr->e_phoff != 0)
    elf->p_hdr = (Elf64_Phdr *)(elf->base + elf->e_hdr->e_phoff);
  else{
    elf->p_hdr = NULL;
  }
  elf->s_hdr = (Elf64_Shdr *)(elf->base + elf->e_hdr->e_shoff);
}

//returns the section header string table, can be used to check section name.
uint8_t* get_shstrtab(const ELF64* elf){
  return (elf->base + elf->s_hdr[elf->e_hdr->e_shstrndx].sh_offset);
}

//given the section name, it returns the section of given ELF32 struct if there is otherwise returns NULL.
Elf64_Shdr* get_section(const char* section, ELF64 *elf){
  uint8_t *shstrtab = get_shstrtab(elf);

  for (int i = 0; i < elf->e_hdr->e_shnum; i++){
    const char *section_name = (const char*) (shstrtab + elf->s_hdr[i].sh_name);

    size_t len = strlen(section_name);

    if (strlen(section) == len && !strcmp(section, section_name)) {
        if (elf->s_hdr[i].sh_size)
            return elf->s_hdr + i;
        }
  }
  return NULL;
}

//checks if there is a section by given the name.
bool is_section_valid(const char* s, ELF64 *elf){
    return get_section(s, elf) != NULL;
}

//it returns the symbol table of the given ELF32 struct.
Elf64_Sym * get_symbol(ELF64* elf){
    const Elf64_Shdr *symtab = get_section(".symtab", elf);
    return (Elf64_Sym *)(elf->base + symtab->sh_offset);
}

uint64_t page_align(uint64_t n, uint64_t page_size)
{
    return (n + (page_size - 1)) & ~(page_size - 1);
}

//It returns the function given the name from ELF64 struct. It maps the .text to memory and then make it executable. And then using the symbol table it returns the function.
void *get_function(const char* func_name, ELF64 *elf){
  Elf64_Sym *symbols;
  uint64_t page_size;
  uint8_t *text_base;
  
  const Elf64_Shdr *symtab = get_section(".symtab", elf);
  if (!symtab) {
    perror("Failed to find .symtab\n");
    exit(ENOEXEC);
  }

  symbols = (Elf64_Sym *)(elf->base + symtab->sh_offset);
  int num_symbols = symtab->sh_size / symtab->sh_entsize;

  Elf64_Shdr *strtab = get_section(".strtab", elf);
    if (!strtab) {
        perror("Failed to find .strtab\n");
        exit(ENOEXEC);
    }

  char* strtab_ = (char *)(elf->base + strtab->sh_offset);

  page_size = sysconf(_SC_PAGESIZE);

  Elf64_Shdr *text = get_section(".text", elf);
  if (!text) {
      perror("Failed to find .text\n");
      exit(ENOEXEC);
  }

  text_base = mmap(NULL, page_align(text->sh_size, page_size), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (text_base == MAP_FAILED) {
      perror("Failed to allocate memory for .text");
      exit(errno);
  }


  memcpy(text_base, elf->base + text->sh_offset, text->sh_size);

  if (mprotect(text_base, page_align(text->sh_size, page_size), PROT_READ | PROT_EXEC)) {
      perror("Failed to make .text executable");
      exit(errno);
    }

  size_t name_len = strlen(func_name);

  for (int i = 0; i < num_symbols; i++){
    if (ELF64_ST_TYPE(symbols[i].st_info) == STT_FUNC) {
      const char *function_name = strtab_ + symbols[i].st_name;

      size_t function_name_len = strlen(function_name);

      if (name_len == function_name_len && !strcmp(func_name, function_name)) {
          return (void*) (text_base + symbols[i].st_value);
      }
    }
  }
  return NULL;
}