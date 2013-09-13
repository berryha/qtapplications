
#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <QWidget>
#include "../ui/ui_chatmessagewindow.h"
#include "./emoticonselecter/emoticonselector.h"
#include "../contactsmanager/contact.h"
#include "contactsmanager/interestgroup.h"

//#include "contactsmanager/contactgroup.h"
#include "../../sharedim/contactgroupbase.h"


#include "HHSharedGUI/hscreenshot.h"

class QTextBrowser;
//class MessageEditor;


namespace HEHUI {

class ChatMessageWindow: public QWidget {
    Q_OBJECT

public:
    enum ChatMessageWindowType{CMWT_Unknown, CMWT_Contact, CMWT_InterestGroup, CMWT_TempGroup};

    ChatMessageWindow(QWidget *parent = 0);
    ChatMessageWindow(Contact *m_contact, QWidget *parent = 0);
//    ChatMessageWindow(ContactGroup *m_contactGroup, QWidget *parent = 0);
    ChatMessageWindow(InterestGroup *m_interestGroup, QWidget *parent = 0);

    ~ChatMessageWindow();

    QTextEdit *messageEditor();
    QWebView *messageBrowser();

    void setContact(Contact *c);
    Contact * getContact();

//    void setContactGroup(ContactGroup *group);
//    ContactGroup * getContactGroup();

    void setInterestGroup(InterestGroup *group);
    InterestGroup * getInterestGroup();


    void appendMessageReceivedFromContact(const QString &message, Contact *contact = 0, const QString &datetime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz"));


protected:
    void closeEvent(QCloseEvent * event);

private:
    void initUI();
    void getStyleString();


signals:
//    void sendMsgButtonClicked(const QString &contactID, const QString &message, const QStringList &imageList);
    void sendMsgButtonClicked(Contact *contact, const QString &message, const QStringList &imageList);
    void sendMsgButtonClicked(InterestGroup *interestGroup, const QString &message, const QStringList &imageList);



public slots:
    void updateImage(const QString &imageName);
    
private slots:
    //void sendMsg(const QStringList &msgList);
    //void sendMsg();

    void showFontFrame();
    void showEmotions();
    void insertImage();
    void sendFile();
    void takeScreenshot();
    void screenshotDone(const QImage &image);

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textColor();

    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    void emitSendMsgSignal();
    //void insertEmotion(const QString &emotionName);
    void insertEmoticon(const QString &iconPath, bool isSystemEmoticon);

    void scrollWebFrameToBottom(const QSize &contentsSize);

private:
    Ui::MessageWindowUi ui;
    QWebFrame *m_mainWebFrame;

    //const QString &contactID;
    Contact *m_contact;
//    ContactGroup *m_contactGroup;
    InterestGroup *m_interestGroup;
    ChatMessageWindowType m_chatMessageWindowType;

    QString myDisplayName;
    QString myUserID;
    QString imageCachePath;

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

    void appendMessage(const QString &message);



    //EmotionsListPage *emotionsListPage;

    EmoticonSelector *smileyPopup;


    //QString htmlForMessagesView;

    QTextCharFormat fmt;

    QString m_defaultFontName;
    QString m_defaultFontSize;
    QString m_styleString;

    Screenshot *m_screenshot;


};

} //namespace HEHUI

#endif // MESSAGEWINDOW_H
