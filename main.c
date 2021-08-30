#include <stdio.h>
#include "elf64Loader.h"

int main(void) {

  ELF64 *elf;
  const char* func_name = "max";
  int (*foo)(int,int);
  mmap_file("./elf64", elf);
  init_elf64(elf);
  foo = get_function(func_name, elf);

  printf("Function %s is invoked. The result is : %d\n", func_name, foo(1,2));

  func_name = "sum";

  foo = get_function(func_name, elf);

  printf("Function %s is invoked. The result is : %d\n", func_name, foo(12,324));

  func_name = "min";

  foo = get_function(func_name, elf);

  printf("Function %s is invoked. The result is : %d\n", func_name, foo(431,234));

  return 0;
}