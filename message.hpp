#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>


class Message {
public:
    enum { maxBytes = 512 };
    enum { header = 4 };


    Message() : bodyLength_(0) {}


    Message(std::string message) {
        bodyLength_ = getNewBodyLength(message.size());
        encodeHeader();
        std::memcpy(data + header, message.c_str(), bodyLength_);
    }

    // ---------------- Utility and Helper Functions ----------------

    size_t getNewBodyLength(size_t newLength) {
        if (newLength > maxBytes)
            return maxBytes;
        return newLength;
    }

    void encodeHeader() {
        char new_header[header + 1] = "";
        std::sprintf(new_header, "%04d", bodyLength_);
        std::memcpy(data, new_header, header);
    }


    bool decodeHeader() {
        char new_header[header + 1] = "";
        std::strncpy(new_header, data, header);
        new_header[header] = '\0';

        int headerValue = std::atoi(new_header);
        if (headerValue > maxBytes) {
            bodyLength_ = 0;
            return false;
        }
        bodyLength_ = headerValue;
        return true;
    }

    // ---------------- Data Retrieval Functions ----------------


    std::string getData() {
        int length = header + bodyLength_;
        return std::string(data, length);
    }


    std::string getBody() {
        return std::string(data + header, bodyLength_);
    }


    void printMessage() {
        std::cout << "Message received: " << getBody() << std::endl;
    }


    size_t getBodyLength() {
        return bodyLength_;
    }

private:
    char data[header + maxBytes];
    size_t bodyLength_;
};

#endif // MESSAGE_HPP
