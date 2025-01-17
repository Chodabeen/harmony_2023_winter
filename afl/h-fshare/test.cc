#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "fshare.h"

// class using crude send function
class Test {
public:
    virtual ssize_t send(int sockfd, const void * buf, size_t len, int flags) = 0;
    virtual ssize_t recv(int sockfd, void * buf, size_t len, int flags) = 0;
};

// Mock Send class
class MockTest : public Test {
    using Test::send;
    using Test::recv;
public:
    MOCK_METHOD4(send, ssize_t(int sockfd, const void * buf, size_t len, int flags));
    MOCK_METHOD4(recv, ssize_t(int sockfd, void * buf, size_t len, int flags));
};


TEST(SEND_BYTES, SEND_BYTES_1) {
    char * buf = strdup("i hate pohang\nplease leave me alone....\nI wanna go tokyo\n");
    EXPECT_EXIT(
        {
            send_bytes(1, buf, strlen(buf));
            exit(0);
        },
        ::testing::ExitedWithCode(0), ""
    );
}


TEST(GET_CMD_CODE, GET_CMD_CODE_1) {
    char * buf1 = strdup("list");
    char * buf2 = strdup("get");
    char * buf3 = strdup("put");
    char * buf4 = strdup("hi");
    EXPECT_EXIT(
        {
            get_cmd_code(buf1);
            get_cmd_code(buf2);
            get_cmd_code(buf3);
            get_cmd_code(buf4);
            exit(0);
        },
        ::testing::ExitedWithCode(0), ""
    );
}


TEST(GET_OPTION, GET_OPTION_1) {
    char * list[] = {strdup("./fshare"), strdup("127.0.0.1:8080"), strdup("list"), NULL};
    char * get[] = {strdup("./fshare"), strdup("127.0.0.1:8080"), strdup("get"), strdup("../files/hello.txt"), strdup("get_dest"), NULL};
    char * put[] = {strdup("./fshare"), strdup("127.0.0.1:8080"), strdup("put"), strdup("../put_src/hi.txt"), strdup("files"), NULL};
    char * non[] = {strdup("./fshare"), NULL};
    char * i[] = {strdup("./fshare"), strdup("-i"), NULL};
    EXPECT_EXIT(
        {
            get_option(sizeof(list) / sizeof(list[0]) - 1, list);
            get_option(sizeof(get) / sizeof(get[0]) - 1, get);
            get_option(sizeof(put) / sizeof(put[0]) - 1, put);
            get_option(sizeof(non) / sizeof(non[0]) - 1, non);
            get_option(sizeof(i) / sizeof(i[0]) - 1, i);
            exit(0);
        },
        ::testing::ExitedWithCode(0), ""
    );
}


TEST(MAKE_DIRECTORY, MAKE_DIRECTORY_1) {
    char * dir = strdup("../f0/f1/f2");
    EXPECT_EXIT(
        {
            make_directory(dir);
            exit(0);
        },
        ::testing::ExitedWithCode(0), ""
    );
}


MockTest* mockTest;
    

ssize_t send(int sockfd, const void* buf,  size_t len, int flags) {
    // printf("call send\n");
    return mockTest->send(sockfd, buf, len, flags);
    // return write(sockfd, buf, len);
}

ssize_t recv(int sockfd, void * buf, size_t len, int flags) {
    printf("call recv\n");
    // return mockTest->recv(sockfd, buf, len, flags);
    return read(sockfd, buf, len);
}

