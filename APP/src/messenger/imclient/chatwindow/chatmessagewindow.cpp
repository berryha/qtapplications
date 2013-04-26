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

#include "../imuser.h"
//#include "imageresource.h"
//#include "../../shared/gui/imageresourcebase.h"


namespace HEHUI {

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

void ChatMessageWindow::initUI(){

    ui.setupUi(this);

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


    myDisplayName = QString(IMUser::instance()->getNickName());
    myUserID = IMUser::instance()->getUserID();

    imageCachePath = Settings::instance()->getPictureCacheDir();

    //connect(ui.sendMsgPushButton, SIGNAL(clicked()), this, SLOT(sendMsg()));


    //setMinimumSize(sizeHint());
    //resize(sizeHint());

    //connect(ui.textEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)), this, SLOT(currentCharFormatChanged(const QTextCharFormat &)));
    //connect(ui.textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

    connect(ui.fontStyleToolButton, SIGNAL(clicked()), this, SLOT(showFontFrame()));
    connect(ui.emotionToolButton, SIGNAL(clicked()), this, SLOT(showEmotions()));
    connect(ui.insertImageToolButton, SIGNAL(clicked()), this, SLOT(insertImage()));
    connect(ui.sendFileToolButton, SIGNAL(clicked()), this, SLOT(sendFile()));
    connect(ui.toolButtonScreenshot, SIGNAL(clicked()), this, SLOT(takeScreenshot()));

    //Font Type
    connect(ui.fontComboBox, SIGNAL(activated(const QString &)), this, SLOT(textFamily(const QString &)));
    //Font Size
    QFontDatabase db;
    foreach(int size, db.standardSizes())
        ui.fontSizeComboBox->addItem(QString::number(size));

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

    //	fontChanged(ui.textEdit->font());
    //	colorChanged(ui.textEdit->textColor());
    //alignmentChanged(ui.textEdit->alignment());

    connect(ui.sendMsgPushButton, SIGNAL(clicked()), this, SLOT(emitSendMsgSignal()));

    //emotionsListPage = new EmotionsListPage();
    //connect(emotionsListPage, SIGNAL(clicked(const QString &)), this, SLOT(insertEmotion(const QString &)));
    //emotionsListPage->setVisible(false);

    smileyPopup = new EmoticonSelector();
    connect(smileyPopup, SIGNAL(signalEmoticonSelected(const QString&, bool)), this, SLOT(insertEmoticon(const QString&, bool)));


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

    //ui.webView->setUrl(QUrl("qrc:/text/resources/text/sample.html"));
    //ui.webView->load(QUrl("qrc:/text/resources/text/sample.html"));
    //qWarning()<<"HTML:\n"<<ui.webView->page()->mainFrame()->toHtml();

    //        QString htmlPath = imageCachePath + QString("/%1.htm").arg(contact->getUserID());
    //        QFile::copy("qrc:/text/resources/text/sample.html", htmlPath);
    //        ui.webView->setUrl(htmlPath);



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

/*
 void ChatMessageWindow::sendMsg(const QStringList &msgList)
 {
 //QMessageBox::warning(this, tr("msg!"), msg );

 messageBrowser()->append(msgList[0]);
 messageBrowser()->append(msgList[1]);
 messageBrowser()->append("");
 }
 */


void ChatMessageWindow::appendMessageReceivedFromContact(const QString &message, Contact *contact, const QString &datetime){
    qDebug()<<"--ChatMessageWindow::appendMessageReceivedFromContact(...)";

   Contact *c = contact;
   if(!c){
        c = m_contact;
   }
   Q_ASSERT_X(c != 0, "appendMessageReceivedFromContact", "NULL Contact!");

   qDebug()<<"----Message Received From Contact! contactID:"<<c->getUserID()<<" Time:"<<datetime<<" Msg:"<<message;;
        
//        QString msg = "<dl>";
//        msg += QString("<dt>%1  %2  </dt>").arg(contact->getNickName()).arg(datetime);
//        msg += "<dd >";
//        msg += QString(" %1 ").arg(message);
//        msg += "</dd >";
//        msg += "</dl >";
        
    
    QWebFrame *frame = ui.webView->page()->mainFrame();
    QWebElement doc = frame->documentElement();
    
//    if(message.contains(QRegExp("<style type=\"text/css\">"))){
//        QString styeID = "";
//        QString styleInfo = "";
        
//        QRegExp regExp(QString("\\.%1\\d+").arg(contact->getUserID()));
//        regExp.setCaseSensitivity(Qt::CaseInsensitive);
//        regExp.setMinimal(true);
//        int pos = regExp.indexIn(message);
//        if(pos != -1){
//            styeID = regExp.cap(0);
//        }
        
//        //regExp.setPattern("font-family.*;");
//        regExp.setPattern("\\.%1\\d+{.+}");
//        pos = regExp.indexIn(message);
//        if(pos != -1){
//            styleInfo = regExp.cap(0);
//        }
        
//        if(!styeID.isEmpty() && !styleInfo.isEmpty()){
//            //doc.setStyleProperty(m_styleID, styleInfo); 
//            QWebElement styleElement = doc.findFirst("style");
//            styleElement.appendInside(styleInfo);
//        }

//    }
    
    //QString msg = QString("<div><span>%1 %2</span></div>").arg(contact->getNickName()).arg(datetime);    
    QString msg = QString("<span>%1 %2</span>").arg(c->getNickName()).arg(datetime);

//    QString msg = "<dl>";
//    msg += QString("<dt>%1  %2  </dt>").arg(contact->getNickName()).arg(datetime);
//    msg += "<dd >";
    
    QRegExp regExp("<div.+>.*</div>");
    regExp.setCaseSensitivity(Qt::CaseInsensitive);
    regExp.setMinimal(true);
    int pos = regExp.indexIn(message);
    if(pos != -1){
        msg += regExp.cap(0);
    }

    
//    msg += "</dd >";
//    msg += "</dl >";
       
        QWebElement div = doc.findFirst("div");
        div.appendInside(msg);
         

        ui.webView->scroll(0, 0);
        //qDebug()<<"HTML:\n"<<ui.webView->page()->mainFrame()->toHtml();

}


void ChatMessageWindow::appendMessage(const QString &message){
//	QStringList msgList;
//	msgList << myDisplayName + ":\t" + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz") << message;

//        ui.webView->append(msgList[0]);
//        ui.webView->append(msgList[1]);
//        ui.webView->append("");
        
        
//        appendMessageReceivedFromContact(message, QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz") );

}

void ChatMessageWindow::emitSendMsgSignal() {
	//TODO:是否要转换成UTF8 
    
    QString message = ui.textEdit->toPlainText();

	if(message.isEmpty()){
		qDebug()<<"XXXXChatMessageWindow::emitSendMsgSignal()~~ Empty Message!";

		ui.textEdit->setFocus();
		return;
	}

	if(message.size() > MAX_MESSAGE_SIZE){
		QMessageBox::critical(this, tr("Error"), tr("The message is too long!"));
                qCritical()<<"ERROR! ChatMessageWindow::emitSendMsgSignal()~~Message too long! HTML:"<<ui.textEdit->toHtml().size()<<" Plain:"<<ui.textEdit->toPlainText().size();
		ui.textEdit->setFocus();
		return;
	}

        QString richMessage = "";

        QWebElement doc = ui.webView->page()->mainFrame()->documentElement();     
        QWebElement div = doc.findFirst("div");

//        QString styleInfo = "";
//        styleInfo = QString("font-family:'%1'; font-size:%2pt;").arg(ui.fontComboBox->currentText()).arg(ui.fontSizeComboBox->currentText());
//        styleInfo += QString("font-weight:%1;").arg(ui.fontBoldToolButton->isChecked()?"600":"400");
//        styleInfo += QString("font-style:%1;").arg(ui.fontItalicToolButton->isChecked()?"italic":"normal");
//        if(ui.fontUnderlineToolButton->isChecked()){
//            styleInfo += QString("text-decoration: underline;");
//        }
//        styleInfo += QString("color:%1;").arg(fmt.foreground().color().name());
//        //styleInfo += "position: relative;";
        

        
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
        
        



        
        
        //qWarning()<<richMessage;

//        QString msgToShow = "<dl>";
//        msgToShow += QString("<dt>%1  %2  </dt>").arg(myDisplayName).arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
//        msgToShow += "<dd >";
//        msgToShow += QString(" %1 ").arg(richMessage);
//        msgToShow += "</dd >";
//        msgToShow += "</dl >";
        
        //div.appendInside(QString("<div><span>%1 %2</span></div>").arg(myDisplayName).arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
//        div.appendInside(QString("<span>%1 %2</span>").arg(myDisplayName).arg(QDateTime::currentDateTime().toString("hh:mm:ss")));        
//        div.appendInside(richMessage);
        
        QString temp = QString("<div><span>%1 %2</span>").arg(myDisplayName).arg(QDateTime::currentDateTime().toString("hh:mm:ss")) + richMessage +"</div>";
        div.appendInside(temp);
         
        
        ui.textEdit->clear();
        ui.textEdit->setFocus();        
        ui.textEdit->setCurrentCharFormat(fmt);
        
        
        qDebug()<<"-------richMessage1:"<<richMessage;
//        richMessage.replace(QRegExp("src=\"(([a-zA-Z]:/\\w)|(/\\w)).+\\.((png)|(gif)|(jpg)|(jpeg))\"", Qt::CaseInsensitive), "");
        
        
        
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
        
        //qDebug()<<"-------richMessage2:"<<richMessage;
        
        //emit sendMsgButtonClicked(m_contact->getUserID(), richMessage, imageList);


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

      
        
        //qWarning()<<"HTML:\n"<<ui.webView->page()->mainFrame()->toHtml();
                
                
}




void ChatMessageWindow::showFontFrame() {
	if (ui.fontStyleToolButton->isChecked()) {
		ui.fontFrame->setVisible(true);
	} else {
		ui.fontFrame->setVisible(false);
	}
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

        if (!fileName.isEmpty()){
            ui.textEdit->insertHtml(QLatin1String("<img src=\"") + fileName + QLatin1String("\"/>"));
        }

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
        qWarning()<<"Save Image Failed!";
        return;
    }

    ui.textEdit->insertHtml(QLatin1String("<img src=\"") + fileName + QLatin1String("\"/>"));


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
        styleInfo += QString("font-weight: bold;");       
    }
    
    if(ui.fontItalicToolButton->isChecked()){
        styleInfo += QString("font-style:italic;");   
    }
    
    if(ui.fontUnderlineToolButton->isChecked()){
        styleInfo += QString("text-decoration: underline;");
    }
    
    if(fmt.foreground().color().name() != "#000000"){
        styleInfo += QString("color:%1;").arg(fmt.foreground().color().name());
    }
    
    //styleInfo += "position: relative;";
    
    m_styleString = styleInfo;
    


}  

void ChatMessageWindow::updateImage(const QString &imageName){
    QWebElement doc = ui.webView->page()->mainFrame()->documentElement();     
    
    QWebElementCollection elements = doc.findAll("img");
    foreach (QWebElement element, elements){
        if(element.attribute("src") == imageName){
            element.setAttribute("src", imageCachePath +"/"+imageName); 
        }
        qWarning()<<element.toInnerXml();
    }

}





















} //namespace HEHUI
