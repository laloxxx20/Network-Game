#include "protocol.h"
#include "client.h"
#include "server.h"
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    int port = 1100;
    char const* IP_CLIENT = "192.168.110.135";

    if(strcmp(argv[1], "s") == 0)
    {
        Server* s = new Server(port, 4 , 1000);
        s->connection();        
    }
    if(strcmp(argv[1], "c") == 0){
        Client* c = new Client(IP_CLIENT, port, 4, 10);
        c->read_server();
    }
    else
        cout<<"Please insert a value to execute server(s) or client(c) "<<endl;

    return 0;
}
