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
        QMessageBox::information(this,"Ȩ�޴���","����Ȩ�޲���");
        return;
    }
    while(1)
    {
        bool ok;
        QString user = QInputDialog::getText(this,QString("�����û�"),"�������û���",QLineEdit::Normal,"",&ok);
        if(!ok)
            return;
        if(user.isEmpty()) {
            QMessageBox::information(this,"����","�û�������Ϊ��");
            continue;
        }
        if(isUserExist(user))
        {
            QMessageBox::information(this,"����","�û��Ѵ���");
            continue;
        }
        QString key = QInputDialog::getText(this,QString("�����û�"),"����������",QLineEdit::Normal,"",&ok);
        if(key.isEmpty()) {
            QMessageBox::information(this,"����","���벻��Ϊ��");
            continue;
        }
        if(QMessageBox::information(this,"�����û�","��ȷ�������û���Ϣ:\n\t�û���:"+user+"\t"\
                                    "\n\t����:"+key+"\t",QMessageBox::Yes,QMessageBox::No)==QMessageBox::No)
            continue;
        setUser(user,key);
        if(isUserValid(user,key))
            QMessageBox::information(this,"��ʾ","�û���ӳɹ���");
        else
            QMessageBox::information(this,"����","�û����ʧ�ܣ�");
        refreshAllUser();
    }
}

void UserManagementUI::on_DeleteUser_clicked()
{
    if(UserName!="Administrator")
    {
        QMessageBox::information(this,"Ȩ�޴���","����Ȩ�޲���");
        return;
    }
    auto itemList = ui->listWidget->selectedItems();
    for(auto x:itemList)
    {
        if(x->text()=="Administrator")
        {
            QMessageBox::information(this,"Ȩ�޴���","����Ա�û��޷�ɾ����");
            return;
        }
        deleteUser(x->text())?QMessageBox::information(this,"��ʾ","�û�"+x->text()+"ɾ���ɹ���"):QMessageBox::information(this,"��ʾ","�û�"+x->text()+"ɾ��ʧ�ܣ�");
        refreshAllUser();
    }
}

void UserManagementUI::on_ResetUser_clicked()
{
    auto itemList = ui->listWidget->selectedItems();
    if(itemList.size()>1)
    {
        QMessageBox::information(this,"��ʾ","��һ��ֻ��ѡ��һ���û��������룡");
        return;
    }
    for(auto x:itemList)
    {
        if(UserName!="Administrator"&&UserName!=x->text())
        {
            QMessageBox::information(this,"��ʾ","��û�����������û������Ȩ�ޣ�");
            return;
        }
        bool ok;
        if(UserName!="Administrator")
        {
            QString key = QInputDialog::getText(this,"��������","��������û�����:",QLineEdit::Normal,"",&ok);
            if(!ok)
                return;
            if(!isUserValid(x->text(),key))
            {
                QMessageBox::information(this,"����","��������������������ԣ�");
                return;
            }
        }
        QString key = QInputDialog::getText(this,"��������","��������û���������:",QLineEdit::Normal,"",&ok);
        if(QMessageBox::information(this,"�����û�","��ȷ���޸���Ϣ:\n\t�û���:"+x->text()+"\t"\
                                    "\n\t��������Ϊ:"+key+"\t",QMessageBox::Yes,QMessageBox::No)==QMessageBox::No)
            return;
        setUser(x->text(),key);
        if(isUserValid(x->text(),key))
            QMessageBox::information(this,"��ʾ","�û��������óɹ���");
        else
            QMessageBox::information(this,"����","�û���������ʧ�ܣ�");
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
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// ��ȡ��������ӿ��б�
    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i = 0; i < nCnt; i ++)
    {
       // ���������ӿڱ�������������в��Ҳ��ǻػ���ַ�������������Ҫ�ҵ�Mac��ַ
       if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
       {
           strMacAddr = nets[i].hardwareAddress();
           break;
       }
    }
    return strMacAddr+reg.value("InstallDate").toString();
}
