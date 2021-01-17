#include "make_log.h"

int main(int argc, char *argv[])
{
	char *p = "hello milo";
    LOG("test_log", "test", "test info[%s]", p);
  
	return 0;
}
