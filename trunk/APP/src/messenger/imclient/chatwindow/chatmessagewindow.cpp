#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QColorDialog>
#include <QColor>
#include <QDebug>

#include <QtWebKit>
#include <QtWebKitWidgets>

#include "./chatmessagewindow.h"
#include "../contactsmanager/contact.h"
#include "../../sharedim/constants_global_shared.h"



//#include "imageresource.h"
//#include "../../shared/gui/imageresourcebase.h"
#include "HHSharedCore/hutilities.h"

namespace HEHUI {

const QString URLScheme_Contact = "contact"; //contact://contact_id
const QString URLScheme_Image= "image"; //image://image_name

const QString ImagePath_Normal = "qrc:/chatmessagewindow/image.png";
const QString ImagePath_Downloading = "qrc:/chatmessagewindow/imagedownloading.gif";
const QString ImagePath_DownloadingFailed = "qrc::/chatmessagewindow/imagedownloadingfailed.png";



ChatMessageWindow::ChatMessageWindow(QWidget *parent)
    :QWidget(parent), m_contact(0), m_interestGroup(0) {

    initUI();

    m_chatMessageWindowType = CMWT_Unknown;

}

ChatMessageWindow::ChatMessageWindow(Contact *contact, QWidget *parent)
    :QWidget(parent), m_contact(contact), m_interestGroup(0) {
    
    initUI();

    m_chatMessageWindowType = CMWT_Contact;

}

ChatMessageWindow::ChatMessageWindow(InterestGroup *interestGroup, QWidget *parent)
    :QWidget(parent), m_contact(0), m_interestGroup(interestGroup) {

    initUI();

    m_chatMessageWindowType = CMWT_InterestGroup;

}

ChatMessageWindow::~ChatMessageWindow() {

}

void ChatMessageWindow::closeEvent(QCloseEvent * event){

    if(!ui.textEdit->toPlainText().trimmed().isEmpty()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Question"), tr("Message Not Sent! <br> Close the window?"),
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply == QMessageBox::No) {
            event->ignore();
            return;
        }
    }

    event->accept();

}

void ChatMessageWindow::initUI(){

    ui.setupUi(this);

    static QString htmlForMessagesView = "";
    if(htmlForMessagesView.isEmpty()){
        QFile file(":/text/resources/text/sample.html");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            htmlForMessagesView = file.readAll();
        }else{
            htmlForMessagesView = "<html xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-16\" /><title>Chat</title></head><body><div></div></body></html>";
        }
    }
    ui.webView->setHtml(htmlForMessagesView);
    //connect(ui.webView, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));

    QWebPage *page = ui.webView->page();
    page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(page, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));


    m_mainWebFrame = page->mainFrame();
    connect(m_mainWebFrame, SIGNAL(contentsSizeChanged(const QSize &)), this, SLOT(scrollWebFrameToBottom(const QSize &)));



    ui.mainSplitter->setStretchFactor(1, 1);


    //    QString displayName = contact->getNickName();
    //    QString contactID = contact->getUserID();

    //    if (displayName.isEmpty()) {
    //        displayName = contactID;
    //    }else if(contactID != displayName){
    //        displayName = contact->getNickName() + "("  + contact->getUserID() + ")";
    //    }
    //    setWindowTitle(displayName);
    //TODO:
    //setWindowIcon(ImageResource::createMixedIcon((QString(RESOURCE_PATH)+QString(APP_ICON_PATH)), contact->getOnlineState()));





    m_myself = IMUser::instance();
    myDisplayName = QString(m_myself->getNickName());
    myUserID = m_myself->getUserID();

    imageCachePath = Settings::instance()->getPictureCacheDir();


    connect(ui.fontStyleToolButton, SIGNAL(clicked()), this, SLOT(showFontFrame()));
    connect(ui.emotionToolButton, SIGNAL(clicked()), this, SLOT(showEmotions()));
    connect(ui.insertImageToolButton, SIGNAL(clicked()), this, SLOT(insertImage()));
    connect(ui.sendFileToolButton, SIGNAL(clicked()), this, SLOT(sendFile()));
    connect(ui.toolButtonScreenshot, SIGNAL(clicked()), this, SLOT(takeScreenshot()));

    //Font Type
    connect(ui.fontComboBox, SIGNAL(activated(const QString &)), this, SLOT(textFamily(const QString &)));
    //Font Size
    QFontDatabase db;
    foreach(int size, db.standardSizes()){
        ui.fontSizeComboBox->addItem(QString::number(size));
    }

    connect(ui.fontSizeComboBox, SIGNAL(activated(const QString &)), this, SLOT(textSize(const QString &)));
    ui.fontSizeComboBox->setCurrentIndex(ui.fontSizeComboBox->findText(
                                             QString::number(QApplication::font() .pointSize())));

    //Font Style
    connect(ui.fontBoldToolButton, SIGNAL(clicked()), this, SLOT(textBold()));
    connect(ui.fontItalicToolButton, SIGNAL(clicked()), this, SLOT(textItalic()));
    connect(ui.fontUnderlineToolButton, SIGNAL(clicked()), this, SLOT(textUnderline()));

    //Font Color
    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    connect(ui.textColorToolButton, SIGNAL(clicked()), this, SLOT(textColor()));
    ui.textColorToolButton->setIcon(pix);

    ui.textEdit->setFocus();
    ui.fontStyleToolButton->setChecked(false);
    ui.fontFrame->setVisible(false);


    connect(ui.sendMsgPushButton, SIGNAL(clicked()), this, SLOT(emitSendMsgSignal()));


    smileyPopup = new EmoticonSelector();
    connect(smileyPopup, SIGNAL(signalEmoticonSelected(const QString&, bool)), this, SLOT(insertEmoticon(const QString&, bool)));


    m_defaultFontName = ui.fontComboBox->currentText();
    m_defaultFontSize = "12";
    ui.fontSizeComboBox->setCurrentIndex(ui.fontSizeComboBox->findText(m_defaultFontSize));
    textSize(m_defaultFontSize);

    m_styleString = "";

    m_screenshot = 0;


}

