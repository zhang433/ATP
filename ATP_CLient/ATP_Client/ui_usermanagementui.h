/********************************************************************************
** Form generated from reading UI file 'usermanagementui.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERMANAGEMENTUI_H
#define UI_USERMANAGEMENTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UserManagementUI
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QListWidget *listWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *AddNewUser;
    QPushButton *DeleteUser;
    QPushButton *ResetUser;

    void setupUi(QDialog *UserManagementUI)
    {
        if (UserManagementUI->objectName().isEmpty())
            UserManagementUI->setObjectName(QString::fromUtf8("UserManagementUI"));
        UserManagementUI->resize(480, 640);
        horizontalLayout_2 = new QHBoxLayout(UserManagementUI);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(UserManagementUI);
        label->setObjectName(QString::fromUtf8("label"));
        label->setLineWidth(1);

        verticalLayout->addWidget(label);

        listWidget = new QListWidget(UserManagementUI);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        verticalLayout->addWidget(listWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        AddNewUser = new QPushButton(UserManagementUI);
        AddNewUser->setObjectName(QString::fromUtf8("AddNewUser"));

        horizontalLayout->addWidget(AddNewUser);

        DeleteUser = new QPushButton(UserManagementUI);
        DeleteUser->setObjectName(QString::fromUtf8("DeleteUser"));

        horizontalLayout->addWidget(DeleteUser);

        ResetUser = new QPushButton(UserManagementUI);
        ResetUser->setObjectName(QString::fromUtf8("ResetUser"));

        horizontalLayout->addWidget(ResetUser);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);


        retranslateUi(UserManagementUI);

        QMetaObject::connectSlotsByName(UserManagementUI);
    } // setupUi

    void retranslateUi(QDialog *UserManagementUI)
    {
        UserManagementUI->setWindowTitle(QApplication::translate("UserManagementUI", "\347\224\250\346\210\267\347\256\241\347\220\206", nullptr));
        label->setText(QApplication::translate("UserManagementUI", "\347\224\250\346\210\267\345\210\227\350\241\250:", nullptr));
        AddNewUser->setText(QApplication::translate("UserManagementUI", "\346\226\260\345\242\236\347\224\250\346\210\267", nullptr));
        DeleteUser->setText(QApplication::translate("UserManagementUI", "\345\210\240\351\231\244\347\224\250\346\210\267", nullptr));
        ResetUser->setText(QApplication::translate("UserManagementUI", "\345\257\206\347\240\201\351\207\215\347\275\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UserManagementUI: public Ui_UserManagementUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERMANAGEMENTUI_H
