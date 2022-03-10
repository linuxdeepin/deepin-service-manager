## DDE服务管理框架

DDE服务管理框架，用于加载放置在指定目录下的动态库(即插件)



### 插件加载流程：

1、进程启动后遍历指定插件的目录下(系统级：/usr/lib/dde-plugins/system， 用户级：/usr/lib/dde-plugins/session)的所有动态库文件

2、检测到动态库文件后，按照检测顺序逐个进行解析

3、优先通过Info接口获取插件的meta信息，从而决定是立即加载、拒绝加载、延迟加载

4、注册DBus管理服务，方便通过DBus接口加载插件、卸载插件、查看插件加载情况等



### 启动方式：

此框架的进程在系统中一般有一个系统级进程以及和登陆用户数量相关的用户级进程



root权限进程将在系统启动后被systemd拉起，

对应的service配置文件为：/usr/lib/systemd/system/dde-service-manager-system.service



用户权限进程在进入桌面后，被startdde通过启动/etc/xdg/autostart/dde-service-manager.desktop文件拉起

启动后，将执行systemctl --user enable dde-service-manager-session.service --now，用于进程意外停止后被自动拉起

对应的service配置文件为：/usr/lib/systemd/user/dde-service-manager-session.service



### 插件固定接口：

插件接口需要以C语言风格进行提供

基础接口（必须提供）：

| 接口版本 | 接口                                | 说明                                                         | 备注                                                         |
| :------: | ----------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
|   1.0    | bool Start();                       | 调用后，插件开始工作                                         | 当插件的Info文件中未限定依赖的服务时，框架会在启动后直接调用Start接口，否则会等待对应的服务启动后才会调用<br>插件也可在Start被调用后，实现自己的等待逻辑，待对应条件达成后，再执行真正的初始化工作 |
|   1.0    | bool Stop();                        | 调用后，插件停止工作，插件应销毁自己管理的资源               | 插件应该在Stop后，释放所有资源，包括自己分配的内存、打开的文件等、创建的线程等 |
|   1.0    | const char *Info();                 | 插件加载器可以通过此接口获取插件的基本信息，其格式将在下面进行介绍 |                                                              |
|   1.0    | void UnloadCallBack(UnloadFun fun); | 注：typedef void (*UnloadFun)(const char *);<br>插件取消运行的回调函数，如果有实现此接口，并在合适的时机调用，框架会卸载此插件 | 意味着插件可以决定自己是否要卸载，而非被动等待               |

Info接口的数据为一段json格式的文本，其字段格式如下(name和version字段是必须的，否则可能拒绝加载)：

```json
{
name: "clipboard",
service: "",
session_type: 1,
version: 1.0
}
```

| 字段         | 说明               | 备注                                                         |
| ------------ | ------------------ | ------------------------------------------------------------ |
| name         | 插件名             | 插件框架用于标识插件，需确保和其他插件名不一致，否则可能拒绝加载 |
| service      | 插件依赖的dbus服务 | 此服务注册时，插件框架才会调用插件的Start接口。如果为空，表示不依赖任何服务 |
| session_type | 以来的服务级别     | 1:session;0:system;其他值均无效，和service字段结合使用       |
| version      | 插件的接口版本     | 当前只有1.0，不符合的版本将会拒绝加载                        |



### 插件管理DBus：

无论是系统还是用户级别的DBus服务，其服务信息如下：

Bus Name:com.deepin.dde.ServiceManager

Path:/com/deepin/dde/ServiceManager

Interface:com.deepin.dde.ServiceManager



#### 接口说明

| 接口                                            | 说明                                                         | 备注 |
| ----------------------------------------------- | ------------------------------------------------------------ | ---- |
| Method:                                         |                                                              |      |
| IsRunning (String pluginName) ↦ (Boolean arg_0) | pluginName:插件的文件名，例如libtest.so的文件名为‘‘libtest’’.<br>如果插件成功加载并运行则返回true<br>否则返回false |      |
| Load (String pluginName) ↦ ()                   | pluginName:加载指定插件。<br>插件需要提前放置在对应的插件目录中 |      |
| PluginList () ↦ (Array of [String] arg_0)       | 返回所有加载成功的插件文件名                                 |      |
| UnLoad (String pluginName) ↦ ()                 | 卸载指定插件                                                 |      |
| Signals:                                        |                                                              |      |
| PluginLoaded(String)                            | 插件加载成功的信号，参数为对应的插件名                       |      |
| PluginUnLoaded(String)                          | 插件卸载成功的信号，参数为对应的插件名                       |      |



**附：**

**被调用时加载插件**：

创建dbus的service文件，并放入/usr/share/dbus-1/services/目录下

文件内容如下(需要根据实际插件进行修改)：

```ini
[D-BUS Service]
Name=com.deepin.dde.ClipboardLoader
Exec=/usr/bin/qdbus com.deepin.dde.ServiceManager /com/deepin/dde/ServiceManager com.deepin.dde.ServiceManager.Load 'libclipboard.so'
```



**一段时间无操作后卸载插件：**

可通过在Info接口返回的配置文件中添加autoexit=true来设置自动退出，默认此值为false

自动退出的时长默认为30s，当然，你也可以手动指定时长，delay-quit=60



#### 建议：

我们特别建议您的插件有专属的线程去运行，这对整体进程的运行将有较大的好处。
