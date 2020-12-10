/**
 * \file
 *
 * \brief A basic 'socket dial' applet
 *
 * (C) NimbeLink Corp. 2020
 *
 * All rights reserved except as explicitly granted in the license agreement
 * between NimbeLink Corp. and the designated licensee.  No other use or
 * disclosure of this software is permitted. Portions of this software may be
 * subject to third party license terms as specified in this software, and such
 * portions are excluded from the preceding copyright notice of NimbeLink Corp.
 */
#pragma once

#include <cstddef>
#include <cstdlib>

#include <kernel.h>
#include <net/socket.h>
#include <net/net_context.h>
#include <zephyr.h>

#include <string>

#include "nimbelink/sdk/secure_services/at.h"

#define HTTP_PORT 80
#define HTTP_HEAD                                                   \
    "POST /dweet/for/skywire_nano_socket_dial_widget HTTP/1.1\r\n"  \
    "Host: dweet.io:80\r\n"                                         \
    "Connection: close\r\n\r\n"
#define HTTP_HEAD_LEN (sizeof(HTTP_HEAD) - 1)
#define HTTP_HDR_END "\r\n\r\n"
#define RECV_BUF_SIZE 2048
#define MAX_TRANSMISSION 2000

namespace NimbeLink::Examples
{
    class Socket;
}

class NimbeLink::Examples::Socket
{
    public:
        // Interface class for classes that want their data to be posted to the web
        //
        // This enforces overriding Retrieve, which populates a string with how
        // the widget wants to be posted to the web.
        class Data
        {
            public:
                virtual void Retrieve(char *buffer, uint16_t max_length) = 0;
        };

    private:
        // Our Zephyr stack
        //
        // C++ isn't a big fan of placing a class' member in a section, so
        // we'll reproduce what the Zephyr library does when someone uses
        // K_THREAD_STACK_DEFINE().
        __attribute__((aligned(STACK_ALIGN))) struct _k_thread_stack_element stack[4096 + MPU_GUARD_ALIGN_AND_SIZE];

        // Our Zephyr thread
        struct k_thread thread;

        // Our thread's ID
        k_tid_t threadId;

        // List of objects that inherit from Data that want to be posted to the
        // web
        Data *datas[10];

        uint8_t size = 0;
        uint8_t registered = 0;

    private:
        static void Handler(void *arg1, void *arg2, void *arg3);

        uint8_t SendCommand(const char *command, int length, const char *expected);
        int SetSocketUp(void);

        void Run(void);

        int AtCommsInit(void);

    public:
        Socket(void);

        bool RegisterData(Data &data);
};
