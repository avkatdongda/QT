// ConsoleDialog.h
#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextCursor>
#include <QDateTime>

// 简单控制台对话框，可以输入和显示命令
class ConsoleDialog : public QDialog
{
    Q_OBJECT  // Qt信号槽必需

public:
    explicit ConsoleDialog(QWidget *parent = nullptr);  // 构造函数

signals:
    // 定义一个信号，当用户输入命令并按下回车时发射
    void commandReady(const QString &cmd);

private slots:
    // 槽函数：当用户按下回车键时调用
    void onInputReturnPressed();

private:
    QPlainTextEdit *outputArea;  // 显示历史命令的区域
    QLineEdit *inputLine;        // 输入命令的行

    // 辅助函数：添加时间戳
    QString getTimestamp() const {
        return QDateTime::currentDateTime().toString("hh:mm:ss");
    }
};

#endif // CONSOLEDIALOG_H
