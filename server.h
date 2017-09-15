#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <thread>
#include <map>

using namespace std;

typedef char const* chars;

class Server {

    public:
        struct sockaddr_in stSockAddr;
        int SocketFD;
        int port;
        char buffer[255];
        char message[255];
        int n;
        map<int, chars> table_gamers;

        int packet_size;
        int header_size;
        Protocol* protocol;

        Server();
        // port, header_size, packet_size,
        Server(int, int, int);        
        void connection();
        void new_client_connection(int);
        void print_gamers();
        void broadcast(chars);
};

Server::Server(){}

Server::Server(int port, int header_size , int packet_size){
    this->protocol = new Protocol();

    this->SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    this->port = port;
    this->packet_size = packet_size;
    this->header_size = header_size;

    if(-1 == this->SocketFD)
    {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }

    memset(&this->stSockAddr, 0, sizeof(struct sockaddr_in));

    this->stSockAddr.sin_family = AF_INET;
    this->stSockAddr.sin_port = htons(this->port);
    this->stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if(-1 == bind(this->SocketFD,(const struct sockaddr *)&this->stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("error bind failed");
        close(this->SocketFD);
        exit(EXIT_FAILURE);
    }

    if(-1 == listen(this->SocketFD, 10))
    {
        perror("error listen failed");
        close(this->SocketFD);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a connection ... \n");
}

void Server::print_gamers(){
    map<int, chars>::iterator it;

    cout<<"GAMERS"<<endl;
    cout<<"======"<<endl;
    for(it=this->table_gamers.begin(); it!=this->table_gamers.end(); ++it)
        cout << it->first << " => " << it->second << '\n';
    cout<<endl<<endl;
}

void Server::broadcast(chars message){
    map<int, chars>::iterator it;

    for(it=this->table_gamers.begin(); it!=this->table_gamers.end(); ++it){
        n = write(it->first, message, 255);
        if (n < 0) perror("ERROR writing to socket");
    }
}

void Server::new_client_connection(int connect_id){

    for(;;)
    {
    // do
    // {
        /*bzero(this->buffer, 256);
        n = read(connect_id, this->buffer, 255);
        if (n < 0) perror("ERROR reading from socket");
        
        chars unwrapped_messa = this->protocol->unwrap(this->buffer);
        printf("Message of client: << %s >>>\n", unwrapped_messa);*/

        /*cout<<"!this->table_gamers[connect_id]: "<<!this->table_gamers[connect_id]<<endl;*/

        if(!this->table_gamers[connect_id]){
            bzero(this->buffer, 256);
            n = read(connect_id, this->buffer, 255);
            if (n < 0) perror("ERROR reading from socket");
            
            chars unwrapped_messa = this->protocol->unwrap(this->buffer);
            this->table_gamers[connect_id] = unwrapped_messa;
            chars spe_messa = "Please insert your position to start to play (x enter y): ";
            spe_messa = this->protocol->envelop("simple-message", spe_messa);
            n = write(connect_id, spe_messa, 255);
            if (n < 0) perror("ERROR writing to socket");
        }
        else{
            if(strlen(buffer) > 0){
                bzero(this->buffer, 256);
                n = read(connect_id, this->buffer, 255);
                if (n < 0) perror("ERROR reading from socket");
                
                chars unwrapped_messa = this->protocol->unwrap(this->buffer);
                printf("Message of client: << %s >>>\n", unwrapped_messa);
                this->broadcast(this->buffer);
            }
            else{
                printf("Client desconnected !!! \n");
                break;
            }
        }

        this->print_gamers();
        /*this->table_gamers[connect_id] = unwrapped_messa;*/

        /*chars messa = "";
        if(strlen(buffer) > 0){
            printf("Enter message to client: ");
            scanf("%s" , this->message);
            messa = this->protocol->envelop("simple-message", this->message);
            this->broadcast(messa);            
        }
        else {
            printf("Client desconnected !!! \n");
            break;
        }*/

        if (n < 0) perror("ERROR writing to socket");
    }
    // } while(buffer != "chao");
    shutdown(connect_id, SHUT_RDWR);
    close(connect_id);
}

void Server::connection(){

    for(;;){

        int ConnectFD = accept(this->SocketFD, NULL, NULL);        

        if(0 > ConnectFD)
        {
            perror("error accept failed");
            close(this->SocketFD);
            exit(EXIT_FAILURE);
        }

        thread t(&Server::new_client_connection, this, ConnectFD);
        t.detach();

        printf("Client connected !!! \n");

    }
}