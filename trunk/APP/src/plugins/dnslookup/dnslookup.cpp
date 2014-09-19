#include "dnslookup.h"
#include "ui_dnslookup.h"


#include <QUrl>
#include <QMessageBox>
#include <QHostAddress>


namespace HEHUI {


DnsLookupWidget::DnsLookupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DnsLookup)
{
    ui->setupUi(this);

    m_response = 0;

    ui->comboBoxType->addItem("A", QVariant(QDnsLookup::A));
    ui->comboBoxType->addItem("AAAA", QVariant(QDnsLookup::AAAA));
    ui->comboBoxType->addItem("ANY", QVariant(QDnsLookup::ANY));
    ui->comboBoxType->addItem("CNAME", QVariant(QDnsLookup::CNAME));
    ui->comboBoxType->addItem("MX", QVariant(QDnsLookup::MX));
    ui->comboBoxType->addItem("NS", QVariant(QDnsLookup::NS));
    ui->comboBoxType->addItem("PTR", QVariant(QDnsLookup::PTR));
    ui->comboBoxType->addItem("SRV", QVariant(QDnsLookup::SRV));
    ui->comboBoxType->addItem("TXT", QVariant(QDnsLookup::TXT));
    ui->comboBoxType->setCurrentIndex(0);

    m_nameServersHash.insert(ui->checkBoxCT1->text(), ui->lineEditRecordCT1);
    m_nameServersHash.insert(ui->checkBoxCT2->text(), ui->lineEditRecordCT2);
    m_nameServersHash.insert(ui->checkBoxCT3->text(), ui->lineEditRecordCT3);
    m_nameServersHash.insert(ui->checkBoxCT4->text(), ui->lineEditRecordCT4);

    m_nameServersHash.insert(ui->checkBoxCU1->text(), ui->lineEditRecordCU1);
    m_nameServersHash.insert(ui->checkBoxCU2->text(), ui->lineEditRecordCU2);
    m_nameServersHash.insert(ui->checkBoxCU3->text(), ui->lineEditRecordCU3);
    m_nameServersHash.insert(ui->checkBoxCU4->text(), ui->lineEditRecordCU4);

    m_nameServersHash.insert(ui->checkBoxCM1->text(), ui->lineEditRecordCM1);
    m_nameServersHash.insert(ui->checkBoxCM2->text(), ui->lineEditRecordCM2);
    m_nameServersHash.insert(ui->checkBoxCM3->text(), ui->lineEditRecordCM3);
    m_nameServersHash.insert(ui->checkBoxCM4->text(), ui->lineEditRecordCM4);

    m_nameServersHash.insert(ui->checkBox114->text(), ui->lineEditRecord114);
    m_nameServersHash.insert(ui->checkBoxGoogle->text(), ui->lineEditRecordGoogle);
    m_nameServersHash.insert(ui->checkBoxOpenDNS->text(), ui->lineEditRecordOpenDNS);



}

DnsLookupWidget::~DnsLookupWidget()
{
    delete ui;

}


void DnsLookupWidget::on_pushButtonLookup_clicked(){

    m_response = 0;

    foreach (QLineEdit *obj, m_nameServersHash.values()) {
        obj->clear();
    }
    ui->lineEditRecordCustom1->clear();
    ui->lineEditRecordCustom2->clear();
    m_nameServers.clear();


    QUrl url = QUrl::fromUserInput(ui->lineEditDomainName->text().trimmed());
    if(!url.isValid() || url.isLocalFile()){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Domain Name!"));
        return;
    }
    m_domainName = url.host();
    ui->lineEditDomainName->setText(m_domainName);

    if(ui->groupBoxChinaTelecom->isVisible()){
        foreach (QObject *obj, ui->groupBoxChinaTelecom->children()) {
            QCheckBox *box = qobject_cast<QCheckBox *>(obj);
            if(box && box->isChecked()){
                m_nameServers.append(box->text());
            }
        }
    }
    if(ui->groupBoxChinaUnicom->isVisible()){
        foreach (QObject *obj, ui->groupBoxChinaUnicom->children()) {
            QCheckBox *box = qobject_cast<QCheckBox *>(obj);
            if(box && box->isChecked()){
                m_nameServers.append(box->text());
            }
        }
    }
    if(ui->groupBoxChinaMobile->isVisible()){
        foreach (QObject *obj, ui->groupBoxChinaMobile->children()) {
            QCheckBox *box = qobject_cast<QCheckBox *>(obj);
            if(box && box->isChecked()){
                m_nameServers.append(box->text());
            }
        }
    }
    if(ui->groupBoxOthers->isVisible()){
        foreach (QObject *obj, ui->groupBoxOthers->children()) {
            QCheckBox *box = qobject_cast<QCheckBox *>(obj);
            if(box && box->isChecked()){
                m_nameServers.append(box->text());
            }
        }
    }

    QString ns1 = ui->lineEditNSIPCustom1->text().trimmed();
    QString ns2 = ui->lineEditNSIPCustom2->text().trimmed();
    if(!ns1.isEmpty()){
        m_nameServers.append(ns1);
        m_nameServersHash.insert(ns1, ui->lineEditRecordCustom1);
    }
    if(!ns2.isEmpty()){
        m_nameServers.append(ns2);
        m_nameServersHash.insert(ns2, ui->lineEditRecordCustom2);
    }


    if(m_nameServers.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("No name server!"));
        return;
    }


