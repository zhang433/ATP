#include "usermanagementui.h"
#include "ui_usermanagementui.h"
#include <QDebug>

QSettings UserManagementUI::REG("HKEY_CURRENT_USER\\Software\\ATPClient",QSettings::NativeFormat);
QCryptographicHash UserManagementUI::MD5(QCryptographicHash::Md5);
const unsigned char UserManagementUI::table[12] = {109,113,127,131,137,139,149,151,157,163,167,173};

UserManagementUI::UserManagementUI(QString UserName,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserManagementUI),
    UserName(UserName)
{
    ui->setupUi(this);
}

UserManagementUI::~UserManagementUI()
{
    delete ui;
}

void UserManagementUI::on_AddNewUser_clicked()
{
    if(UserName!="Administrator")
    {
        QMessageBox::information(this,"权限错误","您的权限不足");
        return;
    }
    while(1)
    {
        bool ok;
        QString user = QInputDialog::getText(this,QString("新添用户"),"请输入用户名",QLineEdit::Normal,"",&ok);
        if(!ok)
            return;
        if(user.isEmpty()) {
            QMessageBox::information(this,"错误","用户名不可为空");
            continue;
        }
        if(isUserExist(user))
        {
            QMessageBox::information(this,"错误","用户已存在");
            continue;
        }
        QString key = QInputDialog::getText(this,QString("新添用户"),"请输入密码",QLineEdit::Normal,"",&ok);
        if(key.isEmpty()) {
            QMessageBox::information(this,"错误","密码不可为空");
            continue;
        }
        if(QMessageBox::information(this,"新添用户","请确认新添用户信息:\n\t用户名:"+user+"\t"\
                                    "\n\t密码:"+key+"\t",QMessageBox::Yes,QMessageBox::No)==QMessageBox::No)
            continue;
        setUser(user,key);
        if(isUserValid(user,key))
            QMessageBox::information(this,"提示","用户添加成功！");
        else
            QMessageBox::information(this,"错误","用户添加失败！");
        refreshAllUser();
    }
}

void UserManagementUI::on_DeleteUser_clicked()
{
    if(UserName!="Administrator")
    {
        QMessageBox::information(this,"权限错误","您的权限不足");
        return;
    }
    auto itemList = ui->listWidget->selectedItems();
    for(auto x:itemList)
    {
        if(x->text()=="Administrator")
        {
            QMessageBox::information(this,"权限错误","管理员用户无法删除！");
            return;
        }
        deleteUser(x->text())?QMessageBox::information(this,"提示","用户"+x->text()+"删除成功！"):QMessageBox::information(this,"提示","用户"+x->text()+"删除失败！");
        refreshAllUser();
    }
}

void UserManagementUI::on_ResetUser_clicked()
{
    auto itemList = ui->listWidget->selectedItems();
    if(itemList.size()>1)
    {
        QMessageBox::information(this,"提示","您一次只能选择一个用户重置密码！");
        return;
    }
    for(auto x:itemList)
    {
        if(UserName!="Administrator"&&UserName!=x->text())
        {
            QMessageBox::information(this,"提示","您没有重置其他用户密码的权限！");
            return;
        }
        bool ok;
        if(UserName!="Administrator")
        {
            QString key = QInputDialog::getText(this,"密码重置","请输入该用户密码:",QLineEdit::Normal,"",&ok);
            if(!ok)
                return;
            if(!isUserValid(x->text(),key))
            {
                QMessageBox::information(this,"错误","您输入的密码有误，请重试！");
                return;
            }
        }
        QString key = QInputDialog::getText(this,"密码重置","请输入该用户的新密码:",QLineEdit::Normal,"",&ok);
        if(QMessageBox::information(this,"新添用户","请确认修改信息:\n\t用户名:"+x->text()+"\t"\
                                    "\n\t重置密码为:"+key+"\t",QMessageBox::Yes,QMessageBox::No)==QMessageBox::No)
            return;
        setUser(x->text(),key);
        if(isUserValid(x->text(),key))
            QMessageBox::information(this,"提示","用户密码重置成功！");
        else
            QMessageBox::information(this,"错误","用户密码重置失败！");
    }
}

QStringList UserManagementUI::refreshAllUser()
{
    QStringList QSL = REG.allKeys();
    if(QSL.contains("."))
        QSL.removeAt(QSL.indexOf("."));
    ui->listWidget->clear();
    ui->listWidget->addItems(QSL);
    return QSL;
}

bool UserManagementUI::setUser(QString user, QString key)
{
    QString key_MD5 = Encode2MD5(key);
    REG.setValue(user,key_MD5+"-"+QString::number(QDateTime::currentSecsSinceEpoch()));
    return isUserValid(user,key_MD5);
}

bool UserManagementUI::isRegstered()
{
    if(!isUserExist("."))
        return false;
    QString val = REG.value(".").toString();
    if(val!=Encode2MD5(getMacAndInstallTimeStr()))
    {
        qDebug()<<"val is "<<val;
        qDebug()<<"aim is "<<Encode2MD5(getMacAndInstallTimeStr());
        return false;
    }
    return true;
}

bool UserManagementUI::regsterThisMachine(QString input)
{
    if(input!=Encode2MD5(getMacAndInstallTimeStr()))
        return false;
    REG.setValue(".",input);
    return true;
}

QString UserManagementUI::getEncodeSequence()
{
    QString MD5Str = Encode2MD5(getMacAndInstallTimeStr());
    Encrypt e;
    string output;
    e.Encode(MD5Str.toStdString(),&output);
    return QString::fromStdString(output);
}

bool UserManagementUI::deleteUser(QString user)
{
    REG.remove(user);
    return !isUserExist(user);
}

bool UserManagementUI::isUserValid(QString user, QString key)
{
    if(!REG.value(user).toString().contains("-"))
        return false;
    return REG.value(user).toString().split("-")[0] == Encode2MD5(key);
}

bool UserManagementUI::isUserExist(QString user)
{
    return REG.value(user) != QVariant();
}

QString UserManagementUI::Encode2MD5(QString input)
{
    MD5.reset();
    MD5.addData(input.toUtf8());
    return MD5.result().toHex().toUpper();
}

QString UserManagementUI::getMacAndInstallTimeStr()
{
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",QSettings::NativeFormat);
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i = 0; i < nCnt; i ++)
    {
       // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
       if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
       {
           strMacAddr = nets[i].hardwareAddress();
           break;
       }
    }
    return strMacAddr+reg.value("InstallDate").toString();
}
