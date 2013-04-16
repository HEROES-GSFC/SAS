#include "Commanding.hpp"

int main()
{
    Commanding *commanding;
    commanding = new Commanding;

    //tStream->test_checksum();
    commanding->init_socket();
    commanding->listen();
    commanding->print_packet();
    commanding->close_socket();
    
    delete commanding;
    exit(0);
}
