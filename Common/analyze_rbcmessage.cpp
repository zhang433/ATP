#include "analyze_rbcmessage.h"
#include <QDebug>//528，531，547，554，557，564，596，
QMap<quint8,PackageMaskRBC> Analyze_RBCMessage::RBC_MessageMask;
QMap<quint8,PackageMaskRBC> Analyze_RBCMessage::RBC_InfoPackageMask;
int Analyze_RBCMessage::offset_now = 0;
char* Analyze_RBCMessage::msgptr = nullptr;
quint16 Analyze_RBCMessage::RBC_length = 0;
PackCollectorRBC Analyze_RBCMessage::Analyze_Result;//解析后的结果
PackCollectorRBC Analyze_RBCMessage::Error_Result;

Analyze_RBCMessage::Analyze_RBCMessage()
{
}

void Analyze_RBCMessage::Init()
{
    QVector<ItemRBC> errpackage;
    errpackage.push_back(ItemRBC());
    errpackage[0].remark="解析失败";
    Error_Result.push_back(errpackage);
    /***************************开始地到车消息的解析模板填充*****************/
    PackageMaskRBC package;
    package=
    {
        {"消息3：行车许可（MA）",0,"","",0,0},
        {"消息标识号",8,"NID_MESSAGE","",0,0},
        {"消息长度",10,"L_MESSAGE","取决于包含的变量和信息包内容，单位为字节。",0,0},
        {"时间戳",32,"T_TRAIN","取决于消息发送时间",0,0},
        {"是否必须确认",1,"M_ACK","0：接收到该消息后，不需返回确认。\n1：接收到该消息后，必须返回确认。",0,0},
        {"LRBG标识号",24,"NID_LRBG","列车最新位置报告中的NID_LRBG",0,0}
    };//地到车无线消息，公共部分
    RBC_MessageMask[3] = package;//消息3，行车许可+可选包15，5，21，27，3，41，65，68，80，131
    package[0].remark="消息6：确认退出冒进防护模式";
    RBC_MessageMask[6] = package;//消息6，确认退出冒进防护模式
    package.push_back({"被确认消息的时间戳",32,"T_TRAIN","取决于被确认消息的时间戳",0,0});
    package[0].remark="消息8：列车数据确认";
    RBC_MessageMask[8] = package;//消息8，列车数据确认
    package.resize(6);
    package.push_back({"紧急消息标识号",4,"NID_EM","1-15:紧急停车消息的标识号",0,0});//消息15，有条件紧急停车
    package.push_back({"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0});
    package.push_back({"传输数据的有效性方向",2,"Q_DIR","0：反向有效\n1：正向有效",0,0});
    package.push_back({"LRBG至紧急停车点的距离",15,"D_EMERGENCYSTOP","",0,0});
    package[0].remark="消息15：有条件紧急停车";
    RBC_MessageMask[15] = package;//消息15：有条件紧急停车
    package.resize(6);
    package.push_back({"紧急消息标识号",4,"NID_EM","",0,0});
    package[0].remark="消息16：无条件紧急停车";
    RBC_MessageMask[16] = package;//消息16：无条件紧急停车
    package.resize(6);
    package.push_back({"紧急消息标识号",4,"NID_EM","",0,0});
    package[0].remark="消息18：取消紧急停车";
    RBC_MessageMask[18] = package;//消息18：取消紧急停车
    package.resize(6);
    package[0].remark="消息24：通常消息";
    RBC_MessageMask[24] = package;//消息24：通常消息+可选包3，41，42，57，58，65，66，72
    package.push_back({"被确认消息的时间戳",32,"T_TRAIN","取决于被确认消息的时间戳",0,0});
    package[0].remark="消息27：拒绝调车模式";
    RBC_MessageMask[27] = package;//消息27：拒绝调车模式
    package[0].remark="消息28：授权调车模式";
    RBC_MessageMask[28] = package;//消息28：授权调车模式
    package.resize(6);
    package.push_back({"CTCS语言版本",7,"M_VERSION","信息版本号",0,0});
    package[0].remark="消息32：系统版本";
    RBC_MessageMask[32] = package;//消息32：系统版本
    package.resize(6);
    package.push_back({"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0});
    package.push_back({"参考距离",16,"D_REF","LRBG和位置参照点之间的距离",0,0});
    package[0].remark="消息33：位置参照点调整后的MA";
    RBC_MessageMask[33] = package;//消息33：位置参照点调整后的MA+15+5+21+27+41+65+68+80
    package.resize(6);
    package.push_back({"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0});
    package.push_back({"传输数据的有效性方向",2,"Q_DIR","0：反向有效\n1：正向有效\n2：双向有效\n3：备用",0,0});
    package.push_back({"至应显示“前方轨道空闲请求”的距离",15,"D_TAFDISPLAY","LRBG到绝缘节EOA的距离-300-TAF窗口长度：LRBG与绝缘节EOA间的距离超过TAF窗口长度300 米；\n0：LRBG与绝缘节EOA间的距离小于或等于TAF窗口长度300 米 。",0,0});
    package.push_back({"应显示“前方轨道空闲请求”信息的区域长度",15,"L_TAFDISPLAY","TAF窗口长度300：LRBG与绝缘节EOA间的距离超过TAF窗口长度300 米；\nLRBG与绝缘节EOA间的距离：LRBG与EOA绝缘节间的距离小于或等于TAF窗口长度300 米。 ",0,0});
    package[0].remark="消息34：确认前方轨道空闲";
    RBC_MessageMask[34] = package;//消息34：确认前方轨道空闲
    package.resize(6);
    package[0].remark="消息39：通信会话结束确认";
    RBC_MessageMask[39] = package;//消息39：通信会话结束确认
    package[0].remark="消息41：接受列车";
    RBC_MessageMask[41] = package;//消息41：接受列车
    package.push_back({"分配给单应答器组的坐标系",1,"Q_ORIENTATION","0:列车已经反向通过应答器组\n1:列车已经正向通过应答器组",0,0});
    package[0].remark="消息45：分配坐标系";
    RBC_MessageMask[45] = package;//消息45：分配坐标系
    /***************************地到车消息解析模板初始化结束，开始车到地消息的解析模板填充*****************/
    package=
    {{"消息3：行车许可（MA）",0,"","",0,0},
     {"消息标识号",8,"NID_MESSAGE","",0,0},
     {"消息长度",10,"L_MESSAGE","",0,0},
     {"时间戳",32,"T_TRAIN","",0,0},
     {"车载设备的CTCS标识号",24,"NID_ENGINE","",0,0}
    };//车到地无线消息，公共部分
    package[0].remark="消息129：经过确认的列车数据";
    RBC_MessageMask[129] = package;//消息129：经过确认的列车数据+0or1+11
    package[0].remark="消息130：请求调车模式";
    RBC_MessageMask[130] = package;//消息130：请求调车模式+0or1
    package.push_back({"删除线路描述",1,"Q_TRACKDEL","0：车载设备未删除线路数据\n1：车载设备已删除线路数据",0,0});
    package[0].remark="消息132：请求MA";
    RBC_MessageMask[132] = package;//消息132：请求MA+0or1
    package.resize(5);
    package[0].remark="消息136：列车位置报告";
    RBC_MessageMask[136] = package;//消息136：列车位置报告+0or1+4
    package.push_back({"车载设备时钟",32,"T_TRAIN","取决于被确认的消息时间戳",0,0});
    package[0].remark="消息146：确认";
    RBC_MessageMask[146] = package;//消息146：确认
    package.resize(5);
    package.push_back({"紧急消息标识号",4,"NID_EM","被确认的紧急消息的标识号",0,0});
    package.push_back({"紧急停车确认限定词",2,"Q_EMERGENCYSTOP","0：接受有条件紧急停车；\n1：忽略有条件紧急停车；\n2：接受无条件紧急停车；\n3：备用。",0,0});
    package[0].remark="消息147：紧急停车确认";
    RBC_MessageMask[147] = package;//消息147：紧急停车确认+0or1
    package.resize(5);
    package[0].remark="消息149：前方轨道空闲确认";
    RBC_MessageMask[149] = package;//消息149：前方轨道空闲确认+0or1
    package[0].remark="消息150：任务结束";
    RBC_MessageMask[150] = package;//消息150：任务结束+0or1
    package[0].remark="消息154：版本不兼容";
    RBC_MessageMask[154] = package;//消息154：版本不兼容
    package[0].remark="消息155：通信会话开始";
    RBC_MessageMask[155] = package;//消息155：通信会话开始
    package[0].remark="消息156：通信会话结束";
    RBC_MessageMask[156] = package;//消息156：通信会话结束
    package.push_back({"SoM位置报告状态",2,"Q_STATUS","0：位置无效；\n1：位置有效；\n2：位置未知；\n3：备用。",0,0});
    package[0].remark="消息157：SoM 位置报告";
    RBC_MessageMask[157] = package;//消息157：SoM 位置报告+0or1+4
    package.resize(5);
    package[0].remark="消息159：通信会话已建立";
    RBC_MessageMask[159] = package;//消息159：通信会话已建立
    /***************************信息包解析模板*****************/
    package=
    {{"信息包0：位置报告",0,"","",0,0},
     {"信息包标识号",8,"NID_PACKET","",0,0},
     {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
     {"时间戳距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
     {"最近相关应答器组（LRBG）的标识号",24,"NID_LRBG","依照平面设计图确定编号",0,0},
     {"LRBG与列车估计前端（在激活的驾驶室侧）之间的距离",15,"D_LRBG","由列车实际位置与LRBG之间距离计算获得",0,0},
     {"相对于LRBG方向的列车取向（激活驾驶室的位置取向）",2,"Q_DIRLRBG","0：反向\n1：正向\n2：未知\n3：备用",0,0},
     {"限定词指出列车估计前端位于LRBG哪一侧",2,"Q_DLRBG","0：反向\n1：正向\n2：未知\n3：备用",0,0},
     {"过读误差，L_DOUBTOVER是置信间隔低限与D_LRBG估计值的差",15,"L_DOUBTOVER","车载设备测距估计结果",0,0},
     {"欠读误差，L_DOUBTOVER是置信间隔高限与D_LRBG估计值的差",15,"L_DOUBTUNDER","车载设备测距估计结果",0,0},
     {"列车完整性状态的限定词，用于确定列车完整性信息的有效性。",2,"Q_LENGTH","0：无列车完整性信息",0,0},
     {"实际列车速度",7,"V_TRAIN","车载设备测速结果",0,0},
     {"相对于LRBG方向的列车运行方向",2,"Q_DIRTRAIN","0：反向\n1：正向\n2：未知\n3：备用",0,0},
     {"车载设备操作模式",4,"M_MODE","0：完全监督模式\n1：引导模式\n2：目视行车模式\n3：调车模式\n5：休眠模式\n6：待机模式\n7：冒进防护模式\n8：冒进后防护模式\n9：系统故障模式\n10：隔离模式\n13：SN模式\n14：退行模式\n4、11、12、15：备用",0,0},
     {"当前操作等级",3,"M_LEVEL","1：CTCS- 2级\n3：CTCS- 3级\n4：CTCS -4级",0,0},
     {"如果 M_LEVEL = 1",8,"NID_STM","3：CTCS -2级",0,0x11},
    };
    RBC_InfoPackageMask[0] = package;
    package=
    {{"信息包1：基于两个应答器组的位置报告",0,"","",0,0},
     {"信息包标识号",8,"NID_PACKET","",0,0},
     {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
     {"时间戳距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
     {"最近相关应答器组（LRBG）的标识号",24,"NID_LRBG","依照平面设计图确定编号",0,0},
     {"前一个应答器组标识号，在列车向被识别为LRBG的应答器组运行而且中间未改变方向的情况下，探测到的前一个应答器组。地区标识号（NID_C）+ 前一个应答器组的标识号（NID_BG）。RBC通过PRVBG和LRBG识别出列车运行方向（列车从PRVBG向LRBG方向运行）",24,"NID_PRVBG","依照平面设计图确定编号",0,0},
     {"最近相关应答器组与列车估计前端（在激活效驾驶室侧）之间的距离",15,"D_LRBG","由列车实际位置与LRBG之间距离计算获得",0,0},
     {"相对于LRBG方向的列车取向（激活驾驶室的位置取向）",2,"Q_DIRLRBG","0：反向\n1：正向\n2：未知\n3：备用",0,0},
     {"限定词指出估计列车前端位于LRBG哪一侧",2,"Q_DLRBG","0：反向\n1：正向\n2：未知\n3：备用",0,0},
     {"过读误差，L_DOUBTOVER是置信间隔低限与D_LRBG估计值的差",15,"L_DOUBTOVER","车载设备测距估计结果",0,0},
     {"欠读误差，L_DOUBTOVER是置信间隔高限与D_LRBG估计值的差",15,"L_DOUBTUNDER","车载设备测距估计结果",0,0},
     {"列车完整性状态的限定词，用于确定列车完整性信息的有效性。相关安全的列车长度信息由L_TRAININT给出。",2,"Q_LENGTH","0：无列车完整性信息",0,0},
     {"实际列车速度",7,"V_TRAIN","车载设备测速结果",0,0},
     {"相对于LRBG方向的列车运行方向",2,"Q_DIRTRAIN","0：反向\n1：正向\n2：未知\n3：备用",0,0},
     {"车载设备操作模式",4,"M_MODE","0：完全监督模式\n1：引导模式\n2：目视行车模式\n3：调车模式\n5：休眠模式\n6：待机模式\n7：冒进防护模式\n8：冒进后防护模式\n9：系统故障模式\n10：隔离模式\n13：SN模式\n14：退行模式\n4、11、12、15：备用",0,0},
     {"当前操作等级",3,"M_LEVEL","1：CTCS- 2级\n3：CTCS- 3级\n4：CTCS -4级",0,0},
     {"如果 M_LEVEL = 1",8,"NID_STM","3：CTCS -2级",0,0x11},
    };
    RBC_InfoPackageMask[1] = package;
    package=
    {{"信息包3：车载设备电话号码",0,"","",0,0},
     {"信息包标识号",8,"NID_PACKET","",0,0},
     {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
     {"包含电话号码的数量",5,"N_ITER","",0,0},
     {"车载设备电话号码",64,"NID_RADIO (k)","来自工程数据",0,0},
    };
    RBC_InfoPackageMask[3] = package;
    package=
    {{"信息包4：错误报告",0,"","",0,0},
     {"信息包标识号",8,"NID_PACKET","",0,0},
     {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
     {"错误类型标识号",8,"M_ERROR","",0,0},
    };
    RBC_InfoPackageMask[4] = package;
    package=
    {{"信息包11：经过确认的列车数据",0,"","",0,0},
     {"信息包标识号",8,"NID_PACKET","",0,0},
     {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
     {"列车车次号，运行列车的运行车次号",32,"NID_OPERATIONAL","司机输入的列车车次号",0,0},
     {"列车所属的类型，用于静态速度曲线的计算。NC_TRAIN提供了静态速度曲线（SSP）列表，列车必需从列表中选择一条适用每一位代表一个种类。\n列车可以属于不同种类。",15,"NC_TRAIN","0：基本静态素度曲线",0,0},
     {"列车长度，这是列车绝对的真实长度",12,"L_TRAIN","司机输入的列车长度",0,0},
     {"列车最大允许速度，考虑了列车编组中每个车辆的最大速度",7,"V_MAXTRAIN","取决于列车配置参数",0,0},
     {"限界曲线",8,"M_LOADINGGAUGE","0，1",0,0},
     {"轴重",7,"M_AXLELOAD","0～125：轴重/0.5（0≤轴重≤40时）\n126：轴重>40\n其他：备用",0,0},
     {"存在气密系统，说明列车上是否装有气密系统",2,"M_AIRTIGHT","0：未安装",0,0},
     {"包含牵引类型的数量",5,"N_ITER","0或2",0,0},
     {"包含STM类型的数量",5,"N_ITER","1或5",0,0},
     {"可用的STM类型",8,"NID_STM (k)","司机输入的列车车次号",0,0},
    };
    RBC_InfoPackageMask[11] = package;
    package=
    {{"信息包3：配置参数",0,"","",0,0},
     {"信息包标识号",8,"NID_PACKET","",0,0},
     {"验证方向",2,"Q_DIR","2：双向有效",0,0},
     {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
     {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
     {"至配置参数生效地点的距离",15,"D_VALIDNV","至配置参数生效地点的距离",0,0},
     {"配置参数适用的地区数量",5,"N_ITER","按照平面设计图确定，包含RBC管辖范围内所有应答器的NID_C个数",0,0},
     {"配置参数适用的地区编号（高7位=大区编号，低3位=分区编号）",10,"NID_C(k)","按照平面设计图确定，包含RBC管辖范围内所有应答器的NID_C",0,0},
     {"调车模式（允许）速度限制",7,"V_NVSHUNT","0，8：40km/h",0,0},
     {"目视行车模式（允许）速度限制",7,"V_NVSTFF","8：40km/h",0,0},
     {"引导行车模式（允许）速度限制",7,"V_NVONSIGHT","8：40km/h",0,0},
     {"未装备模式（允许）速度限制",7,"V_NVUNFIT","0",0,0},
     {"开口速度的速度限制",7,"V_NVREL","0",0,0},
     {"列车遛逸距离限制，用于遛逸防护和退行防护",15,"D_NVROLL","0或者5",0,0},
     {"当监督制动到目标点时，允许实施常用制动",1,"Q_NVSRBKTRG","1：允许",0,0},
     {"紧急制动缓解限定词",1,"Q_NVEMRRLS","0：仅当列车停车后才能缓解",0,0},
     {"允许司机选择“越行EOA”功能的最大速度限制",7,"V_NVALLOWOVTRP","0",0,0},
     {"“越行EOA”功能激活后的允许速度限制",7,"V_NVSUPOVTRP","8：40km/h",0,0},
     {"抑制列车冒进防护功能的最大走行距离",15,"D_NVOVTRP","32767",0,0},
     {"抑制列车冒进防护功能的最长时间",8,"T_NVOVTRP","0(60s或者255s)",0,0},
     {"在冒进后防护模式下允许退行的最大距离",15,"D_NVPOTRP ","0",0,0},
     {"T_NVCONTACT反应，表示当T_NVCONTACT定时器定时结束时要执行的反应",2,"M_NVCONTACT","1：实施常用制动",0,0},
     {"无新的“安全”消息的最大时间，如果超过T_NVCONTACT秒未从轨旁收到“安全”信息，必须按照M_NVCONTACT触发适当的措施。",8,"T_NVCONTACT","20",0,0},
     {"运行时是否允许输入司机号",1,"M_NVDERUN","0：否",0,0},
     {"目视行车模式的最大走行距离",15,"D_NVSTFF","32767：表示∞",0,0},
     {"司机修改轨旁粘着系数的限定词",1,"Q_NVDRIVER_ADHES","0：不允许",0,0},
    };
    RBC_InfoPackageMask[3] = package;
    package=
    {
        {"信息包5：链接信息",0,"","",0,0},
        {"信息包标识号",8,"NID_PACKET","5",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效",0,0},
        {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"到下一个链接应答器组的距离增量",15,"D_LINK","依照平面设计图确定",0,0},
        {"下一个链接应答器组与前一个的地区关系",1,"Q_NEWCOUNTRY","0：相同\n1：不同",0,0},
        {"地区编号（Q_NEWCOUNTRY=1）",10,"NID_C","依照平面设计图确定",0,0x11},
        {"应答器组编号（下一个应答器）",14,"NID_BG","依照平面设计图确定",0,0},
        {"列车通过被链接应答器组时的运行方向",1,"Q_LINKORIENTATION","0：反向\n1：正向",0,0},
        {"当链接失败时，ATP采取的措施",2,"Q_LINKREACTION","0：冒进\n1：常用制动\n2：无反应",0,0},
        {"链接应答器允许的安装偏差",6,"Q_LOCACC","5",0,0},
        {"包含链接应答器组的数量",5,"N_ITER","依照运行许可长度确定",0,0},
        {"到下一个链接应答器组的距离增量",15,"D_LINK(k)","依照平面设计图确定",0,0},
        {"下一个链接应答器组与前一个的关系\n（0=相同，1=不同）",1,"Q_NEWCOUNTRY(k)","0：相同\n1：不同",0,0},
        {"地区编号（Q_NEWCOUNTRY=1）",10,"NID_C(k)","依照平面设计图确定",0,0x11},
        {"应答器组编号（下一个应答器组）",14,"NID_BG(k)","依照平面设计图确定",0,0},
        {"列车通过被链接应答器组时的运行方向",1,"Q_LINKORIENTATION(k)","0：反向\n1：正向",0,0},
        {"当链接失败时，ATP采取的措施",2,"Q_LINKREACTION(k)","0：冒进\n1：常用制动\n2：无反应",0,0},
        {"链接应答器允许的安装偏差",6,"Q_LOCACC(k)","5或者7",0,0},
    };
    RBC_InfoPackageMask[5] = package;
    package=
    {
        {"信息包15：CTCS-3级的行车许可",0,"","",0,0},
        {"信息包标识号",8,"NID_PACKET","15",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效",0,0},
        {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"限制性许可（LOA）的允许速度",7,"V_LOA","0",0,0},
        {"可以设置为“无超时定时”",10,"T_LOA","1023：无超时定时",0,0},
        {"如果MA中只有一个区段则设置为0",5,"N_ITER","0",0,0},
        {"MA末区段的长度",15,"L_ENDSECTION","依照运行许可长度确定",0,0},
        {"用于指示相关区段是否有区段超时的限定词",1,"Q_SECTIONTIMER","0：无区段定时信息",0,0},
        {"用于指出MA末尾区段是否存在末区段定时信息的限定词",1,"Q_ENDTIMER","0：无末区段定时信息",0,0},
        {"危险点描述的限定词",1,"Q_DANGERPOINT","0：无危险点信息\n1：后面跟随有危险点信息",0,0},
        {"从运行许可终点到危险点的距离",15,"D_DP","依照平面设计图确定",0,0},
        {"危险点的开口速度",7,"V_RELEASEDP","127",0,0},
        {"说明是否有保护区段的限定词，如果存在保护区段或已经指定开口速度时，该变量被置为1",1,"Q_OVERLAP","0：无保护区段信息\n1：有保护区段信息",0,0},
        {"保护区段定时器开始计时的地点至运行许可终点的距离",15,"D_STARTOL","依照平面设计图确定",0,0},
        {"保护区段的有效时间，列车可以使用保护区段的时间，从列车到达D_STARTOL指定位置的时刻开始计时。",10,"T_OL","1023：表示∞",0,0},
        {"运行许可终点至保护区段端点的距离",15,"D_OL","依照平面设计图确定",0,0},
        {"保护区段的开口速度",7,"V_RELEASEOL","0",0,0},
    };
    RBC_InfoPackageMask[15] = package;
    package=
    {
        {"信息包0：位置报告",0,"","",0,0},
        {"信息包标识号",8,"NID_PACKET","21",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效",0,0},
        {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"到本应答器所描述的坡道信息起始点的距离",15,"D_GRADIENT","依照平面设计图确定",0,0},
        {"坡度识别",1,"Q_GDIR","0：下坡或平坡\n1：上坡",0,0},
        {"安全坡度",8,"G_A","依照平面设计图确定",0,0},
        {"包含坡度变化点的数量",5,"N_ITER","依照运行许可长度确定",0,0},
        {"到下一个坡度变化点的距离增量",15,"D_GRADIENT(k)","依照平面设计图确定",0,0},
        {"坡度识别",1,"Q_GDIR(k)","0：下坡或平坡\n1：上坡",0,0},
        {"安全坡度",8,"G_A(k)","依照平面设计图确定",0,0},
    };
    RBC_InfoPackageMask[21] = package;
    package=
    {
        {"信息包27：静态速度曲线",0,"","",0,0},
        {"信息包标识号",8,"NID_PACKET","27",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效",0,0},
        {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"到本应答器所描述的速度信息起始点的距离",15,"D_STATIC","依照平面设计图确定",0,0},
        {"线路最大允许列车运行速度（分辨率=5km/h）\n（127=非数字值，当前线路速度的描述在D_STATIC(k)结束）",7,"V_STATIC","依照 平面设计图确定",0,0},
        {"允许运行速度对车头、车尾的有效性",1,"Q_FRONT","0：尾有效，进入升速区段\n1：头有效，进入降速区段",0,0},
        {"包含列车类型的数量",5,"N_ITER","0",0,0},
        {"包含速度变化点的数量",5,"N_ITER","依照运行许可长度确定",0,0},
        {"到下一个速度变化点的距离增量",15,"D_STATIC(k)","依照平面设计图确定",0,0},
        {"线路最大允许列车运行速度（分辨率=5km/h）",7,"V_STATIC(k)","依照平面设计图确定",0,0},
        {"允许运行速度对车头尾的有效性\n（0=尾有效，如：进入升速区段；1=头有效，如：进入降速区段）",1,"Q_FRONT(k)","0：尾有效 ，进入升速区段\n1：头有效，进入降速区段",0,0},
        {"包含列车类型的数量",5,"N_ITER(k)","0",0,0},
    };
    RBC_InfoPackageMask[27] = package;
    package=
    {
        {"信息包41：等级转换命令",0,"","",0,0},
        {"信息包标识号",8,"NID_PACKET","41",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效",0,0},
        {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"到等级转换点的距离",15,"D_LEVELTR","依照平面设计图确定",0,0},
        {"转换的CTCS等级",3,"M_LEVELTR","1：CTCS-2级\n3：CTCS-3级",0,0},
        {"转换的CTCS等级，当M_LEVELTR=1时使用",8,"NID_STM","3：CTCS-2级",0,0x11},
        {"等级转换确认区段长度",15,"L_ACKLEVELTR","450",0,0},
        {"包含等级转换点的数量",5,"N_ITER","依照平面设计图确定",0,0},
        {"转换的CTCS等级",3,"M_LEVELTR(k)","1：CTCS-2级\n3：CTCS-3级",0,0},
        {"转换的CTCS等级",8,"NID_STM(k)","3：CTCS-2级",0,0x11},
        {"等级转换确认区段长度",15,"L_ACKLEVELTR(k)","450",0,0},
    };
    RBC_InfoPackageMask[41] = package;
    package=
    {
        {"信息包42：通信会话管理",0,"","",0,0},
        {"信息包标识号",8,"NID_PACKET","42",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效\n2：双向有效",0,0},
        {"信息包位数",13,"L_PACKET","113",0,0},
        {"通信会话命令限定词",1,"Q_RBC","0：终止通信会话\n1：建立通信会话",0,0},
        {"RBC地区编号",10,"NID_C","RBC配置信息",0,0},
        {"RBC编号",14,"NID_RBC","RBC配置信息",0,0},
        {"无线用户电话号码",64,"NID_RADIO","RBC配置信息",0,0},
        {"休眠设备的会话管理",1,"Q_SLEEPSESSION","0：忽略会话管理信息\n1：考虑会话管理信息",0,0},
    };
    RBC_InfoPackageMask[42] = package;
    package=
    {
        {"信息包57：行车许可请求参数",0,"","",0,0},
        {"信息包标识号 ",8,"NID_PACKET","57",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效\n2：双向有效",0,0},
        {"信息包位数",13,"L_PACKET","49",0,0},
        {"到达EOA/LOA显示界限前的时间",8,"T_MAR","60",0,0},
        {"任一区段定时器终止前的时间",10,"T_TIMEOUTRQST","1023",0,0},
        {"运行许可两次循环请求之间的时间间隔，当列车请求移动许可时，它将每隔T_CYCRQST秒重复请求直到收到一个新的MA。",8,"T_CYCRQST","60",0,0},
    };
    RBC_InfoPackageMask[57] = package;
    package=
    {
        {"信息包58：位置报告参数",0,"","",0,0},
        {"信息包标识号 ",8,"NID_PACKET","58",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效\n2：双向有效",0,0},
        {"信息包位数",13,"L_PACKET","56",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"列车发送的两个位置报告间的时间间隔，列车在每个T_CYCLOC必须发送其位置",8,"T_CYCLOC","6",0,0},
        {"列车两个位置报告之间的距离，每隔D_CYCLOC米列车必须报告其位置",15,"D_CYCLOC","32767：列车不必周期报告其位置",0,0},
        {"列车必须报告其位置的特殊地点/时刻",3,"M_LOC ","001：每经过一个应答器组 ",0,0},
        {"包含列车位置的数量",5,"N_ITER","0",0,0},
    };
    RBC_InfoPackageMask[58] = package;
    package=
    {
        {"信息包65：设置临时限速",0,"","",0,0},
        {"信息包标识号 ",8,"NID_PACKET","65",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效\n2：双向有效",0,0},
        {"信息包位数",13,"L_PACKET","71",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"临时限速区段的标识号",8,"NID_TSR","127~254：RBC自行确定\n255：不可撤销的TSR",0,0},
        {"到临时限速区段的距离",15,"D_TSR","依照平面设计图及来自TSR服务器的临时限速命令确定",0,0},
        {"临时限速区段的长度",15,"L_TSR","依照来自TSR服务器的临时限速命令确定",0,0},
        {"限速对列车头、车尾的有效性",1,"Q_FRONT","0：尾有效，如：进入升速区段\n1：头有效，如：进入降速区段",0,0},
        {"临时限速的限制速度（分辨率=5km/h）",7,"V_TSR","依照来自TSR服务器的临时限速命令确定",0,0},
    };
    RBC_InfoPackageMask[65] = package;
    package=
    {
        {"信息包66：取消临时限速",0,"","",0,0},
        {"信息包标识号 ",8,"NID_PACKET","66",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效\n2：双向有效",0,0},
        {"信息包位数",13,"L_PACKET","31",0,0},
        {"被取消的临时限速的标识号",8,"NID_TSR","127~254 ",0,0},
    };
    RBC_InfoPackageMask[66] = package;
    package=
    {
        {"信息包68：线路条件",0,"","",0,0},
        {"信息包标识号",8,"NID_PACKET","68",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效",0,0},
        {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"恢复初始状态（进入特殊区段）的要求",1,"Q_TRACKINIT","0：无要求\n1：有要求",0,0},
        {"到恢复初始状态开始点的距离（Q_TRACKINIT=1）",15,"D_TRACKINIT","依照平面设计图确定",0,0x11},
        {"到特殊轨道区段的距离（Q_TRACKINIT=0）",15,"D_TRACKCOND","依照平面设计图确定",0,0x20},
        {"特殊轨道区段的长度（Q_TRACKINIT=0）",15,"L_ TRACKCOND","依照平面设计图确定",0,0x30},
        {"特殊轨道区段定义（Q_TRACKINIT=0）",4,"M_TRACKCOND","0000=禁停：隧道  初始状态：允许停车（无隧道）\n0001=禁停：桥梁  初始状态：允许停车（无桥梁）\n0010=禁停：其他  初始状态：允许停车\n0011=无电区间：落下受电弓 初始状态：有电区间\n0100=无线盲区  初始状态：有无线通信\n1001=无电区间：关闭主电源 初始状态：有电区间\n依照平面设计图确定",0,0x40},
        {"包含特殊轨道区段的数量",5,"N_ITER","依照平面设计图确定",0,0},
        {"到特殊轨道区段的距离（Q_TRACKINIT=0）",15,"D_TRACKCOND(k)","依照平面设计图确定",0,0x60},
        {"特殊轨道区段的长度（Q_TRACKINIT=0）",15,"L_TRACKCOND(k)","依照平面设计图确定",0,0x70},
        {"特殊轨道区段定义（Q_TRACKINIT=0）",4,"M_TRACKCOND(k)","0000=禁停：隧道  初始状态：允许停车（无隧道）\n0001=禁停：桥 梁  初始状态：允许停车（无桥梁）\n0010=禁停：其他  初始状态：允许停车\n0011=无电区间：落下受电弓 初始状态：有电区间\n0100=无线盲区  初始状态：有无线通信\n1001=无电区间：关闭主电源 初始状态：有电区间",0,0x80},
    };
    RBC_InfoPackageMask[68] = package;
    package=
    {
        {"信息包72：发送纯文本消息的信息包",0,"","",0,0},
        {"信息包标识号 ",8,"NID_PACKET","72",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效\n2：双向有效",0,0},
        {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"显示消息的种类，Q_TEXTCLASS指定包含在相同信息包内的文本信息的种类（纯文本或固定文本信息）",2,"Q_TEXTCLASS","0：辅助 信息\n1：重要信息",0,0},
        {"文本信息条件组合限定词，Q_TEXTDISPLAY 定义文本信息开始/结束条件是否要组合",1,"Q_TEXTDISPLAY","1：组合，只要/直到所有条件满足才显示",0,0},
        {"至应显示文本信息的距离",15,"D_TEXTDISPLAY","32767：文本显示不应受距离的限制",0,0},
        {"文本显示车载设备运行模式，当进入/只要处于指定模式时，就显示该文本信息。",4,"M_MODETEXTDISPLAY","15：文本显示不应受模式的限制",0,0},
        {"车载设备操作等级的文本显示，当车载设备进入/只要处于指定等级就显示该文本。",3,"M_LEVELTEXTDISPLAY","5：文本显示不应受等级限制3：CTCS-3级",0,0},
        {"应显示文本的区域长度",15,"L_TEXTDISPLAY","32767：文本显示不受距离限制",0,0},
        {"文本被显示的时间",10,"T_TEXTDISPLAY","1023：文本显示不受时间限制",0,0},
        {"结束条件",4,"M_MODETEXTDISPLAY","15：文本显示不应受模式的限制",0,0},
        {"结束条件",3,"M_LEVELTEXTDISPLAY","5：文本显示不应受等级限制",0,0},
        {"文本确认的要求/反应",2,"Q_TEXTCONFIRM","0：无确认需要\n1：继续显示直到确认\n2：当结束条件满足时还未确认，则实施常用 制动",0,0},
        {"文本串长度，L_TEXT定义文本串长度( L_TEXT * X_TEXT) ",8,"L_TEXT","依照实际传输文本确定，每个中文字符占2个字符长度",0,0},
        {"文本字符串元素，用于传输文本消息的文本字符串。",8,"X_TEXT(L_TEXT)","依照实际传输文本确定，中文字符采用GB18030字库",0,0},
    };
    RBC_InfoPackageMask[72] = package;
    package=
    {
        {"信息包80：引导/调车区段的设置信息",0,"","",0,0},
        {"信息包标识号 ",8,"NID_PACKET","80",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效",0,0},
        {"信息包位数",13,"L_PACKET","取决于信息包内容",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"至MA内下一个模式变换点的距离增量",15,"D_MAMODE","依照平面设计图及进路状态确定",0,0},
        {"部分MA的模式请求",2,"M_MAMODE","0：引导CO模式",0,0},
        {"请求模式有关的速度",7,"V_MAMODE","8：40km/h",0,0},
        {"所请求模式的区域长度",15,"L_MAMODE","依照平面设计图及进路状态确定",0,0},
        {"在请求模式起点后方的确认区域的长度",15,"L_ACKMAMODE","400",0,0},
        {"后续模式曲线数目",5,"N_ITER","依照行车许可及进路状态确定",0,0},
        {"至MA内下一个模式变换点的距离增量",15,"D_MAMODE(k)","依照平面设计图及进路状态确定",0,0},
        {"部分MA的模式请求",2,"M_MAMODE(k)","0：引导CO模式",0,0},
        {"请求模式有关的速度",7,"V_MAMODE(k)","8：40km/h",0,0},
        {"所请求模式的区域长度",15,"L_MAMODE(k)","依照平面设计图及进路状态确定",0,0},
        {"在请求模式起点后方的确认区域的长度",15,"L_ACKMAMODE(k)","400",0,0},
    };
    RBC_InfoPackageMask[80] = package;
    package=
    {
        {"信息包131：RBC切换命令",0,"","",0,0},
        {"信息包标识号 ",8,"NID_PACKET","131",0,0},
        {"验证方向",2,"Q_DIR","0：反向有效\n1：正向有效",0,0},
        {"信息包位数",13,"L_PACKET","129",0,0},
        {"距离/长度的分辨率",2,"Q_SCALE","1：分辨率为1m",0,0},
        {"至RBC切换点的距离",15,"D_RBCTR","LRBG到RBC交权执行应答器的距离",0,0},
        {"国家或其他地区的标识号",10,"NID_C","",0,0},
        {"接收RBC的标识号",14,"NID_RBC","",0,0},
        {"接收RBC无线用户号码",64,"NID_RADIO","",0,0},
        {"休眠设备的会话管理",1,"Q_SLEEPSESSION","0",0,0},
    };
    RBC_InfoPackageMask[131] = package;
}

bool Analyze_RBCMessage::GetMsgID(QByteArray &input_data, QVector<ItemRBC>* ret)
{
	qDebug() << QString::fromLocal8Bit("RBC data ID is:") << static_cast<int>(static_cast<unsigned char>(input_data[0]));
	RBC_length = static_cast<unsigned char>(input_data[2] << 8) + static_cast<unsigned char>(input_data[1]);
	qDebug() << QString::fromLocal8Bit("RBC data length is:") << RBC_length << "bytes";
	if (RBC_length != input_data.size() - 3)
	{
		qDebug() << QString::fromLocal8Bit("The length of message doesn't match,length is:") << input_data.size() - 3 << QString::fromLocal8Bit("bytes,length should be(from record):") << RBC_length << "bytes";
		Error_Result[0][0].remark = "The length of message doesn't match";
		QString binary_data;
		for (auto iter = input_data.begin(); iter != input_data.end(); iter++)
		{
			binary_data.append(QString("%1").arg((unsigned char)*iter, 2, 16, QChar('0')) + " ");
		}
		qDebug() << "binary data is:" << binary_data;
		*ret = Error_Result[0];
		return false;
	}
	msgptr = input_data.data() + 3;
	offset_now = 0;
	quint8 RBC_Message_ID = static_cast<quint8>(FetchValue(8));
	qDebug() << "RBC MessageID is:" << RBC_Message_ID;
	if (RBC_MessageMask.find(RBC_Message_ID) == RBC_MessageMask.end())
	{
		qDebug() << "RBC message ID not defined:" << RBC_Message_ID;
		Error_Result[0][0].remark = "RBC message ID not defined:" + QString::number(RBC_Message_ID);
		QString binary_data;
		for (auto iter = input_data.begin(); iter != input_data.end(); iter++)
		{
			binary_data.append(QString("%1").arg((unsigned char)*iter, 2, 16, QChar('0')) + " ");
		}
		qDebug() << "binary data is:" << binary_data;
		*ret = Error_Result[0];
		return false;
	}
	offset_now = 0;
	QVector<ItemRBC> temp_VecItem;
	ItemRBC temp_Item;
	for (auto iter = RBC_MessageMask[RBC_Message_ID].begin(); iter != RBC_MessageMask[RBC_Message_ID].end(); iter++)
	{
		temp_Item.name = iter->name;
		temp_Item.remark = iter->remark;
		temp_Item.value = FetchValue(iter->length);
		temp_Item.valueinfo = iter->valueinfo;
		temp_VecItem.push_back(temp_Item);
	}
	*ret = temp_VecItem;
	return true;
}

PackCollectorRBC &Analyze_RBCMessage::Resolve(QByteArray &input_data)
{
    Analyze_Result.clear();
	ItemRBC temp_Item;
	QVector<ItemRBC> temp_VecItem;
	if (!GetMsgID(input_data, &temp_VecItem))//消息头获取错误立即返回错误结果
		return Error_Result;
    Analyze_Result.push_back(temp_VecItem);
    while(RBC_length*8-offset_now>16)//解析信息包，一个信息包的长度必然大于16bit
    {
        temp_VecItem.clear();
        qDebug()<<QString::fromLocal8Bit("Length of the rest data is:")<<RBC_length*8-offset_now<<"bit";
        quint8 infopackageindex = static_cast<quint8>(FetchValue(8));
        if(RBC_InfoPackageMask.find(infopackageindex)==RBC_InfoPackageMask.end())
        {
            qDebug()<<QString::fromLocal8Bit("RBC InfoPackage ID not defined:")<<infopackageindex;
			QString binary_data;
			for (auto iter = input_data.begin(); iter != input_data.end(); iter++)
			{
				binary_data.append(QString("%1").arg((unsigned char)*iter, 2, 16, QChar('0'))+" ");
			}
			qDebug() << "binary data is:" << binary_data;
            Error_Result[0][0].remark="RBC InfoPackage ID not defined:"+QString::number(infopackageindex);
            return Error_Result;
        }
        offset_now-=8;
        for(auto iter = RBC_InfoPackageMask[infopackageindex].begin();iter!=RBC_InfoPackageMask[infopackageindex].end();iter++)
        {
            if(iter->mark>0)//表示此项的存在性由其他项所决定，判断存在条件
            {
                quint8 AssociatedItemDistance = (static_cast<quint8>(iter->mark>>4));
                quint8 AimValue = (static_cast<quint8>(iter->mark&0x0f));
                if((iter - AssociatedItemDistance)->value!=AimValue)//不符合条件，此项不存在，继续解析下一项
                    continue;
            }
			iter->value = FetchValue(iter->length);
            temp_Item.value = iter->value;
            temp_Item.name = iter->name;
            temp_Item.remark = iter->remark;
            temp_Item.valueinfo = iter->valueinfo;
            temp_VecItem.push_back(temp_Item);
            //qDebug()<<iter->name<<iter->value<<offset_now<<RBC_length*8;
            if(iter->value==-1)
            {
                Error_Result[0][0].remark="FetchValue Function over bound";
				QString binary_data;
				for (auto iter = input_data.begin(); iter != input_data.end(); iter++)
				{
					binary_data.append(QString("%1").arg((unsigned char)*iter, 2, 16, QChar('0')) + " ");
				}
				qDebug() << "binary data is:" << binary_data;
                return Error_Result;
            }
            if(temp_Item.name.startsWith("N_ITER"))//循环起始项
            {
                int N_ITER = static_cast<int>(temp_Item.value);
                auto iter_end = iter;
                iter_end++;
                if(iter_end==RBC_InfoPackageMask[infopackageindex].end())//本N_ITER的下一个迭代器已经是尾后迭代器了
                {
                    continue;
                }
                while(iter_end->name.endsWith("(k)"))//表示这一项是循环项
                {
                    iter_end++;
                    if(iter_end==RBC_InfoPackageMask[infopackageindex].end())
                        break;
                }//至此，temp_iter指向N_ITER循环体的后一项
                for(int i=0;i<N_ITER;++i)
                {
                    for(auto iter_begin = iter+1;iter_begin<iter_end;++iter_begin)//循环解析
                    {
                        if(iter_begin->mark>0)//表示此项的存在性由其他项所决定，判断存在条件
                        {
                            quint8 AssociatedItemDistance = (static_cast<quint8>(iter_begin->mark>>8));
                            quint8 AimValue = (static_cast<quint8>(iter_begin->mark&0x0f));
                            if((iter_begin - AssociatedItemDistance)->value!=AimValue)//不符合条件，此项不存在，继续解析下一项
                                continue;
                        }
                        //进行解析
						iter_begin->value = FetchValue(iter_begin->length);
						temp_Item.value = iter_begin->value;
                        temp_Item.name = iter_begin->name;
                        temp_Item.name.replace("(k)","("+QString::number(i)+")");
                        temp_Item.remark = iter_begin->remark;
                        temp_Item.valueinfo = iter_begin->valueinfo;
                        temp_VecItem.push_back(temp_Item);
                        if(iter_begin->value==-1)
                        {
                            Error_Result[0][0].remark="FetchValue Function over bound";
							QString binary_data;
							for (auto iter = input_data.begin(); iter != input_data.end(); iter++)
							{
								binary_data.append(QString("%1").arg((unsigned char)*iter, 2, 16, QChar('0')) + " ");
							}
							qDebug() << "binary data is:" << binary_data;
                            return Error_Result;
                        }
                    }
                }
                iter = iter_end-1;
            }
        }
        Analyze_Result.push_back(temp_VecItem);
    };
    return Analyze_Result;
}

/*************************************************************************
*  函数名称：FetchValue
*  功能说明：从offset_now的位置提取length位长度的值
*  参数说明：length：需要提取的位长度
*  函数返回：提取的数值
*  修改时间：2017-6-21 已测试通过
*  备    注：
*************************************************************************/
qint64 Analyze_RBCMessage::FetchValue(unsigned int length)
{
    if (static_cast<unsigned int>(offset_now) + length > RBC_length*8)
    {
        qDebug()<<QString::fromLocal8Bit("FetchValue Function over bound");
        return -1;//表示取值越界
    }
    else if (length == 0)
    {
        return 0;
    }
    qint64 ret = 0;
    while (true)
    {
        unsigned char temp_value = static_cast<unsigned char>(msgptr[offset_now / 8]);
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


