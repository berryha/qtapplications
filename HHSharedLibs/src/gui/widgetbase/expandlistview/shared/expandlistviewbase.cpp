/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

/*
TRANSLATOR qdesigner_internal::ExpandListViewBase
*/

#include "expandlistviewbase.h"
//#include "qdesigner_utils_p.h"

//#include <ui4_p.h>

#include <QtCore/QRegExp>
#include <QtCore/QDebug>
#include <QtCore/QXmlStreamReader>

QT_BEGIN_NAMESPACE

namespace HEHUI {

ExpandListViewBase::ExpandListViewBase(QWidget *parent, Qt::WindowFlags flags)
    : ExpandListViewInterface(parent, flags),
      m_loadMode(LoadMerge)
{

}

ExpandListViewBase::LoadMode ExpandListViewBase::loadMode() const
{
    return m_loadMode;
}

void ExpandListViewBase::setLoadMode(LoadMode lm)
{
     m_loadMode = lm;
}

// Convenience to find a objectItem by class name
bool ExpandListViewBase::findObjectItem(const ExpandListViewInterface *wbox,
                                    const QString &className,
                                    const QString &category,
                                    ObjectItem *widgetData)
{
    // Note that entry names do not necessarily match the class name
    // (at least, not for the standard widgets), so,
    // look in the XML for the class name of the first objectItem to appear
    const QString widgetTag = QLatin1String("<widget");
    QString pattern = QLatin1String("^<widget\\s+class\\s*=\\s*\"");
    pattern += className;
    pattern += QLatin1String("\".*$");
    const QRegExp regexp(pattern);
    Q_ASSERT(regexp.isValid());
    const int catCount = wbox->categoryCount();
    for (int c = 0; c < catCount; c++) {
        const Category cat = wbox->category(c);
        if (category.isEmpty() || cat.name() == category) {
            const int widgetCount =  cat.objectItemCount();
            for (int w = 0; w < widgetCount; w++) {
                const ObjectItem widget = cat.objectItem(w);
                QString xml = widget.domXml(); // Erase the <ui> tag that can be present starting from 4.4
                const int widgetTagIndex = xml.indexOf(widgetTag);
                if (widgetTagIndex != -1) {
                    xml.remove(0, widgetTagIndex);
                    if (regexp.exactMatch(xml)) {
                        *widgetData = widget;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


/*


// Convenience to create a Dom ObjectItem from objectItem box xml code.
DomUI *ExpandListViewBase::xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel,
                                   QString *errorMessage)
{
    QXmlStreamReader reader(xml);
    DomUI *ui = 0;

    // The xml description must either contain a root element "ui" with a child element "objectItem"
    // or "objectItem" as the root element (4.3 legacy)
    const QString widgetTag = QLatin1String("objectItem");

    while (!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement) {
            const QStringRef name = reader.name();
            if (ui) {
                reader.raiseError(tr("Unexpected element <%1>").arg(name.toString()));
                continue;
            }

            if (name.compare(QLatin1String("objectItem"), Qt::CaseInsensitive) == 0) { // 4.3 legacy, wrap into DomUI
                ui = new DomUI;
                DomWidget *objectItem = new DomWidget;
                objectItem->read(reader);
                ui->setElementWidget(objectItem);
            } else if (name.compare(QLatin1String("ui"), Qt::CaseInsensitive) == 0) { // 4.4
                ui = new DomUI;
                ui->read(reader);
            } else {
                reader.raiseError(tr("Unexpected element <%1>").arg(name.toString()));
            }
        }
   }

    if (reader.hasError()) {
        delete ui;
        *errorMessage = tr("A parse error occurred at line %1, column %2 of the XML code "
                           "specified for the objectItem %3: %4\n%5")
                           .arg(reader.lineNumber()).arg(reader.columnNumber()).arg(name)
                           .arg(reader.errorString()).arg(xml);
        return 0;
    }

    if (!ui || !ui->elementWidget()) {
        delete ui;
        *errorMessage = tr("The XML code specified for the objectItem %1 does not contain "
                           "any objectItem elements.\n%2").arg(name).arg(xml);
        return 0;
    }

    if (insertFakeTopLevel)  {
        DomWidget *fakeTopLevel = new DomWidget;
        fakeTopLevel->setAttributeClass(QLatin1String("QWidget"));
        QList<DomWidget *> children;
        children.push_back(ui->takeElementWidget());
        fakeTopLevel->setElementWidget(children);
        ui->setElementWidget(fakeTopLevel);
    }

    return ui;
}

// Convenience to create a Dom ObjectItem from objectItem box xml code.
DomUI *ExpandListViewBase::xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel)
{
    QString errorMessage;
    DomUI *rc = xmlToUi(name, xml, insertFakeTopLevel, &errorMessage);
    if (!rc)
        qdesigner_internal::designerWarning(errorMessage);
    return rc;
}


*/



}  // namespace HEHUI

QT_END_NAMESPACE
