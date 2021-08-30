# Dynamic Loader for Relocatable Object Files
This is a dynamic loader for 64 bit relocatable object files. You can use your object file as a library and load it to memory then invoke functions from it.
Unfortunately, it does not support the section dependecies, if your function uses variables that is not created in stack-frame then it will give you segmentation fault.

To use it, you need to define an ELF64 struct and then pass it to the mmap_file function with the object files name, then call init_elf64 function. After this, you can give the function name to get_function function. It will return you the function.


### Executable and Linkable Format
  The Executable and Linkable Format, also known as ELF, is the common 
  file format for executable files, object files, shared libraries and core dumps. It 
  defines the structure for binaries, libraries and core files. In 1999, it was chosen 
  as the standard binary file format for Unix and Unix-like systems on x86 
  processors by the 86open project. By design, the ELF format is flexible, 
  extensible, and cross-platform. ELF files are typically the output of a compiler 
  or linker and are a binary format. With the right tool like readelf, such files can 
  be analyzed and better understood.
### ELF Files
  1. Relocatable file contains code and data suitable for linking with other 
  object files to create an executable or a shared object file.
  2. Shared Object file contains code and data suitable for linking in two 
  contexts. First link editor may process it with other relocatable and shared 
  object files to create another object file. Second, the dynamic linker 
  combines it with an executable file and other shared objects to create a 
  process image.
  3. Executable file contains a program suitable for execution.
### Why would we wanna have loader for object files ? 
  There may be many reasons but the main reason is we may wanna be able 
to use relocatable object files as some sort of library.
