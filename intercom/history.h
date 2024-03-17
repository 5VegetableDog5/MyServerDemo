#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>


#include "config.h"

namespace Ui {
class History;
}

class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);

    ~History();

signals:
    void aboutToClose();

protected:
    //重写closeEvent方法
    void closeEvent(QCloseEvent *event) override {
        emit aboutToClose();
        QWidget::closeEvent(event);
    }

private:
    Ui::History *ui;

    //初始化控件
    void initControl();

    //初始化历史记录
    void initHistory();

    //查找按钮相关函数
    void handleSearchButtonClicked();

    //“显示全部”按钮
    void handleShowAllButtonClicked();

    //下拉框相关函数
    void handleComboBoxIndexChanged(int currentIndex);


};

#endif // HISTORY_H
