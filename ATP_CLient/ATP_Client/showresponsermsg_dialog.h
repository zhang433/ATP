#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#ifndef SHOWRESPONSERMSG_DIALOG_H
#define SHOWRESPONSERMSG_DIALOG_H

#include <QDialog>
#include "qtreewidget.h"
#include "self_define.h"

namespace Ui {
class ShowResponserMsg_DIALOG;
}

class ShowResponserMsg_DIALOG : public QDialog
{
    Q_OBJECT


public:
    explicit ShowResponserMsg_DIALOG(BaliseBitMessage& input_data,QWidget *parent = 0);
    ~ShowResponserMsg_DIALOG();

private:
    Ui::ShowResponserMsg_DIALOG *ui;
};

#endif // SHOWRESPONSERMSG_DIALOG_H
