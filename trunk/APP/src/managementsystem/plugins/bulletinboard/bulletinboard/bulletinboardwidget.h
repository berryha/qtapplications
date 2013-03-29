#ifndef BULLETINBOARD_H
#define BULLETINBOARD_H

#include <QWidget>
#include <QCloseEvent>
#include <QMenu>


#include "ui_bulletinboardwidget.h"



namespace HEHUI {

class BulletinBoardWidget : public QWidget
{
    Q_OBJECT

public:
    BulletinBoardWidget(const QString &adminName, quint32 announcementID, const QString &serverAnnouncement, QWidget *parent = 0);
    ~BulletinBoardWidget();


    void showServerAnnouncement(const QString &adminName, quint32 announcementID, const QString &serverAnnouncement);

 
public slots:


protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_toolButtonPrevious_clicked();
    void on_toolButtonNext_clicked();


private:
    Ui::SystemManagementWidgetClass ui;

    QHash<quint32/*announcementID*/, QString/*Announcement*/> announcements;
    
    int curAnnouncementIndex;
    int totalCount;

  


};

} //namespace HEHUI

#endif // BULLETINBOARD_H
