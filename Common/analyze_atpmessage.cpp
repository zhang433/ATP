#include "analyze_atpmessage.h"
#include <QDebug>


Analyze_ATPMessage::Analyze_ATPMessage()
{

}


void Analyze_ATPMessage::AnalyzeGetMainInfo(QVector<unsigned char>& input)
{
    this->ReceivedData=&input;


    int order=0;
    offset_now = 1600 + 24;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(32);//列车自位置
    offset_now = 1600 + 56;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(16);//列车当前速度
    offset_now = 1600 + 232;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(16);//列车当前允许速度,cm
    offset_now = 8384;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(4);//车长,0-未知，1-单组，2-双组
    offset_now = 1919;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(16);//最大车头误差,cm
    offset_now = 1903;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(16);//最小车头误差,cm
    offset_now = 9449;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(3);//当前列控等级,0-Level_0,1-Level_1,2-Level_2,3-Level_3 ,4-Level_4
    offset_now = 9452;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(4);//当前控车模式,0-NP,1-SB,2-SH,3-PS,4-CO,5-FS,6-OS,7-TR,8-PT,9-LKJ,10-TVM
    offset_now = 8528;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(32);//当前MA长度,MA位置，cm
    offset_now = 2252;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(1);//列车当前制动状态-紧急
    offset_now = 2253;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(1);//列车当前制动状态-常用
    offset_now = 9314;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(7);//年
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(4);//月
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(5);//日
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(5);//时
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(6);//分
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(6);//秒
    offset_now = 8512;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);//DMI文本显示
    offset_now = 1864;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(16);//对应DMI规范的Vint,干预速度,cm/s
    offset_now = 1816;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(16);//对应DMI规范的Vtarget,目标速度cm/s
    offset_now = 8352;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);//当前轨道电路载频,0-NO_CODE,1-L3,2-L,3-L2,4-LU,5-U2,6-LU2,7-U, 8-UU, 9-UUS, 10-U2S, 11-L5, 12-U3, 13-L4, 14-HB, 15-25.7,16-HU,17-27.9,18-H
    offset_now = 8360;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);//当前轨道电路低频,0-F_0000, 1-F_1700, 2-F_2000, 3-F_2300, 4-F_2600,5-F1_1700,6-F2_1700,7-F1_2000,8-F2_2000,9-F1_2300,10-F2_2300, 11-F1_2600,12-F2_2600,13-F_550,14-F_650, 15-F_750,16-F_850,17-F_550_750,18-F_650_850,19-F_1698p7_1701p4,20-F_1998p7_2001p4, 21-F_2298p7_2301p4, 22-F_2598p7_2601p4, 23-F_1700_2300, 24-F_2000_2600, 25-F_1698p7_2298p7, 26-F_1698p7_2301p4, 27-F_1701p4_2298p7, 28-F_1701p4_2301p4, 29-F_1998p7_2598p7, 30-F_1998p7_2601p4, 31-F_2001p4_2598p7, 32-F_2001p4_2601p4,33-F_Unknown
	offset_now = 10360;
	ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);//车次号
	ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);
	ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);
	ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);
	ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);
	ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);
	ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);
	ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(8);
	offset_now = 1600 + 72;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(32);//里程
    offset_now = 9347;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(5);//毫秒
    offset_now = 1784;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(32);//目标距离
    offset_now = 3456;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(16);//EBP速度
    offset_now = 10088;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(1);//SB1
    offset_now = 10089;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(1);//SB4
    offset_now = 9548;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(16);//司机编号第一部分
    offset_now = 9564;
    ATPItem_Public[order++].value = Analyze_ATPMessage::FetchValue(16);//司机编号第二部分


    order=0;
    offset_now = 8752;
    ATPItem_CTCS_1[order++].value = Analyze_ATPMessage::FetchValue(8);//是否进入新的区段
    ATPItem_CTCS_1[order++].value = Analyze_ATPMessage::FetchValue(8);//轨道区段个数
    for(int i=0;i<5;i++)
    {
        ATPItem_CTCS_1[order++].value = Analyze_ATPMessage::FetchValue(8);//末端信号机类型
        ATPItem_CTCS_1[order++].value = Analyze_ATPMessage::FetchValue(8);//载频/定义同CTCS_CARRIER
        ATPItem_CTCS_1[order++].value = Analyze_ATPMessage::FetchValue(32);//起点/cm
        ATPItem_CTCS_1[order++].value = Analyze_ATPMessage::FetchValue(32);//终点/cm
    }

    order=0;
    offset_now = 1976;
    ATPItem_ETCS_79[order++].value = Analyze_ATPMessage::FetchValue(32);//列车当前公里标

    order=0;
    offset_now = 5648;
    ATPItem_ETCS_27[order++].value = Analyze_ATPMessage::FetchValue(8);//变速点个数
    ATPItem_ETCS_27[order++].value = Analyze_ATPMessage::FetchValue(32);//SSP总数据的终点/cm
    for(int i=0;i<11;i++)
    {
        ATPItem_ETCS_27[order++].value = Analyze_ATPMessage::FetchValue(32);//变速点距离/cm
        ATPItem_ETCS_27[order++].value = Analyze_ATPMessage::FetchValue(16);//速度值/cm/s
    }

    order=0;
    offset_now = 6168;
    ATPItem_ETCS_21[order++].value = Analyze_ATPMessage::FetchValue(8);//变坡点个数
    ATPItem_ETCS_21[order++].value = Analyze_ATPMessage::FetchValue(32);//坡度总数据的终点/cm
    for(int i=0;i<5;i++)
    {
        ATPItem_ETCS_21[order++].value = Analyze_ATPMessage::FetchValue(23);//第一个变坡点的位置/cm
        ATPItem_ETCS_21[order++].value = Analyze_ATPMessage::FetchValue(1);//1-上坡，0-下坡或平坡
        ATPItem_ETCS_21[order++].value = Analyze_ATPMessage::FetchValue(8);//坡度值
    }

    order=0;
    offset_now = 9358;
    ATPItem_ETCS_5[order++].value = Analyze_ATPMessage::FetchValue(10);//NID_C地区编号
    offset_now = 3392;
    ATPItem_ETCS_5[order++].value = Analyze_ATPMessage::FetchValue(16);//NID_BG应答器组编号
    offset_now = 9368;
    ATPItem_ETCS_5[order++].value = Analyze_ATPMessage::FetchValue(14);//NID_BG应答器组编号
    offset_now = 3096;
    ATPItem_ETCS_5[order++].value = Analyze_ATPMessage::FetchValue(16);//下一组应答器组编号
    offset_now = 3120;
    ATPItem_ETCS_5[order++].value = Analyze_ATPMessage::FetchValue(32);//下一个应答器组的位置
    offset_now = 3112;
    ATPItem_ETCS_5[order++].value = Analyze_ATPMessage::FetchValue(8);//列车通过被链接应答器组时的运行方向


    order=0;
    offset_now = 8320;
    ATPItem_CTCS_2[order++].value = Analyze_ATPMessage::FetchValue(32);//临时限速范围的剩余长度，cm

    order=0;
    offset_now = 2840;
    ATPItem_ETCS_72[order++].value = Analyze_ATPMessage::FetchValue(8);//应显示的文本区域长度
    for(int i=0;i<30;i++)
    {
        ATPItem_ETCS_72[order++].value = Analyze_ATPMessage::FetchValue(8);//文本字节值
    }

    offset_now = 1600;
}

