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
#include <cstddef>
#include <cstdlib>

#include <at_cmd.h>
#include <at_notif.h>
#include <kernel.h>
#include <net/socket.h>

#include "examples/socket/socket.h"
#include "examples/utils.h"

using namespace NimbeLink::Examples;

/**
 * \brief Zephyr thread handler
 *
 * \param *arg1
 *      A pointer to our socket object
 * \param *arg2
 *      Unused
 * \param *arg3
 *      Unused
 *
 * \return none
 */
void Socket::Handler(void *arg1, void *arg2, void *arg3)
{
    (void)arg2;
    (void)arg3;

    Socket *socket = static_cast<Socket *>(arg1);

    if (socket == nullptr)
    {
    #   if CONFIG_SOCKET_DEBUG
        printk("returning nullptr\n");
    #   endif

        return;
    }

    socket->Run();
}

/**
 * \brief Creates a new socket instance and thread
 *
 * \param none
 *
 * \return none
 */
Socket::Socket(void)
{
    // Create our thread
    this->threadId = k_thread_create(
        &this->thread,
        this->stack,
        std::size(this->stack),
        Socket::Handler,
        static_cast<void *>(this),
        nullptr,
        nullptr,
        K_LOWEST_APPLICATION_THREAD_PRIO,
        0,
        0
    );
}

/**
 * \brief Initializes AT communications
 *
 * \param none
 *
 * \return 0
 *      Success
 * \return <0
 *      Error
 */
int Socket::AtCommsInit(void)
{
    int err = at_cmd_init();

    if (err)
    {
        printk("Failed to initialize AT commands, err %d\n", err);

        return err;
    }

    err = at_notif_init();

    if (err)
    {
        printk("Failed to initialize AT notifications, err %d\n", err);

        return err;
    }

    return 0;
}

/**
 * \brief Wrapper for sending AT commands
 *
 * \param command
 *      The command as a string
 * \param length
 *      The length to make the response buffer that is passed along with the
 *      command
 * \param expected
 *      The expected response from the command
 *
 * \return 0
 *      Successful
 * \return 1
 *      Command failed or returned what was not expected
 */
uint8_t Socket::SendCommand(const char *command, int length, const char *expected)
{
    char resp[length];
    NimbeLink::Sdk::SecureServices::At::Result result;
    NimbeLink::Sdk::SecureServices::At::Error error;

    int32_t ret = NimbeLink::Sdk::SecureServices::At::RunCommand(&result, &error, command, strlen(command), resp, length, nullptr);

#   if CONFIG_SOCKET_DEBUG
    printk("Command: %s\n", command);
    printk("ret: %d\n", ret);
#   endif

    // If the AT command was not successfully processed, stop
    if (ret != 0)
    {
        return 1;
    }

    // If the AT command failed, stop
    if (result != 0)
    {
    #   if CONFIG_SOCKET_DEBUG
        NimbeLink::Examples::Utils::PrintError(std::string_view(command), result, error);
    #   endif

        return 1;
    }

    // If the AT command succeeded, but the response was not what was expected, stop
    if (std::string_view(resp).find(std::string_view(expected)))
    {
    #   if CONFIG_SOCKET_DEBUG
        printk("Expected %s, recieved %s\n", expected, resp);
    #   endif

        return 1;
    }

    // The AT command succeeded and the response was what we expected
#   if CONFIG_SOCKET_DEBUG
    printk("recieved %s\n", resp);
#   endif

    return 0;
}

/**
 * \brief sets up the socket
 *
 * \param none
 *
 * \return file descriptor of the socket if successful, -1 if not
 */
int Socket::SetSocketUp(void)
{
    // Struct that contains the AT command and the desired response for the
    // command check connection
    struct
    {
        std::string_view command;
        std::string_view expected;
    } commands[] = {
        {"AT+CFUN?","+CFUN: 1"},
        {"AT+CEREG?","+CEREG: 0,1"},
    };

    // Iterate through the struct and execute the commands with a 5 second
    // pause between commands
    for (std::size_t i = 0; i < std::size(commands); i++)
    {
        k_sleep(5000);

        if (SendCommand(std::data(commands[i].command), 100, std::data(commands[i].expected)) != 0)
        {
            return -1;
        }
    }

    // Set up socket
    struct addrinfo *res;
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    int err = getaddrinfo(std::data("dweet.io"), NULL, &hints, &res);

    if (err)
    {
        printk("unable to get address info, err %d\n", err);
    }

    ((struct sockaddr_in *)res->ai_addr)->sin_port = htons(HTTP_PORT);

    int socketId = socket(AF_INET, SOCK_STREAM, 0);

    if (socketId < 0)
    {
        printk("Unable to open socket\n");

        return socketId;
    }

    err = connect(socketId, res->ai_addr, sizeof(struct sockaddr_in));

    if (err != 0)
    {
        printk("Unable to connect to dweet.io, err %d\n", err);

        return err;
    }

    return socketId;
}

/**
 * \brief Runs our socket example
 *
 * \param none
 *
 * \return none
 */
void Socket::Run(void)
{
    while (true)
    {
        // For each widget that wants to be posted on the web, create a socket
        // for it
        for (std::size_t i = 0; i < std::size(datas); i++)
        {
            // Check if there is actually a widget at this index and above
            if (i >= this->registered)
            {
                break;
            }

            // Get the data that wants to be posted
            char data[MAX_TRANSMISSION];

            this->datas[i]->Retrieve(data, MAX_TRANSMISSION);

        #   if CONFIG_SOCKET_DEBUG
            printk("data: %s\n", data);
        #   endif

            // Set up the HTTP request ("POST ... HTTP/1.1\r\n\r\n<payload>")
            char post[] = "POST /dweet/for/skywire_nano_socket_dial_widget";

            uint32_t payload_length = std::size(post) + std::size(" HTTP/1.1\r\n\r\n") + strlen(data);
            char payload[payload_length];

            snprintf(payload, payload_length, "%s?%s HTTP/1.1\r\n\r\n", post, data);

        #   if CONFIG_SOCKET_DEBUG
            printk("payload: %s", payload);
        #   endif

            int send_sock = this->SetSocketUp();

            send(send_sock, payload, payload_length, 0);

            struct pollfd sock[1];

            sock[0].fd = send_sock;
            sock[0].events = POLLIN;

            if (poll(sock, 1, -1) < 0)
            {
                printk("Invalid poll return value\n");
            }

        #   if CONFIG_SOCKET_DEBUG
            if (sock[0].revents & POLLIN)
            {
                char buffer[1024];
                int valread = recv(send_sock, buffer, 1024, 0);

                printk("Read %d, output:\n%s\n", valread, buffer);
            }
        #   endif

            close(send_sock);
        }

        // Wait until we're ready to send data again
        k_sleep(K_SECONDS(CONFIG_SOCKET_POST_RATE));
    }
}

/**
 * \brief Registers a widget that wants to be posted
 *
 * \param &data
 *      The widget that wants to be posted and subclasses the Data class
 *
 * \return 0
 *      Succes
 * \return 1
 *      Full
 */
bool Socket::RegisterData(Data &data)
{
    if (this->size >= std::size(this->datas))
    {
        return 1;
    }

    this->datas[this->size++] = &data;
    this->registered++;

    return 0;
}
