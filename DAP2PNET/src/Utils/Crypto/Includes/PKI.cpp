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

void PKI::SaveCert(X509* cert){
	BIO *cert_file = BIO_new_file("CA.pem", "w+");
	PEM_write_bio_X509(cert_file, cert);
	BIO_free_all(cert_file);
}

void PKI::GenerateKeyPair(){
	BIGNUM *e = BN_new();
	BN_set_word(e, RSA_F4);
	RSA *rsa = RSA_new();
	RSA_generate_key_ex(rsa, 2048, e, NULL);
	// 2. save public key
	BIO *pem_public = BIO_new_file("public.pem", "w+");
	PEM_write_bio_RSAPublicKey(pem_public, rsa);
// 3. save private key
	BIO *pem_priv = BIO_new_file("private.pem", "w+");
	PEM_write_bio_RSAPrivateKey(pem_priv, rsa, NULL, NULL, 0, NULL, NULL);
	BIO_free_all(pem_public);
	BIO_free_all(pem_priv);
	RSA_free(rsa);
	BN_free(e);
	return;
}

void PKI::GenSelfSigned(){ //Generate a fresh CA.pem
	std::fstream fs;
	fs.open("CA.pem");
	if (fs.fail()){
		GenerateKeyPair();
		X509 *cert = X509_new();
		BIO *pem_priv = BIO_new_file("private.pem", "r+");
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
			                       (unsigned char *)"ADMIN", -1, -1, 0);
		X509_set_issuer_name(cert, name);
		X509_sign(cert, pkey, EVP_sha256());
		BIO *cert_file = BIO_new_file("CA.pem", "w+");
		PEM_write_bio_X509(cert_file, cert);
		BIO_free_all(cert_file);
		BIO_free_all(pem_priv);
		X509_free(cert);
		RSA_free(rsa);

		std::cout << "CA GENERATED!" << std::endl;
	}
}

X509* PKI::CSRtoX509(std::string strCsr){
	BIO* bo = BIO_new( BIO_s_mem() );
	BIO_write( bo, strCsr.c_str(),strCsr.length());
	//BIO *csr_file = BIO_new_file("csr.pem", "r+");
	X509_REQ *csr=NULL;
	if (!PEM_read_bio_X509_REQ(bo, &csr, NULL, NULL)){
		perror("PEM_read_bio_X509_REQ");
		return(NULL);
	}
	EVP_PKEY *pkey = X509_REQ_get_pubkey(csr);
	X509_NAME *name=X509_REQ_get_subject_name(csr);
	X509 *cert = X509_new();
	X509_set_subject_name(cert, name);
	X509_set_pubkey(cert, pkey);
	BIO_free_all(bo);
	X509_REQ_free(csr);
	std::cout << "Parsed" << std::endl;
	return cert;
}

X509* PKI::SignCert(X509 *cert, int id){ //Issue Certificates
	std::cout << "Called" << std::endl;
	BIO *pem_priv = BIO_new_file("private.pem", "r+"); std::cout << "0" << std::endl;
	RSA *rsa=NULL;
	PEM_read_bio_RSAPrivateKey(pem_priv, &rsa, NULL, NULL); std::cout << "1" << std::endl;
	EVP_PKEY * pkey = EVP_PKEY_new(); std::cout << "2" << std::endl;
	EVP_PKEY_assign_RSA(pkey, rsa); std::cout << "3" << std::endl;
	ASN1_INTEGER_set(X509_get_serialNumber(cert), 1); std::cout << "4" << std::endl;
	X509_gmtime_adj(X509_get_notBefore(cert), 0); std::cout << "5" << std::endl;
	X509_gmtime_adj(X509_get_notAfter(cert), 31536000L); std::cout << "6" << std::endl;
	X509_NAME * name = X509_NAME_new(); std::cout << "7" << std::endl;//key difference VS self signed
	X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC,
	                           (unsigned char *)"ES", -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC,
	                           (unsigned char *)"DAP2PNET", -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
	                           (unsigned char *)"ADMIN", -1, -1, 0);
	X509_set_issuer_name(cert, name); std::cout << "8" << std::endl;
	X509_sign(cert, pkey, EVP_sha256()); std::cout << "9" << std::endl;
	std::cout << "SIGNED" << std::endl;
	std::string filename = "user_signed_" + std::to_string(id) + ".pem";
	BIO *cert_file = BIO_new_file(filename.c_str(), "w+");
	PEM_write_bio_X509(cert_file, cert);
	BIO_free_all(pem_priv);
	BIO_free_all(cert_file);
	RSA_free(rsa);
	X509_NAME_free(name);
	X509_free(cert);
	return cert;
}
