#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QFileDialog>
#include "common.h"
#include "clientthread.h"

namespace Ui {
class ftpClient;
}

class ClientThread;

class ftpClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit ftpClient(QWidget *parent = 0);
    ~ftpClient();

private slots:
    void recvListItem(QString, QString, QString);
    void recvInfo(QString);
    void recvSuccess();
    void recvClearList();

    void on_connectButton_clicked();    // 下载按键
    void on_downButton_clicked();       // 连接按键
    void on_upButton_clicked();         // 上传按键
    void on_renameButton_clicked();     // 重命名按键
    void on_deleteButton_clicked();     // 删除按键
    void on_newButton_clicked();        // 新建按键

    void on_fileTree_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::ftpClient *ui;
    ClientThread* m_clientThread;
    bool m_connected = false;       // 连接状态
    QString allInfo;
};



#endif // FTPCLIENT_H
