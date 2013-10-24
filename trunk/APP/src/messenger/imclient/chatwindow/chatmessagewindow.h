
#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <QWidget>
#include <QStringList>


#include "ui_chatmessagewindow.h"


#include "./emoticonselecter/emoticonselector.h"
#include "../contactsmanager/contact.h"
#include "contactsmanager/interestgroup.h"

//#include "contactsmanager/contactgroup.h"
#include "../../sharedim/contactgroupbase.h"

#include "../imuser.h"


#include "HHSharedGUI/hscreenshot.h"


class QTextBrowser;
//class MessageEditor;


namespace HEHUI {

class ChatMessageWindow: public QWidget {
    Q_OBJECT

public:
    enum ChatMessageWindowType{CMWT_Unknown, CMWT_Contact, CMWT_InterestGroup, CMWT_TempGroup};
    enum ImageDownloadStatus{ImageDownloading, ImageDownloaded, ImageDownloadingFailed};

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

    bool isDownloadingImage(const QString &imageName);
    void processImageDownloadResult(const QString &imageName, bool downloaded);

    void appendChatMessage(const QString &message, IMUserBase *sender, const QString &datetime);


protected:
    void closeEvent(QCloseEvent * event);


signals:
    void sendMsgButtonClicked(Contact *contact, const QString &message, const QStringList &imageList);
    void sendMsgButtonClicked(InterestGroup *interestGroup, const QString &message, const QStringList &imageList);
    void signalRequestDownloadImage(const QString &contactID, const QString &imageName);


public slots:
    

private slots:

    void emitSendMsgSignal();
    void emitSendMsgSignal2();

    void updateImage(const QString &imageName, ImageDownloadStatus downloadStatus);
    void updateImage2(const QString &imageName);



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

    //void insertEmotion(const QString &emotionName);
    void insertEmoticon(const QString &iconPath, bool isSystemEmoticon);

    void scrollWebFrameToBottom(const QSize &contentsSize);
    void linkClicked(const QUrl & url);


private:
    void initUI();
    void getStyleString();

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

    QString getRichMessageBlock();

private:
    Ui::MessageWindowUi ui;
    QWebFrame *m_mainWebFrame;



    //const QString &contactID;
    IMUser *m_myself;
    Contact *m_contact;
    //    ContactGroup *m_contactGroup;
    InterestGroup *m_interestGroup;
    ChatMessageWindowType m_chatMessageWindowType;

    QString myDisplayName;
    QString myUserID;
    QString imageCachePath;


    //EmotionsListPage *emotionsListPage;

    EmoticonSelector *smileyPopup;


    //QString htmlForMessagesView;

    QTextCharFormat fmt;

    QString m_defaultFontName;
    QString m_defaultFontSize;
    QString m_styleString;

    Screenshot *m_screenshot;

    QStringList m_imagesDownloading;
    QStringList m_imagesUploading;





};

} //namespace HEHUI

#endif // MESSAGEWINDOW_H
