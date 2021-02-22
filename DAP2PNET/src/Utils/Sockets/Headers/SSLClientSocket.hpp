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
#include <mutex>
#include "../Headers/ClientSocket.hpp"

class SSLClientSocket : public ClientSocket {

private:

SSL_CTX* ctx=0;
SSL*     ssl=0;
X509*    client_cert=0;
char     buf [4096]={0};
SSL_METHOD *meth=0;
std::mutex lock_rw;
bool mutual_auth = 0;

private:

	void InitSSL();
	static int VerificationCallback(int flag, X509_STORE_CTX *ctx);

public:

	SSLClientSocket(std::string remote_IP, int port) : ClientSocket(remote_IP,port){;}
	SSLClientSocket(int sockfd, std::string source_IP) : ClientSocket(sockfd, source_IP){ InitSSL(); }
	SSLClientSocket(int sockfd, std::string source_IP, bool mutual_auth) : ClientSocket(sockfd, source_IP){ this->mutual_auth=mutual_auth; InitSSL(); }
	void Connect();
	void Read();
	void Write(const std::string& message);

};



#endif /* SRC_UTILS_SOCKETS_HEADERS_SSLCLIENTSOCKET_HPP_ */
