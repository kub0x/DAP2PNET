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
#include "../Headers/SSLClientSocket.hpp"

ServerSocket::ServerSocket(int port){
	this->serverfd=0;
	this->port = port;
	InitSocket();
}

ServerSocket::ServerSocket(int port,bool enabled_SSL){
	this->serverfd = 0;
	this->port = port;
	this->enabled_SSL = enabled_SSL;
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
	std::cout << "IN CALLBACK SERVERSOCKET!" << std::endl;
	for(auto it = buff.begin(); it != buff.end(); ++it)
	std::cout << *it << std::endl;
}

void ServerSocket::OnClose(ClientSocket *client){
	std::cout << "Server Handled Disconnection of user: " << client->GetSourceIP() << "," << client->GetID() << std::endl;
	number_conn--;
	std::cout << "Active Clients: " << number_conn << std::endl;
	if (!client->GetRepeatedConnection()){ //A non-repeated client is disconnecting
		//take it away from the connection table
		std::cout << client->GetSourceIP() << " removed from connection table!" << std::endl;
		conn_table.erase(conn_table.find(client->GetSourceIP()));
	}
}

ClientSocket* ServerSocket::OnAccept(int clientfd, struct sockaddr_in *client_addr){
	number_conn++;
	char str_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_addr->sin_addr), str_ip, INET_ADDRSTRLEN);
	std::cout << "Client " << str_ip << " Connected! " << std::endl;
	ClientSocket *client = NULL;
	if(enabled_SSL)
		client = new SSLClientSocket(clientfd, std::string(str_ip));
	else
		client = new ClientSocket(clientfd, std::string(str_ip));
	client->SetCallBackRead(std::bind(&ServerSocket::OnRead, this, std::placeholders::_1, std::placeholders::_2));
	client->SetCallBackClose(std::bind(&ServerSocket::OnClose, this, std::placeholders::_1));
	if (conn_table.find(client->GetSourceIP()) == conn_table.end()){
		//Not in table
		conn_table.insert(std::make_pair(client->GetSourceIP(), client));
		std::thread t(&ClientSocket::Read, client);
		t.detach();
	}else{
		client->SetRepeatedConnection(true);//for handling OnClose
		std::cout << client->GetSourceIP() << " already connected!" << std::endl;
		//Force Shutdown as it's in table
		client->Close();
		client=0;//to handle in overridden methods
	}
	return client;
}

void ServerSocket::Listen(){
	 int clientfd = 0;
	 int err = listen(serverfd, MAX_CONNECTIONS);
	 if (err == -1){
		 perror("listen");
		 exit(-1);
	 }
	 std::cout << "Server started to listen" << std::endl;
	 while(true)
	 {
		 if(number_conn >= MAX_CONNECTIONS){
			 usleep(1000000);
			 std::cout << "MAX_CONNECTIONS REACHED!";
			 continue;
		 }
		 struct sockaddr_in client_addr={0};
		 socklen_t addr = sizeof(client_addr);
		 //non blocking socks, returns -1 when no client is waiting to be accepted
		 clientfd = accept4(serverfd, (struct sockaddr*)&client_addr, &addr, SOCK_NONBLOCK);
		 if (clientfd > 0){
			 OnAccept(clientfd, &client_addr);
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
