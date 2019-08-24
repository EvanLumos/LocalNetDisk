#include "Logging.h"
#include "Mutex.h"
#include "EventLoopThread.h"
#include "TcpClient.h"

#include <memory>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <EventLoop.h>
#include <thread>
#include <fstream>
#include <string>

#define kHeaderLen 0

using namespace muduo;
using namespace muduo::net;

typedef std::shared_ptr<FILE> FilePtr;
const int kBufSize = 64*1024;
const std::string hostIp = "127.0.0.1";
const uint16_t port = 8080;

bool download_finished = false;
MutexLock mylock;

class LNDClient : noncopyable
{
public:
    LNDClient(EventLoop* loop, const InetAddress& serverAddr)
            : client_(loop, serverAddr, "LNDClient")
              //codec_(std::bind(&LNDClient::onStringMessage, this, _1, _2, _3))
    {
        client_.setConnectionCallback(
                std::bind(&LNDClient::onConnection, this, _1));
        client_.setMessageCallback(
                std::bind(&LNDClient::onMessage, this, _1, _2, _3));
        client_.enableRetry();
    }

    void connect()
    {
        client_.connect();
    }

    void disconnect()
    {
        client_.disconnect();
    }

    void write(const StringPiece& message)
    {
        MutexLockGuard lock(mutex_);
        if (connection_)
        {
            send(get_pointer(connection_), message);
        }
    }
    void send(muduo::net::TcpConnection* conn,
              const muduo::StringPiece& message)
    {
        muduo::net::Buffer buf;
        buf.append(message.data(), message.size());
        int32_t len = static_cast<int32_t>(message.size());
        int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
        buf.prepend(&be32, sizeof be32);
        conn->send(&buf);
    }
    void setFilename(string name){
        filename_ = name;
    }

    unsigned long long getTotalBytes(){
        return totalBytes_;
    }
    bool getFinishState(){
        return finished_;
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");

        MutexLockGuard lock(mutex_);
        if (conn->connected())
        {
            connection_ = conn;
        }
        else
        {
            connection_.reset();
            //LOG_INFO << "**************** reset ************************";
        }
    }
    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp receiveTime)
    {
        //MutexLockGuard guard(mutex1_);
        //LOG_INFO << "Start to download "<<"filename_";
        //int count = 1;
        std::fstream fs;
        fs.open(filename_,std::fstream::in|std::fstream::binary|std::fstream::app);
        if(!fs.is_open())
            return;

        while (buf->readableBytes() > 0) // kHeaderLen == 0
        {
            auto len = buf->readableBytes();
            fs.write(buf->peek(),len);
            totalBytes_ += static_cast<unsigned long long>(len);
            buf->retrieve(len);
        }
        fs.close();
//        if(oldValue == totalBytes_){
//            LOG_INFO << "Received "<< getTotalBytes() << " bytes.";
//            LOG_INFO << "Download finished !!!";
//            MutexLockGuard guard1(mutex_);
//            connection_ ->disconnected();
//        }

    }

    void onStringMessage(const TcpConnectionPtr&,
                         const string& message,
                         Timestamp)
    {
        printf("<<< %s\n", message.c_str());
    }
    MutexLock mutex1_;
    unsigned long long totalBytes_ GUARDED_BY(mutex1_);
    bool finished_ = false;
    string filename_;
    TcpClient client_;
   // LengthHeaderCodec codec_;
    MutexLock mutex_;
    TcpConnectionPtr connection_ GUARDED_BY(mutex_);
};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 1)
    {

        EventLoopThread loopThread;  //新建线程
        //static_cast<uint16_t>(atoi(argv[2]));
        InetAddress serverAddr(hostIp, port);
        LNDClient client(loopThread.startLoop(), serverAddr);
        client.connect();//创建socket并connect
        std::string filename(argv[1]);
        client.setFilename(filename);
        LOG_INFO << "Download started !!!";
        //client.write(filename); // 发送文件名
       // LOG_INFO << "Received "<< client.getTotalBytes() << " bytes.";
       // LOG_INFO << "Download finished !!!";
        client.disconnect();
        CurrentThread::sleepUsec(1000*1000);  // wait for disconnect, see ace/logging/client.cc
    }
    else
    {
        printf("Usage: %s filename\n", argv[0]);
    }

}

