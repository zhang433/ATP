#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include "mainwindow.h"
#include <QApplication>
#include <direct.h>
#include <thread>
#include "qinputdialog.h"
#include "self_define.h"
#include "tcpdataclient.h"
#include "tcpcommandclient.h"
#include <QDesktopWidget>
#include "usermanagementui.h"
#include <analyze_rbcmessage.h>
#include <QSettings>
const char mask_novalue[8] = { static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF),static_cast<char>(0xFF) };


TcpDataClient* tcpDataClient;
TcpCommandClient* tcpCommandClient;

/*
TcpDataSocket->SendData_SIGNAL(const char*ptr,qint64 len);
*/

int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    //����ʱ���豸ע����֤��⹤��
    if(!UserManagementUI::isRegstered())
    {
        bool ok;
        QInputDialog::getText(nullptr,"�豸δע��","���豸δע�ᣬ�뽫�������������뷢�͸���Ŀ�����Ա�Ի�ȡ��Ȩ��",QLineEdit::Normal,UserManagementUI::getEncodeSequence(),&ok);
        if(!ok)
            exit(0);
        QString sequence;
        while(1){
            sequence = QInputDialog::getText(nullptr,"�豸δע��","�����뱾�豸����Ȩ������:",QLineEdit::Normal,"",&ok);
            if(!ok)
                exit(0);
            if(UserManagementUI::regsterThisMachine(sequence))
                break;
            QMessageBox::information(nullptr,"�豸δע��","��Ȩ������Ч������������");
        }
    }

    //���û������Ĭ�ϵĳ�������ԱȨ�����룬��ô������һ��Ĭ�ϵ�
    if(!UserManagementUI::isUserExist("Administrator"))
        UserManagementUI::setUser("Administrator","123456789");

    QString user;
    //�û���¼����
    while(1)
    {
        bool ok;
        user = QInputDialog::getText(nullptr,QString("�û���¼"),"�������û���",QLineEdit::Normal,"Administrator",&ok);
        if(!ok)
            exit(0);
        //��Ȩ�û�����ѯע����м�¼������
        if(user.isEmpty()||!UserManagementUI::isUserExist(user)) {
            QMessageBox::information(nullptr,"��¼����","�û��������ڣ�");
            continue;
        }
        else {
            QString key = QInputDialog::getText(nullptr,QString("�û���¼"),"����������",QLineEdit::Normal,"",&ok);
            if(UserManagementUI::isUserValid(user,key))
            {
                if(user=="Administrator" && key=="123456789")
                {
                    while(1)
                    {
                        QString key1 = QInputDialog::getText(nullptr,QString("�������"),"��ǰ����Ա����ΪĬ�����룬�������µĹ���Ա���룺",QLineEdit::Normal,"",&ok);
                        if(!ok)exit(0);
                        QString key2 = QInputDialog::getText(nullptr,QString("�������"),"��������һ�飺",QLineEdit::Normal,"",&ok);
                        if(!ok)exit(0);
                        if(key1!=key2)
                        {
                            QMessageBox::information(nullptr,"����","������������벻һ�£����������룡");
                            continue;
                        }
                        if(key1=="123456789")
                        {
                            QMessageBox::information(nullptr,"����","�������벻����Ĭ��������ͬ������������");
                            continue;
                        }
                        UserManagementUI::setUser(user,key1);
                        break;
                    }
                }
                break;
            }
            else
                QMessageBox::information(nullptr,"��¼����","�������");
        }
    }

    Analyze_RBCMessage::Init();

    tcpDataClient = new TcpDataClient();
    QThread* TCPClient_THREAD_DATA = new QThread();
    tcpDataClient->moveToThread(TCPClient_THREAD_DATA);
    TCPClient_THREAD_DATA->start();


    tcpCommandClient = new TcpCommandClient();
    QThread* TCPClient_THREAD_COMMOND = new QThread();
    tcpCommandClient->moveToThread(TCPClient_THREAD_COMMOND);
    TCPClient_THREAD_COMMOND->start();

    MainWindow w(user);

    //������ĵز�״̬���ĸ���
    QObject::connect(tcpCommandClient, &TcpCommandClient::UpdateMainWondowStatue_SIGNAL, &w, &MainWindow::UpdateStatusState_SLOT);
	QObject::connect(tcpDataClient, &TcpDataClient::UpdateMainWondowStatue_SIGNAL, &w, &MainWindow::UpdateStatusState_SLOT);
    //������������ͼ���ڵ��ź�
    QObject::connect(tcpCommandClient,&TcpCommandClient::ReDraw_MainWindow_SIGNAL,&w,&MainWindow::ReDraw_MainWindow);
    QObject::connect(tcpDataClient,&TcpDataClient::ReDraw_MainWindow_SIGNAL,&w,&MainWindow::ReDraw_MainWindow);

    //��ʼ����
    emit tcpDataClient->init();
    emit tcpCommandClient->init();

    //���ó�����ʾ�����С�ͳ�ʼλ��
    QDesktopWidget* desktopWidget = QApplication::desktop();
    int curMonitor = desktopWidget->screenNumber();
    QRect rect = desktopWidget->screenGeometry(curMonitor);
    QRect app_size;
    app_size.setWidth(rect.width()*0.8);
    app_size.setHeight(rect.height()*0.6);
    w.setGeometry(app_size);
    w.move((rect.width() - w.width())/2, (rect.height() - w.height())/2);

	w.show();

	return a.exec();
}
