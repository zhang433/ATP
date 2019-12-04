#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QStringList>
MainWindow* w;

int main(int argc, char *argv[])
{
    QStringList QSL;
    for(int i=1;i<argc;++i)
        QSL<<argv[i];
    QApplication a(argc, argv);
    w = new MainWindow();

	QFile file(QApplication::applicationDirPath() + "/filelist.datastream");
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray QBA = file.readAll();
		file.close();
		QDataStream QDS(&QBA, QIODevice::ReadOnly);
		QDS.setVersion(QDataStream::Qt_5_12);
		QDS.startTransaction();
		QDS >> w->fileNameList;
		QDS.commitTransaction();
	}
	if (!w->fileNameList.isEmpty())
	{
		QByteArray QBA;
		QDataStream QDS(&QBA, QIODevice::WriteOnly);
		QDS.setVersion(QDataStream::Qt_5_12);
		if (!w->ConvertFile())
		{
			QDS << QString("ERROR") << w->InnerInfo;
		}
		else
		{
			QDS << QString("OK") << w->InnerInfo << DesignData::accessRodeMap << DesignData::stationVec << DesignData::gradeDownProVec << DesignData::gradeUpProVec\
				<< DesignData::gradeDownBackVec << DesignData::gradeUpBackVec << DesignData::neutralSectionUpVec << DesignData::neutralSectionDownVec\
				<< DesignData::pathWayDataDownProVec << DesignData::pathWayDataUpProVec << DesignData::pathWayDataDownBackVec << DesignData::pathWayDataUpBackVec\
				<< DesignData::pathWaySpeedDownVec << DesignData::pathWaySpeedUpVec << DesignData::pathWaySpeedDownBackVec << DesignData::pathWaySpeedUpBackVec\
				<< DesignData::baliseLocationUpMap << DesignData::baliseLocationDownMap << DesignData::balishUseMap << DesignData::brokenLinkVec << DesignData::stationSideVec;
		}
		QFile dataSheet(QApplication::applicationDirPath() + '/' + "sheet.datastream");
		if (dataSheet.open(QIODevice::WriteOnly))
		{
			dataSheet.write(QBA);
			dataSheet.close();
		}
		exit(0);
	}

	w->show();
    return a.exec();
}
