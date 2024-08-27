//Created by mars on 5/8/24

#ifndef MARS_BUFFER_H
#define MARS_BUFFER_H

#include "../Base/copyable.h"

#include <cassert>
#include <vector>
#include <string>
#include <algorithm>

namespace mars {

class Buffer : public base::copyable {
public:
    const static size_t kCheapPrepend = 8;
    const static size_t kInitialSize = 1024;

    Buffer()
        : m_buffer(kCheapPrepend + kInitialSize),
          m_readIndex(kCheapPrepend),
          m_writeIndex(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == kInitialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    size_t readableBytes() const {return m_writeIndex - m_readIndex;}
    size_t writableBytes() const {return m_buffer.size() - m_writeIndex;}
    size_t prependableBytes() const {return m_readIndex;}

    const char* peek() const {return begin() + m_readIndex;}

    const char* findCRLF() const
    {
        // FIXME: replace with memmem()?
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    void swap(Buffer& rhs){
        m_buffer.swap(rhs.m_buffer);
        std::swap(m_readIndex, rhs.m_readIndex);
        std::swap(m_writeIndex, rhs.m_writeIndex);
    }


    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if (len < readableBytes())
        {
            m_readIndex += len;
        }
        else
        {
            retrieveAll();
        }
    }

    void retrieveUntil(const char* end){
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveAll(){
        m_readIndex = kCheapPrepend;
        m_writeIndex = kCheapPrepend;
    }

    std::string retrieveAsString(){
        std::string str(peek(), readableBytes());
        retrieveAll();
        return str;
    }

    void append(const std::string& str){
        append(str.data(), str.size());
    }

    void hasWritten(size_t len){
        m_writeIndex += len;
    }

    void ensureWritableBytes(size_t len){
        if(writableBytes() < len){
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    void append(const char* data, size_t len){
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    void append(const void* data, size_t len){
        append(static_cast<const char*>(data), len);
    }

    void prepend(const void* /*restrict*/ data, size_t len){
        assert(len <= prependableBytes());
        m_readIndex -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d+len, begin()+m_readIndex);
    }

    const char* beginWrite() const
    { return begin() + m_writeIndex; }

    ssize_t readFd(int, int* savedErrno);//todo

    char* beginWrite() {return begin() + m_writeIndex;}

private:
    char* begin() {return &*m_buffer.begin();}

    const char* begin() const {return &*m_buffer.begin();}

    void makeSpace(size_t len){
        if(writableBytes() + prependableBytes() < len + kCheapPrepend){
            m_buffer.resize(m_writeIndex + len);
        } else {
            assert(kCheapPrepend < m_readIndex);
            size_t readable = readableBytes();
            std::copy(begin() + m_readIndex, begin() + m_writeIndex, begin() + kCheapPrepend);
            m_readIndex = kCheapPrepend;
            m_writeIndex = m_readIndex + readable;
            assert(readable == readableBytes());
        }
    }

    std::vector<char> m_buffer;
    size_t m_readIndex;
    size_t m_writeIndex;

    static const char kCRLF[];
};

}

#endif //MARS_BUFFER_H
