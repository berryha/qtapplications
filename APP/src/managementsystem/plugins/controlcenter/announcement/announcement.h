#ifndef ANNOUNCEMENT_H
#define ANNOUNCEMENT_H

#include <QWidget>
#include <QTableView>

#include "ui_announcement.h"

#include "networkmanager/controlcenterpacketsparser.h"



namespace HEHUI {

class Announcement : public QWidget
{
    Q_OBJECT

public:
    Announcement(QTableView *tableView, const QString &adminName, ControlCenterPacketsParser *controlCenterPacketsParser, QWidget *parent = 0);
    ~Announcement();
    
    
private slots:
    void on_pushButtonSend_clicked();
    
    void on_comboBoxTargetType_currentIndexChanged ( int index );

    
    
private:
    Ui::AnnouncementClass ui;
    
    QTableView *m_tableView;
    QString m_adminName;
    ControlCenterPacketsParser *m_controlCenterPacketsParser;
    
    
    
};

} //namespace HEHUI 

#endif // ANNOUNCEMENT_H