TEST(REQUEST, REQUEST_1) {

    mockTest = new MockTest();
    char * list[] = {strdup("./fshare"), strdup("127.0.0.1:8080"), strdup("list"), NULL};
    char * get[] = {strdup("./fshare"), strdup("127.0.0.1:8080"), strdup("get"), strdup("../files/hello.txt"), strdup("get_dest"), NULL};
    char * put[] = {strdup("./fshare"), strdup("127.0.0.1:8080"), strdup("put"), strdup("../put_src/hi.txt"), strdup("files"), NULL};

    
    // ON_CALL(*mockSend, send(testing::_, testing::_, testing::_, testing::_))
    //     .WillOnce(testing::Return(5));
                    
    // int result = request(1);
    // EXPECT_EQ(result, 5);
    
    EXPECT_EXIT(
        {
            // consider there is no error in get_option/request
            // list
            get_option(sizeof(list) / sizeof(list[0]) - 1, list);
            request(1);
            // get
            get_option(sizeof(get) / sizeof(get[0]) - 1, get);
            request(1);
            // put
            get_option(sizeof(put) / sizeof(put[0]) - 1, put);
            request(1);
            exit(0);
        },
        ::testing::ExitedWithCode(0), ""
    );
    delete mockTest;
}


TEST(RECEIVE_RESPONSE, RECEIVE_RESPONSE_1) {
 
    server_header list_sh1, list_sh2, get_sh;
    
    // payload: list
    list_sh1.is_error = 0;
    list_sh1.payload_size = 4;
    char * list_buf1 = strdup("list");
    // payload: listlist
    list_sh2.is_error = 0;
    list_sh2.payload_size = 513;
    char list_buf2 = (char*)malloc(sizeof(char) * 513);
    strcpy(list_buf2, "listlist");

    
    // // payload: getget
    // get_sh.is_error = 0;
    // get_sh.payload_size = 513;
    // char * get_buf = (char*)malloc(sizeof(char) * 513);
    // // char get_buf[513];
    // strcpy(get_buf, "getget");


    // write list_t file which is smaller than 512
    int list_fd1 = open("../recv/list_t", O_CREAT | O_WRONLY);
    write(list_fd1, &list_sh1, sizeof(list_sh1));
    write(list_fd1, list_buf1, sizeof(list_buf1));
    close (list_fd1);
    // write list_f file which is bigger than 512
    int list_fd2 = open("../recv/list_f", O_CREAT | O_WRONLY);
    write(list_fd2, &list_sh2, sizeof(list_sh2));
    write(list_fd2, list_buf2, sizeof(list_buf2));
    close (list_fd2);

    
    // // write get_f file which is bigger than 512
    // int get_fd = open("../recv/get_f", O_CREAT | O_WRONLY);
    // write(get_fd, &get_sh, sizeof(get_sh));
    // write(get_fd, get_buf, strlen(get_buf));
    // close (get_fd);

    mockTest = new MockTest();

    char * list[] = {strdup("./fshare"), strdup("127.0.0.1:8080"), strdup("list"), NULL};
    char * get[] = {strdup("./fshare"), strdup("127.0.0.1:8080"), strdup("get"), strdup("../files/hello.txt"), strdup("../get_dest"), NULL};
    char * put[] = {strdup("./fshare"), strdup("127.0.0.1:8080"), strdup("put"), strdup("../put_src/hi.txt"), strdup("files"), NULL};
    
    EXPECT_EXIT(
        {
            // consider there is no error in get_option
            /* list */
            get_option(sizeof(list) / sizeof(list[0]) - 1, list);

            int R_list_fd1 = open("../recv/list_t", O_RDONLY);
            receive_response(R_list_fd1);
            close (R_list_fd1);

            int R_list_fd2 = open("../recv/list_f", O_RDONLY);
            receive_response(R_list_fd2);
            close (R_list_fd2);
            /* list */

            /* get */
            // get_option(sizeof(get) / sizeof(get[0]) - 1, get);

            // int R_get_fd = open("../recv/get_f", O_RDONLY);
            // receive_response(R_get_fd);
            // close (R_get_fd);
            /* get */

            // put

            // remove files
            unlink("../recv/list_t");
            unlink("../recv/list_f");
            unlink("../recv/get_f");
            
            exit(0);
        },
        ::testing::ExitedWithCode(0), ""
    );

    
    delete mockTest;
}



int main(int argc, char * argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) = "RECEIVE_RESPONSE.RECEIVE_RESPONSE_1";
    return RUN_ALL_TESTS();
    
}
