#include "codec.h"

#include "Logging.h"
#include "Mutex.h"
#include "EventLoopThread.h"
#include "TcpClient.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <EventLoop.h>
#include <thread>


using namespace muduo;
using namespace muduo::net;

typedef std::shared_ptr<FILE> FilePtr;
const int kBufSize = 64*1024;
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
//        client_.setMessageCallback(
//                std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
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
    void writeFlie(string filename)
    {
        FILE* fp = ::fopen(filename.c_str(), "w"); //read only
        if (fp)
        {
            FilePtr ctx(fp, ::fclose);
            char buf[kBufSize];
            int recv_sz;
            while((recv_sz = recv()))

        }
        else
        {
            conn->shutdown();
            LOG_INFO << "FileServer - no such file";
        }
//        FilePtr fp(fopen(filename.c_str(),"w"));
//        muduo::net::Buffer buf;
//        int recv_sz;
//        while((recv_sz = recv((*connection_).)))
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
        }
    }

    void onStringMessage(const TcpConnectionPtr&,
                         const string& message,
                         Timestamp)
    {
        printf("<<< %s\n", message.c_str());
    }

    TcpClient client_;
    //LengthHeaderCodec codec_;
    MutexLock mutex_;
    TcpConnectionPtr connection_ GUARDED_BY(mutex_);
};

void waitForDownload(){

    while (!download_finished){
        LOG_INFO<<"Downloading ...";
        usleep(100);
    }
    LOG_INFO<<"Download finished !!!";
}

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 3)
    {
        EventLoopThread loopThread;  //新建线程
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        InetAddress serverAddr(argv[1], port);
        LNDClient client(loopThread.startLoop(), serverAddr);
        client.connect();//创建socket并connect
        std::string filename;

        while (std::getline(std::cin, filename))
        {
            client.write(filename); // 发送文件名
            LOG_INFO << "Start to download "<<filename;
            std::thread msgThread(waitForDownload);
            client.writeFlie(filename);
            {
                MutexLockGuard g(mylock);
                download_finished = true;
            }
            msgThread.join();

        }
        client.disconnect();
        CurrentThread::sleepUsec(1000*1000);  // wait for disconnect, see ace/logging/client.cc
    }
    else
    {
        printf("Usage: %s host_ip port CMD\n", argv[0]);
    }
}

