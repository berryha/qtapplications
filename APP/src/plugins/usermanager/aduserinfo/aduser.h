#ifndef ADUSER_H
#define ADUSER_H

#include <QObject>
#include <QHash>



namespace HEHUI {


class ADUser : public QObject
{
    Q_OBJECT
public:
    explicit ADUser(QObject *parent = 0);


    void setAttribute(const QString &attributeName, const QString &attributeValue);
    QString getAttribute(const QString &attributeName);

    static QString CommonAttributeName(const QString &attributeName);

signals:


public slots:


private:
    static QHash<QString/*AD Attribute Name*/, QString/*Common Attribute Name*/> *commonAttributeNameHash;

    QHash<QString/*AD Attribute Name*/, QString/*Attribute Value*/> attributeHash;
    


};

} //namespace HEHUI

#endif // ADUSER_H
