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

#include "abstractexpandlistviewInterface.h"

QT_BEGIN_NAMESPACE

/*!
    \class ExpandListViewInterface

    \brief The ExpandListViewInterface class allows you to control
    the contents of Qt Designer's objectItem box.

    \inmodule QtDesigner

    ExpandListViewInterface contains a collection of functions
    that is typically used to manipulate the contents of \QD's objectItem
    box.

    \QD uses an XML file to populate its objectItem box. The name of that
    file is one of the objectItem box's properties, and you can retrieve
    it using the fileName() function.

    ExpandListViewInterface also provides the save() function that
    saves the contents of the objectItem box in the file specified by the
    objectItem box's file name property. If you have made changes to the
    objectItem box, for example by dropping a objectItem into the objectItem box,
    without calling the save() function, the original content can be
    restored by a simple invocation of the load() function:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 0

    The ExpandListViewInterface class is not intended to be
    instantiated directly. You can retrieve an interface to Qt
    Designer's objectItem box using the
    QDesignerFormEditorInterface::widgetBox() function. A pointer to
    \QD's current QDesignerFormEditorInterface object (\c formEditor
    in the example above) is provided by the
    QDesignerCustomWidgetInterface::initialize() function's
    parameter. When implementing a custom objectItem plugin, you must
    subclass the QDesignerCustomWidgetInterface to expose your plugin
    to \QD.

    If you want to save your changes, and at the same time preserve
    the original contents, you can use the save() function combined
    with the setFileName() function to save your changes into another
    file. Remember to store the name of the original file first:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 1

    Then you can restore the original contents of the objectItem box by
    resetting the file name to the original file and calling load():

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 2

    In a similar way, you can later use your customized XML file:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 3


    \sa QDesignerFormEditorInterface
*/

/*!
    Constructs a objectItem box interface with the given \a parent and
    the specified window \a flags.
*/
ExpandListViewInterface::ExpandListViewInterface(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
}

/*!
    Destroys the objectItem box interface.
*/
ExpandListViewInterface::~ExpandListViewInterface()
{
}

/*!
    \internal
*/
int ExpandListViewInterface::findOrInsertCategory(const QString &categoryName)
{
    int count = categoryCount();
    for (int index=0; index<count; ++index) {
        Category c = category(index);
        if (c.name() == categoryName)
            return index;
    }

    addCategory(Category(categoryName));
    return count;
}

/*!
    \internal
    \fn int ExpandListViewInterface::categoryCount() const
*/

/*!
    \internal
    \fn Category ExpandListViewInterface::category(int cat_idx) const
*/

/*!
    \internal
    \fn void ExpandListViewInterface::addCategory(const Category &cat)
*/

/*!
    \internal
    \fn void ExpandListViewInterface::removeCategory(int cat_idx)
*/

/*!
    \internal
    \fn int ExpandListViewInterface::objectItemCount(int cat_idx) const
*/

/*!
    \internal
    \fn ObjectItem ExpandListViewInterface::objectItem(int cat_idx, int wgt_idx) const
*/

/*!
    \internal
    \fn void ExpandListViewInterface::addObjectItem(int cat_idx, const ObjectItem &wgt)
*/

/*!
    \internal
    \fn void ExpandListViewInterface::removeObjectItem(int cat_idx, int wgt_idx)
*/

/*!
    \internal
    \fn void ExpandListViewInterface::dropObjectItems(const QList<QDesignerDnDItemInterface*> &item_list, const QPoint &global_mouse_pos)

*/

/*!
    \fn void ExpandListViewInterface::setFileName(const QString &fileName)

    Sets the XML file that \QD will use to populate its objectItem box, to
    \a fileName. You must call load() to update the objectItem box with
    the new XML file.

    \sa fileName(), load()
*/

/*!
    \fn QString ExpandListViewInterface::fileName() const

    Returns the name of the XML file \QD is currently using to
    populate its objectItem box.

    \sa setFileName()
*/

/*!
    \fn bool ExpandListViewInterface::load()

    Populates \QD's objectItem box by loading (or reloading) the currently
    specified XML file. Returns true if the file is successfully
    loaded; otherwise false.

    \sa setFileName()
*/

/*!
    \fn bool ExpandListViewInterface::save()

    Saves the contents of \QD's objectItem box in the file specified by
    the fileName() function. Returns true if the content is
    successfully saved; otherwise false.

    \sa fileName(), setFileName()
*/


/*!
    \internal

    \class ExpandListViewInterface::ObjectItem

    \brief The ObjectItem class specified a objectItem in Qt Designer's objectItem
    box component.
*/

/*!
    \enum ExpandListViewInterface::ObjectItem::Type

    \value Default
    \value Custom
*/

/*!
    \fn ExpandListViewInterface::ObjectItem::ObjectItem(const QString &aname, const QString &xml, const QString &icon_name, Type atype)
*/

/*!
    \fn QString ExpandListViewInterface::ObjectItem::name() const
*/

/*!
    \fn void ExpandListViewInterface::ObjectItem::setName(const QString &aname)
*/

/*!
    \fn QString ExpandListViewInterface::ObjectItem::domXml() const
*/

/*!
    \fn void ExpandListViewInterface::ObjectItem::setDomXml(const QString &xml)
*/

/*!
    \fn QString ExpandListViewInterface::ObjectItem::iconName() const
*/

/*!
    \fn void ExpandListViewInterface::ObjectItem::setIconName(const QString &icon_name)
*/

/*!
    \fn Type ExpandListViewInterface::ObjectItem::type() const
*/

/*!
    \fn void ExpandListViewInterface::ObjectItem::setType(Type atype)
*/

/*!
    \fn bool ExpandListViewInterface::ObjectItem::isNull() const
*/


/*!
    \class ExpandListViewInterface::Category
    \brief The Category class specifies a category in Qt Designer's objectItem box component.
    \internal
*/

/*!
    \enum ExpandListViewInterface::Category::Type

    \value Default
    \value Scratchpad
*/

/*!
    \fn ExpandListViewInterface::Category::Category(const QString &aname, Type atype)
*/

/*!
    \fn QString ExpandListViewInterface::Category::name() const
*/

/*!
    \fn void ExpandListViewInterface::Category::setName(const QString &aname)
*/

/*!
    \fn int ExpandListViewInterface::Category::objectItemCount() const
*/

/*!
    \fn ObjectItem ExpandListViewInterface::Category::objectItem(int idx) const
*/

/*!
    \fn void ExpandListViewInterface::Category::removeObjectItem(int idx)
*/

/*!
    \fn void ExpandListViewInterface::Category::addObjectItem(const ObjectItem &awidget)
*/

/*!
    \fn Type ExpandListViewInterface::Category::type() const
*/

/*!
    \fn void ExpandListViewInterface::Category::setType(Type atype)
*/

/*!
    \fn bool ExpandListViewInterface::Category::isNull() const
*/

/*!
    \typedef ExpandListViewInterface::CategoryList
    \internal
*/

/*!
    \typedef ExpandListViewInterface::DataObjectList
    \internal
*/

QT_END_NAMESPACE
