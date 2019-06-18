#pragma once
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <QDialog>
#include <QSettings>
#include <QDataStream>
#include <QCryptographicHash>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QProcess>
#include <QNetworkInterface>
#include <QByteArray>
#include <encrypt.h>

namespace Ui {
class UserManagementUI;
}

class UserManagementUI : public QDialog
{
    Q_OBJECT

public:
    explicit UserManagementUI(QString UserName,QWidget *parent = nullptr);
    static bool isUserExist(QString user);
    static bool isUserValid(QString user,QString key);
    static bool setUser(QString user,QString key);
    static bool isRegstered();
    static bool regsterThisMachine(QString input);
    static QString getEncodeSequence();
    QStringList refreshAllUser();
    ~UserManagementUI();

private slots:
    void on_AddNewUser_clicked();

    void on_DeleteUser_clicked();

    void on_ResetUser_clicked();

private:
    Ui::UserManagementUI *ui;

    static const unsigned char table[12];

    QString UserName;
    static bool deleteUser(QString user);
    static QSettings REG;
    static QCryptographicHash MD5;
    static QString Encode2MD5(QString input);
    static QString getMacAndInstallTimeStr();
};
