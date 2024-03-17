#include "history.h"
#include "ui_history.h"

extern QSqlDatabase db;

History::History(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::History)
{
    ui->setupUi(this);
    initControl();

#if DEBUG
    printfLog("正在加载历史记录....");
#endif
    initHistory();

}


/*
*函数名：initControl
*功能：初始化控件
*/
void History::initControl(){

    //初始化Table View控件
    // 显示水平分割线
    ui->tableView_history->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView_history->horizontalHeader()->setStyleSheet("QHeaderView::section { border-bottom: 1px solid black; }");
    // 设置列宽自适应内容
    ui->tableView_history->resizeColumnsToContents();

    //初始化查找按钮
    connect(ui->pushButton_search,&QPushButton::clicked,this,&History::handleSearchButtonClicked);

    //初始化“显示全部”按钮
    connect(ui->pushButton_showAll,&QPushButton::clicked,this,&History::handleShowAllButtonClicked);

    //初始化QStackedWidget
    //默认为第一页
    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->comboBox,&QComboBox::currentIndexChanged,this,&History::handleComboBoxIndexChanged);
}


/*
*函数名：initHistory
*功能：初始化数据库信息至TableView中
*/
void History::initHistory(){
    // 创建 QSqlQueryModel
    QSqlQueryModel *model = new QSqlQueryModel;

    // 执行 SQL 查询语句
    QString sqlQuery = "SELECT * FROM history";
    model->setQuery(sqlQuery);

    // 检查是否发生错误
    if (model->lastError().isValid()) {
        qDebug() << "Error: Failed to execute query:" << model->lastError().text();
#if DEBUG
        printfLog("数据库读取失败！");
#endif
        return ;
    }


    ui->tableView_history->setModel(model);

#if DEBUG
    printfLog("历史记录加载成功！");
#endif

}

void History::handleSearchButtonClicked(){
    QString data;

    // 创建一个QSqlQueryModel对象
    QSqlQueryModel *model = new QSqlQueryModel;
    QSqlQuery qry;
    QString query;

    // 根据不同的查询方式准备不同的查询语句
    if(ui->comboBox->currentIndex() == 0){
        data = ui->lineEdit_search->text();
        if(data.isEmpty()){
            // 如果字符串为空，弹出警告窗口
            QMessageBox::critical(this, "错误", "字符串为空！");
            return ;
        }

        query = "SELECT * FROM history WHERE dialer = :search_value OR answer = :search_value";
        qry.prepare(query);
        qry.bindValue(":search_value", data);
    }else if(ui->comboBox->currentIndex() == 1){

        if(ui->lineEdit_small->text().toInt() == -1 ||
            (ui->lineEdit_small->text().toInt() > ui->lineEdit_big->text().toInt() && ui->lineEdit_big->text().toInt() != -1 ) ||
            (ui->lineEdit_small->text().isEmpty() || ui->lineEdit_big->text().isEmpty()) ||
            (ui->lineEdit_big->text().toInt()<=0 && ui->lineEdit_big->text().toInt() != -1) ||
            ui->lineEdit_small->text().toInt()<0){
            QMessageBox::critical(this, "查询错误", "请检查输入的数值！");
            return ;
        }

        if(ui->lineEdit_big->text().toInt() != -1){
            query = "SELECT * FROM history WHERE callDuration BETWEEN :min_duration AND :max_duration";
            qry.prepare(query);
            qry.bindValue(":min_duration", ui->lineEdit_small->text());
            qry.bindValue(":max_duration", ui->lineEdit_big->text());
            //qDebug()<<ui->lineEdit_small->text()<<"  "<<ui->lineEdit_big->text()<< "\n" <<query;
        }else{
            query = "SELECT * FROM history WHERE callDuration >= :min_duration";
            qry.prepare(query);
            qry.bindValue(":min_duration", ui->lineEdit_small->text());
        }


    }else{
        //获取时间
        QDateTime dateTime_small = ui->dateTimeEdit_small->dateTime();
        QDateTime dateTime_big = ui->dateTimeEdit_big->dateTime();

        if(dateTime_big < dateTime_small){
            QMessageBox::critical(this, "查询错误", "请检查输入的数值！");
            return ;
        }

        query = "SELECT * FROM history WHERE beginTime BETWEEN :start_time AND :end_time";
        qry.prepare(query);
        qry.bindValue(":start_time", dateTime_small);
        qry.bindValue(":end_time", dateTime_big);


    }


    // 执行查询
    if(qry.exec()) {
        // 将查询结果设置为模型的查询结果
        model->setQuery(qry);

        // 将模型设置为TableView的模型
        ui->tableView_history->setModel(model);
    } else {
#if DEBUG
        printfLog("查询失败！");
#endif
    }

}

void History::handleShowAllButtonClicked(){
    initHistory();
}

void History::handleComboBoxIndexChanged(int currentIndex){
    ui->stackedWidget->setCurrentIndex(currentIndex);
}


History::~History()
{
    delete ui;
}
