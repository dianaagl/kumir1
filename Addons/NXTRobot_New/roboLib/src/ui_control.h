/********************************************************************************
** Form generated from reading UI file 'control.ui'
**
** Created
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROL_H
#define UI_CONTROL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlForm
{
public:
    QHBoxLayout *horizontalLayout_4;
    QWidget *widget;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QPushButton *scanButton;
    QLabel *statusl;
    QComboBox *addrLine;
    QGroupBox *motorAtuningBox;
    QHBoxLayout *horizontalLayout_3;
    QSlider *calibrateMotorA;
    QSpinBox *spinBoxA;
    QGroupBox *motorBtuningBox;
    QHBoxLayout *horizontalLayout_2;
    QSlider *calibrateMotorB;
    QSpinBox *spinBoxB;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *disconnectButton;
    QPushButton *connectButton;
    QWidget *imageBox;
    QVBoxLayout *verticalLayout;

    void setupUi(QWidget *ControlForm)
    {
        if (ControlForm->objectName().isEmpty())
            ControlForm->setObjectName(QString::fromUtf8("ControlForm"));
        ControlForm->resize(400, 320);
        horizontalLayout_4 = new QHBoxLayout(ControlForm);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        widget = new QWidget(ControlForm);
        widget->setObjectName(QString::fromUtf8("widget"));
        verticalLayout_3 = new QVBoxLayout(widget);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupBox = new QGroupBox(widget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        scanButton = new QPushButton(groupBox);
        scanButton->setObjectName(QString::fromUtf8("scanButton"));

        gridLayout->addWidget(scanButton, 0, 1, 1, 1);

        statusl = new QLabel(groupBox);
        statusl->setObjectName(QString::fromUtf8("statusl"));

        gridLayout->addWidget(statusl, 1, 0, 1, 2);

        addrLine = new QComboBox(groupBox);
        addrLine->setObjectName(QString::fromUtf8("addrLine"));
        addrLine->setMinimumSize(QSize(230, 0));
        addrLine->setEditable(true);

        gridLayout->addWidget(addrLine, 0, 0, 1, 1);


        verticalLayout_3->addWidget(groupBox);

        motorAtuningBox = new QGroupBox(widget);
        motorAtuningBox->setObjectName(QString::fromUtf8("motorAtuningBox"));
        motorAtuningBox->setEnabled(true);
        horizontalLayout_3 = new QHBoxLayout(motorAtuningBox);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        calibrateMotorA = new QSlider(motorAtuningBox);
        calibrateMotorA->setObjectName(QString::fromUtf8("calibrateMotorA"));
        calibrateMotorA->setMinimum(50);
        calibrateMotorA->setMaximum(150);
        calibrateMotorA->setValue(100);
        calibrateMotorA->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(calibrateMotorA);

        spinBoxA = new QSpinBox(motorAtuningBox);
        spinBoxA->setObjectName(QString::fromUtf8("spinBoxA"));
        spinBoxA->setMinimum(50);
        spinBoxA->setMaximum(150);
        spinBoxA->setValue(100);

        horizontalLayout_3->addWidget(spinBoxA);


        verticalLayout_3->addWidget(motorAtuningBox);

        motorBtuningBox = new QGroupBox(widget);
        motorBtuningBox->setObjectName(QString::fromUtf8("motorBtuningBox"));
        motorBtuningBox->setEnabled(true);
        horizontalLayout_2 = new QHBoxLayout(motorBtuningBox);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        calibrateMotorB = new QSlider(motorBtuningBox);
        calibrateMotorB->setObjectName(QString::fromUtf8("calibrateMotorB"));
        calibrateMotorB->setMinimum(50);
        calibrateMotorB->setMaximum(150);
        calibrateMotorB->setValue(100);
        calibrateMotorB->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(calibrateMotorB);

        spinBoxB = new QSpinBox(motorBtuningBox);
        spinBoxB->setObjectName(QString::fromUtf8("spinBoxB"));
        spinBoxB->setMinimum(50);
        spinBoxB->setMaximum(150);
        spinBoxB->setValue(100);

        horizontalLayout_2->addWidget(spinBoxB);


        verticalLayout_3->addWidget(motorBtuningBox);

        widget_2 = new QWidget(widget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        disconnectButton = new QPushButton(widget_2);
        disconnectButton->setObjectName(QString::fromUtf8("disconnectButton"));
        disconnectButton->setEnabled(false);

        horizontalLayout->addWidget(disconnectButton);

        connectButton = new QPushButton(widget_2);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));

        horizontalLayout->addWidget(connectButton);


        verticalLayout_3->addWidget(widget_2);


        horizontalLayout_4->addWidget(widget);

        imageBox = new QWidget(ControlForm);
        imageBox->setObjectName(QString::fromUtf8("imageBox"));
        verticalLayout = new QVBoxLayout(imageBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

        horizontalLayout_4->addWidget(imageBox);


        retranslateUi(ControlForm);
        QObject::connect(calibrateMotorA, SIGNAL(sliderMoved(int)), spinBoxA, SLOT(setValue(int)));
        QObject::connect(spinBoxA, SIGNAL(valueChanged(int)), calibrateMotorA, SLOT(setValue(int)));
        QObject::connect(calibrateMotorB, SIGNAL(sliderMoved(int)), spinBoxB, SLOT(setValue(int)));
        QObject::connect(spinBoxB, SIGNAL(valueChanged(int)), calibrateMotorB, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(ControlForm);
    } // setupUi

    void retranslateUi(QWidget *ControlForm)
    {
        ControlForm->setWindowTitle(QApplication::translate("ControlForm", "Lego NXT", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ControlForm", "Bluetooth address", 0, QApplication::UnicodeUTF8));
        scanButton->setText(QApplication::translate("ControlForm", "\320\237\320\276\320\270\321\201\320\272", 0, QApplication::UnicodeUTF8));
        statusl->setText(QApplication::translate("ControlForm", "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 \320\260\320\264\321\200\320\265\321\201 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260 \320\270\320\273\320\270 \320\275\320\260\320\266\320\274\320\270\321\202\320\265 \320\272\320\275\320\276\320\277\320\272\321\203 \"\320\237\320\276\320\270\321\201\320\272\"", 0, QApplication::UnicodeUTF8));
        motorAtuningBox->setTitle(QApplication::translate("ControlForm", "Motor A tuning", 0, QApplication::UnicodeUTF8));
        motorBtuningBox->setTitle(QApplication::translate("ControlForm", "Motor B tuning", 0, QApplication::UnicodeUTF8));
        disconnectButton->setText(QApplication::translate("ControlForm", "\320\236\321\202\320\272\320\273\321\216\321\207\320\270\321\202\321\214\321\201\321\217", 0, QApplication::UnicodeUTF8));
        connectButton->setText(QApplication::translate("ControlForm", "\320\237\320\276\320\264\320\272\320\273\321\216\321\207\320\270\321\202\321\214\321\201\321\217", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ControlForm: public Ui_ControlForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROL_H
