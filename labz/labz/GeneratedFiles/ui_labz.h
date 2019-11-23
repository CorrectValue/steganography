/********************************************************************************
** Form generated from reading UI file 'labz.ui'
**
** Created: Mon 19. Mar 22:22:38 2018
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LABZ_H
#define UI_LABZ_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_labzClass
{
public:
    QWidget *centralWidget;
    QPushButton *btnGo;
    QComboBox *cmbAttack;
    QPushButton *btnAttack;
    QComboBox *cmbMethod;
    QGroupBox *groupBox;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_5;
    QLabel *label_4;
    QLabel *label_3;
    QLabel *label_6;
    QDoubleSpinBox *dspnBER;
    QDoubleSpinBox *dspnPSNR;
    QDoubleSpinBox *dspnSSIM;
    QDoubleSpinBox *dspnRMS;
    QLabel *label;
    QDoubleSpinBox *dspnNCC;
    QDoubleSpinBox *dspnBrightness;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *labzClass)
    {
        if (labzClass->objectName().isEmpty())
            labzClass->setObjectName(QString::fromUtf8("labzClass"));
        labzClass->resize(600, 400);
        centralWidget = new QWidget(labzClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        btnGo = new QPushButton(centralWidget);
        btnGo->setObjectName(QString::fromUtf8("btnGo"));
        btnGo->setGeometry(QRect(430, 20, 75, 23));
        cmbAttack = new QComboBox(centralWidget);
        cmbAttack->setObjectName(QString::fromUtf8("cmbAttack"));
        cmbAttack->setGeometry(QRect(240, 80, 81, 22));
        btnAttack = new QPushButton(centralWidget);
        btnAttack->setObjectName(QString::fromUtf8("btnAttack"));
        btnAttack->setGeometry(QRect(430, 80, 75, 23));
        cmbMethod = new QComboBox(centralWidget);
        cmbMethod->setObjectName(QString::fromUtf8("cmbMethod"));
        cmbMethod->setGeometry(QRect(140, 80, 69, 22));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 160, 421, 161));
        gridLayoutWidget = new QWidget(groupBox);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(210, 30, 190, 126));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 3, 0, 1, 1);

        dspnBER = new QDoubleSpinBox(gridLayoutWidget);
        dspnBER->setObjectName(QString::fromUtf8("dspnBER"));
        dspnBER->setDecimals(6);
        dspnBER->setMaximum(1000);

        gridLayout->addWidget(dspnBER, 0, 1, 1, 1);

        dspnPSNR = new QDoubleSpinBox(gridLayoutWidget);
        dspnPSNR->setObjectName(QString::fromUtf8("dspnPSNR"));
        dspnPSNR->setDecimals(6);
        dspnPSNR->setMaximum(1000);

        gridLayout->addWidget(dspnPSNR, 1, 1, 1, 1);

        dspnSSIM = new QDoubleSpinBox(gridLayoutWidget);
        dspnSSIM->setObjectName(QString::fromUtf8("dspnSSIM"));
        dspnSSIM->setDecimals(6);
        dspnSSIM->setMaximum(1000);

        gridLayout->addWidget(dspnSSIM, 2, 1, 1, 1);

        dspnRMS = new QDoubleSpinBox(gridLayoutWidget);
        dspnRMS->setObjectName(QString::fromUtf8("dspnRMS"));
        dspnRMS->setDecimals(6);
        dspnRMS->setMaximum(1000);

        gridLayout->addWidget(dspnRMS, 3, 1, 1, 1);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 4, 0, 1, 1);

        dspnNCC = new QDoubleSpinBox(gridLayoutWidget);
        dspnNCC->setObjectName(QString::fromUtf8("dspnNCC"));
        dspnNCC->setDecimals(6);

        gridLayout->addWidget(dspnNCC, 4, 1, 1, 1);

        dspnBrightness = new QDoubleSpinBox(centralWidget);
        dspnBrightness->setObjectName(QString::fromUtf8("dspnBrightness"));
        dspnBrightness->setGeometry(QRect(240, 110, 62, 22));
        dspnBrightness->setValue(1);
        labzClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(labzClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        labzClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(labzClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        labzClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(labzClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        labzClass->setStatusBar(statusBar);

        retranslateUi(labzClass);

        QMetaObject::connectSlotsByName(labzClass);
    } // setupUi

    void retranslateUi(QMainWindow *labzClass)
    {
        labzClass->setWindowTitle(QApplication::translate("labzClass", "labz", 0, QApplication::UnicodeUTF8));
        btnGo->setText(QApplication::translate("labzClass", "Go!", 0, QApplication::UnicodeUTF8));
        cmbAttack->clear();
        cmbAttack->insertItems(0, QStringList()
         << QApplication::translate("labzClass", "Brightness", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "Crop", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "GaussianBlur", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "MedianBlur", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "Noise", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "EqualizeHist", 0, QApplication::UnicodeUTF8)
        );
        btnAttack->setText(QApplication::translate("labzClass", "Attack", 0, QApplication::UnicodeUTF8));
        cmbMethod->clear();
        cmbMethod->insertItems(0, QStringList()
         << QApplication::translate("labzClass", "LSB", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "\320\237\321\201\320\265\320\262\320\264\320\276\321\201\320\273\321\203\321\207\320\260\320\271\320\275\321\213\320\271 \320\270\320\275\321\202\320\265\321\200\320\262\320\260\320\273", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "Pastorfide", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "Hempstalk", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "ICPES", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("labzClass", "IEEE", 0, QApplication::UnicodeUTF8)
        );
        groupBox->setTitle(QApplication::translate("labzClass", "\320\234\320\265\321\202\321\200\320\270\320\272\320\270", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("labzClass", "SSIM", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("labzClass", "PSNR", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("labzClass", "BER", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("labzClass", "RMS", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("labzClass", "NCC(\320\272\320\276\321\215\321\204\321\204. \320\272\320\276\321\200\321\200.)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class labzClass: public Ui_labzClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LABZ_H
