#include <iostream>
#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <cstring>

class Message{
    public:
    Message():bodyLength_(0){} //default constructor,intializing bodyLength as 0
    enum {maxBytes=512}; //maximum size of message body
    enum {header=4}; //header size

    Message(std::string message){ //accepting string message
        bodyLength_=getNewBodyLength(message.length());
        encodeHeader();
        std::memcpy(data+header,message.c_str(),bodyLength_);
    }
    size_t getNewBodyLength(size_t newLength){
        if(newLength>maxBytes){
            return maxBytes;
        }
        return newLength;

    }
    std::string getData(){   //useful for sending data
        int length=header+bodyLength_;
        std::string result(data,length);
        return result;
    }
    std::string getBody(){  //use ful for displaying the data
        std::string dataStr=getData();
        std::string result=dataStr.substr(header,bodyLength_); //copy the actual message in header from header till bodylen
        return result;
    }
      
    bool decodeHeader (){   //to send raw data from room to client
        char new_header[header+1]="";
        strncpy(new_header,data,header); //copy the header from data into new header
        new_header[header]='\0';
        int headervalue=atoi(new_header); //converts to string
        if(headervalue>maxBytes){   //if extracted length is greater than maxbytes set length 0
            bodyLength_=0;
            return false;

        }
        bodyLength_=headervalue;
        return true;

    }

    void encodeHeader(){ //encoding the message length in first 4 bytes
        char new_header[header+1]="";
        sprintf(new_header, "%4d", static_cast<int>(bodyLength_));

        memcpy(data,new_header,header);
    }
    size_t getBodyLength(){
        return bodyLength_;
    }


    private:
      char data[header+maxBytes]; //array to store header+message
      size_t bodyLength_;

    
};


#endif 


