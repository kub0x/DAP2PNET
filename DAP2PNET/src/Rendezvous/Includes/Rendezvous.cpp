/*
 * Rendezvous.cpp
 *
 *  Created on: Dec 9, 2020
 *      Author: kub0x
 */


#include "../Headers/Rendezvous.hpp"
#include "../../Utils/Sockets/Headers/SSLClientSocket.hpp"
#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

Rendezvous::Rendezvous(int port) : ServerSocket(port,true){
	std::thread t(&Rendezvous::UpdatePeerList, this);
	t.detach();
}

void Rendezvous::OnRead(ClientSocket *client, std::vector<char*> buff){

}

ClientSocket* Rendezvous::OnAccept(int clientfd, struct sockaddr_in *client_addr){
	number_conn++;
	char str_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_addr->sin_addr), str_ip, INET_ADDRSTRLEN);
	std::cout << "Client " << str_ip << " Connected! " << std::endl;
	ClientSocket *client = NULL;
	if(enabled_SSL)
		client = new SSLClientSocket(clientfd, std::string(str_ip), true); //mutual auth SSL/TLS
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
	if (client){//non-repeated connection object's alive
		while (client->nodeID.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		std::cout << "New client!" << std::endl;
		peers.push_back(client->nodeID + ":" + client->GetSourceIP());
	}else{
		std::cout << "Object Destroyed!" << std::endl;
	}
	return client;
}

void Rendezvous::UpdatePeerList(){ //periodic task that sends a peer list to connected nodes
	while(true){
		std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIMEOUT));
		if (OnUpdatePeerList())
			std::cout << "[Rendezvous]Peer list sent!" << std::endl;
		else
			std::cout << "[Rendezvous]Less than 2 contacts" << std::endl;
	}
}

bool Rendezvous::OnUpdatePeerList(){
	if (peers.size() < 2) return 0; //cannot work with less than 2 peers
	//Limit the range by MAX_PEERS if possible
	unsigned int start = MAX_PEERS;
	if (peers.size() < MAX_PEERS)
		start = peers.size();
	std::random_device rnd;
	std::mt19937 gen(rnd());
	std::uniform_int_distribution<> dist(0, start-1);
	start -=1; //minus 1 cause we exclude the node itself
	for (auto it : conn_table){ //iterate through every connected client (inherited from base class)
		std::string curIP = it.first;
		std::string list;
		for (unsigned int i=0; i < start; i++){
			std::cout << "endless?" << std::endl;
			int value = dist(gen);
			std::cout << "val:" << value << std::endl;
			std::string IP = peers.at(value);
			std::cout << "str:" << IP << std::endl;
			if (IP.find(curIP) == std::string::npos) //current node's IP doesn't match peer's IP
				list.append(IP + ",");
			else
				i--; //IPs do match so decrement to calculate a new one
		}
		list.pop_back(); //erase last char (comma)
		ClientSocket *client = it.second;
		client->Write(list); //send the peer list to the current node
	}
	return 1;
}
