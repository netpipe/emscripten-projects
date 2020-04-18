#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h> 
#include <emscripten.h>

using namespace std;

//emcc displayInfo.cpp -o display.html

static void list_dir(const char *path)
{
  struct dirent *entry;
  DIR *dir = opendir(path);
  if (dir == NULL) {
      return;
  }

  while ((entry = readdir(dir)) != NULL) {
      printf("%s\n",entry->d_name);
  }

  closedir(dir);
}
EM_JS(void, initfn, (), {

  var button = document.createElement('input');
  button.type = "file";
  button.mulitple = "true";
  button.className = "btn btn-info";
  var ul = document.createElement('ul');
  document.body.appendChild(button);
  document.body.appendChild(ul);
});

int main() {
	mkdir("./test",1); // for test
  initfn();
	list_dir("/");

  emscripten_run_script("handleFiles()");
  return 0;
}