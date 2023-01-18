#ifndef POLICY_H
#define POLICY_H

#include <QMap>
#include <QObject>

// TODO :namespace

struct PolicyWhitelist {
    QString name;
    QStringList process;
};
typedef QMap<QString, PolicyWhitelist> QMapWhitelists;
// hide, default:false
typedef QMap<QString, bool> QMapPathHide;
// subpath, default:false
typedef QMap<QString, bool> QMapSubPath;
// typedef QMap<QString, bool> QMapProcess;
struct PolicyMethod {
    QString method;
    bool needPermission;
    QStringList processes;
};
typedef QMap<QString, PolicyMethod> QMapMethod;
struct PolicyProperty {
    QString property;
    bool needPermission;
    QStringList processes;
};
typedef QMap<QString, PolicyProperty> QMapProperty;
struct PolicyInterface {
    QString interface;
    bool needPermission;
    QStringList processes;
    QMapMethod methods;
    QMapProperty properties;
};
typedef QMap<QString, PolicyInterface> QMapInterface;
struct PolicyPath {
    QString path;
    bool needPermission;
    QStringList processes;
    QMapInterface interfaces;
};
typedef QMap<QString, PolicyPath> QMapPath;

enum CallDestType { Method, Property };

class Policy : public QObject
{
    Q_OBJECT
public:
    explicit Policy(QObject *parent = nullptr);

    void ParseConfig(const QString &path);

    bool CheckPathHide(const QString &path);
    bool CheckMethodPermission(const QString &process,
                               const QString &path,
                               const QString &interface,
                               const QString &method);
    bool CheckPropertyPermission(const QString &process,
                                 const QString &path,
                                 const QString &interface,
                                 const QString &property);
    bool CheckPermission(const QString &process,
                         const QString &path,
                         const QString &interface,
                         const QString &dest,
                         const CallDestType &type);

    //    void Check(); // TODO
    void Print();

private:
    bool readJsonFile(QJsonDocument &outDoc, const QString &fileName);
    bool parseWhitelist(const QJsonObject &obj);
    bool parsePolicy(const QJsonObject &obj);
    bool parsePolicyPath(const QJsonObject &obj);
    bool parsePolicyInterface(const QJsonObject &obj, PolicyPath &policyPath);
    bool parsePolicyMethod(const QJsonObject &obj,
                           PolicyInterface &policyInterface);
    bool parsePolicyProperties(const QJsonObject &obj,
                               PolicyInterface &policyInterface);

    bool jsonGetString(const QJsonObject &obj,
                       const QString &key,
                       QString &value,
                       QString defaultValue = "");
    bool jsonGetBool(const QJsonObject &obj,
                     const QString &key,
                     bool &value,
                     bool defaultValue = false);

public:  // TODO
    // 数据定义
    // 插入速度要求不高，查询速度要求很高，因此解析json的结果会通过冗余、预处理来提高查询速度，即以查询的速度角度来定义结构
    // 配置文件和此处数据没有一一对应，解析文件时，需要为此处数据服务，填充相关数据
    // 如果文件没有配置某参数，那也不允许空值，根据数据向下继承，比如某个参数interface没有设置，则interface的这个参数继承path
    // 不允许空值的作用是减少查询时的逻辑判断，把这些逻辑处理转移到解析文件阶段
    // 隐藏 - In:path，Out:pathhide
    // 权限PATH - In:process、path，Out：bool
    // 权限INTERFACE - In:process、path、interface，Out：bool
    // 权限METHOD - In:process、path、interface、method，Out：bool
    // 权限PROPERTIES - In:process、path、interface，Out：bool
    QMapWhitelists m_mapWhitelist;
    QMapPathHide m_mapPathHide;
    QMapSubPath m_mapSubPath;
    QMapPath m_mapPath;

public:  // TODO
    QString m_name;
    QString m_group;
    QString m_libPath;
    QString m_policyVersion;
    QString m_policyStartType;
};

#endif  // POLICY_H
