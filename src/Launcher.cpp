#include <iostream>
#include <DirHandler.h>

int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      std::cout << "Usage: PhotoManager DIR_1 [DIR_2] ... [DIR_N]" << std::endl;
   }

   int index = 1;
   while (index < argc)
   {
      CDirHandler manager = CDirHandler(argv[index], "");
      manager.Execute();
      ++index;
   }

   return 0;
}
