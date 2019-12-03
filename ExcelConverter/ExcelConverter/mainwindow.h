#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#ifdef _MSC_VER
#pragma execution_character_set("utf-8") 
#endif
#include <QMainWindow>
#include "../../Common/designdata.h"
#include "../../Common/basestructer.h"
enum class SheetType
{
    UP=0,//上行表
    DOWN,//下行表
    UP_FRONT,//上行正向表
    UP_BACK,//上行反向表
    DOWN_FRONT,//下行正向表
    DOWN_BACK,//下行反向表
    UNKNOWN,//未知表
};

typedef QList<QString> SheetLine;
typedef QPair<SheetType,QList<SheetLine>> Sheet;
typedef QList<Sheet> ExcelFile;

struct FileInfo
{
	QString fileName;
	FileType fileType;
};
bool operator<(const FileInfo& l, const FileInfo& r);

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	QString InnerInfo;
	QStringList fileNameList;
	bool ConvertFile();
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
	void printinfo(QString info);
	void printerr(QString err, QString line="", QString extra="");
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QList<QPair<QString, Sheet>> All_ConvertSheet;
	bool Print2Screen = false;

};
extern MainWindow* w;
#endif // MAINWINDOW_H
