#ifndef EXCEL2QSTR_H
#define EXCEL2QSTR_H
#include <QObject>
#include <QMap>
#include <QAxObject>
#include "mainwindow.h"



class Excel2QStr
{
public:
    Excel2QStr(QStringList list,QString& info);
    QList<QPair<FileInfo, ExcelFile>> getQStrTable();
    ~Excel2QStr();
private:
    Sheet castVariant2ListListVariant(const QVariant &var);
    QStringList list;
    const static QMap<QString,FileType> KeyWord_FileName;
    QAxObject excel;
    QAxObject *workbooks;
    QString& info;
};

#endif // EXCEL2QSTR_H
