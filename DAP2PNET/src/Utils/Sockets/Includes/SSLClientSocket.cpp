/*
 * SSLClientSocket.cpp
 *
 *  Created on: Dec 2, 2020
 *      Author: kub0x
 */

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include<unistd.h>	//usleep
#include "../Headers/SSLClientSocket.hpp"

SSLClientSocket *thisPtr =0;

void SSLClientSocket::InitSSL(){
	std::cout << "Initializing SSL Layer for client " << sockfd << std::endl;
	thisPtr = this; //USE WITH CARE
	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	meth = (SSL_METHOD*)TLS_server_method();
	ctx = SSL_CTX_new (meth);
	if (!ctx) {
		ERR_print_errors_fp(stderr);
		exit(2);
	}

	SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1_3); //STRIP v1.3 for debugging purposes on WireShark

	if (mutual_auth){

		if (SSL_CTX_load_verify_locations(ctx, "certs/CA.pem", NULL) != 1) {
	        fprintf(stderr, "Could not set the CA file location\n");
	        exit(-1);
	    }

	    /* Load the client's CA file location as well */
		SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file("certs/CA.pem"));
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, VerificationCallback);
		SSL_CTX_set_verify_depth(ctx, 1);
	}


	 if (SSL_CTX_use_certificate_chain_file(ctx, "certs/tls_chain.pem") <= 0) {
	   ERR_print_errors_fp(stderr);
	   exit(3);
	 }
	 if (SSL_CTX_use_PrivateKey_file(ctx, "priv/tls_priv.pem", SSL_FILETYPE_PEM) <= 0) {
	   ERR_print_errors_fp(stderr);
	   exit(4);
	 }
	 if (!SSL_CTX_check_private_key(ctx)) {
	   fprintf(stderr,"Private key does not match the certificate public key\n");
	   exit(5);
	 }
	int err = 0;
	ssl = SSL_new (ctx);
	if (!ssl) exit(-1);

	err=SSL_set_fd (ssl, sockfd);
	if (!err) exit(-1);

	err = SSL_accept (ssl); //send TLS Handshake negotiation to the client
	/*if (mutual_auth){
		while (!SSL_get_peer_cert_chain(ssl)){
			usleep(1000000);
		}
	}*/
	/*if (err <= 0){
		err = SSL_get_error(ssl, err);
		if (err == SSL_VERIFY_FAIL_IF_NO_PEER_CERT || err == SSL_VERIFY_CLIENT_ONCE){
			std::cout << "[Two-way SSL] Verification error!" << std::endl;
			perror("SSL_accept");
			exit(-1);
		}
	}*/
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
    // elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
  }
  return elems;
}

///CN=3736e75add62e855b2f2844f9656f5/C=ES/O=DAP2PNET USER
int SSLClientSocket::VerificationCallback(int flag, X509_STORE_CTX *ctx){
	X509* cert = X509_STORE_CTX_get_current_cert(ctx);
	char *subject = X509_NAME_oneline(X509_get_subject_name(cert), NULL, 0);
	std::cout << "[Two-Way SSL] Received cert: " << subject << std::endl;
	std::string data = std::string(subject);
	std::vector<std::string> sep = split(data, '/');
	for (unsigned int i = 0; i< sep.size(); i++){
		std::string curStr = sep.at(i);
		if (curStr.find("CN=") != std::string::npos){
			curStr.erase(curStr.begin(), curStr.begin() + 3);
			if (curStr.compare("User") != 0){
				thisPtr->nodeID = std::string(curStr); //USE WITH CARE
				std::cout << curStr << std::endl;
			}
		}
	}
	return flag;
}

//use base class' connect() method and negotiate TLS after
void SSLClientSocket::Connect(){
	ClientSocket::Connect();
	/*int err=0;
	ssl = SSL_new (ctx);
	SSL_set_fd (ssl, sockfd);
	err = SSL_connect (ssl);
	X509 *server_cert = SSL_get_peer_certificate (ssl);*/
	//TODO CONTINUE. NOT NEEDED FROM NOW
	//ONLY SERVER SIDE SSL IS CONTEMPLATED
	//SO WE ONLY TURN THE SOCKET INTO SSL AND READ AFTER
}

//Overriding virtual Read. Define the equivalent in OpenSSL non-blocking mode
void SSLClientSocket::Read(){
	std::cout << "Using SSLClient's READ" << std::endl;
	char buf[PACKET_LENGTH]={0};
	std::vector<char*> recv_data;
	std::cout << "Reading messages" << std::endl;
	while(true){
		lock_rw.lock();
		int len = SSL_read(ssl, (void *) &buf, PACKET_LENGTH);
		std::cout << len << std::endl;
		lock_rw.unlock();
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
		}else{ //Client finished sending data!
			//Now must response back!
			if (!recv_data.empty()){
				//OnRead calls write so it's already locked. Unlock here!
				ptr_read(this, recv_data);
				recv_data.clear();
			}
			//sleep here
			usleep(1000000);
		}
	}
}

void SSLClientSocket::Write(const std::string& message){
	std::cout << "ON PRE-SSL WRITE!" << std::endl;
	lock_rw.lock();
	std::cout << "ON SSL WRITE!" << std::endl;
	int err = SSL_write(ssl, message.c_str(), message.length()+1);
	if (err <= 0){
		std::cout << "ERROR" << std::endl;
		ERR_print_errors_fp(stderr);
	}
	std::cout << "Finished WRITE!" << std::endl;
	lock_rw.unlock();
}