/*************************************************************************
*  函数名称：GetBelishMessage
*  功能说明：从指定的atp数据中提取应答器报文信息
*  参数说明：input：输入的atp数据
*          BBM：提取应答器报文后要存储的应答器报文数据结构对象
*  函数返回： -1：在消息有效的情况下，消息标识是未定义的（只定义了235与236）
*           0：本次atp数据不包含应答器报文
*           1：本次传输的是应答器报文的前半部分
*           2：本次传输的是应答器报文的后半部分
*  修改时间：2017-8-1
*  备   注：
*************************************************************************/
int Analyze_ATPMessage::GetBelishMessage(const QVector<unsigned char> &input,BaliseBitMessage& BBM)
{
    this->ReceivedData=&input;

    offset_now = 1600 + 4840;
    if(FetchValue(8))//消息有效
    {
        qint64 tempfetchvalue = FetchValue(16);
        if(tempfetchvalue==235)//本次传输为前52个字节
        {
            offset_now+=48;//跳过预留与消息周期的项
            BBM.Locspace = FetchValue(32);//应答器接收位置
            BBM.Loctime = FetchValue(32);//应答器接收时间
            for(int i=0;i<52;i++)
            {
                BBM.Balise_Tel[i] = FetchValue(8);
            }
            offset_now = 1600;
            return 1;
        }
        else if(tempfetchvalue==236)//本次传输为后52个字节
        {
            offset_now+=48;//跳过预留与消息周期的项
            for(int i=52;i<104;i++)
            {
                BBM.Balise_Tel[i] = FetchValue(8);
            }
            offset_now = 1600;
            return 2;
        }
        else//未知情况，返回错误代码
        {
            offset_now = 1600;
            return -1;//表示没有应答器报文
        }
    }
    else
    {
        offset_now = 1600;
        return 0;//表示没有应答器报文
    }
}


