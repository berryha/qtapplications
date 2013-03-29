#ifndef SYSTEMISUMMARYNFO_H
#define SYSTEMISUMMARYNFO_H

#include <QWidget>
#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QSqlQueryModel>


#include "ui_systemsummaryinfo.h"
#include "networkmanager/bulletinboardpacketsparser.h"
//#include "../../sharedms/networkmanager.h"
#include "bulletinboard/bulletinboardwidget.h"



//#include "HHSharedCore/huser.h"
//#include "HHSharedCore/hmysharedlib_global.h"

   


namespace HEHUI {


class SystemSummaryInfo : public QMainWindow
{
    Q_OBJECT

public:
    SystemSummaryInfo(QWidget *parent = 0);
    ~SystemSummaryInfo();



    static bool isRunning(){return running;}
    


    

    void retranslateUi();

protected:
    void languageChange();
    void closeEvent(QCloseEvent *e);

public slots:





private:
    Ui::SystemSummaryInfoClass ui;

    static bool running;


    QString m_currentUserNameOfOS;

    QString localComputerName;


};

} //namespace HEHUI

#endif // SYSTEMISUMMARYNFO_H
