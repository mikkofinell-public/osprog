#include "togglecase.h"
#include <string.h>

void togglecase(char * buffer, int length) {
    int i;
    for (i = 0; i < length; i++) {
        if (isupper(buffer[i])) {
            buffer[i] = tolower(buffer[i]);
        }
        else {
            buffer[i] = toupper(buffer[i]);
        }
    }
}
