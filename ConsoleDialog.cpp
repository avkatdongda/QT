// ConsoleDialog.cpp
#include "ConsoleDialog.h"
#include <QKeyEvent>

// 构造函数：初始化界面
ConsoleDialog::ConsoleDialog(QWidget *parent)
    : QDialog(parent)  // 调用父类构造函数
{
    // 设置窗口属性
    setWindowTitle("FPGA 命令控制台");  // 窗口标题
    resize(500, 300);                    // 窗口大小
    setMinimumSize(400, 200);            // 最小大小

    // 创建垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 1. 创建输出区域（显示历史）
    outputArea = new QPlainTextEdit(this);
    outputArea->setReadOnly(true);                      // 只读，用户不能修改
    outputArea->setFont(QFont("Consolas", 10));         // 等宽字体更好看
    outputArea->setPlaceholderText("命令历史将显示在这里...");
    mainLayout->addWidget(outputArea);                  // 添加到布局

    // 2. 创建输入行
    inputLine = new QLineEdit(this);
    inputLine->setPlaceholderText("输入32位十六进制命令 (如 810002FF)，按Enter发送");
    mainLayout->addWidget(inputLine);                   // 添加到布局

    // 3. 连接信号和槽
    // 当用户在inputLine按下回车键时，调用onInputReturnPressed函数
    connect(inputLine, &QLineEdit::returnPressed,
            this, &ConsoleDialog::onInputReturnPressed);

    // 4. 显示欢迎信息
    outputArea->appendPlainText("[" + getTimestamp() + "] FPGA命令控制台已启动");
    outputArea->appendPlainText("[" + getTimestamp() + "] 常用命令示例:");
    outputArea->appendPlainText("  81000001 - 相机开启");
    outputArea->appendPlainText("  81020000 - 自由模式");
    outputArea->appendPlainText("  81020001 - 外触发模式");
    outputArea->appendPlainText("  81020002 - 软件触发模式");
    outputArea->appendPlainText("--------------------------------");

    // 5. 设置焦点到输入行（打开窗口后可以直接输入）
    inputLine->setFocus();
}

// 槽函数：处理回车键按下
void ConsoleDialog::onInputReturnPressed()
{
    // 获取输入的文字，去掉前后空格
    QString cmd = inputLine->text().trimmed();

    if (!cmd.isEmpty()) {  // 如果不是空命令
        // 1. 在输出区域显示用户输入的命令（带时间戳）
        outputArea->appendPlainText("[" + getTimestamp() + "] >>> " + cmd);

        // 2. 滚动到最后一行
        QTextCursor cursor = outputArea->textCursor();
        cursor.movePosition(QTextCursor::End);
        outputArea->setTextCursor(cursor);

        // 3. 发射信号，告诉MainWindow有新的命令
        emit commandReady(cmd);

        // 4. 清空输入行，准备接收下一条命令
        inputLine->clear();
    }
}

