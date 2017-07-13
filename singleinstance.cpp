#include "singleinstance.h"


SingleInstance::SingleInstance(QObject *parent)
    : QObject(parent)
{
    connect(&m_server, &QLocalServer::newConnection,
            this, &SingleInstance::onNewConnection);
}

void SingleInstance::onNewConnection()
{
   emit newInstance();
}

void SingleInstance::listen(QString name)
{
    m_server.removeServer(name);
    m_server.listen(name);
}

bool SingleInstance::hasPrevious(QString name)
{
    QLocalSocket socket;
    socket.connectToServer(name, QLocalSocket::ReadOnly);

    return socket.waitForConnected();
}
