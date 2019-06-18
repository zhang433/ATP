#pragma once
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include <QString>
#include <QDataStream>
#include <QDebug>
#include <QFile>
//#include "Command.h"

#include <qmutex.h>
class HistoryFile {
public:
    HistoryFile() = default;
    void SetFilePath(QString path)
	{
		file.setFileName(path);
	}
	bool isOpen()
	{
		return file.isOpen();
	}
	void writeQBA(QByteArray& QBA)
	{
        if(!file.isOpen())
        {
            file.open(QIODevice::WriteOnly);
            file.close();
            file.open(QIODevice::ReadWrite);
            fileStream.setDevice(&file);
            fileStream.setVersion(QDataStream::Qt_5_6);
        }
		QMutexLocker lc(&m_mutex);
		file.write(QBA);
		file.flush();
	}
	QString name()
	{
		return file.fileName();
	}
	QFile& getfile()
	{
		return file;
	}
	void reName(QString name)
	{
        if(!file.isOpen())
        {
            file.open(QIODevice::WriteOnly);
            file.close();
            file.open(QIODevice::ReadWrite);
            fileStream.setDevice(&file);
            fileStream.setVersion(QDataStream::Qt_5_6);
        }
		QMutexLocker lc(&m_mutex);
		file.close();
		QString suffix = file.fileName().mid(file.fileName().lastIndexOf('.'));
		file.rename(file.fileName().mid(0, file.fileName().lastIndexOf('/') + 1) + name + suffix);
		qDebug() << "after rename:" << file.fileName();
		file.open(QIODevice::ReadWrite);
		fileStream.setDevice(&file);
		fileStream.setVersion(QDataStream::Qt_5_6);
    }
	template<typename T>
	HistoryFile& operator<<(const T& RTD)
	{
        if(!file.isOpen())
        {
            file.open(QIODevice::WriteOnly);
            file.close();
            file.open(QIODevice::ReadWrite);
            fileStream.setDevice(&file);
            fileStream.setVersion(QDataStream::Qt_5_6);
        }
		QMutexLocker lc(&m_mutex);
		fileStream << RTD;
		file.flush();
		return *this;
	}
	template<typename T>
	HistoryFile& operator>>(T& RTD)
	{
        if(!file.isOpen())
        {
            file.open(QIODevice::WriteOnly);
            file.close();
            file.open(QIODevice::ReadWrite);
            fileStream.setDevice(&file);
            fileStream.setVersion(QDataStream::Qt_5_6);
        }
		QMutexLocker lc(&m_mutex);
		fileStream >> RTD;
		return *this;
	}
	~HistoryFile()
	{
		file.close();
	}
	HistoryFile(const HistoryFile&) = delete;
	void operator=(const HistoryFile&) = delete;
private:
	QDataStream fileStream;
	QFile file;
	QMutex m_mutex;
};
