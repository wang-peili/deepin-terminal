#ifndef DBUSMANAGER_H
#define DBUSMANAGER_H
#include "service.h"

#include <QObject>
#include <QList>
#include <QVariant>

// deepin-terminal
#define TERMINALSERVER "com.deepin.terminal.reborn"
#define TERMINALINTERFACE "/window"

// kwin dbus
#define KWinDBusService "org.kde.KWin"
#define KWinDBusPath "/KWin"

// deepin Appearance
#define AppearanceService "com.deepin.daemon.Appearance"
#define AppearancePath "/com/deepin/daemon/Appearance"

class DBusManager : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", TERMINALSERVER)
public:
    DBusManager();
    ~DBusManager();
    // 初始化terminal相关dbus
    bool initDBus();

    // kwin dbus
    static int callKDECurrentDesktop();
    static void callKDESetCurrentDesktop(int index);

    // Appearance
    static QStringList callAppearanceFont(QString fontType);

    /******** Add by ut001000 renfeixiang 2020-06-16:增加 调用DBUS的show获取的等宽字体，并转换成QStringList Begin***************/
    //将QJsonArray文件转换成QStringList
    static QStringList converToList(const QString &type, const QJsonArray &array);
    //调用DBUS的SHow方法
    static QStringList callAppearanceShowFont(QStringList fontList, QString fontType);
    /******** Add by ut001000 renfeixiang 2020-06-16:增加 调用DBUS的show获取的等宽字体，并转换成QStringList End***************/

    // deepin terminal
    // 获取能否创建窗口的状态值
    static bool callCreateRequest();
    // 创建窗口
    static void callTerminalEntry(QStringList args);

public slots:
    void entry(QStringList args);
    // 获取状态值
    bool createRequest();

signals:
    // To Service entry function
    void entryArgs(QStringList args);
};

#endif // DBUSMANAGER_H
