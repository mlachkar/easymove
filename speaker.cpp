#include <speaker.h>

void speaker::run()
{
    int heap_size = 210000;  // default scheme heap size
    int load_init_files = 1; // we want the festival init files loaded
    festival_initialize(load_init_files, heap_size);
    festival_say_text("test test");
}

void speaker::say(char* string) {
    festival_say_text(string);
}
