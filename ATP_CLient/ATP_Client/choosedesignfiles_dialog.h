#pragma once
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include "basestructer.h"
#include "designdata.h"
#include <QDialog>
#include <QStringList>
#include <QCloseEvent>
#include "basestructer.h"
#include "readdesignfile_thread.h"

namespace Ui {
class ChooseDesignFiles_DIALOG ;
}

class ChooseDesignFiles_DIALOG  : public QDialog
{
    Q_OBJECT
public:
    static bool importAgain;
private:
    QStringList fileNameList;
    DesignData *designData;
	ReadFileThread* readFileThread;
    int jinluNum;
    int chezhanNum;
    int poduNum;
    int guofenxiangNum;
    int shujuNum;
    int suduNum;
    int yingdaqiNum;
    int duanlianNum;
    int zuobiaoxiNum;
    int lichengNum;
    int zhantaiceNum;


public:
    explicit ChooseDesignFiles_DIALOG (QWidget *parent = 0);
    ~ChooseDesignFiles_DIALOG ();

private slots:
    void on_pushButton_clicked();
    void handleProgressBar(int);
    void handleResult(QString);
    void deleteThread();
    void closeEvent(QCloseEvent *event);
    void addFileNum(FileType);

    void on_finishPushButton_clicked();

private:
    Ui::ChooseDesignFiles_DIALOG  *ui;
};