QTextEdit *ChatMessageWindow::messageEditor() {
    return ui.textEdit;
}

QWebView *ChatMessageWindow::messageBrowser() {
    return ui.webView;
}

void ChatMessageWindow::setContact(Contact *c){
    m_contact = c;
    m_chatMessageWindowType = CMWT_Contact;

    if(m_contact){
        ui.sendMsgPushButton->setEnabled(true);
    }else{
        ui.sendMsgPushButton->setEnabled(false);
    }
}

Contact * ChatMessageWindow::getContact(){
    return m_contact;
}

void ChatMessageWindow::setInterestGroup(InterestGroup *group){
    m_interestGroup = group;
    m_chatMessageWindowType = CMWT_InterestGroup;

    if(m_interestGroup){
        ui.sendMsgPushButton->setEnabled(true);
    }else{
        ui.sendMsgPushButton->setEnabled(false);
    }
}

InterestGroup * ChatMessageWindow::getInterestGroup(){
    return m_interestGroup;
}

bool ChatMessageWindow::isDownloadingImage(const QString &imageName){
    return m_imagesDownloading.contains(imageName);
}

void ChatMessageWindow::appendChatMessage(const QString &message, IMUserBase *sender, const QString &datetime){
    qDebug()<<"--ChatMessageWindow::appendChatMessage(...)";


    QString userID;
    QString nickName;


    if(!sender){
        userID = m_contact->getUserID();
        nickName = m_contact->getNickName();
    }else{
        userID = sender->getUserID();
        nickName = sender->getNickName();
    }

    if(userID != myUserID){
        Contact *contact = qobject_cast<Contact *>(sender);
        if(contact){
            QString remarkName = contact->getRemarkName();
            if(!remarkName.trimmed().isEmpty()){
                nickName = remarkName;
            }
        }
    }



    qDebug()<<"----Message Received From Contact! contactID:"<<userID<<" Time:"<<datetime<<" Msg:"<<message;;




    //        QString msg = "<dl>";
    //        msg += QString("<dt>%1  %2  </dt>").arg(contact->getNickName()).arg(datetime);
    //        msg += "<dd >";
    //        msg += QString(" %1 ").arg(message);
    //        msg += "</dd >";
    //        msg += "</dl >";



    //URL: contact://contactid
    QString msg = QString("<span><a title=\"%1\" href=\"%2://%1\">%3</a> %4</span>").arg(userID).arg(URLScheme_Contact).arg(nickName).arg(datetime);
    
    //Find Div tag
    //QRegExp regExp("<div.+>.*</div>");
    //regExp.setCaseSensitivity(Qt::CaseInsensitive);
    //regExp.setMinimal(true);
    //int pos = regExp.indexIn(message);
    //if(pos != -1){
    //    msg += regExp.cap(0);
    //}
    msg += message;

    QWebElement doc = m_mainWebFrame->documentElement();
    QWebElement div = doc.findFirst("div");
    div.appendInside(msg);


    //Modify images path
    QWebElement messageElement = div.lastChild();
    QWebElementCollection elements = messageElement.findAll("img");
    foreach (QWebElement element, elements){
        QString imageSRC = element.attribute("src");
        if(!imageSRC.trimmed().startsWith("qrc:/", Qt::CaseInsensitive)){
            QString localCacheImage = imageCachePath + "/" + imageSRC;
            if(QFile::exists(localCacheImage)){
                element.setAttribute("src", "file://" + localCacheImage);
            }else{
                //Need to download the image
                element.setAttribute("id", imageSRC);
                if(m_myself->isAutoDownloadImageFromContact()){
                    //Download image
                    element.setAttribute("src", ImagePath_Downloading);
                    m_imagesDownloading.append(imageSRC);
                    emit signalRequestDownloadImage(userID, imageSRC);
                }else{
                    element.setAttribute("src", ImagePath_Normal);
                }
            }
            //URL: image://imagename
            QString url = QString("%1://%2@%3").arg(URLScheme_Image).arg(userID).arg(imageSRC);
            element.setOuterXml(QString("<a href=\"%1\">%2</a>").arg(url).arg(element.toOuterXml()));

        }
    }

    qDebug();
    qDebug()<<"------msg:"<<msg;
    qDebug();
    qDebug()<<"------messageElement: "<<messageElement.toOuterXml();

    qDebug();

    //qDebug()<<"HTML:\n"<<m_mainWebFrame->toHtml();



}

