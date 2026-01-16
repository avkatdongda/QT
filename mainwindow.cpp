#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "usb_fun.h"
#include "ConsoleDialog.h" // 新增：包含控制台头文件
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QShortcut>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    isplay(false),
    m_Qimage(nullptr),   // 初始化为空指针
    firstFrame(true),   // 新增：初始化单帧模式为false
    lastFrameTime(0),  // 初始化为nullptr
    consoleDialog(nullptr),
    ui(new Ui::MainWindow),
    singleFrameMode(false)

{

    ui->setupUi(this);
    this->setWindowTitle(tr("MSXBO USB3.0 Video(V1.01)"));

    image_h= 9600;  //1280 9600
    image_v= 6380;   //720  6380
    // isplay = false;
    // firstFrame = true;  // 添加这行初始化
    // lastFrameTime = 0;      // 初始化为0


    ui->H_PIX->setText(QString::number(image_h));
    ui->V_PIX->setText(QString::number(image_v));

    m_QTimer_c2h = new QTimer(this);
    connect(m_QTimer_c2h,SIGNAL(timeout()),this,SLOT(disp_slot()));//时间到了触发disp函数


   ui->pushButton_singleFrame->setText("SingleFrame");


    // USB初始化
    unsigned char *buffer = nullptr;  // 创建缓冲区指针
    if(usb_init(buffer) < 0)  // 传递参数
    {
        QMessageBox::information(this,"ERROR","usb init error");
        return;
    }
    else
    {
        qDebug()<<"usb_open ok";
    }

    // ============ 新增的控制台相关代码 ============

    // 1. 创建控制台对话框（但不立即显示）
    consoleDialog = new ConsoleDialog(this);

    // 2. 连接信号：当控制台有命令时，调用send_custom_command发送
    connect(consoleDialog, &ConsoleDialog::commandReady,
            [](const QString &cmd) {
                qDebug() << "准备发送命令:" << cmd;
                // 调用你的C函数发送命令
                QByteArray cmdstr = cmd.toLatin1();
                const char* cmdshowstr = cmdstr.constData();
                int outstate = send_custom_command(cmdshowstr);
                qDebug()<<"usb_setstate"<<outstate;
            });

    // 3. 添加快捷键 Ctrl+`（反引号键）打开控制台
    // 注意：` 键在键盘左上角，Esc键下面
    QShortcut *consoleShortcut = new QShortcut(QKeySequence("Ctrl+`"), this);
    connect(consoleShortcut, &QShortcut::activated, [this]() {
        qDebug() << "Ctrl+` 被按下，打开控制台";
        consoleDialog->show();           // 显示窗口
        consoleDialog->raise();          // 提到最前面
        consoleDialog->activateWindow(); // 激活窗口
    });


    // 或者直接在菜单栏添加（可选）
    QMenu *toolsMenu = ui->menubar->addMenu("工具(&T)");
    QAction *consoleAction = toolsMenu->addAction("打开命令控制台");
    connect(consoleAction, &QAction::triggered, this, &MainWindow::openConsole);


}

// 打开控制台的槽函数
void MainWindow::openConsole()
{
    if (consoleDialog) {
        consoleDialog->show();
        consoleDialog->raise();
        consoleDialog->activateWindow();
    }
}




MainWindow::~MainWindow()
{

    // 停止定时器
    if (m_QTimer_c2h) {
        m_QTimer_c2h->stop();
        delete m_QTimer_c2h;
    }

    // 清理图像
    if (m_Qimage) {
        delete m_Qimage;
        m_Qimage = nullptr;
    }

    //反初始化
    usb_deinit();


    // 清理控制台
    if (consoleDialog) {
        delete consoleDialog;
        consoleDialog = nullptr;
    }


    delete ui;
}


