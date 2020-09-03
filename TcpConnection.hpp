

#include<map>

class EventHandler;
class TcpConnection
{

public:
    TcpConnection(/* args */);
    ~TcpConnection();



    std::map<int,EventHandler*> connlist;
    private:

};

TcpConnection::TcpConnection(/* args */)
{
}

TcpConnection::~TcpConnection()
{
}
