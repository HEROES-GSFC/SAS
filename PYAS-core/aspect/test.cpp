#include "UDPSender.hpp"

int main(){

    UDPSender *sender;
    sender = new UDPSender();

    sender->init_connection();
    sender->close_connection();

    return 0;
}