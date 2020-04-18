#include <iostream>
#include <fstream>
       #include <sys/stat.h>
       #include <sys/types.h>
 #include <dirent.h>
 #include <fcntl.h>
//emcc directoryList.cpp -o test.html


using namespace std;

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


int main()
{

	//if((f_trans != stdout) && (f_trans != stderr))
	//	fclose(f_trans);  // needed for WinCe
//emscripten_run_script(saveAs(blob, "test", 1););

EM_ASM(
var btn = document.createElement('input');
btn.type = 'button';
btn.name
= btn.value = 'OK';
btn.onclick = function() {
saveAs(blob, "test", 1);
};
document.body.appendChild(btn);
);




mkdir("./test",1);

//    fstream fs;
//    fs.open("/test", ios::out);
//    fs << "Writing this to a file.\n";
//    fs.close();

list_dir("/");

}
