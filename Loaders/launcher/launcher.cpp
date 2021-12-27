//#include <cstdio>
#include <iostream>
////#include <memory>
////#include <stdexcept>
////#include <string>
////#include <array>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include<stdio.h>
#include<unistd.h>
#include <stdlib.h> //has exit()

using namespace std;

void main_loop(){

//printf ("launcher 1");
//
std::cout << "launcher 1";
//std::string s = "./media/printsome.bc" ;
  std::string s = "/media/webm/ffmpeg_g" ;

execvp(s.c_str(), NULL);

  printf("hello, world!\n");
  sleep(100);
   exit(0);
}



int main() {

    #ifdef __EMSCRIPTEN__
	//emscripten_set_main_loop(main_loop,0,1);
    #endif

 //   char *cmd = "/media/webm/ffmpeg_g";
//char *argv[3];
//argv[0] = "ffmpeg_g";
//argv[1] = "-h";
//argv[2] = NULL;
//execvp(cmd, argv);
  printf("main!\n");
 // std::string s = "/media/printsome.bc" ;
    std::string s = "printsome.bc" ;
  //execvp(s.c_str(), NULL);

 // system("./media/printsome.bc");
  system("printsome.bc");
  return 0;
}
