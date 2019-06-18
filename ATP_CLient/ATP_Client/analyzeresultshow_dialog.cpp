#include "analyzeresultshow_dialog.h"
#include "ui_rbcshow_dialog.h"
#include "analyze_rbcmessage.h"

AnalyzeResultShow_DIALOG::AnalyzeResultShow_DIALOG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RBCShow_DIALOG)
{
    ui->setupUi(this);
}

AnalyzeResultShow_DIALOG::~AnalyzeResultShow_DIALOG()
{
    delete ui;
}

void AnalyzeResultShow_DIALOG::ChangeAnalyzeResult(BaliseBitMessage *BBM)
{
    Analyze_BaliseMessage::Resolve(*BBM);
    this->setWindowTitle(Analyze_BaliseMessage::GetBaliseID(*BBM));
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(3);
    int rowcount=0;
    for(int i=0;i<Analyze_BaliseMessage::Analyze_Result.size();i++)
        rowcount+=Analyze_BaliseMessage::Analyze_Result[i].size();
    ui->tableWidget->setRowCount(rowcount+1);
    //列名
    ui->tableWidget->setItem(0,0,new QTableWidgetItem("项名"));
    ui->tableWidget->setItem(0,1,new QTableWidgetItem("值"));
    ui->tableWidget->setItem(0,2,new QTableWidgetItem("解析结果"));
    rowcount=0;
    QTableWidgetItem* delete_item;
    //数据内容填充
    for(int i=0;i<Analyze_BaliseMessage::Analyze_Result.size();i++)
    {
        for(int j=0;j<Analyze_BaliseMessage::Analyze_Result[i].size();j++)
        {
            rowcount++;
            delete_item = ui->tableWidget->item(rowcount,0);
            if(delete_item!=nullptr)
                delete delete_item;
            ui->tableWidget->setItem(rowcount,0,new QTableWidgetItem(Analyze_BaliseMessage::Analyze_Result[i][j].name));
            if(j==0)
                continue;
            delete_item = ui->tableWidget->item(rowcount,1);
            if(delete_item!=nullptr)
                delete delete_item;
            ui->tableWidget->setItem(rowcount,1,new QTableWidgetItem(QString::number(Analyze_BaliseMessage::Analyze_Result[i][j].value,10)+tr(" (0x")+QString::number(Analyze_BaliseMessage::Analyze_Result[i][j].value,16)+tr(")")));
            delete_item = ui->tableWidget->item(rowcount,2);
            if(delete_item!=nullptr)
                delete delete_item;
            ui->tableWidget->setItem(rowcount,2,new QTableWidgetItem(Analyze_BaliseMessage::Analyze_Result[i][j].remark));
        }
    }
}

void AnalyzeResultShow_DIALOG::ChangeAnalyzeResult(QByteArray *RBC)
{
    PackCollectorRBC PCR = Analyze_RBCMessage::Resolve(*RBC);
    this->setWindowTitle(PCR[0][0].remark);
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(4);
    int rowcount=0;
    for(int i=0;i<PCR.size();i++)
        rowcount+=PCR[i].size();
    ui->tableWidget->setRowCount(rowcount+1);
    //列名
    ui->tableWidget->setItem(0,0,new QTableWidgetItem("项名"));
    ui->tableWidget->setItem(0,1,new QTableWidgetItem("值"));
    ui->tableWidget->setItem(0,2,new QTableWidgetItem("项的说明"));
    ui->tableWidget->setItem(0,3,new QTableWidgetItem("解析结果"));
    rowcount=0;
    QTableWidgetItem* delete_item;
    QColor c = Qt::yellow;
    //数据内容填充
    for(int i=0;i<PCR.size();i++)
    {
        for(int j=0;j<PCR[i].size();j++)
        {
            if(i!=0)
                c=Qt::lightGray;
            rowcount++;
            delete_item = ui->tableWidget->item(rowcount,0);
            if(delete_item!=nullptr)
                delete delete_item;
            if(j==0)
            {
                ui->tableWidget->setItem(rowcount,0,new QTableWidgetItem(PCR[i][j].remark));
                ui->tableWidget->item(rowcount,0)->setBackgroundColor(c);
                continue;
            }
            else
            {
                ui->tableWidget->setItem(rowcount,0,new QTableWidgetItem(PCR[i][j].name));
                ui->tableWidget->item(rowcount,0)->setBackgroundColor(c);
                delete_item = ui->tableWidget->item(rowcount,1);
                if(delete_item!=nullptr)
                    delete delete_item;
                ui->tableWidget->setItem(rowcount,1,new QTableWidgetItem(QString::number(PCR[i][j].value,10)+tr(" (0x")+QString::number(PCR[i][j].value,16)+tr(")")));
                ui->tableWidget->item(rowcount,1)->setBackgroundColor(c);
                delete_item = ui->tableWidget->item(rowcount,2);
                if(delete_item!=nullptr)
                    delete delete_item;
                ui->tableWidget->setItem(rowcount,2,new QTableWidgetItem(PCR[i][j].remark));
                ui->tableWidget->item(rowcount,2)->setBackgroundColor(c);
                delete_item = ui->tableWidget->item(rowcount,3);
                if(delete_item!=nullptr)
                    delete delete_item;
                ui->tableWidget->setItem(rowcount,3,new QTableWidgetItem(PCR[i][j].valueinfo));
                ui->tableWidget->item(rowcount,3)->setBackgroundColor(c);
            }
        }
    }
    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableWidget->verticalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    //ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
    //ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableWidget->verticalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}
