#ifndef ANALYZE_RBCMESSAGE_H
#define ANALYZE_RBCMESSAGE_H
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include <QMap>
#include <self_define.h>


class Analyze_RBCMessage
{
public:
    Analyze_RBCMessage();
    static void Init();
    static PackCollectorRBC& Resolve(QByteArray &input_data);//传入一个RBC报文，进行解析
	static bool GetMsgID(QByteArray &input_data, QVector<ItemRBC>* ret);//传入一个RBC报文，获取其消息头
private:
    static int offset_now;
    static char* msgptr;
    static quint16 RBC_length;
    static PackCollectorRBC Analyze_Result;//解析后的结果
    static PackCollectorRBC Error_Result;//解析错误时的结果
    static QMap<quint8,PackageMaskRBC> RBC_MessageMask;
    static QMap<quint8,PackageMaskRBC> RBC_InfoPackageMask;
    static qint64 FetchValue(unsigned int length);
};

#endif // ANALYZE_RBCMESSAGE_H
