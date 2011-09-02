/*
 ****************************************************************************
 * aboutdialog.cpp
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




#include "aboutdialog.h"
#include "../shared/app_constants.h"


namespace HEHUI {

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	retranslateUi();

}

AboutDialog::~AboutDialog()
{

}

void AboutDialog::languageChange(){

	ui.retranslateUi(this);
	retranslateUi();

}

void AboutDialog::retranslateUi(){

        QString appInfo = QString("<center><h3>%1</h3><p>Build %2 </p></center>")
                                                        .arg(APP_NAME).arg(APP_VERSION);
	appInfo += QString(tr("<p>Author: %1</p><p>License: <a href=\"http://www.gnu.org/copyleft/lesser.html\">LGPL</a></p><p>Email: %2</p>"))
							.arg(APP_AUTHOR).arg(APP_AUTHOR_EMAIL);
	appInfo += QString(tr("<p><center>The program is provided AS IS with NO WARRANTY OF ANY KIND.</center></p>"));
	//appInfo += QString(tr("<p><center></center></p>"));

	ui.generalInfoLabel->setText(appInfo);

}










} //namespace HEHUI


