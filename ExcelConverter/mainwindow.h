#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
typedef QList<QString> SheetLine;
typedef QList<SheetLine> Sheet;
typedef QList<Sheet> ExcelFile;

enum FileType
{
	JINLU = 0,
	CHEZHAN,
	PODU,
	FENXIANG,
    XIANLUSHUJU,
	SUDU,
	YINGDAQIWEIZHI,
	DUANLIAN,
	ZHANTAI,
	NONE
};

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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
	void printinfo(QString info);
	void printerr(QString err, QString line="", QString extra="");
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
extern MainWindow* w;
#endif // MAINWINDOW_H
