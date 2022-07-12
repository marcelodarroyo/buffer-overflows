#include <stdio.h>
#include <unistd.h>

// Used instead of gets to avoid issues with pipes and buffering, etc
void getstr(char* buffer) {
    int pos = -1;
    ssize_t n = 0;
    do {
        pos += 1;
        n = read(0, &buffer[pos], 1);
    } while (n != 0 && buffer[pos] != '\n');
    buffer[pos] = '\0';
}


int echo_line(int arg) {
    char buffer[80];
    printf("&arg = %p; buffer = %p diff = %d\n",
          &arg, buffer, (int)((char*) &arg - buffer));
    getstr(buffer);
    printf("line = %s\n", buffer);
    return 99;
}

int main() {
    printf("start\n");
    int value = echo_line(999);
    printf("value = %d\n", value);
    return 0;
}
