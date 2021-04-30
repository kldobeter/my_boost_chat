这是我的测试项目
1.使用boost.asio来实现群聊的聊天室功能
2.使用boost.log来实现日志
3.使用boost来实现消息序列化
4.也提供json或protobuf的方式，实现消息的传输
5.boost::asio::io_service不保证线程安全，所以通过boost::asio::io_service::strand 来保证功能按照一定顺序执行
