#include <DirHandler.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <libexif/exif-data.h>

CDirHandler::CDirHandler(std::string dir_name, std::string prefix, bool recursive)
   : m_dir_name(dir_name)
   , m_prefix(prefix)
   , m_recursive(recursive)
   , m_sdate("")
   , m_edate("")
{
   if (!recursive)
   {
      m_prefix.append("-");
      m_prefix.append(dir_name);
   }
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
   char date[9];
   char hour[7];
   while ((node = readdir(dir)))
   {
      // Rename file.
      if (node->d_type == DT_REG)
      {
         date[0] = '\0';
         hour[0] = '\0';

         // Get file extension.
         name = node->d_name;
         ext = name.substr(name.find_last_of(".") + 1);

         // Try to get EXIF metadata.
         ExifData * metadata = exif_data_new_from_file(node->d_name);
         if (metadata)
         {
            ExifEntry * entry = exif_content_get_entry(
               metadata->ifd[EXIF_IFD_EXIF], EXIF_TAG_DATE_TIME_ORIGINAL);
            if (entry)
            {
               char field[32];
               memset(field, 0, 32);

               exif_entry_get_value(entry, field, 32);
               if (strlen(field) > 0)
               {
                  char * token = NULL;
                  size_t token_len = 0;
                  int del_char = 0;
                  unsigned int i = 0;


                  token = strtok(field, " ");
                  token_len = strlen(token);
                  for (i = 0, del_char = 0; i < token_len && i - del_char < 8; ++i)
                  {
                      if (token[i] == ':' || token[i] == '-') { ++del_char; }
                      else { date[i - del_char] = token[i]; }
                  }
                  date[8] = '\0';

                  token = strtok(NULL, " ");
                  token_len = strlen(token);
                  for (i = 0, del_char = 0; i < token_len && i - del_char < 6; ++i)
                  {
                      if (token[i] == ':') { ++del_char; }
                      else { hour[i - del_char] = token[i]; }
                  }
                  hour[6] = '\0';
               }
            }

            exif_data_unref(metadata);
         }

         // Get file metada (only if EXIF metada is not correct).
         if (strlen(date) != 8 || strlen(hour) != 6)
         {
            if (stat(node->d_name, &file_attr) != 0 ||
                !(data = localtime(&file_attr.st_mtime)))
            {
               std::cout << "Error getting file metada from " << node->d_name
                  << std::endl;
               return -3;
            }

            // Generate date information.
            snprintf(date, 9, "%d%.2d%.2d",
               1900 + data->tm_year, data->tm_mon + 1, data->tm_mday);
            snprintf(hour, 7, "%.2d%.2d%.2d",
               data->tm_hour, data->tm_min, data->tm_sec);
         }

         // Update directoy date limits.
         if (m_sdate.empty() || m_sdate.compare(date) > 0) { m_sdate = date; }
         if (m_edate.empty() || m_edate.compare(date) < 0) { m_edate = date; }

         // Generate name.
         name.clear();
         name.append(date);
         name.append("_");
         name.append(hour);
         name.append(m_prefix);
         if (!ext.empty())
         {
            name.append(".");
            name.append(ext);
         }

         // Check if file with the same name already exists.
         int index = 0;
         char ch_num[4];
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

            snprintf(ch_num, 4, "%.3d", index);
            name.append(ch_num);

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
      CDirHandler sub_dir = CDirHandler(m_list_dirs.front(), m_prefix, false);
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
   if (m_recursive) { return 0; }

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

