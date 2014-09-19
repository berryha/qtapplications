#ifndef DNSLOOKUP_H
#define DNSLOOKUP_H

#include <QWidget>
#include <QDnsLookup>
#include <QLineEdit>

namespace Ui {
class DnsLookup;
}

namespace HEHUI {


class DnsLookupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DnsLookupWidget(QWidget *parent = 0);
    ~DnsLookupWidget();



private slots:
    void on_pushButtonLookup_clicked();

    void on_checkBoxChinaTelecom_clicked(bool checked);
    void on_checkBoxChinaUnicom_clicked(bool checked);
    void on_checkBoxChinaMobile_clicked(bool checked);
    void on_checkBoxOthers_clicked(bool checked);

    void handleServers();




private:
    Ui::DnsLookup *ui;

    QString m_domainName;
    QHash<QString/*IP*/, QLineEdit*> m_nameServersHash;
    QStringList m_nameServers;
    int m_response;



};

} //namespace HEHUI

#endif // DNSLOOKUP_H
