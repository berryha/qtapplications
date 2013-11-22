#include "messagehistoryview.h"
#include "ui_messagehistoryview.h"

#include "messageview.h"

namespace HEHUI {


MessageHistoryView::MessageHistoryView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageHistoryViewUI)
{
    ui->setupUi(this);


    m_messageView = new MessageView(this);
    m_messageView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    //connect(m_messageView, SIGNAL(signalRequestDownloadImage(const QString &, const QString &)), this, SLOT(requestDownloadImage(const QString &, const QString &)));


    ui->gridLayout->addWidget(m_messageView, 0, 0, 1, 1);

    static QString htmlForMessagesView = "";
    if(htmlForMessagesView.isEmpty()){
        QFile file(":/text/resources/text/sample.html");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            htmlForMessagesView = file.readAll();
        }else{
            htmlForMessagesView = "<html xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-16\" /><title>Chat</title></head><body><div></div></body></html>";
        }
    }
    m_messageView->setHtml(htmlForMessagesView);


}

MessageHistoryView::~MessageHistoryView()
{
    delete ui;



}






} //namespace HEHUI
