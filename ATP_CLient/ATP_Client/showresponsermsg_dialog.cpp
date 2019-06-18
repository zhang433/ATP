#include "showresponsermsg_dialog.h"
#include "ui_showresponsermsg_dialog.h"
#include "analyze_resmessage.h"
#include <QDebug>


ShowResponserMsg_DIALOG::ShowResponserMsg_DIALOG(BaliseBitMessage &input_data,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowResponserMsg_DIALOG)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    //this->setCentralWidget(ui->ResponserMSGResult_TREE);
//    msgptr=&input_data;//更新本对象的数据指针
//    AnalyzeResponserMsg();//对输入数据进行解析
    Analyze_BaliseMessage::Resolve(input_data);

	this->setWindowTitle(Analyze_BaliseMessage::GetBaliseID(input_data));
    ui->ResponserMSGResult_TREE->setColumnCount(3);//设置treewidget控件为三列表示
    ui->ResponserMSGResult_TREE->setHeaderLabels(QStringList()<<"包名/项名"<<"十进制表示（十六进制表示值）"<<"备注");//设置每一列的标题
    /********************************将解析内容反映到treewidget控件上**********************/
    QList<QTreeWidgetItem*> temp_treeitem;
    for(int i=0;i<Analyze_BaliseMessage::Analyze_Result.size();i++)
    {
        ui->ResponserMSGResult_TREE->addTopLevelItem(new QTreeWidgetItem(QStringList(Analyze_BaliseMessage::Analyze_Result[i][0].name)));
        for(int j=1;j<Analyze_BaliseMessage::Analyze_Result[i].size();j++)
        {
            temp_treeitem.push_back(new QTreeWidgetItem(QStringList()<<Analyze_BaliseMessage::Analyze_Result[i][j].name<<QString::number(Analyze_BaliseMessage::Analyze_Result[i][j].value,10)+tr(" (0x")+QString::number(Analyze_BaliseMessage::Analyze_Result[i][j].value,16)+tr(")")<<Analyze_BaliseMessage::Analyze_Result[i][j].remark));
        }
        ui->ResponserMSGResult_TREE->topLevelItem(i)->addChildren(temp_treeitem);
        //设置鼠标悬停的提示，以便信息显示不全的时候可以通过人工鼠标悬停的方式查看完整信息
        for(int j=1;j<Analyze_BaliseMessage::Analyze_Result[i].size();j++)
        {
            ui->ResponserMSGResult_TREE->topLevelItem(i)->child(j-1)->setToolTip(2,Analyze_BaliseMessage::Analyze_Result[i][j].remark);
        }
        temp_treeitem.clear();
    }
    ui->ResponserMSGResult_TREE->topLevelItem(0)->setExpanded(true);

    //this->setWindowTitle(ARM.msg_identifier);//设置窗口标题
}


ShowResponserMsg_DIALOG::~ShowResponserMsg_DIALOG()
{
    delete ui;
}




