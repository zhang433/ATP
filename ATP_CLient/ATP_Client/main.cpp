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
    //开启时的设备注册认证检测工作
    if(!UserManagementUI::isRegstered())
    {
        bool ok;
        QInputDialog::getText(nullptr,"设备未注册","本设备未注册，请将以下特征序列码发送给项目组管理员以获取授权码",QLineEdit::Normal,UserManagementUI::getEncodeSequence(),&ok);
        if(!ok)
            exit(0);
        QString sequence;
        while(1){
            sequence = QInputDialog::getText(nullptr,"设备未注册","请输入本设备的授权序列码:",QLineEdit::Normal,"",&ok);
            if(!ok)
                exit(0);
            if(UserManagementUI::regsterThisMachine(sequence))
                break;
            QMessageBox::information(nullptr,"设备未注册","授权序列无效！请重新输入");
        }
    }

    //如果没有设置默认的超级管理员权限密码，那么就设置一个默认的
    if(!UserManagementUI::isUserExist("Administrator"))
        UserManagementUI::setUser("Administrator","123456789");

    QString user;
    //用户登录操作
    while(1)
    {
        bool ok;
        user = QInputDialog::getText(nullptr,QString("用户登录"),"请输入用户名",QLineEdit::Normal,"Administrator",&ok);
        if(!ok)
            exit(0);
        //授权用户，查询注册表中记录的密码
        if(user.isEmpty()||!UserManagementUI::isUserExist(user)) {
            QMessageBox::information(nullptr,"登录错误","用户名不存在！");
            continue;
        }
        else {
            QString key = QInputDialog::getText(nullptr,QString("用户登录"),"请输入密码",QLineEdit::Normal,"",&ok);
            if(UserManagementUI::isUserValid(user,key))
            {
                if(user=="Administrator" && key=="123456789")
                {
                    while(1)
                    {
                        QString key1 = QInputDialog::getText(nullptr,QString("密码更改"),"当前管理员密码为默认密码，请设置新的管理员密码：",QLineEdit::Normal,"",&ok);
                        if(!ok)exit(0);
                        QString key2 = QInputDialog::getText(nullptr,QString("密码更改"),"请再输入一遍：",QLineEdit::Normal,"",&ok);
                        if(!ok)exit(0);
                        if(key1!=key2)
                        {
                            QMessageBox::information(nullptr,"错误","两次输入的密码不一致，请重新输入！");
                            continue;
                        }
                        if(key1=="123456789")
                        {
                            QMessageBox::information(nullptr,"错误","重置密码不可与默认密码相同，请重新输入");
                            continue;
                        }
                        UserManagementUI::setUser(user,key1);
                        break;
                    }
                }
                break;
            }
            else
                QMessageBox::information(nullptr,"登录错误","密码错误！");
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

    //主界面的地步状态栏的更新
    QObject::connect(tcpCommandClient, &TcpCommandClient::UpdateMainWondowStatue_SIGNAL, &w, &MainWindow::UpdateStatusState_SLOT);
	QObject::connect(tcpDataClient, &TcpDataClient::UpdateMainWondowStatue_SIGNAL, &w, &MainWindow::UpdateStatusState_SLOT);
    //更新主界面视图窗口的信号
    QObject::connect(tcpCommandClient,&TcpCommandClient::ReDraw_MainWindow_SIGNAL,&w,&MainWindow::ReDraw_MainWindow);
    QObject::connect(tcpDataClient,&TcpDataClient::ReDraw_MainWindow_SIGNAL,&w,&MainWindow::ReDraw_MainWindow);

    //开始连接
    emit tcpDataClient->init();
    emit tcpCommandClient->init();

    //设置程序显示截面大小和初始位置
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
