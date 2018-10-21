#include <DirHandler.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

CDirHandler::CDirHandler(std::string dir_name, std::string prefix)
   : m_dir_name(dir_name)
   , m_prefix(prefix)
   , m_sdate("")
   , m_edate("")
{
   m_prefix.append("-");
   m_prefix.append(dir_name);
}

CDirHandler::~CDirHandler()
{
}

int CDirHandler::Execute()
{
   DIR *dir = NULL;
   struct dirent *node = NULL;

   // Change to directory to be processed.
   if (chdir(m_dir_name.c_str()) != 0)
   {
      std::cout << "Error moving to directory " << m_dir_name.c_str() << std::endl;
      return -1;
   }

   // Open directory.
   dir = opendir(".");
   if (!dir)
   {
      std::cout << "Error opening directory " << m_dir_name << std::endl;
      return -2;
   }

   std::cout << "\n\nDIRECTORY: " << m_dir_name << std::endl;

   struct stat file_attr;
   struct tm *data = NULL;
   std::string name = "";
   std::string ext = "";
   char date[16];
   while ((node = readdir(dir)))
   {
      // Rename file.
      if (node->d_type == DT_REG)
      {
         // Get metada.
         if (stat(node->d_name, &file_attr) != 0 ||
             !(data = gmtime(&file_attr.st_mtime)))
         {
            std::cout << "Error getting file metada from " << node->d_name
               << std::endl;
            return -3;
         }

         // Get file extension.
         name = node->d_name;
         ext = name.substr(name.find_last_of(".") + 1);

         // Generate date information.
         snprintf(date, 16, "%d%.2d%.2d_%.2d%.2d%.2d",
            1900 + data->tm_year, data->tm_mon + 1, data->tm_mday,
            data->tm_hour, data->tm_min, data->tm_sec);

         // Update directoy date limitis.
         if (m_sdate.empty() || m_sdate.compare(date) > 0) { m_sdate = date; }
         if (m_edate.empty() || m_edate.compare(date) < 0) { m_edate = date; }

         // Generate name.
         name.clear();
         name.append(date);
         name.append(m_prefix);
         if (!ext.empty())
         {
            name.append(".");
            name.append(ext);
         }

         // Check if file with the same name already exists.
         int index = 0;
         while (access(name.c_str(), F_OK) == 0)
         {
            if (index == 0)
            {
               if (!ext.empty()) { name.erase(name.find_last_of(".")); }
               name.append("-");
            }
            else
            {
               name.erase(name.find_last_of("-") + 1);
            }

            name.append(std::to_string(index));

            if (!ext.empty())
            {
               name.append(".");
               name.append(ext);
            }

            ++index;
         }

         // Rename file.
         if (rename(node->d_name, name.c_str()) != 0)
         {
            std::cout << "Error renaming file " << node->d_name << std::endl;
            return -4;
         }

         std::cout << "Porcessed file: " << node->d_name << " --> "
            << name << std::endl;
      }
      // Store directory to be processed latter.
      else if (node->d_type == DT_DIR)
      {
         if (strcmp(node->d_name, ".") == 0 || strcmp(node->d_name, "..") == 0)
            continue;

         m_list_dirs.push_back(std::string(node->d_name));
         std::cout << "Subdirectory added: " << node->d_name << std::endl;
      }
      // Unexpected file type.
      else
      {
         std::cout << "Unprocessed file: " << node->d_name << std::endl;
      }
   }

   // Close directory.
   closedir(dir);

   // Process subdirectories.
   while (m_list_dirs.size() > 0)
   {
      CDirHandler sub_dir = CDirHandler(m_list_dirs.front(), m_prefix);
      if (sub_dir.Execute() < 0)
      {
         std::cout << "Error processing directory " << m_list_dirs.front()
            << std::endl;
         return -5;
      }
      m_list_dirs.pop_front();
   }

   // Change back to parent directory.
   if (chdir("..") != 0)
   {
      std::cout << "Error moving to directory " << m_dir_name.c_str() << std::endl;
      return -1;
   }

   // Rename direcoty.
   name.clear();
   name.append(m_sdate);
   if (m_sdate.compare(m_edate) != 0)
   {
      name.append("_");
      name.append(m_edate);
   }
   name.append("-");
   name.append(m_dir_name);
   if (rename(m_dir_name.c_str(), name.c_str()) != 0)
   {
      std::cout << "Error renaming directory " << node->d_name << std::endl;
      return -4;
   }

   return 0;
}