// ============ 修改的单帧捕获函数 ============
void MainWindow::on_pushButton_singleFrame_clicked()
{
    qDebug() << "单帧捕获按钮点击，当前播放状态:" << isplay;

    // 如果正在播放，先停止
    if (isplay) {
        qDebug() << "正在播放，先停止播放模式";
        on_pushButton_clicked();  // 调用停止函数
    }

    // 获取分辨率
    image_h = ui->H_PIX->text().toUInt();
    image_v = ui->V_PIX->text().toUInt();

    // 清理旧的图像对象
    if (m_Qimage) {
        delete m_Qimage;
        m_Qimage = nullptr;
    }

    // 创建新的图像对象
    m_Qimage = new QImage(c2h_align_mem_tmp, image_h, image_v, QImage::Format_Grayscale16);
    if (!m_Qimage || m_Qimage->isNull()) {
        qDebug() << "创建QImage失败！";
        QMessageBox::warning(this, "错误", "创建图像对象失败！");
        return;
    }

    // 设置单帧模式并执行一次捕获
    singleFrameMode = true;
    qDebug() << "开始单帧捕获...";
    disp_slot();  // 这会自动重置singleFrameMode

    qDebug() << "单帧捕获完成";
}



// ============ 修改的disp_slot函数 ============
void MainWindow::disp_slot()
{
    // 检查是否有有效的图像对象
    if (!m_Qimage || m_Qimage->isNull()) {
        qDebug() << "disp_slot: 无有效的图像对象";
        return;
    }

    unsigned int len = image_h * image_v * 2;
    int fps = c2h_transfer(len);  // 读取图像数据

    // 更新FPS显示
    QString fpsText;
    if (singleFrameMode) {
        fpsText = QString("单帧: %1 fps").arg(fps);
        qDebug() << "单帧捕获，FPS:" << fps;
    } else {
        fpsText = QString("连续: %1 fps").arg(fps);

        // 计算实际FPS（仅连续模式）
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (lastFrameTime > 0) {
            qint64 frameInterval = currentTime - lastFrameTime;
            double actualFps = 1000.0 / frameInterval;
            qDebug() << "连续模式 - 原始FPS:" << fps
                     << "实际FPS:" << QString::number(actualFps, 'f', 1)
                     << "间隔:" << frameInterval << "ms";
        }
        lastFrameTime = currentTime;
    }
    ui->TEXT_FPS->setText(fpsText);

    // 显示图像
    QPixmap pixmap = QPixmap::fromImage(*m_Qimage, Qt::AutoColor);
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(ui->label->width()/8*8,
                               ui->label->height()/8*8,
                               Qt::IgnoreAspectRatio,
                               Qt::FastTransformation);
        ui->label->setPixmap(pixmap);
    } else {
        qDebug() << "转换QPixmap失败！";
    }

    // 单帧模式：清理并重置状态
    if (singleFrameMode) {
        qDebug() << "单帧模式结束，清理资源";
        singleFrameMode = false;

        // 注意：这里不要删除m_Qimage，因为可能还要显示
        // 而是在下次操作时清理
    }
}




// ============ 修改的播放/停止函数 ============
void MainWindow::on_pushButton_clicked()
{
    if (!isplay) {
        // 切换到播放模式
        qDebug() << "切换到播放模式";

        // 确保不在单帧模式
        singleFrameMode = false;

        // 获取分辨率
        image_h = ui->H_PIX->text().toUInt();
        image_v = ui->V_PIX->text().toUInt();

        // 清理旧的图像对象
        if (m_Qimage) {
            delete m_Qimage;
            m_Qimage = nullptr;
        }

        // 创建新的图像对象
        m_Qimage = new QImage(c2h_align_mem_tmp, image_h, image_v, QImage::Format_Grayscale16);
        if (!m_Qimage || m_Qimage->isNull()) {
            qDebug() << "创建播放模式QImage失败！";
            QMessageBox::warning(this, "错误", "创建图像对象失败！");
            return;
        }

        // 重置时间
        lastFrameTime = 0;

        // 启动定时器
        m_QTimer_c2h->start(600);
        ui->pushButton->setText("STOP");
        isplay = true;

        qDebug() << "播放模式开始";
    } else {
        // 停止播放
        qDebug() << "停止播放模式";

        m_QTimer_c2h->stop();
        isplay = false;

        // 清理图像
        if (m_Qimage) {
            delete m_Qimage;
            m_Qimage = nullptr;
        }

        ui->pushButton->setText("PLAY");
        ui->TEXT_FPS->clear();
        ui->label->clear();

        qDebug() << "播放模式停止";
    }
}

