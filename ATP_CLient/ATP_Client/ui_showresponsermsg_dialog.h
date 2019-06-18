/********************************************************************************
** Form generated from reading UI file 'showresponsermsg_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWRESPONSERMSG_DIALOG_H
#define UI_SHOWRESPONSERMSG_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>

QT_BEGIN_NAMESPACE

class Ui_ShowResponserMsg_DIALOG
{
public:
    QHBoxLayout *horizontalLayout;
    QTreeWidget *ResponserMSGResult_TREE;

    void setupUi(QDialog *ShowResponserMsg_DIALOG)
    {
        if (ShowResponserMsg_DIALOG->objectName().isEmpty())
            ShowResponserMsg_DIALOG->setObjectName(QString::fromUtf8("ShowResponserMsg_DIALOG"));
        ShowResponserMsg_DIALOG->resize(480, 640);
        horizontalLayout = new QHBoxLayout(ShowResponserMsg_DIALOG);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        ResponserMSGResult_TREE = new QTreeWidget(ShowResponserMsg_DIALOG);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        ResponserMSGResult_TREE->setHeaderItem(__qtreewidgetitem);
        ResponserMSGResult_TREE->setObjectName(QString::fromUtf8("ResponserMSGResult_TREE"));

        horizontalLayout->addWidget(ResponserMSGResult_TREE);


        retranslateUi(ShowResponserMsg_DIALOG);

        QMetaObject::connectSlotsByName(ShowResponserMsg_DIALOG);
    } // setupUi

    void retranslateUi(QDialog *ShowResponserMsg_DIALOG)
    {
        ShowResponserMsg_DIALOG->setWindowTitle(QApplication::translate("ShowResponserMsg_DIALOG", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ShowResponserMsg_DIALOG: public Ui_ShowResponserMsg_DIALOG {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWRESPONSERMSG_DIALOG_H
