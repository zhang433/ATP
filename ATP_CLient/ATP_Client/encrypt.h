#pragma once
#include <string>
#include <assert.h>
using namespace std;

class Encrypt
{
public:
    Encrypt() {}
    ~Encrypt() {}
    bool Encode(const string& input,string* output)
    {
        if (input.size() != 32)
            return false;
        for (auto c : input)
            if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')))
                return false;
        string binCode = Hex2Str(input);
        binCode[0] ^= 0b10110101;
        int start_pos = static_cast<unsigned char>(binCode[0]);
        int length = sizeof(table) / sizeof(table[0]);
        for (int i = 1; i < binCode.size(); ++i)
            binCode[i] ^= table[(start_pos + i) % length];
        *output = Str2Hex(binCode);
        return true;
    }
    bool Decode(const string& input, string* output)
    {
        if (input.size() != 32)
            return false;
        for (auto c : input)
            if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')))
                return false;
        string binCode = Hex2Str(input);
        int start_pos = static_cast<unsigned char>(binCode[0]);
        int length = sizeof(table) / sizeof(table[0]);
        binCode[0] ^= 0b10110101;
        for (int i = 1; i < binCode.size(); ++i)
            binCode[i] ^= table[(start_pos + i) % length];
        *output = Str2Hex(binCode);
        return true;
    }
private:
    string Str2Hex(const string& str)
    {
        auto CharDec2Hex = [](char c)->char {
            char start_char = '0';
            assert(c >= 0 && c <= 15);
            if (c >= 10)
                return c - 10 + 'A';
            return c + '0';
        };
        string ret;
        for (int i = 0; i < str.size(); ++i)
        {
            ret.push_back(CharDec2Hex((str[i] >> 4) & 0x0f));
            ret.push_back(CharDec2Hex(str[i] & 0x0f));
        }
        return ret;
    }

    string Hex2Str(const string& str)
    {
        auto CharHex2Dec = [](char c)->char {
            assert((c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'));
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
            return c - '0';
        };
        string ret;
        assert(str.size() % 2 == 0);
        for (int i = 0; i < str.size(); i += 2)
        {
            char c = 0;
            c |= (CharHex2Dec(str[i]) << 4);
            c |= CharHex2Dec(str[i + 1]);
            ret.push_back(c);
        }
        return ret;
    }
    const unsigned char table[12] = { 109,113,127,131,137,139,149,151,157,163,167,173 };
};