void ChatMessageWindow::updateImage2(const QString &imageName){

    QWebElement doc = m_mainWebFrame->documentElement();
    QWebElementCollection elements = doc.findAll("img");
    foreach (QWebElement element, elements){
        //if(element.attribute("src") == imageName){
        if(element.attribute("id") == imageName){
            element.setAttribute("src", "file://" + imageCachePath +"/"+imageName);
        }
    }

}

void ChatMessageWindow::processImageDownloadResult(const QString &imageName, bool downloaded){
    if(downloaded){
        updateImage(imageName, ImageDownloaded);
    }else{
        updateImage(imageName, ImageDownloadingFailed);
    }


    m_imagesDownloading.removeAll(imageName);

}

void ChatMessageWindow::updateImage(const QString &imageName, ImageDownloadStatus downloadStatus){

    QWebElement doc = m_mainWebFrame->documentElement();
    QWebElementCollection elements = doc.findAll("img");
    foreach (QWebElement element, elements){

        //QString imageSRC = element.attribute("src");
        QString imageID = element.attribute("id").trimmed();
        if(imageID != imageName){continue;}


        //if(imageSRC.trimmed().startsWith("qrc:/", Qt::CaseInsensitive)){
            switch (downloadStatus) {
            case ImageDownloading:
            {
                element.setAttribute("src", ImagePath_Downloading);
            }
                break;
            case ImageDownloaded:
            {
                element.setAttribute("src", "file://" + imageCachePath +"/"+imageName);
                element.removeAttribute("id");
            }
                break;

            case ImageDownloadingFailed:
            {
                element.setAttribute("src", ImagePath_DownloadingFailed);
            }
                break;
            default:
                break;
            }

        //}



    }

}

