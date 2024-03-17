# MyServerDemo
一个服务器，按照规定的协议可用作为客户端的桥接，以使客户端进行通信  
V1.1 更新说明：

1. 支持当某个客户端处于接听状态时，主动断开与对方的通话;
1. 修复了一些问题，如在接收时接收的类改为QByteArray类，以确保音频数据正常



## 协议版本

点击此处打开->[通讯协议](./系统文档/通讯协议.md)

- V2.0 更新说明：

  现在服务器的通讯需要按照上述通讯协议来进行，当前版本暂未进行音频数据的传输，但已经测试可以拨通电话；

- V2.0.1更新：修复问题，当TCP可读数据不足时，应该直接return

- V2.1.0更新说明：

  解决了当同时发送多个帧时只处理一个帧的Bug；

- V2.1.1更新说明：

  <span style="color:red;">下线功能暂时屏蔽，可能会导致程序崩溃！！！！！！</span>

- V2.2更新说明，添加部分可视化界面，可查看当前在线用户的IP地址以及状态

- V2.3更新说明，增加录音功能，当双方开启通讯后会开始录音，但还暂未做测试，增加了外部库；

- V2.4更新说明，增加了通话用户可视化，即可查看当前正在通话的用户，且可以强制中断其通话；

  修复了一些bug;

- V2.5更新说明：根据通讯协议的内容新增许多协议功能，修复了一系列调试过程中遇到的问题，增加帧头检测功能，获取帧头时先进行检测，如果帧头错误则纠正帧头;

  现在可在通话概况中看到拨号是否成功，即看到接听方是否同意接听了；

- V2.5.1更新说明：系统开始会检测是否满足运行环境，如录音文件data文件夹是否存在，不存在则创建该文件夹；

- V2.6.0更新说明：经过调试程序已经可以正常运行，<span style="color:red;">结束通话需要连续发送三次才能结束！</span>，录音功能暂未完全可用，调试窗口暂未可用；

- V2.6.1更新说明：修复了一些小bug（挂断需要连续三次挂断指令）以及可视化日志信息，关闭录音功能；

- V2.6.1.2更新说明：修复闪退bug（关闭录音功能造成的遗留问题）；

- V2.7.0更新说明：添加了一个外部库用于监听功能，但限于不能做测试暂时不继续了；先添加历史记录功能，目前添加了界面和ODBC；

- 2.8.0更新说明：添加了历史记录功能，为该项目增加了ODBC数据库模块，目前历史记录可以查看所有通话记录，以及根据三种不同的方式来查询：

  1. 根据通话时长来查询，可查询一个区间
  2. 根据IP地址来查询
  3. 根据通话开始的时间来查询，可查看一段时间内的通话记录

  代码体量越来越大了，越来越看不懂了，感觉前面写的都是依托答辩，希望不要再改前面的东西了😅😅😅

​		还增加了一个config.ini的配置文件，可以在该配置文件内配置数据库的信息；
