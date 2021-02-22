/*
 * PKI.cpp
 *
 *  Created on: Dec 1, 2020
 *      Author: kub0x
 */


#include <iostream>
#include <openssl/pem.h>
#include "../Headers/PKI.hpp"
#include <fstream>
#include <limits.h>
#include <unistd.h>
#include <sstream>

void PKI::ComputeIDFromCSR(std::string& id, X509_REQ *cert){
	unsigned char hash[EVP_MAX_MD_SIZE]={0};
	const EVP_MD *digest = EVP_get_digestbyname("sha1");
	unsigned int n = 0;
	X509_REQ_digest(cert, digest, hash, &n);
	std::stringstream ss;
	for(int i=0; i< 15; ++i) //take only 16bytes, this is 128bit output
	    ss << std::hex << (int)hash[i];
	id = ss.str();
}

std::string PKI::X509ToPEM(X509 *cert){
	char *strCert=0;
	BIO *bpem = BIO_new(BIO_s_mem());
	if (!PEM_write_bio_X509(bpem, cert)){
		perror("PEM_WRITE_bio_X509");
		exit(-1);
	}
	BUF_MEM *bmem = {0};
	BIO_get_mem_ptr(bpem, &bmem);
	strCert = new char[bmem->length]();
	BIO_read(bpem, (void*) strCert, bmem->length);
	BIO_free(bpem);
	X509_free(cert);
	std::cout << strCert << std::endl;
	return std::string(strCert);
}

void PKI::GenerateKeyPair(std::string file_name){
	BIGNUM *e = BN_new();
	BN_set_word(e, RSA_F4);
	RSA *rsa = RSA_new();
	RSA_generate_key_ex(rsa, 2048, e, NULL);
	BIO *pem_public = BIO_new_file((std::string("pub/")+file_name + ("_pub.pem")).c_str(), "w+");
	PEM_write_bio_RSAPublicKey(pem_public, rsa);
	BIO *pem_priv = BIO_new_file((std::string("priv/")+ file_name + ("_priv.pem")).c_str(), "w+");
	PEM_write_bio_RSAPrivateKey(pem_priv, rsa, NULL, NULL, 0, NULL, NULL);
	BIO_free_all(pem_public);
	BIO_free_all(pem_priv);
	RSA_free(rsa);
	BN_free(e);
	return;
}

void PKI::GenSSLX509(){
	GenerateKeyPair("tls");
	X509 *cert = X509_new();
	BIO *pem_priv = BIO_new_file("priv/tls_priv.pem", "r+");
	RSA *rsa=NULL;
	PEM_read_bio_RSAPrivateKey(pem_priv, &rsa, NULL, NULL);
	EVP_PKEY * pkey = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(pkey, rsa);

	ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
	X509_gmtime_adj(X509_get_notBefore(cert), 0);
	X509_gmtime_adj(X509_get_notAfter(cert), 31536000L);
	X509_set_pubkey(cert, pkey);
	X509_NAME * name = X509_get_subject_name(cert);
	X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC,
			                           (unsigned char *)"ES", -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC,
			                           (unsigned char *)"DAP2PNET", -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
				                       (unsigned char *)"TLS", -1, -1, 0);
	SignCert(cert, "tls");
	BIO_free_all(pem_priv);
	RSA_free(rsa);

	std::ifstream if_a("certs/tls.pem", std::ios_base::binary);
	std::ifstream if_b("certs/CA.pem", std::ios_base::binary);
	std::ofstream of_c("certs/tls_chain.pem", std::ios_base::binary);

	of_c << if_a.rdbuf() << if_b.rdbuf();
}

