/********************************************************************************
** Form generated from reading UI file 'rbcshow_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RBCSHOW_DIALOG_H
#define UI_RBCSHOW_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RBCShow_DIALOG
{
public:
    QHBoxLayout *horizontalLayout;
    QTableWidget *tableWidget;

    void setupUi(QWidget *RBCShow_DIALOG)
    {
        if (RBCShow_DIALOG->objectName().isEmpty())
            RBCShow_DIALOG->setObjectName(QString::fromUtf8("RBCShow_DIALOG"));
        RBCShow_DIALOG->resize(640, 480);
        horizontalLayout = new QHBoxLayout(RBCShow_DIALOG);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        tableWidget = new QTableWidget(RBCShow_DIALOG);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        horizontalLayout->addWidget(tableWidget);


        retranslateUi(RBCShow_DIALOG);

        QMetaObject::connectSlotsByName(RBCShow_DIALOG);
    } // setupUi

    void retranslateUi(QWidget *RBCShow_DIALOG)
    {
        RBCShow_DIALOG->setWindowTitle(QApplication::translate("RBCShow_DIALOG", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RBCShow_DIALOG: public Ui_RBCShow_DIALOG {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RBCSHOW_DIALOG_H
