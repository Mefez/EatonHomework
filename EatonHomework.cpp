#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h> 
#include <unordered_map>

using std::vector, std::thread, std::chrono::milliseconds, std::this_thread::sleep_for, std::cout, std::cin, std::ref, std::string, std::unordered_map;

#define port 1234
#define monitor_backlog 5 //maximum number of backlog
#define buffer_size 10 //size of buffer
#define threads_num 4 //number of threads 1 for monitor, the rest for mock device
#define host_ip "127.0.0.1" //ip of local

//Device with unique id and random data, first two character of device_data are unique id of the device
class Device {
    private:
        char device_data[buffer_size];

    public:
        Device(int id)
        {

            for (int i = 0;i < 2;i++)
                
            
            srand(time(0));

            for (int i = 0;i < buffer_size;i++)
            {
                if (i < 2)
                    device_data[i] = (char)(id + 64);
                else
                    device_data[i] = (char) (64 + id + (rand() % 26));
            }
        }

        char* get() { return this->device_data; }
};

//shutdown function called to prevent deadlock on monitor side
void Shutdown(bool& term_flag)
{
    term_flag = true;

    sleep_for(milliseconds(threads_num * 1000));

    char buffer[buffer_size]{};

    //Create Socket
    SOCKET last_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (last_socket < 0)
    {
        cout << "Shutdown Socket ERROR\n";
    }

    //Create Address 
    sockaddr_in last_address;
    inet_pton(AF_INET, host_ip, &last_address.sin_addr.s_addr);
    last_address.sin_port = htons(port);
    last_address.sin_family = AF_INET;


    //Connection
    if (connect(last_socket, (sockaddr*)&last_address, sizeof(last_address)) == SOCKET_ERROR)
    {
        cout << "Shutdown Connecting ERROR\n";
    }

    send(last_socket, buffer, sizeof(buffer), 0);

    shutdown(last_socket, SD_SEND);

    closesocket(last_socket);
}

//Mock_device called in asynchronous thread to mock data sending devices
void Mock_device(int threadID, bool& term_flag)
{
    Device D(threadID);

    srand(time(0));

    while (!term_flag)
    {   
        //Device Generating Data
        sleep_for(milliseconds(1000 * (threadID + rand() % 4)));

        if(!term_flag)
        {
            //Create Socket
            SOCKET device_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (device_socket < 0)
            {
                cout << "Device Socket ERROR\n";
                term_flag = true;
            }

            //Create Address 
            sockaddr_in device_address;
            inet_pton(AF_INET, host_ip, &device_address.sin_addr.s_addr);
            device_address.sin_port = htons(port);
            device_address.sin_family = AF_INET;

            //Connect to Monitor
            if (connect(device_socket, (sockaddr*)&device_address, sizeof(device_address)) == SOCKET_ERROR)
            {
                cout << "Device Connecting ERROR\n";
                term_flag = true;
            }

            //Send Data to Monitor
            send(device_socket, D.get(), sizeof(D.get()), 0);

            //Shutdown the Connection
            shutdown(device_socket, SD_SEND);

            //Close the Socket
            closesocket(device_socket);

            cout << D.get()[0] << D.get()[1] << " sent data\n";
        }
    }

    
        
}

//Monitor called in asynchronous thread to count the messages of mock_device threads
void Monitor(int threadID, bool& term_flag) 
{

    char buffer[buffer_size];
    unordered_map<string, int> message_count;
    bool new_device;

    //Create Socket
    SOCKET monitor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (monitor_socket < 0)
    {
        cout << "Monitor Socket ERROR\n";
        term_flag = true;
    }

    //Create Address 
    sockaddr_in monitor_address;
    inet_pton(AF_INET, host_ip, &monitor_address.sin_addr.s_addr);
    monitor_address.sin_port = htons(port);
    monitor_address.sin_family = AF_INET;

    //Port Binding
    if(bind(monitor_socket, (sockaddr*) &monitor_address, sizeof(monitor_address)) < 0)
    {
        cout << "Monitor Binding ERROR\n";
        term_flag = true;
    }

    //Port Listening
    if (listen(monitor_socket, monitor_backlog) < 0)
    {
        cout << "Monitor Listening ERROR\n";
        term_flag = true;
    }

    while (!term_flag)
    {
        new_device = true;

       //Accept Connection and Receive to buffer
        recv(accept(monitor_socket, nullptr, nullptr), buffer, sizeof(buffer), 0);

        string key = string(buffer, 2);

        if (!term_flag)
        {
            //message count kept in unordered map
            if (message_count.find(key) == message_count.end())
                message_count[key] = 1;
            else
                message_count[key]++;
        }
    }

    //Close the Socket
    closesocket(monitor_socket);

    //Printout Total Number for Each Device
    for (auto& [device, count] : message_count)
    {

        cout << device << " : " << count << '\n';
    }
}

//main function initialize the winsocket, call the threads, shutsdown the threads by using termination flag and terminates winsocket
int main() 
{
    WSADATA wsaData;

    vector<thread> threads;
    bool term_flag = false;

    //Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "WSAStartup ERROR\n";
        return 1;
    }

    //Initialize Threads
    for (int i = 0; i < threads_num; ++i)
    {
        if (i == 0)
            threads.push_back(thread(Monitor, i, ref(term_flag)));
        else
            threads.push_back(thread(Mock_device, i, ref(term_flag)));
    }

    //Wait for Input
    cin.get();

    //Shutdown
    Shutdown(term_flag);

    for (auto& thread : threads)
    {
        thread.join();
    }

    //Terminate Winsock
    WSACleanup();

    return 0;
}