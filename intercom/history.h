#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>

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
};

#endif // HISTORY_H
