/*
 * ServerSocket.cpp
 *
 *  Created on: Nov 30, 2020
 *      Author: kub0x
 */

#include <functional>
#include <thread>
#include <iostream>
#include "../Headers/ServerSocket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "../../Crypto/Headers/PKI.hpp"

ServerSocket::ServerSocket(int port){
	this->serverfd=0;
	this->port=0;
	this->port = port;
	InitSocket();
}

void ServerSocket::InitSocket(){
	struct sockaddr_in serv_addr={0};
	serverfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	std::cout << std::to_string(serverfd) << std::endl;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	int err = bind(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (err == -1){
		perror("bind");
		exit(-1);
	}
	std::cout << std::to_string(err) << std::endl;
}

void ServerSocket::OnRead(ClientSocket *client, std::vector<char*> buff){
	std::cout << "IN CALLBACK!" << std::endl;
	std::string strCsr;
	for(auto it = buff.begin(); it != buff.end(); ++it)
		strCsr+=*it;
	std::cout << strCsr << std::endl;
	PKI::GetInstance()->SignCert(PKI::GetInstance()->CSRtoX509(strCsr), client->GetID());
}

void ServerSocket::OnClose(ClientSocket *client){
	std::cout << "Server Handled Disconnection of user: " << client->GetID() << std::endl;
	number_conn--;
	std::cout << "Active Clients: " << number_conn << std::endl;
}

void ServerSocket::Listen(){
	 int clientfd = 0;
	 int err = listen(serverfd, MAX_CONNECTIONS);
	 if (err == -1)
		 perror("listen");
	 while(true)
	 {
		 if(number_conn >= MAX_CONNECTIONS){
			 usleep(1000000);
			 std::cout << "MAX_CONNECTIONS REACHED!";
			 continue;
		 }
		 //non blocking socks, returns -1 when no client is waiting to be accepted
		 clientfd = accept4(serverfd, (struct sockaddr*)NULL, NULL, SOCK_NONBLOCK);
		 if (clientfd > 0){
			 number_conn++;
		 	std::cout << "Client Connected! " << std::endl;
		 	ClientSocket *client = new ClientSocket(clientfd);
		 	client->SetCallBackRead(std::bind(&ServerSocket::OnRead, this, std::placeholders::_1, std::placeholders::_2));
		 	client->SetCallBackClose(std::bind(&ServerSocket::OnClose, this, std::placeholders::_1));
		 	std::thread t(&ClientSocket::Read, client);
		 	t.detach();
		 	//client->Read();
		 }else{ //negative, try to compare with non-blocking flags
			 if (errno == EWOULDBLOCK || errno == EAGAIN){
			 	//std::cout << errno;
			 	//sleep here
			 	//std::cout << "Waiting! " << std::endl;
			 	usleep(1000000);
			 }else{//unexpected error
				 perror("listen");
				 exit(-1);
			 }
		 }
	 }
}

void ServerSocket::Write(int sockfd){

}

void ServerSocket::Close(){

}
