#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#pragma once

#include <QThread>
#include <QStringList>
#include <QString>
#include <ActiveQt/QAxObject>
#include <QDebug>
#include <qt_windows.h>
#include <sapi.h>
#include "designdata.h"
#include "basestructer.h"
#include "tcpcommandclient.h"

enum FilterType {
	None = 0x00,
	Num = 0x01,
	LowerLetter = 0x02,
	UpperLetter = 0x04,
	Chinese = 0x80
};

typedef struct {
	QString sheetName;
	bool isReturn = false;
}SheetRecord;

extern QMutex sheetMutex;

extern QVector<SheetRecord> sRecordVec;

class ReadFileThread : public QThread
{
    Q_OBJECT

public:
    explicit ReadFileThread(QObject* parent);
    QStringList filePath;
    bool listHandler(QAxObject *excel, QString fileName, FileType fileType);

    void castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res);
    QString num2Alpha(int num);
    QString processKm(QString kmStr); //将表中公里标规范化，只留数字
    static QString strFilter(QString input, QString filter, FilterType filtertype, bool flag);
protected:
    void run();

signals:
    void showProgressBar(int);
    void showResult(QString);
    void addNumSignal(FileType);
    void workfinished_SIGNAL();

private:
	
};
