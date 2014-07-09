#include <speaker.h>

speaker::speaker(const char* string)
    : QThread()
{
    _string = string;
}

void speaker::setString(const char *string)
{
    _string = string;
}

void speaker::run()
{
    int heap_size = 2100000;  // default scheme heap size
    int load_init_files = 1; // we want the festival init files loaded
    festival_initialize(load_init_files, heap_size);
    festival_say_text(_string);
}

