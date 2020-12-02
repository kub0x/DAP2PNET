/*
 * SSLSocket.hpp
 *
 *  Created on: Dec 2, 2020
 *      Author: kub0x
 */

#ifndef SRC_UTILS_SOCKETS_HEADERS_SSLCLIENTSOCKET_HPP_
#define SRC_UTILS_SOCKETS_HEADERS_SSLCLIENTSOCKET_HPP_

#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../Headers/ClientSocket.hpp"

class SSLClientSocket : public ClientSocket {

private:

SSL_CTX* ctx;
SSL*     ssl;
X509*    client_cert;
char*    str;
char     buf [4096];
SSL_METHOD *meth;

private:

	void InitSSL();

public:

	SSLClientSocket(std::string IP, int port) : ClientSocket(IP,port){;}
	SSLClientSocket(int sockfd) : ClientSocket(sockfd){ InitSSL(); }
	void Connect();
	void Read();
	void Write();

};



#endif /* SRC_UTILS_SOCKETS_HEADERS_SSLCLIENTSOCKET_HPP_ */
