#include <iostream>
using namespace std;

extern "C" {

    char* lower_string(char* str) {
        for (int i = 0; str[i] != '\0'; i++) {
            if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] = str[i] + 32;
        }
        return str;
    }

    char* upper_string(char* str) {
        for (int i = 0; str[i] != '\0'; i++) {
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] = str[i] - 32;
        }
        return str;
    }

    const char* str_conv(const char* inpStr) {
        string retStr
                = "The string: " + (string) inpStr + "\n"
                + "Lower case: " + (string) lower_string((char *) inpStr) + "\n"
                + "Upper case: " + (string) upper_string((char *) inpStr) + "\n";
        const char *cstr = retStr.c_str();
        return cstr;
    }
}
