#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#ifndef SELF_DEFINE_H
#define SELF_DEFINE_H
#include <vector>
#include <QEvent>
#include <QString>
#include <qmutex.h>
#include <fstream>
#include <QThread>
#include <QMessageBox>
#include <QVector>
#include <QMetaType>

#define  UdpDataReady_Event QEvent::Type(1001)
#define  UdpIntReady_Event QEvent::Type(1002)
#define  Balise_Excel_Position_ERROR QEvent::Type(1003)
#define  Balise_Atp_Position_ERROR QEvent::Type(1004)


#define Frame_Size 1413

//当所有值都是1的时候表示这个值是无效的数值
extern const char mask_novalue[8];
#define MEANINGLESS(x) (memcmp(&x,mask_novalue,sizeof(x))==0)//用于判断RealTimeDatastructure数据中的某一项是否为无效数据

enum BaliseType
{
	Normal = 0,
	Active = 1,
	SideLine = 2
};

typedef struct
{
    QString remark;//该项的备注
    unsigned char length;//该项的存储数据的位长
    QString name;//该项的名称
    unsigned char mark;//表记值，用于区分对于不同项的操作
}ItemMask;

typedef struct
{
    QString remark;//该项的备注
    QString name;//该项的名称
    qint64 value=-1;//该项的值
}Item;

typedef struct
{
    QString remark;//该项的备注
    unsigned char length;//该项的存储数据的位长
    QString name;//该项的名称
    QString valueinfo;//值对应的信息
    qint64 value;
    quint8 mark;//该值高四位表明本项是被前几项所决定的，1代表前一项，低四位表示该项的值是何值时这一项是存在的
}ItemMaskRBC;

typedef struct
{
    QString remark;//该项的说明
    QString name;//该项的名称
    QString valueinfo;//该项的值对应的结果
    qint64 value=-1;//该项的值
}ItemRBC;

typedef struct
{
    unsigned char Balise_Tel[104]={0x90,0x02,0x7F,0xCC,0x22,0x13,0xC1,0x50,0x36,0x25,0xAB,0xC2,0x15,0xE1,0x42,0x3C,0x64,0x21,0x6E,0x14,0x55,0x06,0xF2,0x00,0x00,0x00,0x70,0x81,0x10,0x30,0x7D,0x10,0x90,0x7D,0x00,0xE0,0xC8,0x00,0x00,0xA2,0x90,0x91,0x6A,0x80,0x40,0x69,0x0F,0xF1,0xB4,0x0E,0x48,0x00,0x05,0xC0,0x08,0x15,0x5A,0xD0,0x11,0x8E,0xFF,0x00,0xB1,0x03,0x58,0x09,0x02,0xA2,0x4F,0x82,0x1A,0x81,0x38,0x8B,0x20,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC};
    qint32 Locspace=-2;//应答器的接收位置
    qint32 Loctime=-2;//应答器的接收时间
	BaliseType type = BaliseType::Normal;//是否是侧线应答器
}BaliseBitMessage;

typedef struct
{
    QByteArray data;
    int packageorder = -1;//表明本项是第几包的值
}RBCMessage;

typedef struct
{
    QColor color;
    int packageorder = -1;//表明本项是第几包的值
}FreMessage;

typedef QVector<QVector<Item>> BalisePackCollector;
typedef QVector<Item> BalisePackage;
typedef QVector<ItemMaskRBC> PackageMaskRBC;
typedef QVector<QVector<ItemRBC>> PackCollectorRBC;

#endif 
//Q_DECLARE_METATYPE(BaliseBitMessage);
// DATA_H
