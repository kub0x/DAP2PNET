/*
 * ClientSocket.cpp
 *
 *  Created on: Nov 30, 2020
 *      Author: kub0x
 */


#include "../Headers/ClientSocket.hpp"
#include <iostream>
#include<stdio.h>	//printf
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//usleep
#include<fcntl.h>	//fcntl
#include <vector>
#include <functional>

ClientSocket::ClientSocket(std::string IP, int port){
	this->IP=IP;
	this->port=port;
}

ClientSocket::ClientSocket(int sockfd){
	this->sockfd = sockfd;
}

void ClientSocket::Connect(){
	sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(IP.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (connect(sockfd , (struct sockaddr *)&server , sizeof(server)) < 0){
		perror("connect");
	}else{
		Read();
	}
}

void ClientSocket::SetCallBackRead(std::function<void(ClientSocket*, std::vector<char*>)> ptr_read){
	this->ptr_read = ptr_read;
}

void ClientSocket::SetCallBackClose(std::function<void(ClientSocket*)>ptr_close){
	this->ptr_close = ptr_close;
}

void ClientSocket::Read(){
	std::cout << "Using ClientSocket's READ" << std::endl;
	char buf[PACKET_LENGTH]={0};
	std::vector<char*> recv_data;
	std::cout << "Reading messages" << std::endl;
	while(true){
		int len = recv(sockfd, (void *) &buf, PACKET_LENGTH, 0);
		std::cout << len << std::endl;
		if (len == 0){
			if (!recv_data.empty()){
				ptr_read(this, recv_data);
			}
			Close();
			return;
		}
		if (len > 0){
			std::cout << "Recv of len " << len << std::endl;
			recv_data.push_back(buf);
		}else{
			if (errno == EWOULDBLOCK || errno == EAGAIN){
				if (!recv_data.empty()){
					ptr_read(this, recv_data);
					recv_data.clear();
				}
				//sleep here
				usleep(1000000);
			}
		}
	}
}

void ClientSocket::Write(){

}

void ClientSocket::Close(){
	std::cout << "Client " << sockfd << " Disconnected!" << std::endl;
	if (shutdown(sockfd, 2) < 0)
		perror("shutdown");
	ptr_close(this);
	close(sockfd);
}