void PKI::GenSelfSigned(){ //Generate a fresh CA.pem, tls.pem
	std::fstream fs;
	fs.open("certs/CA.pem");
	if (fs.fail()){
		GenerateKeyPair("CA");
		X509 *cert = X509_new();
		BIO *pem_priv = BIO_new_file("priv/CA_priv.pem", "r+");
		RSA *rsa=NULL;
		PEM_read_bio_RSAPrivateKey(pem_priv, &rsa, NULL, NULL);
		EVP_PKEY * pkey = EVP_PKEY_new();
		EVP_PKEY_assign_RSA(pkey, rsa);
		ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
		X509_gmtime_adj(X509_get_notBefore(cert), 0);
		X509_gmtime_adj(X509_get_notAfter(cert), 31536000L);
		X509_set_pubkey(cert, pkey);
		X509_NAME * name = X509_get_subject_name(cert);
		X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC,
		                           (unsigned char *)"ES", -1, -1, 0);
		X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC,
		                           (unsigned char *)"DAP2PNET", -1, -1, 0);
		X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
			                       (unsigned char *)"REGISTRATION", -1, -1, 0);
		X509_set_issuer_name(cert, name);
		X509_sign(cert, pkey, EVP_sha256());
		BIO *cert_file = BIO_new_file("certs/CA.pem", "w+");
		PEM_write_bio_X509(cert_file, cert);
		BIO_free_all(cert_file);
		BIO_free_all(pem_priv);
		X509_free(cert);
		RSA_free(rsa);

		std::cout << "CA GENERATED!" << std::endl;
		GenSSLX509();
	}
}

X509* PKI::CSRtoX509(std::string strCsr){
	BIO* bo = BIO_new( BIO_s_mem() );
	BIO_write( bo, strCsr.c_str(),strCsr.length());
	X509_REQ *csr=NULL;
	if (!PEM_read_bio_X509_REQ(bo, &csr, NULL, NULL)){
		perror("PEM_read_bio_X509_REQ");
		return(NULL);
	}
	std::string id;
	ComputeIDFromCSR(id, csr);
	std::cout << "ID: " << id << std::endl;
	EVP_PKEY *pkey = X509_REQ_get_pubkey(csr);
	//replacing user's CN in cert
	X509_NAME *name= X509_REQ_get_subject_name(csr);
	X509_NAME_add_entry_by_txt(name, "CN",  MBSTRING_ASC,
		                           (unsigned char *) id.c_str(), -1, -1, 0);
	/*X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC,
		                           (unsigned char *)"ES", -1, -1, 0);
		X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC,
		                           (unsigned char *)"DAP2PNET USER", -1, -1, 0);*/
	X509 *cert = X509_new();
	X509_set_subject_name(cert, name);
	X509_set_pubkey(cert, pkey);
	BIO_free_all(bo);
	X509_REQ_free(csr);
	std::cout << "Parsed" << std::endl;
	return cert;
}

X509* PKI::SignCert(X509 *cert, std::string file_name){ //Issue Certificates signed with CA's private key
	BIO *pem_priv = BIO_new_file("priv/CA_priv.pem", "r+");
	RSA *rsa=NULL;
	PEM_read_bio_RSAPrivateKey(pem_priv, &rsa, NULL, NULL);
	EVP_PKEY * pkey = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(pkey, rsa);
	ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
	X509_gmtime_adj(X509_get_notBefore(cert), 0);
	X509_gmtime_adj(X509_get_notAfter(cert), 31536000L);
	X509_NAME *name = X509_NAME_new(); std::cout << "7" << std::endl;//key difference VS self signed
	X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC,
	                           (unsigned char *)"ES", -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC,
	                           (unsigned char *)"DAP2PNET", -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
	                           (unsigned char *)"REGISTRATION", -1, -1, 0);
	X509_set_issuer_name(cert, name); std::cout << "8" << std::endl;
	X509_sign(cert, pkey, EVP_sha256()); std::cout << "9" << std::endl;
	std::cout << "SIGNED" << std::endl;
	std::string filename = "certs/" + file_name + ".pem";
	BIO *cert_file = BIO_new_file(filename.c_str(), "w+");
	PEM_write_bio_X509(cert_file, cert);
	BIO_free_all(pem_priv);
	BIO_free_all(cert_file);
	RSA_free(rsa);
	X509_NAME_free(name);
	return cert;
}
