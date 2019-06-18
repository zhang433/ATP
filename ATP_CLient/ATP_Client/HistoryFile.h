#pragma once
#include <QString>
#include <QDataStream>
//#include "Command.h"

#include <qmutex.h>
class HistoryFile {
public:
	HistoryFile() {};
	void Open(QString path)
	{
		file.setFileName(path);
		Q_ASSERT(file.open(QIODevice::ReadWrite));
		fileStream.setDevice(&file);
		fileStream.setVersion(QDataStream::Qt_5_6);
	}
	template<typename T>
	HistoryFile& operator<<(const T& RTD)
	{
		QMutexLocker lc(&m_mutex);
		fileStream << RTD;
		file.flush();
		return *this;
	}
	template<typename T>
	HistoryFile& operator>>(T& RTD)
	{
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
