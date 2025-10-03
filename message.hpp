#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>

/* 
header is of 4 bytes and maxBytes can be stored as 512 bytes
header stores the body length that is the current body length
data stores the header+bodyLength with maximum size of header+maxBytes

client attempts to send message:- It will encode header and put message into the data and send data
server gets the message, decodes the header, get the bodylength from the header and hence complete body
then server sends the message to all the clients connected to that room.
*/


class Message {
    public: 
        Message() : bodyLength_(0) {} //intiitlising with bodytlnegth=0
        
        enum {maxBytes = 512};  // similiar to const int MaxSize = 512;
        enum {header = 4};  // similiar to const int header = 4;

        Message(std::string message){
            bodyLength_ = getNewBodyLength(message.size());
            encodeHeader();
            std::memcpy(data + header, message.c_str(), bodyLength_); // copies the data in message.c_str() into data from header position ahead 
        };

        void printMessage(){
            std::string message = getData();
            std::cout<<"Message recieved: "<<message<<std::endl;
        }

        std::string getData(){
            int length = header + bodyLength_;
            std::string result(data, length);
            return result;
        }

        std::string getBody(){
            std::string dataStr = getData();
            std::string result = dataStr.substr(header, bodyLength_);
            return result;
        }

        size_t getNewBodyLength(size_t newLength){
            if(newLength > maxBytes){ // even if clent sends the message wirh size greater than this program will only return maxBytes length
                return maxBytes;
            }
            return newLength;
        }

        void encodeHeader(){
            char new_header[header+1] = "";
            sprintf(new_header, "%4d", static_cast<int>(bodyLength_)); // copies the bodyLength_ to new_header array such as ['1','0','2','3'] just for an example
            memcpy(data, new_header, header); // copies the first header=4 bytes from new_header into data
        }
        
       bool decodeHeader(){
            char new_header[header+1] = "";
            strncpy(new_header, data, header);
            new_header[header] = '\0';
            int headerValue = atoi(new_header);
            if(headerValue > maxBytes){
                bodyLength_ = 0;
                return false;
            }
            bodyLength_ = headerValue;
            return true;
        }

        size_t getBodyLength(){
            return bodyLength_;
        }

    private: 
        char data[header+maxBytes];
        size_t bodyLength_;
};

#endif MESSAGE_HPP
