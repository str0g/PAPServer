#ifndef MYHASH_HPP_INCLUDED
#define MYHASH_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include <string>
#include <cryptopp/md5.h>
#include <cryptopp/rsa.h>
#include <cryptopp/base64.h>
#include "crypto++/hex.h"

///Specials
///Globals Varuabels
/**
* Hash functions from http://nopaste.gamedev.pl/?id=5546
*/
namespace Hash
{
        //Hash data by MD5 algorhitm
        inline std::string MD5(const std::string& data)
        {
                std::string res;
                CryptoPP::MD5 hash;
                byte digest[CryptoPP::MD5::DIGESTSIZE];

                hash.CalculateDigest(digest, (byte*)data.c_str(), data.size());

                CryptoPP::HexEncoder encoder;
                encoder.Attach(new CryptoPP::StringSink(res));
                encoder.Put(digest, sizeof(digest));
                encoder.MessageEnd();

                return res;
        }

        //Hash data by SHA512 algorhitm
        inline std::string SHA512(const std::string& data)
        {
                std::string res;
                CryptoPP::SHA512 hash;

                CryptoPP::StringSource encoder(data, true,
                        new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(res))));

                return res;
        }
}

#endif // MYHASH_HPP_INCLUDED
