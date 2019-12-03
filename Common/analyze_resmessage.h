#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#pragma once
/*************************************************************************************************************************************************************************************************
 * 使用说明：假设input_data是需要解析的应答器报文原始数据
 * 一、获取应答器解析报文的方法：
 *          1，Resolve(input_data)，然后可以从本类的静态变量Analyze_RESMessage::Analyze_Result中获取应答器解析报文
 *          2，ResponserMessage RM = Resolve(input_data)，从返回值获取应答器解析报文
 * 二、获取应答器解析后的标识号的方法：
 *          1，在调用Resolve(input_data)后，在该函数内部就会解析应答器标识号并且存入本类的变量Analyze_RESMessage::msg_identifier中
 *          2，或者也可以调用GetResNumber(input_data)函数，该函数同样会把本类的Analyze_RESMessage::msg_identifier改为传入报文的应答器标识号，同时该函数也会将该标识号以QString的方式返回
 *          备注：在只需要获取应答器标识号而不需要完整解析报文的情况下调用GetResNumber(input_data)以获得更高的效率
 * 三、在一个已经过解析的应答器报文信息结构(ResPackCollector类型，也即QVector<QVector<Item>>类型)中查找是否存在指定的信息包：
 *          调用GetPackagePosition(QString,ResPackCollector)方法，该方法会在传入的ResPackCollector查找名字为name的信息包，如果该信息包存在，那么返回该信息包在ResPackCollector中的位置，如果不存在，那么返回值是-1
 *          备注：当GetPackagePosition(QString,ResPackCollector)的第二个参数没有传入时，默认是本类中的静态变量Analyze_RESMessage::Analyze_Result
 * 四、在一个给定的信息包结构(ResponserPackage，亦即QVector<Item>)中查找是否存在指定的项：
 *          调用FindItem(QString,ResponserPackage)函数，其中第一参数是需要查找的项，第二个参数是查找该项时要用到的信息包，由于在一个信息包中某个项可能出现了不止一次，因此，本函数的返回值是一个QVector<Item>。
 *          备注：返回的QVector<Item>结构中，其中[0]项存储的是要寻找的项，从[1]开始是这个项在指定信息包中的内容，如果QVector.size()==1，那么表示在指定信息包中没有这一项
 * **********************************************************************************************************************************************************************************************/
#include <QObject>
#include <QVector>
#include <QTextCodec>
#include "self_define.h"

class Analyze_BaliseMessage
{
public:
    Analyze_BaliseMessage();
    static QString msg_identifier;//应答器标识号
    static BalisePackCollector Analyze_Result;//解析后的结果

    static BalisePackCollector& Resolve(const BaliseBitMessage &input_data);//传入一个应答器报文，进行解析，同时提取应答器标识号
    static QString GetBaliseID(const BaliseBitMessage &input_data);//从一个应答器原始报文中提取应答器标识号
    static QVector<int> GetPackagePosition(const QString input,const BalisePackCollector& data_collector=Analyze_BaliseMessage::Analyze_Result);//查找给定的信息包的位置
    static BalisePackage FindItem(QString name,BalisePackage package);//在指定的信息包package中找到指定name的项，考虑到会有多项存在的情况，以QVector的形式返回
    static bool CheckBaliseInfoIllegal(const BaliseBitMessage &input_data);//检查报文是否可以正常解析
private:
    static int AnalyzeBaliseMsg();
    static int Analyze_ETCS_27();
    static int Analyze_ETCS_44();
    static qint64 FetchValue(unsigned int length);

    static QString Expend(int k,qint64 value);

    static const BaliseBitMessage*  msgptr;
    static qint16 offset_now;
};
