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

#ifndef ABSTRACTEXPANDLISTVIEWINTERFACE_H
#define ABSTRACTEXPANDLISTVIEWINTERFACE_H


#include <QtCore/QMetaType>
#include <QWidget>
#include <QIcon>



QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

//class DomUI;
class QDesignerDnDItemInterface;

class /*QDESIGNER_SDK_EXPORT*/ ExpandListViewInterface : public QWidget
{
    Q_OBJECT
public:
    class ObjectItem {
    public:
        enum Type { Default, Custom };
        ObjectItem(const QString &aid = QString(), const QString &aname = QString(), const QString &xml = QString(),
                const QString &icon_name = QString(), QIcon::Mode iconMode = QIcon::Normal, Type atype = Default)
            : m_id(aid), m_name(aname), m_xml(xml), m_icon_name(icon_name), m_icon_mode(iconMode), m_type(atype) {}
        QString id() const { return m_id; }
        void setID(const QString &aid) { m_id = aid; }
        QString name() const { return m_name; }
        void setName(const QString &aname) { m_name = aname; }
        QString domXml() const { return m_xml; }
        void setDomXml(const QString &xml) { m_xml = xml; }
        QString iconName() const { return m_icon_name; }
        void setIconName(const QString &icon_name) { m_icon_name = icon_name; }
        QIcon::Mode iconMode() const { return m_icon_mode; }
        void setIconMode(QIcon::Mode icon_mode) { m_icon_mode = icon_mode; }
        QString categoryID() const { return m_category_id; }
        void setCategoryID(const QString &categoryID) { m_category_id = categoryID; }
        Type type() const { return m_type; }
        void setType(Type atype) { m_type = atype; }

        //bool isNull() const { return m_name.isEmpty(); }
        bool isNull() const { return m_id.isEmpty(); }


        //ContactBase* contact() { return &contactBase; }
        //void setContactBase(ContactBase c) { contactBase = c; }

    private:
        QString m_id;
        QString m_name;
        QString m_xml;
        QString m_icon_name;
        QIcon::Mode m_icon_mode;
        QString m_category_id;
        Type m_type;


        //ContactBase contactBase;

    };
    typedef QList<ObjectItem> ObjectItemList;

    class Category {
    public:
        enum Type { Default, Scratchpad };

        Category(const QString &aid = QString(), const QString &aname = QString(), Type atype = Default)
            : m_id(aid), m_name(aname), m_type(atype) {}

        QString id() const { return m_id; }
        void setID(const QString &aid) { m_id = aid; }
        QString name() const { return m_name; }
        void setName(const QString &aname) { m_name = aname; }
        int objectItemCount() const { return m_item_list.size(); }
        ObjectItem objectItem(int idx) const { return m_item_list.at(idx); }
        void removeObjectItem(int idx) { m_item_list.removeAt(idx); }
        void addObjectItem(const ObjectItem &awidget) { m_item_list.append(awidget); }
        Type type() const { return m_type; }
        void setType(Type atype) { m_type = atype; }

        //bool isNull() const { return m_name.isEmpty(); }
        bool isNull() const { return m_id.isEmpty(); }


    private:
        QString m_id;
        QString m_name;
        Type m_type;
        QList<ObjectItem> m_item_list;
    };
    typedef QList<Category> CategoryList;

    ExpandListViewInterface(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~ExpandListViewInterface();

    virtual int categoryCount() const = 0;
    virtual Category category(int cat_idx) const = 0;
    virtual void addCategory(const Category &cat) = 0;
    virtual void removeCategory(int cat_idx) = 0;

    virtual int objectItemCount(int cat_idx) const = 0;
    virtual ObjectItem objectItem(int cat_idx, int wgt_idx) const = 0;
    virtual void addObjectItem(int cat_idx, const ObjectItem &wgt) = 0;
    virtual void removeObjectItem(int cat_idx, int wgt_idx) = 0;

    int findOrInsertCategory(const QString &categoryName);

    virtual void dropObjectItems(const QList<QDesignerDnDItemInterface*> &item_list,
                                const QPoint &global_mouse_pos) = 0;

    virtual void setFileName(const QString &file_name) = 0;
    virtual QString fileName() const = 0;
    virtual bool load() = 0;
    virtual bool save() = 0;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QT_PREPEND_NAMESPACE(ExpandListViewInterface::ObjectItem))

QT_END_HEADER

#endif // ABSTRACTEXPANDLISTVIEWINTERFACE_H
