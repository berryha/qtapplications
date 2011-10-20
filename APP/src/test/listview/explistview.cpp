#include "explistview.h"

ExpListView::ExpListView(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);




        expandListViewManager = new ExpandListViewManager(this);
        //TODO
//        connect(expandListViewManager, SIGNAL(signalContactItemActivated(const QString &)), chatWindowManager, SLOT(slotNewChatWithContact(const QString &)));
//        connect(expandListViewManager, SIGNAL(contextMenuEventOnCategoryOccurs(const QString& ,const QPoint, QMenu*)), this, SLOT(slotContextMenuEventOnCategoryOccurs(const QString& ,const QPoint, QMenu*)));
//        connect(expandListViewManager, SIGNAL(contextMenuEventOnObjectItemOccurs(const QString& ,const QPoint, QMenu*)), this, SLOT(slotContextMenuEventOnObjectItemOccurs(const QString& ,const QPoint, QMenu*)));
//        connect(expandListViewManager, SIGNAL(signalTooltipEventOnObjectItemOccurs(const QString& ,const QPoint, const QPoint)), this, SLOT(slotTooltipEventOnObjectItemOccurs(const QString&, const QPoint, const QPoint)));


        friendsListView = expandListViewManager->createExpandListView(this);
        ui.gridLayout->addWidget(friendsListView, 0, 0, 1, 1);


        Category category;
        category.setID("Group");
        category.setName("Group");

        for(int i=0; i<5; i++)
        {

            ObjectItem objectItem;
            objectItem.setID(QString::number(i));

            QString name = QString::number(i);

            objectItem.setName(name);

            //objectItem.setIconName();

            category.addObjectItem(objectItem);

            qApp->processEvents();
        }

        friendsListView->load(&category);



}

ExpListView::~ExpListView()
{

}