void Analyze_ATPMessage::AnalyzeReceivedMsg(QVector<unsigned char> &input)
{
    static int count=0;
    offset_now=1600;
    this->ReceivedData=&input;
    qDebug()<<"********************************************************************";
    for(unsigned int i=0;i<sizeof(Detail_LS)/sizeof(Detail_LS[0]);i++)
    {
        if(Detail_LS[i].ItemName=="LS_PADDING")
            offset_now+=1584;//第一个帧为添加应答器报文额外添加了198byte
        Detail_LS[i].value=FetchValue(Detail_LS[i].ItemLength);
        qDebug()<<QString::fromStdString(Detail_LS[i].ItemName)<<" "<<QString::number(Detail_LS[i].value,2)<<" "<<QString::number(Detail_LS[i].value,10);
    }
    qDebug()<<"********************************************************************";
    for(unsigned int i=0;i<sizeof(Detail_LS_Bistd)/sizeof(Detail_LS_Bistd[0]);i++)
    {
       Detail_LS_Bistd[i].value=FetchValue(Detail_LS_Bistd[i].ItemLength);
        qDebug()<<QString::fromStdString(Detail_LS_Bistd[i].ItemName)<<" "<<QString::number(Detail_LS_Bistd[i].value,2)<<" "<<QString::number(Detail_LS_Bistd[i].value,10);
    }
    qDebug()<<"********************************************************************";
    for(unsigned int i=0;i<sizeof(Detail_Ls_Ctcs)/sizeof(Detail_Ls_Ctcs[0]);i++)
    {
        if(i==89)//这些项被跳过了
            i=130;
        if(Detail_Ls_Ctcs[i].ItemLength<=64)
            Detail_Ls_Ctcs[i].value[0]=FetchValue(Detail_Ls_Ctcs[i].ItemLength);
        else
        {
            int temp_length=Detail_Ls_Ctcs[i].ItemLength;
            int index=0;
            while(temp_length>64)
            {
                Detail_Ls_Ctcs[i].value[index++]=FetchValue(64);
                temp_length-=64;
            }
            Detail_Ls_Ctcs[i].value[index]=FetchValue(temp_length);
        }
        //qDebug()<<QString::fromStdString(Detail_Ls_Ctcs[i].ItemName)<<" "<<QString::number(Detail_Ls_Ctcs[i].value,2)<<" "<<QString::number(Detail_Ls_Ctcs[i].value,10);
    }
    qDebug()<<QString::number(++count,10);
}

qint64 Analyze_ATPMessage::FetchValue(unsigned int length)
{
    if(offset_now+length>ReceivedData->size()*8)
    {
        return -1;//表示取值越界
    }
    qint64 ret=0;
    while(true)
    {
        unsigned char temp_value=(*ReceivedData)[offset_now/8];
        temp_value<<=offset_now%8;
        int count= 8-offset_now%8;
        while(count--)
        {
            ret<<=1;
            if(temp_value&0x80)
                ret|=0x01u;
            temp_value<<=1;
            offset_now+=1;
            if(--length==0)
                return ret;
        }
    }
    return ret;
}