QString ChatMessageWindow::getRichMessageBlock() const{

    QString out;
    QXmlStreamReader reader(ui.textEdit->toHtml());
    QXmlStreamWriter writer(&out);
    writer.setAutoFormatting(false);
    writer.setAutoFormattingIndent(0);

    while (!reader.atEnd()) {
        switch (reader.readNext()) {
        case QXmlStreamReader::StartElement:
        {
            //elementCount++;
            const QStringRef name = reader.name();
            if( name == QStringLiteral("meta") || name == QStringLiteral("style")){
                reader.readElementText(); // Skip away all nested elements and characters.
            }else{

                if(name == QStringLiteral("html")
                        ||name == QStringLiteral("head")
                        ||name == QStringLiteral("body")
                        ){
                    continue;
                }

                QXmlStreamAttributes attributes = reader.attributes();
                if(name == QStringLiteral("img")){
                    QString fileName;
                    QString imgSRC = attributes.value("src").toString();
                    if(imgSRC.startsWith(":/")){
                        fileName = "qrc" + imgSRC;
                    }else{
                        QFileInfo imgInfo(imgSRC);
                        fileName = imgInfo.fileName();
                    }

                    attributes.clear();
                    attributes.append("src", fileName);
                }else{
                    attributes.clear();
                }

                writer.writeStartElement(name.toString());
                if (!attributes.isEmpty())
                    writer.writeAttributes(attributes);
            }
        }
            break;
        case QXmlStreamReader::Characters:
        {
            QStringRef text = reader.text();
            writer.writeCharacters(text.toString());
        }
            break;
        case QXmlStreamReader::EndElement:
            writer.writeEndElement();
            break;
        default:
            break;
        }

    }


    return out.simplified();

}

void ChatMessageWindow::emitSendMsgSignal() {
    //TODO:是否要转换成UTF8

    QString message = ui.textEdit->toPlainText();

    if(message.isEmpty()){
        ui.textEdit->setFocus();
        return;
    }

    if(message.size() > MAX_MESSAGE_SIZE){
        QMessageBox::critical(this, tr("Error"), tr("The message is too long!"));
        qCritical()<<"ERROR! Message too long! HTML:"<<ui.textEdit->toHtml().size()<<" Plain:"<<ui.textEdit->toPlainText().size();
        ui.textEdit->setFocus();
        return;
    }

    QString richMessage = getRichMessageBlock();
    richMessage.replace("<span>", "");
    richMessage.replace("</span>", "");


    if(m_styleString.trimmed().isEmpty()){
        richMessage = QString("<div>") + richMessage;
    }else{
        richMessage = QString("<div style=\"%1\">").arg(m_styleString) + richMessage;
    }
    richMessage += QString("</div>");

    appendChatMessage(richMessage, m_myself, QDateTime::currentDateTime().toString("hh:mm:ss"));

    ui.textEdit->clear();
    ui.textEdit->setFocus();
    ui.textEdit->setCurrentCharFormat(fmt);

    QStringList imageList;

    switch(m_chatMessageWindowType){
    case CMWT_Contact:
        emit sendMsgButtonClicked(m_contact, richMessage, imageList);
        break;
    case CMWT_InterestGroup:
        emit sendMsgButtonClicked(m_interestGroup, richMessage, imageList);
        break;
    case CMWT_TempGroup:
        break;
    default:

        break;

    }



//    qWarning()<<"HTML:\n"<<m_mainWebFrame->toHtml();



}


