/*
 ****************************************************************************
 * progressdlg.h
 *
 * Created on: 2009-8-22
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2010-05-10
 * Last Modified by: 贺辉
 ***************************************************************************
 */



#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H

#include <QtGui/QWidget>
#include "../ui/ui_progressdlg.h"

#include "HHSharedCore/hmysharedlib_global.h"
 //#include "../core/mysharedlib_global.h"

class MYSHAREDLIB_API ProgressDlg : public QWidget
{
    Q_OBJECT

public:
    ProgressDlg(const QString &jobName = "", QWidget *parent = 0);
    ~ProgressDlg();


    void setJobName(const QString &name);
    void setProgressMessage(const QString &message);
    void setJobProgress(int progressValue);

public slots:
	void slotUpdateProgress(const QString &progressMessage, int progressValue = -1);
        void slotUpdateProcessOutput(const QString &message);

private slots:
	void on_toolButtonShowDetails_toggled ( bool checked );


private:
    Ui::ProgressDlgClass ui;


};

#endif // PROGRESSDLG_H
