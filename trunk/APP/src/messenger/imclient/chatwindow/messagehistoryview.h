#ifndef MESSAGEHISTORYVIEW_H
#define MESSAGEHISTORYVIEW_H

#include <QWidget>

#include "ui_messagehistoryview.h"



namespace HEHUI {

class MessageView;

class MessageHistoryView : public QWidget
{
    Q_OBJECT

public:
    explicit MessageHistoryView(QWidget *parent = 0);
    ~MessageHistoryView();

signals:
    void signalRequestHistoryMessage(QString startTime, QString endTime, const QString &content);

private slots:
    void on_toolButtonShowSearchFrame_clicked(bool checked);
    void on_toolButtonSearch_clicked();

    void on_toolButtonFirstPage_clicked();
    void on_toolButtonPreviousPage_clicked();
    void on_spinBoxPageIndex_valueChanged(int i);
    void on_toolButtonNextPage_clicked();
    void on_toolButtonLastPage_clicked();


private:
    Ui::MessageHistoryViewUI ui;

    MessageView *m_messageView;

    QString m_startTime;
    QString m_endTime;


};

} //namespace HEHUI

#endif // MESSAGEHISTORYVIEW_H