void ChatMessageWindow::emitSendMsgSignal2() {
    //TODO:是否要转换成UTF8
    
    QString message = ui.textEdit->toPlainText();

    if(message.isEmpty()){
        ui.textEdit->setFocus();
        return;
    }

    if(message.size() > MAX_MESSAGE_SIZE){
        QMessageBox::critical(this, tr("Error"), tr("The message is too long!"));
        qCritical()<<"ERROR! Message too long! HTML:"<<ui.textEdit->toHtml().size()<<" Plain:"<<ui.textEdit->toPlainText().size();
        ui.textEdit->setFocus();
        return;
    }

    QString richMessage = "";



    QString htmlOfTextEdit = ui.textEdit->toHtml();

    QRegExp regExp("<p.*</p>");
    regExp.setCaseSensitivity(Qt::CaseInsensitive);
    regExp.setMinimal(true);
    int pos = 0;
    QStringList list;
    while ((pos = regExp.indexIn(htmlOfTextEdit, pos)) != -1) {
        list << regExp.cap(0);
        pos += regExp.matchedLength();
    }

    if(m_styleString.trimmed().isEmpty()){
        richMessage += QString("<div>");
    }else{
        richMessage += QString("<div style = \"%1\">").arg(m_styleString);
    }
    foreach (QString str, list) {
        str.replace(QRegExp("\\bstyle=\".*;\"", Qt::CaseInsensitive), "");
        str.replace(QRegExp("src=\":/", Qt::CaseInsensitive), "src=\"qrc:/");
        richMessage += str;
    }
    richMessage += QString("</div>");


    QString temp = QString("<div><span>%1 %2</span>").arg(myDisplayName).arg(QDateTime::currentDateTime().toString("hh:mm:ss")) + richMessage +"</div>";
    QWebElement doc = m_mainWebFrame->documentElement();
    QWebElement div = doc.findFirst("div");
    div.appendInside(temp);


    ui.textEdit->clear();
    ui.textEdit->setFocus();
    ui.textEdit->setCurrentCharFormat(fmt);


    qDebug()<<"-------richMessage1:"<<richMessage;


    regExp.setPattern("<img.*>");
    pos = 0;
    QStringList imgList;
    while ((pos = regExp.indexIn(richMessage, pos)) != -1) {
        imgList << regExp.cap(0);
        pos += regExp.matchedLength();
    }
    QStringList imageList;
    regExp.setPattern("\"(([a-zA-Z]:/\\w)|(/\\w)).+\\.((png)|(gif)|(jpg)|(jpeg))\"");
    foreach (QString imgsrcTag, imgList) {
        QString iconPath;
        pos = 0;
        if((pos = regExp.indexIn(imgsrcTag, pos)) != -1){
            iconPath = regExp.cap(0);
            iconPath = iconPath.replace("\"", "");
            richMessage.replace(iconPath, QFileInfo(iconPath).fileName());
            imageList.append(iconPath);
        }

    }

    qDebug()<<"-------richMessage2:"<<richMessage;


    switch(m_chatMessageWindowType){
    case CMWT_Contact:
        emit sendMsgButtonClicked(m_contact, richMessage, imageList);
        break;
    case CMWT_InterestGroup:
        emit sendMsgButtonClicked(m_interestGroup, richMessage, imageList);
        break;
    case CMWT_TempGroup:
        break;
    default:

        break;

    }




    //        QWebElement e(div);
    //        //e.setInnerXml(richMessage);
    //        e.replace(richMessage);
    //        //e.appendInside(richMessage);
    //        qWarning()<<"---e:"<<e.toInnerXml();

    //        QWebElementCollection elements = e.findAll("img");
    //        foreach (QWebElement element, elements){
    //            element.setAttribute("style", "background-color: #f0f090");
    //            qWarning()<<element.toInnerXml();
    //        }



    //qWarning()<<"HTML:\n"<<m_mainWebFrame->toHtml();



}

void ChatMessageWindow::scrollWebFrameToBottom(const QSize & contentsSize){
    m_mainWebFrame->setScrollBarValue(Qt::Vertical, contentsSize.height());
}


void ChatMessageWindow::showFontFrame() {
    if (ui.fontStyleToolButton->isChecked()) {
        ui.fontFrame->setVisible(true);
    } else {
        ui.fontFrame->setVisible(false);
    }
}

void ChatMessageWindow::linkClicked(const QUrl & url){
    QString scheme = url.scheme();

    if(scheme == URLScheme_Image){
        QString userID = url.userInfo();
        QString imageName = url.host();


        if(m_imagesDownloading.contains(imageName)){
            return;
        }



        QString localCacheImage = imageCachePath + "/" + imageName;
        if(QFile::exists(localCacheImage)){
            //TODO:Show image
        }else{
            //Download image
            updateImage(imageName, ImageDownloading);
            m_imagesDownloading.append(imageName);
            emit signalRequestDownloadImage(userID, imageName);
        }


    }else if(scheme == URLScheme_Contact){
        //TODO

    }



    qDebug()<<"URL scheme:"<<scheme<<" host:"<<url.host()<<" userInfo:"<<url.userInfo();


}

