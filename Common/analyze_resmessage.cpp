#include "analyze_resmessage.h"
#include <QDebug>
//std::vector<std::vector<Item>> Analyze_Result;
const BaliseBitMessage* Analyze_BaliseMessage::msgptr = nullptr;
qint16 Analyze_BaliseMessage::offset_now;
QString Analyze_BaliseMessage::msg_identifier;
QVector<QVector<Item>> Analyze_BaliseMessage::Analyze_Result;

struct
{
	QString remark;//该项的备注
	unsigned char length;//该项的存储数据的位长
	QString name;//该项的名称
	unsigned char mark;//表记值，用于区分对于不同项的操作
}_ItemMask[253] =
/************************************************************************************
*关于mark的定义：从左至右：
*                前四位：0：表示该项与其他项无关
*                        F：表示该项决定其他项的值
*                        1；表示该项由其他项的值决定，并且当那项为1时，该项存在
*                        7：表示该项由其他项的值决定，并且当那项为0时，该项存在
*                后四位：0：表示该项既不是循环体的一部分，也不是循环体的开头
*                        其他：如果前一项的mark的后四位与本项相同，表示该项是循环体的一部分
*                        如果前一项的mark的后四位与本项不同，表示该项是循环体的开头，循环到mark值的后四位不同的项为止
***********************************************************************************/
{
{"",0,"信息帧",0},
{"信息传送的方向：",  1, "Q_UPDOWN",  0},
{"语言/代码版本编号：",  7, "M_VERSION",  0},
{"信息传输媒介：",  1, "Q_MEDIA",  0},
{"本应答器在应答器组中的位置：",  3, "N_PIG",  0},
{"应答器组中所包含的应答器数量：",  3, "N_TOTAL",  0},
{"本应答器信息与前/后应答器信息的关系：",  2, "M_DUP",  0},
{"报文计数器：",  8, "M_MCOUNT",  0},
{"地区编号：",  10, "NID_C",  0},
{"应答器标识号",  14, "NID_BG",  0},
{"应答器组的链接关系：",  1, "Q_LINK",  0},
{"",0,"EOP",0},
{"",0,"ETCS-5",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数",  13, "L_PACKET",  0},
{"距离/长度分辨率：",  2, "Q_SCALE",  0},
{"到下一个链接应答器组的距离增量：",  15, "D_LINK",  0},
{"下一个链接应答器组与前一个的地区关系：",  1, "Q_NEWCOUNTRY",  0xf0},
{"地区编号：",  10, "NID_C",  0x10},
{"应答器组编号：",  14, "NID_BG",  0},
{"列车通过被链接应答器组时的运行方向：",  1, "Q_LINKORIENTATION",  0},
{"当链接失败时，ATP采取的措施：",  2, "Q_LINKREATION",  0},
{"链接应答器允许的安装偏差：",  6, "Q_LOCACC",  0},
{"包含链接应答器组的数量：",  5, "N_ITER",  0x01},
{"到下一个链接应答器组的距离增量：",  15, "D_LINK(k)",  0x01},
{"下一个链接应答器组与前一个的地区关系：",  1, "Q_NEWCOUNTRY(k)",  0xf1},
{"地区编号：",  10, "NID_C(k)",  0x11},
{"应答器组编号：",  14, "NID_BG(k)",  0x01},
{"列车通过被链接应答器组是的运行方向：",  1, "Q_LINKORIENTATION(k)",  0x01},
{"当链接失败时，ATP采取的措施：",  2, "Q_LINKREATION(k)",  0x01},
{"链接应答器允许的安装偏差：",  6, "Q_LOCACC(k)",  0x01},
{"",0,"EOP",0},
{"",0,"ETCS-16",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"重定位区段长度：",  15, "L_SECTION",  0},
{"",0,"EOP",0},
{"",0,"ETCS-21",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"到本应答器所描述的坡道起始点的距离：",  15, "D_GRADIENT",  0},
{"坡度识别：",  1, "Q_GDIR",  0},
{"安全坡度：",  8, "G_A",  0},
{"包含坡度变化点的数量：",  5, "N_ITER",  0x01},
{"到下一个坡度变化点的距离增量：",  15, "D_GRADIENT(k)",  0x01},
{"坡度识别：",  1, "Q_GDIR(k)",  0x01},
{"安全坡度：",  8, "G_A(k)",  0x01},
{"",0,"EOP",0},
{"",0,"ETCS-27",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"到本应答器所描述的速度信息起始点的距离：",  15, "D_STATIC",  0},
{"线路最大允许列车运行速度：",  7, "V_STATIC",  0},
{"允许运行速度出口对车头/车尾的有效性：",  1, "Q_FRONT",  0},
{"包含列车类型的数量：",  5, "N_ITER",  0x01},
{"列车类型：",  4, "NC_DIEF(n)",  0x01},
{"列车最大允许运行速度：",  7, "V_DIEF(n)",  0x01},
{"包含速度变化点的数量：",  5, "V_ITER",  0x02},
{"到下一个速度变化点的距离增量：",  15, "D_STATIC(k)",  0x02},
{"线路最大允许列成运行速度：",  7, "V_STATIC(k)",  0x02},
{"允许运行速度出口对车头/车尾的有效性：",  1, "Q_FRONT(k)",  0x02},
{"包含列车类型的数量",  5, "N_ITER(k)",  0x06},
{"列车类型：",  4, "NC_DIEF(k,m)",  0x06},
{"列车最大允许运行速度：",  7, "V_DIEF(k,m)",  0x06},
{"",0,"EOP",0},
{"",0,"ETCS-41",0},
{"信息包识别码:",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"到等级转换点的距离：",  15, "Q_LEVELTR",  0},
{"列控的转换等级：",  3, "M_LEVELTR",  0xf1},
{"转换的非ETCS等级：",  8, "NID_STM",  0x10},
{"等级转换点外方确认区段长度：",  15, "L_ACKLEVELTR",  0},
{"包含等级转换等级的数量：",  5, "N_ITER",  0x01},
{"转换的裂空等级：",  3, "M_LEVELTR(k)",  0xf1},
{"转换的非ETCS等级：",  8, "NID_STM(k)",  0x11},
{"等级转换点外放确认区段长度：",  15, "L_ACKLEVELTR(k)",  0x01},
{"",0,"EOP",0},
{"",0,"ETCS-42",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"通信命令：",  1, "Q_RBC",  0},
{"地区编号：",  10, "NID_C",  0},
{"RBC编号：",  14, "NID_RBC",  0},
{"无线用户电话号码：",  64, "NID_RADIO",  0},
{"睡眠设备的通信管理：",  1, "Q_SLEEPSESESSION",  0},
{"",0,"EOP",0},
{"",0,"ETCS-44",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"CTCS用户数据标识码：",  9, "NID_XUSER",  0xff},
{"由NID_XUSER确定的信息包",  0, "XXXXX",  0},
{"",0,"EOP",0},
{"",0,"ETCS-45",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数:",  13, "L_PACKET",  0},
{"无线网标识号:",  24, "NID_MN",  0},
{"",0,"EOP",0},
{"",0,"ETCS-46",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"转换的裂空等级：",  3, "M_LEVELTR",  0xf0},
{"转换的非ETCS等级：",  8, "NID_STM",  0x10},
{"包含等级转换点的数量：",  5, "N_ITER",  0x01},
{"转换的列控等级：",  3, "M_LEVELTR(k)",  0xf1},
{"转换的非ETCS列控等级：",  8, "NID_STM(k)",  0x11},
{"",0,"EOP",0},
{"",0,"ETCS-68",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"恢复初始状态（进入特殊区段）的要求：",  1, "Q_TRACKINIT",  0xf0},
{"到恢复初始状态开始点的距离：",  15, "D_TRACKINIT",  0x10},
{"到特殊轨道区段的距离：",  15, "D_TRACKCOND",  0x70},
{"特殊轨道区段的长度：",  15, "L_TRACKCOND",  0x70},
{"特殊轨道区段定义：",  4, "M_TRACKCOND",  0x70},
{"包含特殊轨道区段的数量：",  5, "N_ITER",  0x71},
{"到特殊轨道区段的距离：",  15, "D_TRACKCOND(k)",  0x71},
{"特殊轨道区段的长度：",  15, "L_TRACKCOND(k)",  0x71},
{"特殊轨道区段定义：",  4, "M_TRACKCOND(k)",  0x71},
{"",0,"EOP",0},
{"",0,"ETCS-72",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"显示消息的种类：",  2, "Q_TEXTCLASS",  0},
{"文本信息显示条件组合要求：",  1, "Q_TEXTDISPLAY",  0},
{"至应显示文本信息的距离：",  15, "D_TEXTDISPLAY",  0},
{"文本显示对车载设备运行模式的要求：",  4, "M_MODETEXTDISPLAY",  0},
{"文本显示对车载设备操作等级的要求：",  3, "M_LEVELTEXTDISPLAY",  0xf0},
{"非ETCS等级：",  8, "NID_STM",  0x10},
{"应显示文本的区域长度：",  15, "L_TEXTDISPLAY",  0},
{"文本显示的时间：",  10, "T_TEXTDISPLAY",  0},
{"取消文本显示对车载设备运行模式的要求：",  4, "M_MODETEXTDISPLAY",  0},
{"取消文本显示对车载设备操作等级的要求：",  3, "M_LEVELTEXTDISPLAY",  0xf0},
{"非ETCS等级：",  8, "NID_STM",  0x10},
{"取消文本显示确认的要求或反应：",  2, "Q_TEXTCONFIRM",  0},
{"文本字符串字节长度：",  8, "L_TEXT",  0x01},
{"文本字节值：",  8, "X_TEXT(L_TEXT)",  0x01},
{"",0,"EOP",0},
{"",0,"ETCS-79",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"参考应答器愚笨应答器组的地区关系：",  1, "Q_NEWCOUNTER",  0xf0},
{"地区编号：",  10, "NID_C",  0x10},
{"应答器组编号：",  14, "NID_BG",  0},
{"线路公里标距离参考应答器的偏移量：",  15, "D_POSOFF",  0},
{"线路公里标计数方向：",  1, "Q_MPOSITION",  0},
{"线路公里标参考点：",  20, "M_POSITION",  0},
{"包含公里标的数量：",  5, "N_ITER",  0x01},
{"参考应答器组与本应答器组的地区关系：",  1, "Q_NEWCOUNTER(k)",  0xf1},
{"地区编号：",  10, "NID_C(k)",  0x11},
{"应答器组编号：",  14, "NID_BG(k)",  0x01},
{"线路公里标距离参考应答器的偏移量：",  15, "D_POSOFF(k)",  0x01},
{"线路公里标技术方向：",  1, "Q_MPOSITION(k)",  0x01},
{"线路公里标参考点：",  20, "M_POSITION(k)",  0x01},
{"",0,"EOP",0},
{"",0,"ETCS-131",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"至RBC切换点的距离",  15, "D_RBCTR",  0},
{"地区编号",  10, "NID_C",  0},
{"RBC标识号：",  14, "NID_RBC",  0},
{"无线用户号码：",  64, "NID_RADIO",  0},
{"睡眠设备的通信管理：",  1, "Q_SLEEPSESESSION",  0},
{"",0,"EOP",0},
{"",0,"ETCS-132",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"调车模式:",  1, "Q_ASPECT",  0},
{"",0,"EOP",0},
{"",0,"ETCS-137",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"制定处于目视行车模式的裂成是否必须停车：",  1, "Q_SRSTOP",  0},
{"",0,"EOP",0},
{"",0,"ETCS-254",0},
{"信息包识别码：",  8, "NID_PACKET",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"",0,"EOP",0},
{"",0,"CTCS-1",0},
{"信息包识别码：",  9, "NID_XUSER",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"长度分辨率：",  2, "Q_SCALE",  0},
{"到本应答器所描述的轨道区段起始点的距离：",  15, "D_SIGNAL",  0},
{"信号机或信号点类型：",  4, "NID_SIGNAL",  0},
{"轨道区段载频：",  5, "NID_FREQUENCY",  0},
{"轨道区段长度：",  15, "L_SECTION",  0},
{"包含轨道区段数：",  5, "N_ITER",  0x01},
{"信号机或信号点类型：",  4, "NID_SIGNAL(k)",  0x01},
{"轨道区段载频：",  5, "NID_FREQUENCY(k)",  0x01},
{"轨道区段长度：",  15, "L_SECTION(k)",  0x01},
{"",0,"EOP",0},
{"",0,"CTCS-2",0},
{"信息包识别码：",  9, "NID_XUSER",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"临时限速信息有效区段长度：",  15, "L_TSRarea",  0},
{"到临时限速区段的距离：",  15, "D_TSR",  0},
{"临时限速区段的长度：",  15, "L_TSR",  0},
{"允许运行速度出口对车头/车尾的有效性：",  1, "Q_FRONT",  0},
{"临时限速的限制速度：",  7, "V_TSR",  0},
{"包含临时限速区段数：",  5, "N_ITER",  0x01},
{"到下一个临时限速区段的距离增量：",  15, "D_TSR(n)",  0x01},
{"临时限速区段的长度：",  15, "L_TSR(n)",  0x01},
{"允许运行速度出口对车头/车尾的有效性：",  1, "Q_FRONT(n)",  0x01},
{"临时限速的限制速度",  7, "V_TSR(n)",  0x01},
{"",0,"EOP",0},
{"",0,"CTCS-3",0},
{"信息包识别码：",  9, "NID_XUSER",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"到反向运行区间开始点的距离：",  15, "D_STARTREVE",  0},
{"反向运行区间的长度：",  15, "L_REVERSEAREA",  0},
{"",0,"EOP",0},
{"",0,"CTCS-4",0},
{"信息包识别码：",  9, "NID_XUSER",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"距离/长度的分辨率：",  2, "Q_SCALE",  0},
{"到大号码道岔距离",  15, "D_TURNOUT",  0},
{"道岔侧向列车最大允许通过速度：",  7, "V_TURNOUT",  0},
{"",0,"EOP",0},
{"",0,"CTCS-5",0},
{"信息包识别码：",  9, "NID_XUSER",  0},
{"验证方向：",  2, "Q_DIR",  0},
{"信息包位数：",  13, "L_PACKET",  0},
{"立即停车:",  1, "Q_STOP",  0},
{"",0,"EOP",0},
};//包括信息包项199个，帧标志项10个，每个信息包的开始标志23个(17个ETCS包 + 5个CTCS包 + 一个结尾标志“*”)
struct
{
	unsigned char Package_Number;
	unsigned char Table_Numer;
}TABLE_MessagePackage[17] =
{
{0,0},
{5,12},
{16,33},
{21,40},
{27,53},
{41,72},
{42,86},
{44,96},
{45,103},
{46,109},
{68,119},
{72,134},
{79,154},
{131,173},
{132,184},
{137,190},
{254,196},
};//信息包对应存储结构体的映射关系
struct
{
	unsigned char Package_Number;
	unsigned char Table_Numer;
}CTCS_map[6] =
{
{0,0},
{1,202},
{2,216},
{3,232},
{4,240},
{5,248},
};//CTCS信息包对应于信息包项的映射

Analyze_BaliseMessage::Analyze_BaliseMessage()
{

}

BalisePackCollector& Analyze_BaliseMessage::Resolve(const BaliseBitMessage &input_data)
{
	Analyze_BaliseMessage::msgptr = &input_data;
	AnalyzeBaliseMsg();
	GetBaliseID(input_data);
	return Analyze_Result;
}


/*************************************************************************
*  函数名称：GetResNumber
*  功能说明：从给定的应答器原始报文中提取应答器标识号
*  参数说明：input_data：给定的应答器原始报文
*  函数返回：应答器标识号（以QString的方式给回）
*  修改时间：2017-6-21 已测试通过
*  备   注：本函数同样会修改本类中的静态变量的msg_identifier值为应答器标识号
*************************************************************************/
QString Analyze_BaliseMessage::GetBaliseID(const BaliseBitMessage &input_data)
{
	QString msg_identifier;
	offset_now = 25;//设置要提取信息的起始位置
	Analyze_BaliseMessage::msgptr = &input_data;
	msg_identifier.clear();
	QString temp_str = QString::number(FetchValue(7), 10);
	for (int i = 0; i < 3 - temp_str.size(); i++)
		msg_identifier.push_back('0');
	msg_identifier += temp_str;//大区编号
	msg_identifier += "-";
	msg_identifier += QString::number(FetchValue(3), 10);//分区编号
	msg_identifier += "-";
	temp_str = QString::number(FetchValue(6), 10);
	for (int i = 0; i < 2 - temp_str.size(); i++)
		msg_identifier.push_back('0');
	msg_identifier += temp_str;//车站编号
	msg_identifier += "-";
	temp_str = QString::number(FetchValue(8), 10);
	for (int i = 0; i < 3 - temp_str.size(); i++)
		msg_identifier.push_back('0');
	msg_identifier += temp_str;//应答器编号

	offset_now = 9;
	int order_of_Baliser_in_group = FetchValue(3) + 1;//在应答器组中的位置
	int num_of_Baliser_in_group = FetchValue(3) + 1;//应答器组中包含的应答器数量
	if (num_of_Baliser_in_group > 1)
	{
		msg_identifier += "-";
		msg_identifier += QString::number(order_of_Baliser_in_group, 10);//组内的位置
	}
	return msg_identifier;
}

/*************************************************************************
*  函数名称：FindItem
*  功能说明：在给定的解析后的信息包中找到给定的项（给定项在信息包中可能存在不止一项，所以返回的是QVector）
*  参数说明：name：输入的要找的项的项名，需严格与_ItemMask[]中的内容相符合
*          package：给定的要搜索的信息包
*  函数返回：一个QVector,其中[0]项存储的是要寻找的项，从[1]开始是这个项在指定信息包中的内容，如果QVector.size()==1，那么表示在指定信息包中没有这一项
*  修改时间：2017-6-21 已测试通过
*  备   注：
*************************************************************************/
BalisePackage Analyze_BaliseMessage::FindItem(QString name, BalisePackage package)
{
	BalisePackage RP;
	Item item;
	item.name = name;
	item.remark = "";
	item.value = 0;
	RP.push_back(item);
	for (int i = 1; i < package.size(); i++)
	{
		if (package[i].name.contains(name))
		{
			RP.push_back(package[i]);
		}
	}
	return RP;
}

/*************************************************************************
*  函数名称：CheckPackage
*  功能说明：在给定的解析后的QVector中找到对应的信息包的位置
*  参数说明：input：输入的信息包包名，需严格与_ItemMask[]中的内容相符合
*          data_collector：给定的解析后的QVector，默认是本类中的静态成员QVector
*  函数返回：-1：未找到该信息包
*          其他：该信息包在解析后的QVector中的位置
*  修改时间：2017-6-21 已测试通过
*  备   注：
*************************************************************************/
QVector<int> Analyze_BaliseMessage::GetPackagePosition(QString input, BalisePackCollector& data_collector)
{
	QVector<int> ret;
	for (int i = 0; i < data_collector.size(); i++)
	{
		if (data_collector[i][0].name == input)
		{
			ret.push_back(i);
		}
	}
	return ret;
}

/*************************************************************************
*  函数名称：FetchValue
*  功能说明：从offset_now的位置提取length位长度的值
*  参数说明：length：需要提取的位长度
*  函数返回：提取的数值
*  修改时间：2017-6-21 已测试通过
*  备    注：
*************************************************************************/
qint64 Analyze_BaliseMessage::FetchValue(unsigned int length)
{
	if (offset_now + length > sizeof((*msgptr).Balise_Tel) / sizeof((*msgptr).Balise_Tel[0]) * 8)
	{
		return -1;//表示取值越界
	}
	else if (length == 0)
	{
		return 0;
	}
	qint64 ret = 0;
	while (true)
	{
		unsigned char temp_value = (*msgptr).Balise_Tel[offset_now / 8];
		temp_value <<= offset_now % 8;
		int count = 8 - offset_now % 8;
		while (count--)
		{
			ret <<= 1;
			if (temp_value & 0x80)
				ret |= 0x01u;
			temp_value <<= 1;
			offset_now += 1;
			if (--length == 0)
				return ret;
		}
	}
	return ret;
}

/*************************************************************************
*  函数名称：AnalyzeResponserMsg
*  功能说明：从报文中提取信息包信息并放入模板数组中
*  参数说明：Message_Order:选取的报文的序号
*  函数返回：-1：表示取值越界
*          -2：信息包识别码提取错误
			0：表示提取成功
*  修改时间：2017-6-3 已测试通过
*  备    注：逻辑复杂，不建议修改
*************************************************************************/
int Analyze_BaliseMessage::AnalyzeBaliseMsg()
{
	Analyze_BaliseMessage::offset_now = 0;
	Analyze_Result.clear();

	unsigned char Package_Number;
	unsigned char SIGN_VALUE = 0;
	unsigned char circle_time;
	Item templete_item;
	QVector<Item> templete_package;
	int i = 0;
	int k = 0;

	for (k = 0; k <= 10; ++k)
	{
        templete_item.name = _ItemMask[k].name;
        templete_item.value = FetchValue(_ItemMask[k].length);
		templete_item.remark = Expend(k, templete_item.value);
		if (templete_item.value == -1)
			return -1;
		templete_package.push_back(templete_item);
	}
	Analyze_Result.push_back(templete_package);
	templete_package.clear();
	while ((Package_Number = FetchValue(8)) != 0xff)
	{
		templete_package.clear();
		i = -1;
		offset_now -= 8;
		//遍历寻找到映射到的存储结构体的位置
		while (Package_Number != TABLE_MessagePackage[++i].Package_Number)
		{
			if (i == 16)
				return -2;//表示报文对不上
		}
		//更新【要显示的信息包列表】当前新添加信息包信息
        templete_item.name = _ItemMask[TABLE_MessagePackage[i].Table_Numer].name;
        templete_item.remark = _ItemMask[TABLE_MessagePackage[i].Table_Numer].remark;
		templete_item.value = -2;
		templete_package.push_back(templete_item);//第一项是信息包的名字和备注
		if (templete_item.name == "ETCS-27")//分析ETCS-27数据包
		{
			Analyze_ETCS_27();
			continue;
		}
		if (templete_item.name == "ETCS-44")//分析ETCS-44数据包
		{
			Analyze_ETCS_44();
			continue;
		}
		//从映射表找到项的起始地址
		k = TABLE_MessagePackage[i].Table_Numer + 1;
        while (_ItemMask[k].name != "EOP")
		{
			/*情况一：该项不与其他项有联系，也与循环体无联系，直接累加该项地址*/
            if (_ItemMask[k].mark == 0)
			{
                templete_item.value = FetchValue(_ItemMask[k].length);
                templete_item.name = _ItemMask[k].name;
				templete_item.remark = Expend(k, templete_item.value);
				templete_package.push_back(templete_item);
			}
			else
			{
				/*情况二：该项与其他项有联系，但是不在循环体中，直接判断是该项决定其他项（记录值并更新地址）还是该项由其他项决定（读取SIGN_VALUE的值并判断是否记录本项地址）*/
                if ((_ItemMask[k].mark & 0xf0) != 0 && (_ItemMask[k].mark & 0x0f) == 0)//如果mark头四位不为0并且后四位等于0，表示该项与其他项之间有联系，且不在循环体中
				{
                    if ((_ItemMask[k].mark & 0xf0) == 0xf0)//如果头四位是0xf0，表示该项的值决定其他项，记录该项的值，并更新地址
					{
                        SIGN_VALUE = FetchValue(_ItemMask[k].length);
						templete_item.value = SIGN_VALUE;
                        templete_item.name = _ItemMask[k].name;
						templete_item.remark = Expend(k, templete_item.value);
						templete_package.push_back(templete_item);
					}
                    else if ((_ItemMask[k].mark & 0xf0) == 0x10)//如果头四位是1，表示该项由SIGN_VALUE决定，是1时该项存在
					{
						if (SIGN_VALUE == 1)//SIGN_VALUE是1时该项存在，更新地址
						{
                            templete_item.value = FetchValue(_ItemMask[k].length);
                            templete_item.name = _ItemMask[k].name;
							templete_item.remark = Expend(k, templete_item.value);
							templete_package.push_back(templete_item);
						}
					}
                    else if ((_ItemMask[k].mark & 0xf0) == 0x70)//如果头四位是7，表示该项由SIGN_VALUE决定，是0时该项存在
					{
						if (SIGN_VALUE == 0)//SIGN_VALUE是0时该项存在，更新地址
						{
                            templete_item.value = FetchValue(_ItemMask[k].length);
                            templete_item.name = _ItemMask[k].name;
							templete_item.remark = Expend(k, templete_item.value);
							templete_package.push_back(templete_item);
						}
					}
				}
                else if ((_ItemMask[k].mark & 0x0f) != 0)//如果后四位不为0，表示该项是循环体的开头，或者在循环体中
				{
					/*情况三：该项是循环体的开头，逐项累加循环体的内容，并判断内容中的每一项是否因为其他项而被消掉*/
                    if ((_ItemMask[k].mark & 0x0f) != (_ItemMask[k - 1].mark & 0x0f))//如果该项的mark值后四位与前一向的mark值后四位不同，表示该项是循环体的开头
					{
                        circle_time = FetchValue(_ItemMask[k].length);
						templete_item.value = circle_time;
                        templete_item.name = _ItemMask[k].name;
						templete_item.remark = Expend(k, templete_item.value);
						templete_package.push_back(templete_item);

                        unsigned char sign = _ItemMask[k].mark & 0x0f;
						unsigned char temp = k;
						if (circle_time == 0)
						{
                            while ((_ItemMask[++k].mark & 0x0f) == sign);
							continue;
						}
						while ((circle_time--) > 0)//循环体循环次数
						{
							k = temp;
                            while ((_ItemMask[++k].mark & 0x0f) == sign)//每次都从第k+1项开始循环,直到某一项mark值后四位与标记值不同为止
							{
                                if ((_ItemMask[k].mark & 0xf0) == 0xf0)//如果头四位是0xf0，表示该项的值决定其他项，记录该项的值，并更新地址
								{
                                    SIGN_VALUE = FetchValue(_ItemMask[k].length);
									templete_item.value = SIGN_VALUE;
                                    templete_item.name = _ItemMask[k].name;
									templete_item.remark = Expend(k, templete_item.value);
									templete_package.push_back(templete_item);
								}
                                else if ((_ItemMask[k].mark & 0xf0) == 0x10)//如果头四位是1，表示该项由SIGN_VALUE决定，是1时该项存在
								{
									if (SIGN_VALUE == 1)//SIGN_VALUE是1时该项存在，更新地址
									{
                                        templete_item.value = FetchValue(_ItemMask[k].length);
                                        templete_item.name = _ItemMask[k].name;
										templete_item.remark = Expend(k, templete_item.value);
										templete_package.push_back(templete_item);
									}
									else//是0时，该项不存在，直接跳过
										continue;
								}
                                else if ((_ItemMask[k].mark & 0xf0) == 0x70)//如果头四位是7，表示该项由SIGN_VALUE决定，是0时该项存在
								{
									if (SIGN_VALUE == 0)//SIGN_VALUE是0时该项存在，更新地址
									{
                                        templete_item.value = FetchValue(_ItemMask[k].length);
                                        templete_item.name = _ItemMask[k].name;
										templete_item.remark = Expend(k, templete_item.value);
										templete_package.push_back(templete_item);
									}
									else//是1时，该项不存在，直接跳过
										continue;
								}
								else//表示只是普通循环体中的一部分，直接累加地址
								{
                                    templete_item.value = FetchValue(_ItemMask[k].length);
                                    templete_item.name = _ItemMask[k].name;
									templete_item.remark = Expend(k, templete_item.value);
									templete_package.push_back(templete_item);
								}
							}
						}
						continue;
					}
				}
			}
			++k;
		}
		Analyze_Result.push_back(templete_package);
	}
	return 0;
}
/*************************************************************************
*  函数名称：Analyze_ETCS_27
*  功能说明：Fill_tempelete_array函数执行时对27信息包的单独分析
*  参数说明：
*  函数返回：
*  修改时间：2017-6-21 已测试通过
*  备    注：
*************************************************************************/
int Analyze_BaliseMessage::Analyze_ETCS_27()
{
	int k = 52;
	unsigned char circle_time1;
	unsigned char circle_time2;
	Item templete_item;
	QVector<Item> templete_package;

    while (_ItemMask[++k].name != "EOP")
	{
		if (k == 61)
		{
            circle_time1 = FetchValue(_ItemMask[k].length);
            templete_item.name = _ItemMask[k].name;
			templete_item.value = circle_time1;
			templete_item.remark = Expend(k, templete_item.value);
			templete_package.push_back(templete_item);

			//ETCS_27中第一个循环必存在的项
			for (int i = 0; i < circle_time1; ++i)
			{
				for (k = 62; k <= 63; ++k)
				{
                    templete_item.name = _ItemMask[k].name;
                    templete_item.value = FetchValue(_ItemMask[k].length);
					templete_item.remark = Expend(k, templete_item.value);
					templete_package.push_back(templete_item);
				}
			}
			k = 64;

            circle_time1 = FetchValue(_ItemMask[k].length);
            templete_item.name = _ItemMask[k].name;
			templete_item.value = circle_time1;
			templete_item.remark = Expend(k, templete_item.value);
			templete_package.push_back(templete_item);

			for (int i = 0; i < circle_time1; ++i)
			{
				//ETCS_27中第二个循环必存在的项
				for (k = 65; k <= 67; ++k)
				{
                    templete_item.name = _ItemMask[k].name;
                    templete_item.value = FetchValue(_ItemMask[k].length);
					templete_item.remark = Expend(k, templete_item.value);
					templete_package.push_back(templete_item);
				}
				//ETCS_27中第二个循环中的嵌套循环
				k = 68;
                circle_time2 = FetchValue(_ItemMask[k].length);
                templete_item.name = _ItemMask[k].name;
				templete_item.value = circle_time2;
				templete_item.remark = Expend(k, templete_item.value);
				templete_package.push_back(templete_item);
				for (int m = 0; m < circle_time2; ++m)
				{
					for (k = 69; k <= 70; ++k)
					{
                        templete_item.name = _ItemMask[k].name;
                        templete_item.value = FetchValue(_ItemMask[k].length);
						templete_item.remark = Expend(k, templete_item.value);
						templete_package.push_back(templete_item);
					}
				}
			}
			Analyze_Result.push_back(templete_package);
			return 0;
		}
        templete_item.name = _ItemMask[k].name;
        templete_item.value = FetchValue(_ItemMask[k].length);
		templete_item.remark = Expend(k, templete_item.value);
		templete_package.push_back(templete_item);
	}
	Analyze_Result.push_back(templete_package);
	return 0;//其实执行不到这一句，为了消除warning加上的
}
/*************************************************************************
*  函数名称：Analyze_ETCS_44
*  功能说明：Fill_tempelete_array函数执行时对44信息包的单独分析
*  参数说明：无
*  函数返回：分析后的偏移地址
*  修改时间：2017-6-21 已测试通过
*  备    注：
*************************************************************************/
int Analyze_BaliseMessage::Analyze_ETCS_44()
{
	int k = 95;

	unsigned char CTCS_NUM;
	unsigned char circle_time;
	Item templete_item;
	QVector<Item> templete_package;

    while (_ItemMask[++k].name != "EOP")
	{
		if (k == 100)
		{
            CTCS_NUM = FetchValue(_ItemMask[k].length);
            templete_item.name = _ItemMask[k].name;
			templete_item.value = CTCS_NUM;
			templete_item.remark = Expend(k, templete_item.value);
			templete_package.push_back(templete_item);

			k = CTCS_map[CTCS_NUM].Table_Numer + 1;
            while (_ItemMask[k].name != "EOP")
			{
                if (_ItemMask[k].mark == 0)
				{
                    templete_item.name = _ItemMask[k].name;
                    templete_item.value = FetchValue(_ItemMask[k].length);
					templete_item.remark = Expend(k, templete_item.value);
					templete_package.push_back(templete_item);//如果mark是0，表示该项必存在且不在循环中
				}
				else
				{
					unsigned char temp = k;
                    circle_time = FetchValue(_ItemMask[k].length);
                    templete_item.name = _ItemMask[k].name;
					templete_item.value = circle_time;
					templete_item.remark = Expend(k, templete_item.value);
					templete_package.push_back(templete_item);

					if (circle_time == 0)
					{
                        while (_ItemMask[++k].name != "EOP");
						continue;
					}
					while ((circle_time--) > 0)//循环体循环次数
					{
						k = temp;
                        while (_ItemMask[++k].mark != 0)//每次都从第k+1项开始循环,直到某一项mark值后四位与标记值不同为止
						{
                            templete_item.name = _ItemMask[k].name;
                            templete_item.value = FetchValue(_ItemMask[k].length);
							templete_item.remark = Expend(k, templete_item.value);
							templete_package.push_back(templete_item);
						}
					}
					continue;
				}
				++k;
			}
			Analyze_Result.push_back(templete_package);
			return 0;
		}
        templete_item.name = _ItemMask[k].name;
        templete_item.value = FetchValue(_ItemMask[k].length);
		templete_item.remark = Expend(k, templete_item.value);
		templete_package.push_back(templete_item);
	}
	Analyze_Result.push_back(templete_package);
	return 0;//其实执行不到这一句，为了消除warning加上的
}

/*************************************************************************
*  函数名称：Expend
*  功能说明：对解析项的内容进行扩展
*  参数说明：k：对第k项的内容进行解析扩展
*          value：这一项的提取值，根据值得不同对该项解析内容扩展
*  函数返回：扩展后的项内容
*  修改时间：2017-6-21 已测试通过
*  备   注：
*************************************************************************/
QString Analyze_BaliseMessage::Expend(int k, qint64 value)
{
	switch (k) {
	case 1: {//Q_UPDOWN，信息传送的方向
		if (value == 0)
            return _ItemMask[k].remark + "车对地";
		else if (value == 1)
            return _ItemMask[k].remark + "地对车";
		else {
			qDebug() << "Expend函数中case1出错";
			return "error";
		}
	}
	case 2: {//M_VERSION，语言/代码版本编号
		if (value == 16)
            return _ItemMask[k].remark + "V1.0";
		else {
			qDebug() << "Expend函数中case2出错";
			return "error";
		}
	}
	case 3: {//Q_MEDIA，信息传输媒介
		if (value == 0)
            return _ItemMask[k].remark + "应答器";
		else if (value == 1)
            return _ItemMask[k].remark + "环线";
		else {
			qDebug() << "Expend函数中case3出错";
			return "error";
		}
	}
	case 4: {//N_PIG，本应答器在应答器组中的位置
		if (value>=0 && value <= 7)
		{
            return _ItemMask[k].remark + QString::number(value+1);
		}
		else {
			qDebug() << "Expend函数中case4出错,N_PIG越界";
			return "error";
		}
	}
	case 5: {//N_TOTAL，应答器组中所包含的应答器数量
		if (value >= 0 && value <= 7)
            return _ItemMask[k].remark + QString::number(value, 10);
		else {
			qDebug() << "Expend函数中case5出错,N_TOTAL越界";
			return "error";
		}
	}
	case 6: {//M_DUP，本应答器信息与前/后应答器信息的关系
		if (value == 0)
            return _ItemMask[k].remark + "不同";
		else if (value == 1)
            return _ItemMask[k].remark + "与后一个相同";
		else if (value == 2)
            return _ItemMask[k].remark + "与前一个相同";
		else {
			qDebug() << "Expend函数中case6出错";
			return "error";
		}
	}
	case 7: {//M_MCOUNT，报文计数器
		if (value == 255 || value == 252 || value == 253 || value == 0)
            return _ItemMask[k].remark + QString::number(value, 10);
		else {
			qDebug() << "Expend函数中case7出错";
			return "error";
		}
	}
	case 8:case 19:case 27:case 91:case 160:case 167:case 179:
	{//NID_C，地区编号
		int temp1 = (int)(value >> 3);
		int temp2 = (int)(value & 7);
        return _ItemMask[k].remark + "大区编号：" + QString::number(temp1, 10) + "小区编号：" + QString::number(temp2, 10);
	}
	case 9:case 161:case 168: {//NID_BG，应答器标识号
		int temp1 = (int)(value >> 8);
		int temp2 = (int)(value & 255);
        return _ItemMask[k].remark + "车站编号：" + QString::number(temp1, 10) + "应答器编号：" + QString::number(temp2, 10);
	}
	case 10: {//Q_LINK，应答器组的链接关系
		if (value == 0)
            return _ItemMask[k].remark + "不被链接";
		else if (value == 1)
            return _ItemMask[k].remark + "被链接";
		else {
			qDebug() << "Expend函数中case10出错";
			return "error";
		}
	}
	case 13:
	case 34:
	case 41:
	case 54:
	case 73:
	case 87:
	case 97:
	case 104:
	case 110:
	case 120:
	case 135:
	case 155:
	case 174:
	case 185:
	case 191:
	case 197:
	case 202:
	case 216:
	case 232:
	case 240:
	case 248: {//NID_PACKET，信息包识别码
		if ((value > 0 && value <= 5) || value == 16 || value == 21 || value == 27 || value == 41 || value == 42 || value == 44 || value == 45 || value == 46 || value == 68 || value == 72 || value == 79 || value == 131 || value == 132 || value == 137 || value == 254)
            return _ItemMask[k].remark + QString::number(value, 10);
		else {
			qDebug() << "Expend函数中case12出错";
			return "error";
		}
	}
	case 14:
	case 35:
	case 42:
	case 55:
	case 74:
	case 88:
	case 98:
	case 105:
	case 111:
	case 121:
	case 136:
	case 156:
	case 175:
	case 186:
	case 192:
	case 198:
	case 203:
	case 217:
	case 233:
	case 241:
	case 249: {//Q_DIR，验证方向
		if (value == 0)
            return _ItemMask[k].remark + "反向有效";
		else if (value == 1)
            return _ItemMask[k].remark + "正向有效";
		else if (value == 2)
            return _ItemMask[k].remark + "双向有效";
		else if (value == 3)
            return _ItemMask[k].remark + "备用";
		else {
			qDebug() << "Expend函数中case13出错";
			return "error";
		}
	}
	case 15:
	case 36:
	case 43:
	case 56:
	case 75:
	case 89:
	case 99:
	case 106:
	case 112:
	case 122:
	case 137:
	case 157:
	case 176:
	case 187:
	case 193:
	case 199:
	case 204:
	case 218:
	case 234:
	case 242:
	case 250: {//L_PACKET，信息包位数
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 16:
	case 37:
	case 44:
	case 57:
	case 76:
	case 123:
	case 138:
	case 158:
	case 177:
	case 205:
	case 219:
	case 235:
	case 243: {//Q_SCALE，距离/长度分辨率
		if (value == 0)
            return _ItemMask[k].remark + "10cm";
		else if (value == 1)
            return _ItemMask[k].remark + "1m";
		else if (value == 2)
            return _ItemMask[k].remark + "10m";
		else {
			qDebug() << "Expend函数中case15出错";
			return "error";
		}
	}
	case 17:
	case 25: {//D_LINK，到下一个链接应答器组的距离增量
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 18:
	case 26: {//NEW_COUNTER，下一个链接应答器组与前一个的地区关系
		if (value == 0)
            return _ItemMask[k].remark + "相同";
		else if (value == 1)
            return _ItemMask[k].remark + "不相同";
		else {
			qDebug() << "Expend函数中case17出错";
			return "error";
		}
	}
	case 20:case 28: {//NID_BG，应答器组编号
		if (value == 16383)
            return _ItemMask[k].remark + "16383为特殊值，当地面无法区分前方进路时，可用该值表示本应答器组前方链接的重定位应答器编号。当【ETCS-5】的该项为16383时，CTCS-2级列控车载设备只接受链接距离内含重定位信息【ETCS-16】的应答器组，链接距离D_LINK选择含重定位信息的应答器组中距离最远的一个";
		else
		{
			int temp1 = (int)(value >> 8);
			int temp2 = (int)(value & 255);
            return _ItemMask[k].remark + "车站编号：" + QString::number(temp1, 10) + "应答器编号：" + QString::number(temp2, 10);
		}
	}
	case 21:
	case 29: {//Q_LINKORIENTATION，列车通过被链接应答器组时的运行方向
		if (value == 0)
            return _ItemMask[k].remark + "反向";
		else if (value == 1)
            return _ItemMask[k].remark + "正向";
		else {
			qDebug() << "Expend函数中case20出错";
			return "error";
		}
	}
	case 22:
	case 30: {//Q_LINKREATION，当链接失败时，ATP采取的措施
		if (value == 0)
            return _ItemMask[k].remark + "紧急制动";
		else if (value == 1)
            return _ItemMask[k].remark + "常用制动";
		else if (value == 2)
            return _ItemMask[k].remark + "无反应";
		else if (value == 3)
            return _ItemMask[k].remark + "备用";
		else {
			qDebug() << "Expend函数中case21出错";
			return "error";
		}
	}

	case 23:
	case 31: {//Q_LOCACC，链接应答器允许的安装偏差
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 24:case 48:case 61:case 68:case 81:case 115:case 129:case 165:case 210:case 225: {//N_ITER项，各信息包的循环起始项
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 38: {//L_SECTION，重定位区段长度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 45: {//D_GRADIENT，到本应答器所描述的坡道起始点的距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 46:
	case 50: {//Q_GDIR，坡度识别
		if (value == 0)
            return _ItemMask[k].remark + "下坡或者平坡";
		else if (value == 1)
            return _ItemMask[k].remark + "上坡";
	}
	case 47:
	case 51: {//G_A，安全坡度
        return _ItemMask[k].remark + QString::number(value, 10) + "%";
	}
	case 49: {//D_GRADIENT(k)，到下一个坡度变化点的距离增量
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 58: case 65: {//D_STATIC，到本应答器所描述的速度信息起始点的距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 59:case 66: {//V_STATIC，线路最大允许列车运行速度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 60:
	case 67:
	case 223:
	case 228: {//Q_FRONT，允许运行速度出口对车头/车尾的有效性
		if (value == 0)
            return _ItemMask[k].remark + "\r\n由车载设备确定头尾有效性";
		else
            return _ItemMask[k].remark + "\r\n头有效，进入降速区段";
	}
	case 62:
	case 69: {//NC_DIEF，列车类型
		if (value == 0)
            return _ItemMask[k].remark + "主动摆式";
		else if (value == 1)
            return _ItemMask[k].remark + "被动摆式";
		else if (value == 2)
            return _ItemMask[k].remark + "对交叉风敏感的";
		else {
			qDebug() << "Expend函数中case58出错";
			return "error";
		}
	}
	case 63:
	case 70: {//V_DIEF，列车最大允许运行速度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 64: {//V_ITER，包含速度变化点的数量
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 77: {//Q_LEVELTR，到等级转换点的距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 78:
	case 82:
	case 113:
	case 116: {//M_LEVELTR，列控的转换等级
		if (value == 0)
            return _ItemMask[k].remark + "ETCS-0";
		else if (value == 1)
            return _ItemMask[k].remark + "STM";
		else if (value == 2)
            return _ItemMask[k].remark + "ETCS-1";
		else if (value == 3)
            return _ItemMask[k].remark + "ETCS-3";
		else if (value == 4)
            return _ItemMask[k].remark + "ETCS-3";
		else {
			qDebug() << "Expend函数中case73出错";
			return "error";
		}
	}
	case 79:
	case 83:
	case 114:
	case 117: {//NID_STM，转换的非ETCS等级
		if (value == 1)
            return _ItemMask[k].remark + "CTCS-0级";
		else if (value == 2)
            return _ItemMask[k].remark + "CTCS-1级";
		else if (value == 3)
            return _ItemMask[k].remark + "CTCS-2级";
		else if (value == 16)
            return _ItemMask[k].remark + "TVM430";
		else {
			qDebug() << "Expend函数中case74出错";
			return "error";
		}
	}
	case 80:case 84: {//L_ACKLEVELTR，等级转换点外方确认区段长度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 92: {//NID_RBC，RBC编号
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 90: {//Q_RBC，通信命令
		if (value == 0)
            return _ItemMask[k].remark + "终止通信";
		else if (value == 1)
            return _ItemMask[k].remark + "建立通信";
		else {
			qDebug() << "Expend函数中case84出错";
			return "error";
		}
	}
	case 93:case 181: {//NID_RADIO，无线用户电话号码
		QString phone_number = "";
		qint64 temp = 0xf000000000000000L;
		for (int i = 0; i < 16; i++) {
			qint64 number = value & temp;
			if (number >= 0 && number <= 9)
				phone_number += QString::number(number, 10);
			temp >>= 4;
			temp &= 0x0fffffffffffffffL;
		}
        return _ItemMask[k].remark + phone_number;
	}
	case 94:
	case 182: {//Q_SLEEPSESESSION，睡眠设备的通信管理
		if (value == 0)
            return _ItemMask[k].remark + "忽略通信管理信息";
		else if (value == 1)
            return _ItemMask[k].remark + "考虑通信管理信息";
		else {
			qDebug() << "Expend函数中case88出错";
			return "error";
		}
	}
	case 100: {//NID_XUSER，CTCS用户数据标识码
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 107: {//NID_MN，无线网标识号
		QString internet_number = "";
		qint64 temp = 0x0000000000f00000L;
		for (int i = 0; i < 6; i++) {
			qint64 number = value & temp;
			if (number >= 0 && number <= 9)
				internet_number += QString::number(number, 10);
			temp >>= 4;
			temp &= 0x0fffffffffffffffL;
		}
        return _ItemMask[k].remark + internet_number;
	}
	case 124:
	{//Q_TRACKINIT，恢复初始状态（进入特殊区段）的要求
		if (value == 0)
            return _ItemMask[k].remark + "没要求";
		else if (value == 1)
            return _ItemMask[k].remark + "有要求";
		else {
			qDebug() << "Expend函数中case114出错";
			return "error";
		}
	}
	case 125:
	{//D_TRACKINIT，到恢复初始状态开始点的距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 126:case 130:
	{//D_TRACKCOND,到特殊轨道区段的距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 127:case 131:
	{//L_TRACKCOND,特殊轨道区段的长度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 141:
	{//D_TEXTDISPLAY,至应显示文本信息的距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 151:
	{//L_TEXT,文本字符串字节长度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 162:case 169:
	{//D_POSOFF,线路公里标距离参考应答器的偏移量
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 164:case 171:
	{//M_POSITION,线路公里标参考点
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 178:
	{//D_RBCTR,至RBC切换点的距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 206:
	{//D_SIGNAL,到本应答器所描述的轨道区段起始点的距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 209:case 213:
	{//L_SECTION,轨道区段长度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 220:case 222:case 227:
	{//L_TSRarea,临时限速信息有效区段长度/临时限速区段的长度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 221:case 226:
	{//D_TSR,到临时限速区段的距离/到下一个临时限速区段的距离增量
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 224:case 229:
	{//V_TSR,临时限速的限制速度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 236:
	{//D_STARTREVE,到反向运行区间开始点的距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 237:
	{//L_REVERSEAREA,反向运行区间的长度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 244:
	{//D_TURNOUT,到大号码道岔距离
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 245:
	{//V_TURNOUT,道岔侧向列车最大允许通过速度
        return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 128:case 132:
	{//M_TRACKCOND，特殊轨道区段定义
		if (value == 0)
            return _ItemMask[k].remark + "禁停：隧道 初始状态：允许停车（无隧道）";
		else if (value == 1)
            return _ItemMask[k].remark + "禁停：桥梁 初始状态：允许停车（无桥梁）";
		else if (value == 2)
            return _ItemMask[k].remark + "禁停：其他 初始状态：允许停车";
		else if (value == 3)
            return _ItemMask[k].remark + "无电区间：落下受电弓 初始状态：有点区间";
		else if (value == 4)
            return _ItemMask[k].remark + "无线盲区 初始状态：有无线通信";
		else if (value == 5)
            return _ItemMask[k].remark + "全气密区间 初始状态：无气密要求";
		else if (value == 6)
            return _ItemMask[k].remark + "关闭再生制动 初始状态：再生制动打开";
		else if (value == 7)
            return _ItemMask[k].remark + "关闭涡流制动 初始状态：涡流制动打开";
		else if (value == 8)
            return _ItemMask[k].remark + "关闭磁铁制动 初始状态：磁铁制动打开";
		else if (value == 9)
            return _ItemMask[k].remark + "无电区间：关闭主电源 初始状态：有点区间";
		else if (value == 10 || value == 15)
            return _ItemMask[k].remark + "备用";
		else {
			qDebug() << "Expend函数中case118出错";
			return "error";
		}
	}
	case 139:
	{//Q_TEXTCLASS，显示消息的种类
		if (value == 0)
            return _ItemMask[k].remark + "辅助信息";
		else if (value == 1)
            return _ItemMask[k].remark + "重要信息";
		else if (value == 2 || value == 3)
            return _ItemMask[k].remark + "备用";
		else {
			qDebug() << "Expend函数中case128出错";
			return "error";
		}
	}
	case 140:
	{//Q_TEXTDISPLAY，文本信息显示条件组合要求
		if (value == 0)
            return _ItemMask[k].remark + "不组合只要/直到一个条件满足就显示";
		else if (value == 1)
            return _ItemMask[k].remark + "组合只要/直到所有条件满足才显示";
		else {
			qDebug() << "Expend函数中case129出错";
			return "error";
		}
	}
	case 142:case 147:
	{//M_MODETEXTDISPLAY，文本显示对车载设备运行模式的要求
		if (value == 0)
            return _ItemMask[k].remark + "完全监督模式（FS）";
		else if (value == 1)
            return _ItemMask[k].remark + "目视行车模式（OS）";
		else if (value == 2)
            return _ItemMask[k].remark + "人工驾驶模式（SR）";
		else if (value == 3)
            return _ItemMask[k].remark + "调车模式（SH）";
		else if (value == 4)
            return _ItemMask[k].remark + "未装备模式（UN）";
		else if (value == 5)
            return _ItemMask[k].remark + "休眠模式（SL）";
		else if (value == 6)
            return _ItemMask[k].remark + "备用模式（SB）";
		else if (value == 7)
            return _ItemMask[k].remark + "冒进模式（TRIP）";
		else if (value == 8)
            return _ItemMask[k].remark + "冒进后模式（POSTTRIP）";
		else if (value == 9)
            return _ItemMask[k].remark + "系统故障模式（SF）";
		else if (value == 10)
            return _ItemMask[k].remark + "隔离模式（IS）";
		else if (value == 11)
            return _ItemMask[k].remark + "非本务模式（NL）";
		else if (value == 12)
            return _ItemMask[k].remark + "STM欧洲模式（STM（E））";
		else if (value == 13)
            return _ItemMask[k].remark + "STM国家模式（STM（N））";
		else if (value == 14)
            return _ItemMask[k].remark + "退行模式（RV）";
		else if (value == 15)
            return _ItemMask[k].remark + "文本显示不受模式限制";
		else {
			qDebug() << "Expend函数中case131出错";
			return "error";
		}
	}
	case 143:case 148:
	{//M_LEVELTEXTDISPLAY，文本显示对车载设备操作等级的要求
		if (value == 0)
            return _ItemMask[k].remark + "ETCS-0级";
		else if (value == 1)
            return _ItemMask[k].remark + "STM(由NID_STM指定)";
		else if (value == 2)
            return _ItemMask[k].remark + "ETCS-1级";
		else if (value == 3)
            return _ItemMask[k].remark + "ETCS-2级";
		else if (value == 4)
            return _ItemMask[k].remark + "ETCS-3级";
		else if (value == 5)
            return _ItemMask[k].remark + "文本显示不受等级限制";
		else if (value == 6 || value == 7)
            return _ItemMask[k].remark + "备用";
		else {
			qDebug() << "Expend函数中case132出错";
			return "error";
		}
	}
	case 144:case 149:
	{//NID_STM，非ETCS等级
		if (value == 1)
            return _ItemMask[k].remark + "CTCS-0级";
		else if (value == 2)
            return _ItemMask[k].remark + "CTCS-1级";
		else if (value == 3)
            return _ItemMask[k].remark + "CTCS-2级";
		else if (value == 4)
            return _ItemMask[k].remark + "CTCS-3级";
		else if (value == 5)
            return _ItemMask[k].remark + "CTCS-4级";
		else if (value == 16)
            return _ItemMask[k].remark + "TVM430";
		else {
			qDebug() << "Expend函数中case132出错";
			return "error";
		}
	}
	case 145:
	{//L_TEXTDISPLAY,应显示文本的区域长度
		if (value == 32767)
            return _ItemMask[k].remark + "文本显示不受距离限制";
		else
            return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 146:
	{//T_TEXTDISPLAY，文本显示的时间
		if (value == 1023)
            return _ItemMask[k].remark + "文本显示不受时间限制";
		else
            return _ItemMask[k].remark + QString::number(value, 10);
	}
	case 150:
	{//Q_TEXTCONFIRM，取消文本显示确认的要求或反应
		if (value == 0)
            return _ItemMask[k].remark + "无确认需要";
		else if (value == 1)
            return _ItemMask[k].remark + "继续显示知道确认";
		else if (value == 2)
            return _ItemMask[k].remark + "当结束条件满足时还未确认，则实施常用制动";
		else if (value == 3)
            return _ItemMask[k].remark + "3未使用";
		else {
			qDebug() << "Expend函数中case139出错";
			return "error";
		}
	}
	case 152:
	{//X_TEXT(L_TEXT)，文本字节值
		if (value > 127) {
            static char last_char = 0;
            if(last_char==0)
            {
                last_char=value;
                return "";
            }
            else
            {
                QByteArray encodedString;
                encodedString.push_back(last_char);
                encodedString.push_back(value);
                QTextCodec *codec = QTextCodec::codecForName("gb18030");
                last_char=0;
                return(codec->toUnicode(encodedString));//转换为Unicode编码
            }
		}
		char temp = (char)value;
        return _ItemMask[k].remark + temp;
	}
	case 159:case 166:
	{//Q_NEWCOUNTER，参考应答器愚笨应答器组的地区关系
		if (value == 0)
            return _ItemMask[k].remark + "相同";
		else if (value == 1)
            return _ItemMask[k].remark + "不相同";
		else {
			qDebug() << "Expend函数中case147出错";
			return "error";
		}
	}
	case 163:case 170:
	{//Q_MPOSITION，线路公里标计数方向
		if (value == 0)
            return _ItemMask[k].remark + "相反";
		else if (value == 1)
            return _ItemMask[k].remark + "相同";
		else {
			qDebug() << "Expend函数中case151出错";
			return "error";
		}
	}
	case 180:
	{//NID_RBC，RBC标识号
		int temp = (int)value;
		int temp1 = temp & 0x3F00;
		int temp2 = temp & 255;
        return _ItemMask[k].remark + "车站编号：" + QString::number(temp1, 10) + "RBC编号：" + QString::number(temp2, 10);
	}
	case 188:
	{//Q_ASPECT，调车模式
		if (value == 0)
            return _ItemMask[k].remark + "若为调车模式，则停车";
		else if (value == 1)
            return _ItemMask[k].remark + "若为调车模式，则继续行车";
		else {
			qDebug() << "Expend函数中case174出错";
			return "error";
		}
	}
	case 194:
	{//Q_SRSTOP，制定处于目视行车模式的裂成是否必须停车
		if (value == 0)
            return _ItemMask[k].remark + "停车";
		else if (value == 1)
            return _ItemMask[k].remark + "继续运行";
		else {
			qDebug() << "Expend函数中case179出错";
			return "error";
		}
	}
	case 207:case 211:
	{//NID_SIGNAL，信号机或信号点类型
		if (value == 0)
            return _ItemMask[k].remark + "没有信号机";
		else if (value == 1)
            return _ItemMask[k].remark + "进站信号机";
		else if (value == 2)
            return _ItemMask[k].remark + "出站信号机";
		else if (value == 3)
            return _ItemMask[k].remark + "通过信号机";
		else if (value == 4)
            return _ItemMask[k].remark + "进路信号机";
		else if (value == 5)
            return _ItemMask[k].remark + "调车信号机";
		else if (value == 6)
            return _ItemMask[k].remark + "出站口";
		else if (value == 7)
            return _ItemMask[k].remark + "设置应答器组的出站信号机";
		else {
			qDebug() << "Expend函数中case190出错";
			return "error";
		}
	}
	case 208:case 212:
	{//NID_FREQUENCY，轨道区段载频
		if (value == 0)
            return _ItemMask[k].remark + "无载频";
		else if (value == 1)
            return _ItemMask[k].remark + "1700";
		else if (value == 2)
            return _ItemMask[k].remark + "2000";
		else if (value == 3)
            return _ItemMask[k].remark + "2300";
		else if (value == 4)
            return _ItemMask[k].remark + "2600";
		else if (value == 5)
            return _ItemMask[k].remark + "1700-1";
		else if (value == 6)
            return _ItemMask[k].remark + "1700-2";
		else if (value == 7)
            return _ItemMask[k].remark + "2000-1";
		else if (value == 8)
            return _ItemMask[k].remark + "2000-2";
		else if (value == 9)
            return _ItemMask[k].remark + "2300-1";
		else if (value == 10)
            return _ItemMask[k].remark + "2300-2";
		else if (value == 11)
            return _ItemMask[k].remark + "2600-1";
		else if (value == 12)
            return _ItemMask[k].remark + "2600-2";
		else {
			qDebug() << "Expend函数中case191出错";
			return "error";
		}
	}
	case 251:
	{//Q_STOP，立即停车
		if (value == 0)
            return _ItemMask[k].remark + "立即停车";
		else if (value == 1)
            return _ItemMask[k].remark + "备用";
		else {
			qDebug() << "Expend函数中case230出错";
			return "error";
		}
	}
	default:
		return "error";
	}
}
