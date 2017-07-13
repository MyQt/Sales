#include "mainwindow.h"
#include "singleinstance.h"

#include <QApplication>
#include <QFontDatabase>
#include <QException>
#include <Windows.h>
#include <QMessageBox>

#pragma execution_character_set("utf-8")

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException){//程式异常捕获
    /*
      ***保存数据代码***
    */
    //这里弹出一个错误对话框并退出程序
    EXCEPTION_RECORD* record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode,16)),errAdr(QString::number((uint)record->ExceptionAddress,16)),errMod;
    QMessageBox::critical(NULL,"程式崩溃","<FONT size=4><div><b>对于发生的错误，表示诚挚的歉意</b><br/></div>"+
        QString("<div>错误代码：%1</div><div>错误地址：%2</div></FONT>").arg(errCode).arg(errAdr),
        QMessageBox::Ok);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
//    QApplication::setDesktopSettingsAware(false);
    QApplication app (argc, argv);

    // Set application information
    app.setApplicationName ("金花布业销售清单系统");
    app.setApplicationVersion ("1.0.0");

    // Load fonts from resources
    QFontDatabase::addApplicationFont (":/fonts/arimo/Arimo-Regular.ttf");
    QFontDatabase::addApplicationFont (":/fonts/roboto-hinted/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont (":/fonts/roboto-hinted/Roboto-Medium.ttf");
    QFontDatabase::addApplicationFont (":/fonts/roboto-hinted/Roboto-Regular.ttf");

    // Prevent many instances of the app to be launched
    QString name = "com.yuanmu.sales";
    SingleInstance instance;
    if (instance.hasPrevious (name)){
        return EXIT_SUCCESS;
    }

    instance.listen (name);

    // Create and Show the app
    MainWindow w;
    w.setWindowTitle("金花布业销售清单系统");
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注冊异常捕获函数

    w.show();

    // Bring the Notes window to the front
    QObject::connect(&instance, &SingleInstance::newInstance, [&](){
        (&w)->setMainWindowVisibility(true);
    });

    return app.exec();
}
