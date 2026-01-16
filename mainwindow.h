#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QThread>
#include <QMessageBox>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QTimer>
#include <QDateTime>
#include <QMainWindow>
#include <QFile>
#include <QMainWindow>
#include <QDateTime>
#include <QElapsedTimer>

// 前向声明，告诉编译器有ConsoleDialog这个类，但不需要知道细节
class ConsoleDialog;


namespace Ui {
class MainWindow;
}

// 声明C函数
#ifdef __cplusplus
extern "C" {
#endif

// 这些函数在你的 usb_fun.h 中定义
int send_custom_command(const char* hex_str);
int usb_init(unsigned char *buffer);
void usb_deinit();
unsigned int c2h_transfer(unsigned int size);

#ifdef __cplusplus
}
#endif




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



public:

    bool isplay;
    unsigned int image_h;
    unsigned int image_v;

    QTimer *m_QTimer_c2h;
    QImage *m_Qimage;

    QElapsedTimer frameTimer;  // 添加这个
    bool firstFrame;           // 添加这个
    qint64 lastFrameTime;        // 或者用这个替代 frameTimer

    // 新增：控制台对话框指针
    ConsoleDialog *consoleDialog;



private slots:
    void disp_slot();                 // 你已有的显示函数

    void on_pushButton_singleFrame_clicked();  // 新增：单帧捕获按钮槽函数
    void on_pushButton_clicked();    // 你已有的播放/停止按钮
//    void processAndReschedule();  // 添加这行声明

    // 新增：打开控制台的槽函数
    void openConsole();


private:
    Ui::MainWindow *ui;
    int i = 0;///


    bool singleFrameMode;  // 新增：单帧模式标志
    QPushButton *btnSingleFrame;  // 新增：单帧捕获按钮

};

#endif // MAINWINDOW_H
