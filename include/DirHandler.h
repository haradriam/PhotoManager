#include <list>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

class CDirHandler
{
public:
   CDirHandler(std::string dir_name, std::string prefix, bool recursive);
   ~CDirHandler();
   int Execute();

public:
   // Root directory.
   std::string m_dir_name;

   // Full prefix.
   std::string m_prefix;

   // Recursive indicator.
   bool m_recursive;

   // List of directories.
   std::list<std::string> m_list_dirs;

   // Start date.
   std::string m_sdate;
   // End date.
   std::string m_edate;
};

