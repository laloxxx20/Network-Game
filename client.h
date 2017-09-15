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
#include <time.h>       /* time */
// #include "protocol.h"

using namespace std;

typedef char const* chars;

class Client{

    public:
        struct sockaddr_in stSockAddr;

        int Res;
        int SocketFD;
        int n;
        int message_server;
        int port;
        chars ip_address;

        /*char buffer[256];*/
        /*char message[256];*/
        char message[256];
        char buffer[256];
        int packet_size;
        int header_size;
        int width = 20;
        int height = 20;
        chars ship;

        Protocol* protocol;

        chars board[20][20] = {
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"},
                            {"_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_"}
                           };



        Client();
        // ip, port, header_size, packet_size, 
        Client(chars, int, int, int);
        string adding_header(string);

        bool login(char*);
        void read_server();
        void write_server();
        void get_list_online();
        void start_client();
        void draw_board();
        void draw_ship(chars, int, int);
        void draw_ship_first(chars);
};


Client::Client(){}

Client::Client(chars ip, int port, int header, int packet)
{

    this->protocol = new Protocol();
    /*chars message = this->protocol->envelop("simple-message", "test text lalito");
    cout<<"envelop message: "<<message<<endl;
    chars unwrapped_messa = this->protocol->unwrap(message);
    cout<<"unwrapped message: "<<unwrapped_messa<<endl;*/

    this->ip_address = ip;
    this->port = port;
    this->header_size = header;
    this->packet_size = packet;
    this->SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    // error while we try create the token
    if (-1 == this->SocketFD)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&this->stSockAddr, 0, sizeof(struct sockaddr_in));

    this->stSockAddr.sin_family = AF_INET;
    this->stSockAddr.sin_port = htons(this->port);
    this->Res = inet_pton(AF_INET, ip_address, &this->stSockAddr.sin_addr);

    if (0 > this->Res)
    {
        perror("error: first parameter is not a valid address family");
        close(this->SocketFD);
        exit(EXIT_FAILURE);
    }
    else if (0 == this->Res)
    {
        perror("char string (second parameter does not contain valid ipaddress");
        close(this->SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(this->SocketFD, (const struct sockaddr *)&this->stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("connect failed");
        close(this->SocketFD);
        exit(EXIT_FAILURE);
    }
  
}

void Client::draw_board(){
    printf("    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9\n");
    for(int i=0; i<this->width; i++){
        printf("%2d| ", i);
        for(int j=0; j<this->height; j++)
            printf("%s ", this->board[i][j]);
        printf("\n");
    }
}

void Client::draw_ship(chars ship, int x, int y){
    this->board[x][y] = ship;
    this->board[x+1][y] = ship;
    this->board[x][y+1] = ship;
    this->board[x+1][y+1] = ship;
}

void Client::draw_ship_first(chars ship){
    srand (time(NULL));
    int x = rand() % (this->width-2) + 1;
    int y = rand() % (this->height-2) + 1;
    while(1){
        if(this->board[x][y] == "_" && this->board[x+1][y] == "_" && this->board[x][y+1] == "_" && this->board[x+1][y+1] == "_")
            break;
        x = rand() % (this->width-2) + 1;
        y = rand() % (this->height-2) + 1;
    }
    cout<<"x: "<<x<<endl;
    cout<<"y: "<<y<<endl;

    this->draw_ship(ship, x, y);
}

void Client::read_server()
{
    int counter = 0;

    for(;;)
    {
        chars messa = "";
        if(counter < 1){
            printf("Enter a letter to play (ex A): ");
            scanf("%s" , this->message);
            this->ship = this->message;
            messa = this->protocol->envelop("simple-message", this->ship);
            this->draw_ship_first(this->ship);
        }
        if(counter > 1){
            this->draw_board();
            printf("Please insert your new position to play (x enter y): ");
            scanf("%s" , this->message);
            system("clear");
        }
        if(counter==1)
            this->draw_board();

        system("clear");
        messa = this->protocol->envelop("simple-message", this->message);
        n = write(this->SocketFD, messa, 255);
        if (n < 0) perror("ERROR writing to socket");
        counter++;
        
        bzero(this->buffer, 255);
        this->message_server = read(this->SocketFD, this->buffer, 255);
        if (this->message_server < 0) perror("ERROR reading from socket");

        chars unwrapped_messa = this->protocol->unwrap(this->buffer);
        printf("Message of server: << %s >>\n", unwrapped_messa);
    }

    shutdown(this->SocketFD, SHUT_RDWR);
    close(this->SocketFD);
}