#include "messagehistoryview.h"

#include "messageview.h"

namespace HEHUI {


MessageHistoryView::MessageHistoryView(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);


    m_messageView = new MessageView(this);
    m_messageView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    //connect(m_messageView, SIGNAL(signalRequestDownloadImage(const QString &, const QString &)), this, SLOT(requestDownloadImage(const QString &, const QString &)));


    ui.verticalLayoutMain->insertWidget(0, m_messageView);

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

//    ui.frameSearch->hide();

}

MessageHistoryView::~MessageHistoryView()
{

    delete m_messageView;

}

void MessageHistoryView::on_toolButtonShowSearchFrame_clicked(bool checked){
    ui.frameSearch->setVisible(checked);

}

void MessageHistoryView::on_toolButtonSearch_clicked(){

}

void MessageHistoryView::on_toolButtonFirstPage_clicked(){

}

void MessageHistoryView::on_toolButtonPreviousPage_clicked(){

}

void MessageHistoryView::on_spinBoxPageIndex_valueChanged(int i){

}

void MessageHistoryView::on_toolButtonNextPage_clicked(){

}

void MessageHistoryView::on_toolButtonLastPage_clicked(){

}






} //namespace HEHUI
