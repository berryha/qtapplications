#ifndef ADUSER_H
#define ADUSER_H

#include <QObject>

class ADUser : public QObject
{
    Q_OBJECT
public:
    explicit ADUser(QObject *parent = 0);
    
signals:
    
public slots:

private:
    QString m_samAccountName;
    QString m_displayName;
    QString m_sid;
    QString m_guid;

    
};

#endif // ADUSER_H
