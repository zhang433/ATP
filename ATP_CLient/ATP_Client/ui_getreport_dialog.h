/********************************************************************************
** Form generated from reading UI file 'getreport_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GETREPORT_DIALOG_H
#define UI_GETREPORT_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GetReport_DIALOG
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QComboBox *comboBox;
    QTreeWidget *treeWidget_FIleList;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_GetFileList;
    QPushButton *pushButton_Download;
    QPushButton *pushButton_Delete;
    QPushButton *pushButton_GetSelectedFile;
    QVBoxLayout *verticalLayout;
    QLabel *label_GenerateLog;
    QProgressBar *progressBar;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *GetReport_DIALOG)
    {
        if (GetReport_DIALOG->objectName().isEmpty())
            GetReport_DIALOG->setObjectName(QString::fromUtf8("GetReport_DIALOG"));
        GetReport_DIALOG->resize(480, 640);
        centralwidget = new QWidget(GetReport_DIALOG);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_3 = new QVBoxLayout(centralwidget);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        comboBox = new QComboBox(centralwidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        verticalLayout_2->addWidget(comboBox);

        treeWidget_FIleList = new QTreeWidget(centralwidget);
        treeWidget_FIleList->setObjectName(QString::fromUtf8("treeWidget_FIleList"));

        verticalLayout_2->addWidget(treeWidget_FIleList);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_GetFileList = new QPushButton(centralwidget);
        pushButton_GetFileList->setObjectName(QString::fromUtf8("pushButton_GetFileList"));

        horizontalLayout->addWidget(pushButton_GetFileList);

        pushButton_Download = new QPushButton(centralwidget);
        pushButton_Download->setObjectName(QString::fromUtf8("pushButton_Download"));

        horizontalLayout->addWidget(pushButton_Download);

        pushButton_Delete = new QPushButton(centralwidget);
        pushButton_Delete->setObjectName(QString::fromUtf8("pushButton_Delete"));

        horizontalLayout->addWidget(pushButton_Delete);

        pushButton_GetSelectedFile = new QPushButton(centralwidget);
        pushButton_GetSelectedFile->setObjectName(QString::fromUtf8("pushButton_GetSelectedFile"));
        pushButton_GetSelectedFile->setCheckable(true);

        horizontalLayout->addWidget(pushButton_GetSelectedFile);


        verticalLayout_2->addLayout(horizontalLayout);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_GenerateLog = new QLabel(centralwidget);
        label_GenerateLog->setObjectName(QString::fromUtf8("label_GenerateLog"));

        verticalLayout->addWidget(label_GenerateLog);

        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);

        verticalLayout->addWidget(progressBar);


        verticalLayout_2->addLayout(verticalLayout);


        verticalLayout_3->addLayout(verticalLayout_2);

        GetReport_DIALOG->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GetReport_DIALOG);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 480, 23));
        GetReport_DIALOG->setMenuBar(menubar);
        statusbar = new QStatusBar(GetReport_DIALOG);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        GetReport_DIALOG->setStatusBar(statusbar);

        retranslateUi(GetReport_DIALOG);

        QMetaObject::connectSlotsByName(GetReport_DIALOG);
    } // setupUi

    void retranslateUi(QMainWindow *GetReport_DIALOG)
    {
        GetReport_DIALOG->setWindowTitle(QApplication::translate("GetReport_DIALOG", "MainWindow", nullptr));
        comboBox->setItemText(0, QApplication::translate("GetReport_DIALOG", "\345\205\250\351\203\250\346\226\207\344\273\266", nullptr));
        comboBox->setItemText(1, QApplication::translate("GetReport_DIALOG", "\345\216\237\345\247\213\346\226\207\344\273\266", nullptr));
        comboBox->setItemText(2, QApplication::translate("GetReport_DIALOG", "\346\212\245\350\241\250\346\226\207\344\273\266", nullptr));
        comboBox->setItemText(3, QApplication::translate("GetReport_DIALOG", "\346\225\260\346\215\256\346\226\207\344\273\266", nullptr));

        QTreeWidgetItem *___qtreewidgetitem = treeWidget_FIleList->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("GetReport_DIALOG", "\346\226\207\344\273\266\347\233\256\345\275\225", nullptr));
        pushButton_GetFileList->setText(QApplication::translate("GetReport_DIALOG", "\346\233\264\346\226\260\346\226\207\344\273\266\345\210\227\350\241\250", nullptr));
        pushButton_Download->setText(QApplication::translate("GetReport_DIALOG", "\344\270\213\350\275\275", nullptr));
        pushButton_Delete->setText(QApplication::translate("GetReport_DIALOG", "\345\210\240\351\231\244", nullptr));
        pushButton_GetSelectedFile->setText(QApplication::translate("GetReport_DIALOG", "\346\225\260\346\215\256\347\224\237\346\210\220", nullptr));
        label_GenerateLog->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GetReport_DIALOG: public Ui_GetReport_DIALOG {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GETREPORT_DIALOG_H
