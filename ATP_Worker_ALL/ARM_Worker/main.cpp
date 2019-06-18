#include <QCoreApplication>
#include <QFile>
#include <compareresdata_thread.h>
#include <QThread>
#include <inner_tcp_client.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	G_innerTcpClient = new InnerTcpClient();

	new CompareBaliseData_THREAD;
	qDebug() << "try to connect to receiver.";
	G_innerTcpClient->keepTryingConnectUntilSuccess();

    return a.exec();
}
