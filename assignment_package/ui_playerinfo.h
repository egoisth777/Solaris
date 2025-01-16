/********************************************************************************
** Form generated from reading UI file 'playerinfo.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYERINFO_H
#define UI_PLAYERINFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlayerInfo
{
public:
    QFrame *line;
    QLabel *label;
    QLabel *label_2;
    QLabel *posLabel;
    QLabel *label_4;
    QLabel *velLabel;
    QLabel *label_6;
    QLabel *accLabel;
    QLabel *lookLabel;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *chunkLabel;
    QLabel *label_11;
    QLabel *zoneLabel;
    QLabel *label_12;
    QLabel *fpsLabel;
    QLabel *aveFpsLabel;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *zoneLabel_2;
    QLabel *zoneLabel_3;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *zoneLabel_4;
    QLabel *zoneLabel_5;
    QLabel *label_17;
    QLabel *zoneLabel_6;
    QLabel *label_18;
    QLabel *zoneLabel_7;
    QLabel *label_19;
    QLabel *label_20;
    QLabel *zoneLabel_8;
    QLabel *label_21;
    QLabel *zoneLabel_9;
    QLabel *label_22;
    QLabel *zoneLabel_10;

    void setupUi(QWidget *PlayerInfo)
    {
        if (PlayerInfo->objectName().isEmpty())
            PlayerInfo->setObjectName("PlayerInfo");
        PlayerInfo->resize(403, 640);
        line = new QFrame(PlayerInfo);
        line->setObjectName("line");
        line->setGeometry(QRect(10, 20, 381, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label = new QLabel(PlayerInfo);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 0, 151, 31));
        QFont font;
        font.setPointSize(12);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(PlayerInfo);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 60, 71, 31));
        QFont font1;
        font1.setPointSize(10);
        label_2->setFont(font1);
        posLabel = new QLabel(PlayerInfo);
        posLabel->setObjectName("posLabel");
        posLabel->setGeometry(QRect(120, 60, 271, 31));
        posLabel->setFont(font1);
        label_4 = new QLabel(PlayerInfo);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(20, 100, 71, 31));
        label_4->setFont(font1);
        velLabel = new QLabel(PlayerInfo);
        velLabel->setObjectName("velLabel");
        velLabel->setGeometry(QRect(120, 100, 271, 31));
        velLabel->setFont(font1);
        label_6 = new QLabel(PlayerInfo);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(20, 140, 81, 31));
        label_6->setFont(font1);
        accLabel = new QLabel(PlayerInfo);
        accLabel->setObjectName("accLabel");
        accLabel->setGeometry(QRect(120, 140, 271, 31));
        accLabel->setFont(font1);
        lookLabel = new QLabel(PlayerInfo);
        lookLabel->setObjectName("lookLabel");
        lookLabel->setGeometry(QRect(120, 180, 271, 31));
        lookLabel->setFont(font1);
        label_9 = new QLabel(PlayerInfo);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(20, 180, 81, 31));
        label_9->setFont(font1);
        label_10 = new QLabel(PlayerInfo);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(20, 220, 81, 31));
        label_10->setFont(font1);
        chunkLabel = new QLabel(PlayerInfo);
        chunkLabel->setObjectName("chunkLabel");
        chunkLabel->setGeometry(QRect(120, 220, 271, 31));
        chunkLabel->setFont(font1);
        label_11 = new QLabel(PlayerInfo);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(20, 260, 91, 31));
        label_11->setFont(font1);
        zoneLabel = new QLabel(PlayerInfo);
        zoneLabel->setObjectName("zoneLabel");
        zoneLabel->setGeometry(QRect(120, 260, 271, 31));
        zoneLabel->setFont(font1);
        label_12 = new QLabel(PlayerInfo);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(230, 0, 31, 31));
        label_12->setFont(font1);
        fpsLabel = new QLabel(PlayerInfo);
        fpsLabel->setObjectName("fpsLabel");
        fpsLabel->setGeometry(QRect(270, 0, 51, 31));
        fpsLabel->setFont(font1);
        aveFpsLabel = new QLabel(PlayerInfo);
        aveFpsLabel->setObjectName("aveFpsLabel");
        aveFpsLabel->setGeometry(QRect(330, 0, 51, 31));
        aveFpsLabel->setFont(font1);
        label_13 = new QLabel(PlayerInfo);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(150, 320, 91, 31));
        label_13->setFont(font1);
        label_13->setAlignment(Qt::AlignCenter);
        label_14 = new QLabel(PlayerInfo);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(20, 390, 111, 31));
        label_14->setFont(font1);
        label_14->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        zoneLabel_2 = new QLabel(PlayerInfo);
        zoneLabel_2->setObjectName("zoneLabel_2");
        zoneLabel_2->setGeometry(QRect(180, 390, 211, 31));
        zoneLabel_2->setFont(font1);
        zoneLabel_3 = new QLabel(PlayerInfo);
        zoneLabel_3->setObjectName("zoneLabel_3");
        zoneLabel_3->setGeometry(QRect(180, 420, 211, 31));
        zoneLabel_3->setFont(font1);
        label_15 = new QLabel(PlayerInfo);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(20, 420, 131, 31));
        label_15->setFont(font1);
        label_15->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_16 = new QLabel(PlayerInfo);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(20, 450, 141, 31));
        label_16->setFont(font1);
        label_16->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        zoneLabel_4 = new QLabel(PlayerInfo);
        zoneLabel_4->setObjectName("zoneLabel_4");
        zoneLabel_4->setGeometry(QRect(180, 450, 211, 31));
        zoneLabel_4->setFont(font1);
        zoneLabel_5 = new QLabel(PlayerInfo);
        zoneLabel_5->setObjectName("zoneLabel_5");
        zoneLabel_5->setGeometry(QRect(180, 480, 211, 31));
        zoneLabel_5->setFont(font1);
        label_17 = new QLabel(PlayerInfo);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(20, 480, 131, 31));
        label_17->setFont(font1);
        label_17->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        zoneLabel_6 = new QLabel(PlayerInfo);
        zoneLabel_6->setObjectName("zoneLabel_6");
        zoneLabel_6->setGeometry(QRect(180, 540, 221, 31));
        zoneLabel_6->setFont(font1);
        label_18 = new QLabel(PlayerInfo);
        label_18->setObjectName("label_18");
        label_18->setGeometry(QRect(20, 540, 151, 31));
        label_18->setFont(font1);
        label_18->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_18->setWordWrap(true);
        zoneLabel_7 = new QLabel(PlayerInfo);
        zoneLabel_7->setObjectName("zoneLabel_7");
        zoneLabel_7->setGeometry(QRect(180, 510, 211, 31));
        zoneLabel_7->setFont(font1);
        label_19 = new QLabel(PlayerInfo);
        label_19->setObjectName("label_19");
        label_19->setGeometry(QRect(20, 510, 141, 31));
        label_19->setFont(font1);
        label_19->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_20 = new QLabel(PlayerInfo);
        label_20->setObjectName("label_20");
        label_20->setGeometry(QRect(20, 570, 151, 31));
        label_20->setFont(font1);
        label_20->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_20->setWordWrap(true);
        zoneLabel_8 = new QLabel(PlayerInfo);
        zoneLabel_8->setObjectName("zoneLabel_8");
        zoneLabel_8->setGeometry(QRect(180, 570, 221, 31));
        zoneLabel_8->setFont(font1);
        label_21 = new QLabel(PlayerInfo);
        label_21->setObjectName("label_21");
        label_21->setGeometry(QRect(20, 600, 151, 31));
        label_21->setFont(font1);
        label_21->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_21->setWordWrap(true);
        zoneLabel_9 = new QLabel(PlayerInfo);
        zoneLabel_9->setObjectName("zoneLabel_9");
        zoneLabel_9->setGeometry(QRect(180, 600, 221, 31));
        zoneLabel_9->setFont(font1);
        label_22 = new QLabel(PlayerInfo);
        label_22->setObjectName("label_22");
        label_22->setGeometry(QRect(20, 360, 111, 31));
        label_22->setFont(font1);
        label_22->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        zoneLabel_10 = new QLabel(PlayerInfo);
        zoneLabel_10->setObjectName("zoneLabel_10");
        zoneLabel_10->setGeometry(QRect(180, 360, 211, 31));
        zoneLabel_10->setFont(font1);

        retranslateUi(PlayerInfo);

        QMetaObject::connectSlotsByName(PlayerInfo);
    } // setupUi

    void retranslateUi(QWidget *PlayerInfo)
    {
        PlayerInfo->setWindowTitle(QCoreApplication::translate("PlayerInfo", "Form", nullptr));
        label->setText(QCoreApplication::translate("PlayerInfo", "Player Information", nullptr));
        label_2->setText(QCoreApplication::translate("PlayerInfo", "Position:", nullptr));
        posLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_4->setText(QCoreApplication::translate("PlayerInfo", "Velocity:", nullptr));
        velLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_6->setText(QCoreApplication::translate("PlayerInfo", "Acceleration:", nullptr));
        accLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        lookLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_9->setText(QCoreApplication::translate("PlayerInfo", "Look vector:", nullptr));
        label_10->setText(QCoreApplication::translate("PlayerInfo", "Chunk:", nullptr));
        chunkLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_11->setText(QCoreApplication::translate("PlayerInfo", "Terrain Zone:", nullptr));
        zoneLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_12->setText(QCoreApplication::translate("PlayerInfo", "FPS", nullptr));
        fpsLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        aveFpsLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_13->setText(QCoreApplication::translate("PlayerInfo", "KEYS", nullptr));
        label_14->setText(QCoreApplication::translate("PlayerInfo", "Photo mode:", nullptr));
        zoneLabel_2->setText(QCoreApplication::translate("PlayerInfo", "C", nullptr));
        zoneLabel_3->setText(QCoreApplication::translate("PlayerInfo", "F", nullptr));
        label_15->setText(QCoreApplication::translate("PlayerInfo", "Fly mode:", nullptr));
        label_16->setText(QCoreApplication::translate("PlayerInfo", "Increase time scale", nullptr));
        zoneLabel_4->setText(QCoreApplication::translate("PlayerInfo", ">", nullptr));
        zoneLabel_5->setText(QCoreApplication::translate("PlayerInfo", "<", nullptr));
        label_17->setText(QCoreApplication::translate("PlayerInfo", "Decrease time scale", nullptr));
        zoneLabel_6->setText(QCoreApplication::translate("PlayerInfo", "1", nullptr));
        label_18->setText(QCoreApplication::translate("PlayerInfo", "Render pass breakdown", nullptr));
        zoneLabel_7->setText(QCoreApplication::translate("PlayerInfo", "/", nullptr));
        label_19->setText(QCoreApplication::translate("PlayerInfo", "Reverse time scale", nullptr));
        label_20->setText(QCoreApplication::translate("PlayerInfo", "Generate wooden house", nullptr));
        zoneLabel_8->setText(QCoreApplication::translate("PlayerInfo", "G", nullptr));
        label_21->setText(QCoreApplication::translate("PlayerInfo", "Sword in the Lake ! ! !", nullptr));
        zoneLabel_9->setText(QCoreApplication::translate("PlayerInfo", "O", nullptr));
        label_22->setText(QCoreApplication::translate("PlayerInfo", "Mouse unfocus", nullptr));
        zoneLabel_10->setText(QCoreApplication::translate("PlayerInfo", "Alt / Double click Alt to refocus", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlayerInfo: public Ui_PlayerInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYERINFO_H
