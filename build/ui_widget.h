/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QLineEdit *lineEditPort;
    QPushButton *pushButtonListen;
    QPushButton *pushButtonStopListen;
    QPushButton *pushButtonOpenCam;
    QPushButton *pushButtonCloseCam;
    QLabel *labelPic;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(1006, 823);
        verticalLayout_2 = new QVBoxLayout(Widget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        groupBox = new QGroupBox(Widget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setEnabled(true);
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(80, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        lineEditPort = new QLineEdit(groupBox);
        lineEditPort->setObjectName(QStringLiteral("lineEditPort"));
        lineEditPort->setMaximumSize(QSize(60, 60));

        horizontalLayout->addWidget(lineEditPort);

        pushButtonListen = new QPushButton(groupBox);
        pushButtonListen->setObjectName(QStringLiteral("pushButtonListen"));

        horizontalLayout->addWidget(pushButtonListen);

        pushButtonStopListen = new QPushButton(groupBox);
        pushButtonStopListen->setObjectName(QStringLiteral("pushButtonStopListen"));

        horizontalLayout->addWidget(pushButtonStopListen);

        pushButtonOpenCam = new QPushButton(groupBox);
        pushButtonOpenCam->setObjectName(QStringLiteral("pushButtonOpenCam"));

        horizontalLayout->addWidget(pushButtonOpenCam);

        pushButtonCloseCam = new QPushButton(groupBox);
        pushButtonCloseCam->setObjectName(QStringLiteral("pushButtonCloseCam"));

        horizontalLayout->addWidget(pushButtonCloseCam);


        verticalLayout_2->addWidget(groupBox);

        labelPic = new QLabel(Widget);
        labelPic->setObjectName(QStringLiteral("labelPic"));
        labelPic->setMinimumSize(QSize(1280, 720));
        labelPic->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(labelPic);

        verticalLayout_2->setStretch(1, 9);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "USB3.0 UVC Camera", Q_NULLPTR));
        groupBox->setTitle(QString());
        label->setText(QApplication::translate("Widget", "\346\234\215\345\212\241\347\253\257\345\217\243\345\217\267\357\274\232", Q_NULLPTR));
        pushButtonListen->setText(QApplication::translate("Widget", "\345\274\200\345\247\213\347\233\221\345\220\254", Q_NULLPTR));
        pushButtonStopListen->setText(QApplication::translate("Widget", "\345\201\234\346\255\242\347\233\221\345\220\254", Q_NULLPTR));
        pushButtonOpenCam->setText(QApplication::translate("Widget", "\346\211\223\345\274\200\346\221\204\345\203\217\345\244\264", Q_NULLPTR));
        pushButtonCloseCam->setText(QApplication::translate("Widget", "\345\205\263\351\227\255\346\221\204\345\203\217\345\244\264", Q_NULLPTR));
        labelPic->setText(QApplication::translate("Widget", "PIC ", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
