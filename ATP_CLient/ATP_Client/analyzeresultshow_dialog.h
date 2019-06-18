#ifndef RBCSHOW_DIALOG_H
#define RBCSHOW_DIALOG_H
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include <self_define.h>
#include <QWidget>
#include <analyze_resmessage.h>

namespace Ui {
class RBCShow_DIALOG;
}

class AnalyzeResultShow_DIALOG : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyzeResultShow_DIALOG(QWidget *parent = nullptr);
    ~AnalyzeResultShow_DIALOG();
    void ChangeAnalyzeResult(BaliseBitMessage* BBM);
    void ChangeAnalyzeResult(QByteArray* BBM);
private:
    Ui::RBCShow_DIALOG *ui;
};

#endif // RBCSHOW_DIALOG_H
