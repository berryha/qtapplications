#ifndef EXPLISTVIEW_H
#define EXPLISTVIEW_H

#include <QtGui/QWidget>
#include "ui_explistview.h"

#include "HHSharedGUI/hexpandlistviewmanager.h"


using namespace HEHUI;

typedef ExpandListViewInterface::ObjectItem ObjectItem;
typedef ExpandListViewInterface::ObjectItemList ObjectItemList;
typedef ExpandListViewInterface::Category Category;
typedef ExpandListViewInterface::CategoryList CategoryList;

class ExpListView : public QWidget
{
    Q_OBJECT

public:
    ExpListView(QWidget *parent = 0);
    ~ExpListView();

private:
    Ui::ExpListViewClass ui;

    ExpandListViewManager *expandListViewManager;
    ExpandListView *friendsListView;


};

#endif // EXPLISTVIEW_H
