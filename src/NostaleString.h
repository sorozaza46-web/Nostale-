#pragma once
#include <string>

struct NostaleStringA
{
    char* m_Buffer;
    size_t  m_Length;

    NostaleStringA(void) : m_Buffer(nullptr), m_Length(0) { }
    NostaleStringA(const char* str) : m_Buffer(nullptr), m_Length(0) { this->set(str); }
    ~NostaleStringA(void) { if (this->m_Buffer != nullptr) delete[] this->m_Buffer; }

    size_t len(void) { return (this->m_Buffer != nullptr) ? this->m_Length : 0; }
    char* get(void) { return (this->m_Buffer != nullptr) ? (char*)(this->m_Buffer + 0x08) : nullptr; }

    void set(const char* str)
    {
        if (this->m_Buffer != nullptr) delete[] this->m_Buffer;
        this->m_Length = strlen(str);
        this->m_Buffer = new char[this->m_Length + 8 + 1];
        *(unsigned int*)(this->m_Buffer + 0x00) = 1; 
        *(unsigned int*)(this->m_Buffer + 0x04) = this->m_Length; 
        memcpy(this->m_Buffer + 0x08, str, this->m_Length);
        this->m_Buffer[this->m_Length + 0x08] = '\0';
    }
};

