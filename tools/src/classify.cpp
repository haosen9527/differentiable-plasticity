#include <dirent.h>
#include <iostream>
#include <tinyxml.h>
#include <string>
#include <vector>
#include <stack>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <tools/random.h>

using namespace std;
void GetFileNames(string param_dir)
{
  struct dirent *ptr;
  DIR *dir;
  dir = opendir(param_dir.data());
  while ((ptr=readdir(dir))!=NULL)
  {
    if(ptr->d_name[0] == '.')
      continue;
    if(int(ptr->d_type)==4)
    {
      string dir_name = param_dir;
      string files_name = ptr->d_name;
      string search_path = dir_name+files_name+"/";
//      std::cout<<full_path<<std::endl;
      GetFileNames(search_path);
    }
    else if (int(ptr->d_type)==8) {

    }
    else if(int(ptr->d_type)==10) {
      string full_path = param_dir+ptr->d_name;
      std::cout<<full_path<<std::endl;
      string ImgName = ptr->d_name;
      std::cout<<ImgName<<std::endl;

      string::const_iterator it;
      int cout = 0;
      stack<int> cout_list;
      for(it = full_path.begin();it!=full_path.end();it++)
      {
        cout++;
        //std::cout<<*it<<std::endl;
        if(*it == '/')
        {
          cout_list.push(cout);
        }
      }
      int last_1;
      for(int i =0;i<cout_list.size();i++)
      {
        last_1 = cout_list.top();
        cout_list.pop();
      }
      //std::cout<<"**************************"<<std::endl;
      //std::cout<<full_path.substr(0,last_1)<<std::endl;
    }
  }
  closedir(dir);
}

int main(int argc, char **argv)
{
  //GetFileNames("./");
  pair<int,int> range;
  //range.first =
  return 0;
}
