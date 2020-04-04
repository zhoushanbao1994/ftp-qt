#include "ftpclient.h"
#include "ui_ftpclient.h"
#include <QDebug>

ftpClient::ftpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpClient)
{
    m_clientThread = new ClientThread();

    connect(m_clientThread, SIGNAL(emitListItem(QString, QString, QString)), this, SLOT(recvListItem(QString, QString, QString)));
    connect(m_clientThread, SIGNAL(emitSuccess()), this, SLOT(recvSuccess()));
    connect(m_clientThread, SIGNAL(finished()), m_clientThread, SLOT(stop()));
    connect(m_clientThread, SIGNAL(emitClearList()), this, SLOT(recvClearList()));
    connect(m_clientThread->curClient->infoThread, SIGNAL(emitInfo(QString)), this, SLOT(recvInfo(QString)));
    //若执行此行，run结束后clientThread会调用析构
    //connect(clientThread, SIGNAL(finished()), clientThread, SLOT(deleteLater()));

    ui->setupUi(this);
}

ftpClient::~ftpClient()
{
    delete ui;
    delete m_clientThread;
}

//slot function------------------------------------------------
// 连接按键
void ftpClient::on_connectButton_clicked()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    if(!m_clientThread->isRunning()){
        if(!m_connected) {
            QString ip_addr = ui->ipEdit->text();
            QString username = ui->userEdit->text();
            QString password = ui->passEdit->text();
            m_clientThread->curClient->login(ip_addr, username, password);
            m_clientThread->task = TConnect;
            m_clientThread->start();
        }
        else {
            m_clientThread->task = TDisconnect;
            m_clientThread->start();
            m_connected = false;
            ui->connectButton->setText("Connect");
        }
    }
}

// 下载按键
void ftpClient::on_downButton_clicked()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    QTreeWidgetItem* curItem = ui->fileTree->currentItem();
    QString downName;
    if(curItem)
        downName = curItem->text(2);
    else
        return;
    QString saveDir = QFileDialog::getExistingDirectory(this, "Choose save path");
    if(!m_clientThread->isRunning()) {
        m_clientThread->task = TDown;
        m_clientThread->arglist[0] = downName.toStdString();
        m_clientThread->arglist[1] = saveDir.toStdString();
        m_clientThread->start();
    }
}

// 上传按键
void ftpClient::on_upButton_clicked()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    std::string localFile;
    localFile = QFileDialog::getOpenFileName(this, "Choose the file to upload").toStdString();
    m_clientThread->task = TUp;
    m_clientThread->arglist[0] = localFile;
    m_clientThread->start();
}

// 重命名按键
void ftpClient::on_renameButton_clicked()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    QTreeWidgetItem* curItem = ui->fileTree->currentItem();
    QString srcName, dstName;
    if(curItem)
        srcName = curItem->text(2);
    else
        return;
    if(srcName=="." || srcName=="..")
        return;
    dstName = QInputDialog::getText(this, "Please input a name", "New name of the file");
    if(dstName.isEmpty())
        return;
    if(dstName=="." || dstName=="..")
        return;
    m_clientThread->arglist[0] = srcName.toStdString();
    m_clientThread->arglist[1] = dstName.toStdString();
    m_clientThread->task = TRename;
    m_clientThread->start();
}

// 删除按键
void ftpClient::on_deleteButton_clicked()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    QTreeWidgetItem* curItem = ui->fileTree->currentItem();
    QString fname;
    if(curItem)
        fname = curItem->text(2);
    else
        return;
    m_clientThread->arglist[0] = fname.toStdString();
    if(fname=="." || fname=="..")
        return;
    if(curItem->text(0)=="d")
        m_clientThread->task = TRmd;
    else
        m_clientThread->task = TDele;
    m_clientThread->start();
}

// 新建按键
void ftpClient::on_newButton_clicked()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    QString name;
    name = QInputDialog::getText(this, "Please input a name.", "Name of new directory");
    if(name.isEmpty())
        return;
    if(name=="." || name=="..")
        return;
    m_clientThread->arglist[0] = name.toStdString();
    m_clientThread->task = TMkd;
    m_clientThread->start();
}


void ftpClient::on_fileTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << column;
    QString type = item->text(0);
    if(type!="d")
        return;
    QString file = item->text(2);
    if(!m_clientThread->isRunning()) {
        m_clientThread->arglist[0] = file.toStdString();
        m_clientThread->task = TCd;
        m_clientThread->start();
    }
}

void ftpClient::recvListItem(QString type, QString size, QString name)
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->fileTree);
    item->setText(0, type);
    item->setText(1, size);
    item->setText(2, name);
    ui->fileTree->addTopLevelItem(item);
}

void ftpClient::recvInfo(QString info)
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    allInfo += info;
    if(allInfo.size()>=10000) {
        allInfo = allInfo.mid(allInfo.size()-10000);
    }
    ui->infoEdit->setText(allInfo);
    QTextCursor cursor = ui->infoEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->infoEdit->setTextCursor(cursor);
}

void ftpClient::recvSuccess()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    if(!m_connected) {      // 连接状态
        m_connected = true;
        ui->connectButton->setText("Disconnect");
    }
    else {
        m_connected = false;
        ui->connectButton->setText("Connect");
    }
}

void ftpClient::recvClearList()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    ui->fileTree->clear();
}

