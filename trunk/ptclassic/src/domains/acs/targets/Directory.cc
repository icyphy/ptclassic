/**********************************************************************
Copyright (c) 1999 Sanders, a Lockheed Martin Company
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE SANDERS, A LOCKHEED MARTIN COMPANY BE LIABLE TO ANY 
PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
SANDERS, A LOCKHEED MARTIN COMPANY HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

SANDERS, A LOCKHEED MARTIN COMPANY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND SANDERS, A LOCKHEED MARTIN
COMPANY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
                                                        COPYRIGHTENDKEY


 Programmers:  Ken Smith
 Date of creation: 8/26/99
 Version: @(#)Arch.cc      1.0     08/26/99
***********************************************************************/
#include "Directory.h"

Directory::Directory()
{
  directory_name=new char[PATH_MAX];
  edirectory_name=new char[PATH_MAX];
}

Directory::~Directory()
{
  delete []directory_name;
  delete []edirectory_name;
}

void Directory::set(char* stored_name)
{
  strcpy(directory_name,stored_name);
  strcpy(edirectory_name,stored_name);

  // Check for a trailing slash, if missing then add it to the
  // edirectory_name version
  char* slash_test=&directory_name[strlen(directory_name)-1];
  if (strncmp(slash_test,"/",1)!=0)
    strcat(edirectory_name,"/");

  // Extract environment variables if they exist
  extract_environment();
}
void Directory::set(const char* stored_name)
{
  strcpy(directory_name,stored_name);
  strcpy(edirectory_name,stored_name);

  // Check for a trailing slash, if missing then add it to the
  // edirectory_name version
  char* slash_test=&directory_name[strlen(directory_name)-1];
  if (strncmp(slash_test,"/",1)!=0)
    strcat(edirectory_name,"/");

  // Extract environment variables if they exist
  extract_environment();
}

char* Directory::retrieve(void)
{
  return(directory_name);
}

char* Directory::retrieve_extended(void)
{
  return(edirectory_name);
}

// If default make 
int Directory::make_directory(void)
{
  char rc=mkdir(directory_name,S_IFDIR | S_IRWXU | S_IRWXG);
/*
  if ((rc!=0) && (errno!=EEXIST))
    perror("ERROR Directory::make_directory unable to make directory\n");
    */
  return(rc);
}
int Directory::make_directory(int flags)
{
  char rc=mkdir(directory_name,flags);
/*
  if ((rc!=0) && (errno!=EEXIST))
    perror("ERROR Directory::make_directory unable to make directory\n");
    */
  return(rc);
}

void Directory::extract_environment(void)
{
  // Trap for environment variable
  if (strncmp(directory_name,"$",1)==0)
    {
      unsigned int str_cnt=strlen(directory_name);

      // Search for the end of the environment variable (if any)
      unsigned int cnt;
      for (cnt=1;cnt<str_cnt;cnt++)
	{
	  const char* char_ptr=&directory_name[cnt];
	  if (strncmp(char_ptr,"/",1)==0)
	    break;
	}

      char* environ=new char[PATH_MAX];
      const char* bypass_dollar=&directory_name[1];
      strncat(environ,bypass_dollar,cnt-1);

      // Extract environment-path setting
      char* environ_path=getenv(environ);

      // Form new path
      char* expanded_path=new char[PATH_MAX];
      char* eexpanded_path=new char[PATH_MAX];
      const char* path_remainder=&directory_name[cnt];
      const char* epath_remainder=&edirectory_name[cnt];
      sprintf(expanded_path,"%s%s",environ_path,path_remainder);
      sprintf(eexpanded_path,"%s%s",environ_path,epath_remainder);
      
      // Swap
      delete []directory_name;
      delete []edirectory_name;
      directory_name=expanded_path;
      edirectory_name=eexpanded_path;
      
      // Cleanup
      delete []environ;
    }
}
