#define _CRT_SECURE_NO_WARNINGS
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <iostream>
#include <string>
using namespace std;

#include <aes.h>
#include <Hex.h>      // StreamTransformationFilter  
#include <modes.h>    // CFB_Mode  
using namespace CryptoPP;
#pragma comment(lib, "cryptlib.lib" )

#include <boost\program_options.hpp>
#include <boost\algorithm\string.hpp>
#include <boost\filesystem.hpp>
#include <boost\algorithm\hex.hpp>
namespace po = boost::program_options;

std::string CBC_AESEncryptStr(std::string sKey, std::string sIV, const char* plainText, __int64 len)
{
	std::string outstr;

	//��key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? std::memcpy(key, sKey.c_str(), sKey.size()) : std::memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//��iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? std::memcpy(iv, sIV.c_str(), sIV.size()) : std::memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);

	AES::Encryption aesEncryption((byte*)key, AES::MAX_KEYLENGTH);

	CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	StreamTransformationFilter cbcEncryptor(cbcEncryption, new HexEncoder(new StringSink(outstr)));
	cbcEncryptor.Put((byte*)plainText, len);
	cbcEncryptor.MessageEnd();

	return outstr;
}

std::string CBC_AESDecryptStr(std::string sKey, std::string sIV, const char* cipherText, __int64 len)
{
	std::string outstr;

	//��key    
	SecByteBlock key(AES::MAX_KEYLENGTH);
	memset(key, 0x30, key.size());
	sKey.size() <= AES::MAX_KEYLENGTH ? std::memcpy(key, sKey.c_str(), sKey.size()) : std::memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

	//��iv    
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x30, AES::BLOCKSIZE);
	sIV.size() <= AES::BLOCKSIZE ? std::memcpy(iv, sIV.c_str(), sIV.size()) : std::memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);


	CBC_Mode<AES >::Decryption cbcDecryption((byte*)key, AES::MAX_KEYLENGTH, iv);

	HexDecoder decryptor(new StreamTransformationFilter(cbcDecryption, new StringSink(outstr)));
	decryptor.Put((byte*)cipherText, len);
	decryptor.MessageEnd();

	return outstr;
}

int main(int argc, char* argv[])
{
	string key = "123456";
	string iv = "654321";
	po::options_description desc("Allowed options");//�����������
	desc.add_options()//�������ѡ��
		("help,h", "produce help message")
		("encode,e", po::value<string>(), "input string")
		("decode", po::value<string>(), "input string")
		("key", po::value<string>(), "change encryption/decryption key to specified string")
		("iv", po::value<string>(), "change initilize vector to specified string");
	po::variables_map vm;//�������������ѡ���ӳ��
	try
	{
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (std::exception& ex)//�쳣����
	{
		std::cout << ex.what() << std::endl;
		return -1;//�������δͨ�����
	}
	//����Ƿ�������
	if (!vm.count("encode") && !vm.count("decode"))
		return -2;
	if (vm.count("key"))
		key = vm["key"].as<string>();
	if (vm.count("iv"))
		iv = vm["iv"].as<string>();
	//ִ��ת��
	string input;
	string output;
	try {
		if (vm.count("encode"))
		{
			input = vm["encode"].as<string>();
			output = CBC_AESEncryptStr(key, iv, input.data(), input.size());
		}
		else if (vm.count("decode"))
		{
			input = vm["decode"].as<string>();
			output = CBC_AESDecryptStr(key, iv, input.data(), input.size());
		}
		cout << output;
	}
	catch (exception e)
	{
		cout << e.what() << endl;
		return -3;
	}
	return 0;
}