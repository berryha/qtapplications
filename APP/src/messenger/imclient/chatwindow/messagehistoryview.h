#ifndef MESSAGEHISTORYVIEW_H
#define MESSAGEHISTORYVIEW_H

#include <QWidget>

namespace Ui {
class MessageHistoryViewUI;
}


namespace HEHUI {

class MessageView;

class MessageHistoryView : public QWidget
{
    Q_OBJECT

public:
    explicit MessageHistoryView(QWidget *parent = 0);
    ~MessageHistoryView();

private:
    Ui::MessageHistoryViewUI *ui;

    MessageView *m_messageView;


};

} //namespace HEHUI

#endif // MESSAGEHISTORYVIEW_H