void ChatMessageWindow::showEmotions() {
    QPoint p = ui.emotionToolButton->mapToGlobal(QPoint(0, 0));

    smileyPopup->move(p.x(), p.y() + ui.emotionToolButton->height());
    smileyPopup->setVisible(true);

    //emotionsListPage->move(p.x(), p.y()+ui.emotionToolButton->height());
    //emotionsListPage->setVisible(true);
}

void ChatMessageWindow::insertEmoticon( const QString &iconPath, bool isSystemEmoticon/*int index const QString &emotionName*/) {
    //QString emoticonsPath = Settings::instance()->getEmoticonsPath();
    //QString emoticon = "<img src=\"" + emoticonsPath + "/" + QString::number(index) + ".gif\"> ";

    smileyPopup->hide();
    
    QString emoticon ;
    
    if(isSystemEmoticon){
        emoticon = QString("<img src=\"" + iconPath + "/\"> ");
    }else{
        QFileInfo fileInfo(iconPath);
        QString fileName = myUserID + "_" + fileInfo.absoluteDir().dirName() + "_" + fileInfo.fileName();
        QString filePath = imageCachePath + "/" + fileName;
        
        if(!QFile::exists(filePath)){
            bool ok = QFile::copy(iconPath, filePath);
            if(!ok){
                filePath = iconPath;
            }
        }
        //emoticon = QString("<img alt=\"%1\" src=\"%2\" /> ").arg(fileName).arg(filePath);
        emoticon = QString("<img src=\"%1\"/> ").arg(filePath);
        
        
    }
    



    

    ui.textEdit->insertHtml(emoticon);


    qWarning()<<"----emoticon:"<<emoticon;

    //QString emotionPath = QDir::currentPath() + QString("/smiley");

    //if (!emotionName.isEmpty())
    //ui.textEdit->insertHtml(QLatin1String("<img src=\"") + emotionPath + "/" + emotionName + ".gif"
    //				+ QLatin1String("\"/>"));

    //emotionsListPage->setVisible(false);


}

void ChatMessageWindow::insertImage() {
    QFileDialog::Options options;
    //options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Chose Your Image"), QDir::homePath(), tr(
                                                        "Images (*.png *.jpg *.gif *.bmp);;All Files (*)"),
                                                    &selectedFilter, options);

    if (fileName.isEmpty()){
        return;
    }


    QString md5String = Utilities::getFileMD5EncodedWithHex(fileName);
    if(md5String.trimmed().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Can not get image's MD5 hash!"));
        return;
    }

    QFileInfo fileInfo(fileName);
    QString format = fileInfo.suffix();

    QString newFileName = imageCachePath + QString("/%1.%2").arg(md5String).arg(format);
    QFile::remove(newFileName);
    if(!QFile::copy(fileName, newFileName)){
        QMessageBox::critical(this, tr("Error"), tr("Can not read image!"));
        return;
    }


    ui.textEdit->insertHtml(QLatin1String("<img src=\"") + newFileName + QLatin1String("\"/>"));


}

void ChatMessageWindow::sendFile() {
    QFileDialog::Options options;
    //options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QStringList files = QFileDialog::getOpenFileNames(this, tr(
                                                          "QFileDialog::getOpenFileNames()"), QDir::homePath(), tr(
                                                          "All Files (*);;Text Files (*.txt)"), &selectedFilter, options);
    if (files.count()) {
        //openFilesPath = files[0];
        //openFileNamesLabel->setText(QString("[%1]").arg(files.join(", ")));
    }
}

void ChatMessageWindow::takeScreenshot(){
    if(!m_screenshot){
        m_screenshot = new Screenshot(this);
        connect(m_screenshot, SIGNAL(imageSelected(const QImage &)), this, SLOT(screenshotDone(const QImage &)));
    }

    m_screenshot->show();

}

void ChatMessageWindow::screenshotDone(const QImage &image){
    qDebug()<<"--ChatMessageWindow::screenshotDone(...)";

    QImage img = image;

    m_screenshot->close();
    delete m_screenshot;
    m_screenshot = 0;

    if(img.isNull()){
        qWarning()<<"Invalid Image!";
        return ;
    }

    QString format = "png";
    QString fileName = imageCachePath + QString("/Screenshot%1.%2").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")).arg(format);
    if(!img.save(fileName, format.toLatin1())){
        QMessageBox::critical(this, tr("Screenshot"), tr("Can not save image!"));
        return;
    }


    QString md5String = Utilities::getFileMD5EncodedWithHex(fileName);
    if(md5String.trimmed().isEmpty()){
        QMessageBox::critical(this, tr("Screenshot"), tr("Can not get image's MD5 hash!"));
        return;
    }
    QString newFileName = imageCachePath + QString("/%1.%2").arg(md5String).arg(format);
    QFile::remove(newFileName);
    QFile::rename(fileName, newFileName);


    ui.textEdit->insertHtml(QLatin1String("<img src=\"") + newFileName + QLatin1String("\"/>"));


}