//    if(!m_dnsLookup){
//        m_dnsLookup = new QDnsLookup(this);
//        connect(m_dnsLookup, SIGNAL(finished()), this, SLOT(handleServers()));
//    }

    ui->pushButtonLookup->setEnabled(false);
    ui->lineEditDomainName->setReadOnly(true);
    ui->comboBoxType->setEnabled(false);
    ui->lineEditNSIPCustom1->setReadOnly(true);
    ui->lineEditNSIPCustom2->setReadOnly(true);


    QDnsLookup::Type type = QDnsLookup::Type(ui->comboBoxType->currentData().toUInt());
    foreach (QString nameServer, m_nameServers) {

        QDnsLookup *dnsLookup = new QDnsLookup(this);
        connect(dnsLookup, SIGNAL(finished()), this, SLOT(handleServers()));

        dnsLookup->setNameserver(QHostAddress(nameServer));

        dnsLookup->setName(m_domainName);
        dnsLookup->setType(type);

        dnsLookup->lookup();
    }

}

void DnsLookupWidget::on_checkBoxChinaTelecom_clicked(bool checked){
    ui->groupBoxChinaTelecom->setVisible(checked);
}

void DnsLookupWidget::on_checkBoxChinaUnicom_clicked(bool checked){
    ui->groupBoxChinaUnicom->setVisible(checked);
}

void DnsLookupWidget::on_checkBoxChinaMobile_clicked(bool checked){
    ui->groupBoxChinaMobile->setVisible(checked);
}

void DnsLookupWidget::on_checkBoxOthers_clicked(bool checked){
    ui->groupBoxOthers->setVisible(checked);
}

void DnsLookupWidget::handleServers(){
    //qDebug()<<"--DnsLookup::handleServers()";


    QDnsLookup *dnsLookup = qobject_cast<QDnsLookup *>(sender());
    if(!dnsLookup){
        delete dnsLookup;
        return;
    }

    QString nameServer = dnsLookup->nameserver().toString();
    QLineEdit *lineEdit = m_nameServersHash.value(nameServer);
    if(!lineEdit){
        delete dnsLookup;
        return;
    }

    if(dnsLookup->name() != m_domainName){
        delete dnsLookup;
        return;
    }

    if(++m_response == (m_nameServers.size()) ){
        ui->pushButtonLookup->setEnabled(true);
        ui->lineEditDomainName->setReadOnly(false);
        ui->comboBoxType->setEnabled(true);
        ui->lineEditNSIPCustom1->setReadOnly(false);
        ui->lineEditNSIPCustom2->setReadOnly(false);
    }


    // Check the lookup succeeded.
    if (dnsLookup->error() != QDnsLookup::NoError) {
        qCritical()<<"DNS lookup failed! "<<dnsLookup->errorString();

        lineEdit->setText(dnsLookup->errorString());
        delete dnsLookup;
        return;
    }

//    qDebug()<<"nameserver:"<<dnsLookup->nameserver();
//    qDebug()<<"type:"<<dnsLookup->type();
//    qDebug()<<"canonicalNameRecords() :"<<dnsLookup->canonicalNameRecords().size();
//    qDebug()<<"hostAddressRecords() :"<<dnsLookup->hostAddressRecords().size();
//    qDebug()<<"mailExchangeRecords() :"<<dnsLookup->mailExchangeRecords().size();
//    qDebug()<<"nameServerRecords() :"<<dnsLookup->nameServerRecords().size();
//    qDebug()<<"pointerRecords() :"<<dnsLookup->pointerRecords().size();
//    qDebug()<<"serviceRecords() :"<<dnsLookup->serviceRecords().size();
//    qDebug()<<"textRecords() :"<<dnsLookup->textRecords().size();


    QStringList values;

    switch (dnsLookup->type()) {
    case QDnsLookup::A:
    {
        foreach (const QDnsHostAddressRecord &record, dnsLookup->hostAddressRecords()) {
            //qWarning()<<record.name()<<" " <<record.value();
            values.append(record.value().toString());

        }
    }
        break;
    case QDnsLookup::AAAA:
    {
        foreach (const QDnsDomainNameRecord &record, dnsLookup->canonicalNameRecords()) {
            //qWarning()<<record.name()<<" " <<record.value();
            values.append(record.value());
        }
    }
        break;
    case QDnsLookup::MX:
    {
        foreach (const QDnsMailExchangeRecord &record, dnsLookup->mailExchangeRecords()) {
            //qWarning()<<record.name()<<" " <<record.exchange();
            values.append(record.exchange());
        }
    }
        break;
    case QDnsLookup::NS:
    case QDnsLookup::PTR:
    {
        foreach (const QDnsDomainNameRecord &record, dnsLookup->nameServerRecords()) {
            //qWarning()<<"Domain:"<<record.name()<<" "<<record.value();
            values.append(record.value());
        }
    }
        break;
    case QDnsLookup::SRV:
    {
        foreach (const QDnsServiceRecord &record, dnsLookup->serviceRecords()) {
            //qWarning()<<"Domain:"<<record.target()<<" "<<record.name();
            values.append(record.name());
        }
    }
        break;
    case QDnsLookup::TXT:
    {
        foreach (const QDnsTextRecord &record, dnsLookup->textRecords()) {
            //qWarning()<<"Domain:"<<record.name()<<" "<<record.values();
            foreach (QByteArray ba, record.values()) {
                values.append(QString(ba));
            }

        }
    }
        break;
    default:
        break;
    }

    lineEdit->setText(values.join(" "));

    delete dnsLookup;

}




} //namespace HEHUI
