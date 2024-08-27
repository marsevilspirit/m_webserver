# m_netlib

这是我学习《Linux多线程服务端编程：使用muduo C++网络库》这本书所写的一个简易的muduo网络库。目前只能编写最基本的TCP高并发服务器，很多处理异常情况的方法都没有编写。

#### 模块

- Base模块
- Log模块
- Net模块
- Socket模块



#### Base模块



##### Timestamp类

Timestamp 使用 64 位整数来表示时间，单位是微秒（microsecond）。这样可以精确到亚秒级别，而不仅仅是秒级别的时间表示。

这种高精度对于网络编程中的事件计时、日志记录等场景非常重要，能够准确记录事件发生的顺序和间隔。

将时间进行包装，提供成员函数，简化时间比较。



#### Net模块



##### Channel类

Channel类用于管理一个文件描述符，它将文件描述符及该文件描述符对应的回调函数绑定在了一起。

一个Channel只属于一个eventloop，只管理一个fd。

回调函数：

```c++
void setReadCallback(ReadEventCallback cb) {read_callback_ = std::move(cb);}
void setWriteCallback(Eventcallback cb) {write_callback_ = std::move(cb);}
void setCloseCallback(EventCallback cb) {close_callback_ = std::move(cb);}
void setErrorCallback(EventCallback cb) {error_callback_ = std::move(cb);}
```



##### EPoller类

负责监听文件描述符事件是否触发以及返回发生事件的文件描述符以及具体事件的模块就是EPoller。所以一个EPoller对象对应一个事件监听器。在Multi-Reactor模型中，有多少Reactor就有多少EPoller。muduo提供了epoll和poll两种IO多路复用方法来实现事件监听。

一个EPoller只属于一个eventloop，一个EPoller管理很多的Channel。



##### EventLoop类

EventLoop就是负责实现“循环”，负责驱动“循环”的重要模块！Channel和Poller其实相当于EventLoop的手下，EventLoop整合封装了二者并向上提供了更方便的接口来使用。

EventLoop核心逻辑：

```c++
void EventLoop::loop() {
    while (!m_quit) {
        m_activeChannels.clear();
        m_pollReturnTime = m_poller->poll(kPollTimeMs, &m_activeChannels);
        for(auto& ch : m_activeChannels) {
            ch->handleEvent(m_pollReturnTime);
        }
        doPendingFunctors();
    }

    LogTrace("EventLoop {} stop looping", (void*)this);
}
```



##### Acceptor类

Acceptor用于接受新用户连接并分发连接给SubReactor（Sub EventLoop），封装了服务器监听套接字fd以及相关处理方法。

Acceptor 类，只在用于main EventLoop中，与 TcpServer 类配合使用，用来管理客户端连接。

服务器的套接字就在这个类里创建并设置。



##### buffer类

Buffer 类的设计目标是提供一种能够高效处理数据读写、并且尽量减少内存分配与拷贝操作的机制。Buffer 类的内部结构类似于一个动态数组或环形缓冲区，可以自动扩展。

核心成员std::vector<char> 通过vector的强大功能实现智能扩缩。

通过readIndex，和writeIndex控制储存的空间。



##### TcpConnection类

这个类主要封装了一个已建立的TCP连接，以及控制该TCP连接的方法（连接建立和关闭和销毁），以及该连接发生的各种事件（读/写/错误/连接）对应的处理函数，以及这个TCP连接的服务端和客户端的套接字地址信息等。

TcpConnection用于Sub EventLoop中，对连接套接字fd及其相关方法进行封装（读消息事件、发送消息事件、连接关闭事件、错误事件等）。

智能指针管理TcpConnection的原因:

- TcpConnection会和用户直接交互，用户可能会手欠删除。

- TcpConnection对象的多线程安全问题



##### TimerQueue类

虽然TimerQueue中有Queue，但是其实现时基于Set的，而不是Queue，set中存储的是pair类型，那么默认先按照pair的第一个元素排序，如果相同，再按照第二个元素排序。这样可以高效地插入、删除定时器，且找到当前已经超时的定时器。TimerQueue的public接口只有两个，添加和删除。

```c++
void addTimerInLoop(Timer* timer);
void cancelInLoop(TimerId timerId);
```

定时器管理类，其中timer类就是TimerQueue需要管理的元素，而timerId就是一个简单的timer封装，避免销毁和创建操作

但是要注意的是timer并没有自己计时的功能，所以需要依靠timerfd这个系统函数统一计时timerfd是一个系统计时函数，当所设置的时间到了，会通过timerfd这个文件描述符进行提示通信。

timerfd每次都设置在计时器列表中到期时间最近的那个到期时间，这样timerfd到期以后，也就是最近的那个计时器到期，所以每次都是手动重置timerfd的计时时间，为最近的计时器到期时间
