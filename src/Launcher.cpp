#include <iostream>
#include <string.h>
#include <DirHandler.h>

int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      std::cout << "Usage: PhotoManager [-R] DIR_1 [DIR_2] ... [DIR_N]" << std::endl;
   }

   int index = 1;
   bool recursive = false;

   // Check if -R option has been provided.
   if (strncmp(argv[index], "-R", 2) == 0)
   {
       ++index;
       recursive = true;
   }

   while (index < argc)
   {
      CDirHandler manager = CDirHandler(argv[index], "", recursive);
      manager.Execute();
      ++index;
   }

   return 0;
}
