/*
 ****************************************************************************
 * aboutdialog.h
 *
 * Created on: 2009-4-27
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
  * Last Modified on: 2010-05-17
  * Last Modified by: 贺辉
  ***************************************************************************
*/




#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "../ui/ui_aboutdialog.h"
//#include "ui_aboutdialog.h"


namespace HEHUI {

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private:
	void retranslateUi();

protected:
	void languageChange();


private:
    Ui::AboutDialogUI ui;


};

} //namespace HEHUI

#endif // ABOUTDIALOG_H
