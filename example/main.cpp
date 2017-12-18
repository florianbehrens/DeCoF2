#include "application.h"

int main(int argc, char* argv[])
{
    return application::instance().run(argc, argv);
}