void ChatMessageWindow::textBold() {
    //QTextCharFormat fmt;
    fmt.setFontWeight(ui.fontBoldToolButton->isChecked() ? QFont::Bold
                                                         : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void ChatMessageWindow::textUnderline() {
    //QTextCharFormat fmt;
    fmt.setFontUnderline(ui.fontUnderlineToolButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void ChatMessageWindow::textItalic() {
    //QTextCharFormat fmt;
    fmt.setFontItalic(ui.fontItalicToolButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void ChatMessageWindow::textFamily(const QString &f) {
    //QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
    // QMessageBox::warning(this, tr("Here!"), f );
}

void ChatMessageWindow::textSize(const QString &p) {
    //QTextCharFormat fmt;
    fmt.setFontPointSize(p.toFloat());
    mergeFormatOnWordOrSelection(fmt);
}

void ChatMessageWindow::textColor() {
    QColor col = QColorDialog::getColor(ui.textEdit->textColor(), this);
    if (!col.isValid())
        return;
    //QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);


}

void ChatMessageWindow::currentCharFormatChanged(const QTextCharFormat &format) {
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void ChatMessageWindow::cursorPositionChanged() {
    //alignmentChanged(textEdit->alignment());
}

void ChatMessageWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format) {

    //	QTextCursor cursor = ui.textEdit->textCursor();
    //	if (!cursor.hasSelection())
    //		cursor.select(QTextCursor::WordUnderCursor);
    //	cursor.mergeCharFormat(format);
    //	ui.textEdit->mergeCurrentCharFormat(format);
    
    
    QTextCursor cursor = ui.textEdit->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(format);

    
    ui.textEdit->setCurrentCharFormat(fmt);
    ui.textEdit->mergeCurrentCharFormat(fmt);
    
    getStyleString();


}

void ChatMessageWindow::fontChanged(const QFont &f) {
    ui.fontComboBox->setCurrentIndex(ui.fontComboBox->findText(
                                         QFontInfo(f).family()));
    ui.fontSizeComboBox->setCurrentIndex(ui.fontSizeComboBox->findText(
                                             QString::number(f.pointSize())));
    ui.fontBoldToolButton->setChecked(f.bold());
    ui.fontItalicToolButton->setChecked(f.italic());
    ui.fontUnderlineToolButton->setChecked(f.underline());
}

void ChatMessageWindow::colorChanged(const QColor &c) {
    QPixmap pix(16, 16);
    pix.fill(c);
    ui.textColorToolButton->setIcon(pix);
}

void ChatMessageWindow::getStyleString(){

    QString styleInfo = "";
    
    if(ui.fontComboBox->currentText() != m_defaultFontName){
        styleInfo = QString("font-family:'%1';").arg(ui.fontComboBox->currentText());
    }
    
    if(ui.fontSizeComboBox->currentText() != m_defaultFontSize){
        styleInfo = QString("font-size:%1pt;").arg(ui.fontSizeComboBox->currentText());
    }
    
    if(ui.fontBoldToolButton->isChecked()){
        styleInfo += QString("font-weight:bold;");
    }
    
    if(ui.fontItalicToolButton->isChecked()){
        styleInfo += QString("font-style:italic;");
    }
    
    if(ui.fontUnderlineToolButton->isChecked()){
        styleInfo += QString("text-decoration:underline;");
    }
    
    if(fmt.foreground().color().name() != "#000000"){
        styleInfo += QString("color:%1;").arg(fmt.foreground().color().name());
    }
    
    //styleInfo += "position: relative;";
    
    m_styleString = styleInfo;
    


}  



















} //namespace HEHUI
