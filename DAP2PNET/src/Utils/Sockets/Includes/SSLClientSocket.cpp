/*
 * SSLClientSocket.cpp
 *
 *  Created on: Dec 2, 2020
 *      Author: kub0x
 */

#include <iostream>
#include<unistd.h>	//usleep
#include "../Headers/SSLClientSocket.hpp"


void SSLClientSocket::InitSSL(){
	std::cout << "Initializing SSL Layer for client " << sockfd << std::endl;
	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	meth = (SSL_METHOD*)TLS_server_method();
	ctx = SSL_CTX_new (meth);
	SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1_3); //STRIP v1.3 for debugging purposes on WireShark
	if (!ctx) {
	   ERR_print_errors_fp(stderr);
	   exit(2);
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
	/*err=SSL_get_error(ssl, err);
	if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE ){
		std::cout << "Should recall" << std::endl;
		//sleep(10);
		err=SSL_accept(ssl);
	}else{
		std::cout << "err:" << err << std::endl;
	}
	std::cout << "err:" << err << std::endl;*/
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
		int len = SSL_read(ssl, (void *) &buf, PACKET_LENGTH);
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
			if (!recv_data.empty()){
				ptr_read(this, recv_data);
				recv_data.clear();
			}
			//sleep here
			usleep(10000000);
		}
	}
}

void SSLClientSocket::Write(){

}
