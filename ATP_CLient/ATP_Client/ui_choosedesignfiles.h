/********************************************************************************
** Form generated from reading UI file 'choosedesignfiles.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHOOSEDESIGNFILES_H
#define UI_CHOOSEDESIGNFILES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ChooseDesignFiles_DIALOG
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QProgressBar *progressBar;
    QListWidget *logListWidget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QPushButton *finishPushButton;

    void setupUi(QDialog *ChooseDesignFiles_DIALOG)
    {
        if (ChooseDesignFiles_DIALOG->objectName().isEmpty())
            ChooseDesignFiles_DIALOG->setObjectName(QString::fromUtf8("ChooseDesignFiles_DIALOG"));
        ChooseDesignFiles_DIALOG->resize(361, 306);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ChooseDesignFiles_DIALOG->sizePolicy().hasHeightForWidth());
        ChooseDesignFiles_DIALOG->setSizePolicy(sizePolicy);
        ChooseDesignFiles_DIALOG->setModal(true);
        verticalLayout_2 = new QVBoxLayout(ChooseDesignFiles_DIALOG);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(horizontalSpacer_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(ChooseDesignFiles_DIALOG);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        progressBar = new QProgressBar(ChooseDesignFiles_DIALOG);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);

        horizontalLayout->addWidget(progressBar);


        verticalLayout->addLayout(horizontalLayout);

        logListWidget = new QListWidget(ChooseDesignFiles_DIALOG);
        logListWidget->setObjectName(QString::fromUtf8("logListWidget"));

        verticalLayout->addWidget(logListWidget);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(68, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton = new QPushButton(ChooseDesignFiles_DIALOG);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout_2->addWidget(pushButton);

        finishPushButton = new QPushButton(ChooseDesignFiles_DIALOG);
        finishPushButton->setObjectName(QString::fromUtf8("finishPushButton"));
        finishPushButton->setCheckable(false);

        horizontalLayout_2->addWidget(finishPushButton);

        horizontalLayout_2->setStretch(0, 3);
        horizontalLayout_2->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(ChooseDesignFiles_DIALOG);

        QMetaObject::connectSlotsByName(ChooseDesignFiles_DIALOG);
    } // setupUi

    void retranslateUi(QDialog *ChooseDesignFiles_DIALOG)
    {
        ChooseDesignFiles_DIALOG->setWindowTitle(QApplication::translate("ChooseDesignFiles_DIALOG", "\345\257\274\345\205\245\345\210\227\346\216\247\345\267\245\347\250\213\346\225\260\346\215\256", nullptr));
        label->setText(QApplication::translate("ChooseDesignFiles_DIALOG", "\350\257\273\345\217\226\350\277\233\345\272\246", nullptr));
        pushButton->setText(QApplication::translate("ChooseDesignFiles_DIALOG", "\345\274\200\345\247\213\345\257\274\345\205\245", nullptr));
        finishPushButton->setText(QApplication::translate("ChooseDesignFiles_DIALOG", "\345\256\214\346\210\220", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChooseDesignFiles_DIALOG: public Ui_ChooseDesignFiles_DIALOG {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHOOSEDESIGNFILES_H
