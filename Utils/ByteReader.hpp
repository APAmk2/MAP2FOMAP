//All copyrights goes to JordanCpp, thnx for allowing me to use it - APAMK2

#pragma once

#include <fstream>
#include <string>

class ByteReader
{
public:
    enum
    {
        BigEndian,
        LittleEndian
    };

    bool Reset(const std::string& path, size_t type);
    void Close();
    ~ByteReader();
    size_t Bytes();
    bool Eof();
    uint8_t u8();
    uint16_t u16();
    uint32_t u32();
    int16_t i16();
    int32_t i32();
    ByteReader& operator>>(uint32_t& i);
    ByteReader& operator>>(uint16_t& i);
    ByteReader& operator>>(uint8_t& i);
    ByteReader& operator>>(int32_t& i);
    ByteReader& operator>>(int16_t& i);
    ByteReader& operator>>(int8_t& i);
    std::string string(int len);
    void Pos(size_t value);
    void Read(void* buffer, size_t bytes);
    size_t CurrPos();
private:
    size_t _Type;
    size_t _Bytes;
    std::fstream _File;
};