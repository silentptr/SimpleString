#ifndef SIMPLE_STRING_H_
#define SIMPLE_STRING_H_

#include <cstdint>
#include <cstring>
#include <ostream>
#include <bitset>
#include <string>

namespace simpstr
{
    // UTF-8/ASCII only
    class simple_string
    {
    private:
        std::size_t m_Len;
        std::size_t m_ByteLen;
        char* m_Data;

        void calc_len() noexcept
        {
            m_Len = 0;

            for (std::size_t i = 0; i < m_ByteLen; ++i)
            {
                std::bitset<8> bits(m_Data[i]);

                if (!bits[7])
                {
                    ++m_Len;
                }
                else if (bits[7] && !bits[6])
                {
                    if (i == m_ByteLen - 1)
                    {
                        ++m_Len;
                    }
                    else
                    {
                        std::bitset<8> next(m_Data[i+1]);

                        if (!(next[7] && !next[6]))
                        {
                            ++m_Len;
                        }
                    }
                }
            }
        }
    public:
        simple_string() noexcept
        {
            m_Len = 0;
            m_ByteLen = 0;
            m_Data = nullptr;
        }

        simple_string(const char* p_Cstr) noexcept
        {
            m_ByteLen = 0;

            while (p_Cstr[m_ByteLen] != '\0')
            {
                ++m_ByteLen;
            }

            m_Data = new char[m_ByteLen + 1];
            std::memcpy(m_Data, p_Cstr, m_ByteLen);
            m_Data[m_ByteLen] = '\0';
            calc_len();
        }

        simple_string(const std::string& p_Str) noexcept : simple_string(p_Str.c_str()) { }

        simple_string(const simple_string& p_Str) noexcept
        : m_Len(p_Str.m_Len), m_ByteLen(p_Str.m_ByteLen)
        {
            m_Data = new char[m_ByteLen];
            std::memcpy(m_Data, p_Str.m_Data, m_ByteLen);
        }

        simple_string(simple_string&& p_Str) noexcept
        : m_Len(p_Str.m_Len), m_ByteLen(p_Str.m_ByteLen), m_Data(p_Str.m_Data)
        {
            std::memset(&p_Str, 0, sizeof(p_Str));
        }

        ~simple_string() noexcept
        {
            if (m_Data) { delete[] m_Data; }
        }

        const char* c_str() const noexcept { return m_Data; }
        inline bool empty() const noexcept { return m_Data != nullptr; }
        std::size_t length() const noexcept { return m_Len; }
        std::size_t byte_length() const noexcept { return m_ByteLen; }

        void clear() noexcept
        {
            if (!m_Data)
            {
                return;
            }

            delete[] m_Data;
            m_Data = nullptr;
            m_Len = 0;
            m_ByteLen = 0;
        }

        std::string to_stdstring() const noexcept { return std::string(m_Data); }

        simple_string& operator=(const simple_string& rhs) noexcept
        {
            if (!rhs.m_Data) { return *this; }

            delete[] m_Data;
            m_Data = rhs.m_Data;
            m_Len = rhs.m_Len;
            m_ByteLen = rhs.m_ByteLen;
            return *this;
        }

        simple_string& operator+=(const simple_string& rhs) noexcept
        {
            if (!rhs.m_Data) { return *this; }

            m_Len += rhs.m_Len;
            char* newData = new char[m_ByteLen + rhs.m_ByteLen + 1];
            newData[m_ByteLen + rhs.m_ByteLen] = '\0';
            std::memcpy(newData, m_Data, m_ByteLen);
            std::memcpy(newData + m_ByteLen, rhs.m_Data, rhs.m_ByteLen);
            m_ByteLen += rhs.m_ByteLen;
            delete[] m_Data;
            m_Data = newData;
            return *this;
        }

        friend simple_string operator+(simple_string lhs, const simple_string& rhs) noexcept
        {
            lhs += rhs;
            return lhs;
        }

        char& operator[](std::size_t index) { return m_Data[index]; }
    };
}

std::ostream& operator<<(std::ostream& os, const simpstr::simple_string& p_Str) noexcept
{
    if (p_Str.c_str())
    {
        os << p_Str.c_str();
    }
    
    return os;
}

#endif