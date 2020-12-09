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

ClientSocket::ClientSocket(std::string remote_IP, int port){
	this->remote_IP=remote_IP;
	this->port=port;
}

ClientSocket::ClientSocket(int sockfd, std::string source_IP){
	this->sockfd = sockfd;
	this->source_IP = source_IP;
}

void ClientSocket::Connect(){
	sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(remote_IP.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (connect(sockfd , (struct sockaddr *)&server , sizeof(server)) < 0){
		perror("connect");
	}else{
		Read();
	}
}

void ClientSocket::SetCallBackRead(fnOnRead ptr_read){
	this->ptr_read = ptr_read;
}

void ClientSocket::SetCallBackClose(fnOnClose ptr_close){
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
	ptr_close(this);
	if (shutdown(sockfd, 2) < 0)
		perror("shutdown");
	close(sockfd);
	delete this; //suicidal object
}
