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

//void main_loop(){
//
////printf ("launcher 1");
////
////std::cout << "launcher 1";
////std::string s = "./media/printsome.bc" ;
////execvp(s.c_str(), NULL);
//
//  printf("hello, world!\n");
//   exit(0);
//}



int main() {

//    #ifdef __EMSCRIPTEN__
//	emscripten_set_main_loop(main_loop,0,1);
//#endif

  printf("hello, world!\n");
  std::string s = "/media/printsome.bc" ;
execvp(s.c_str(), NULL);
  return 0;
}
