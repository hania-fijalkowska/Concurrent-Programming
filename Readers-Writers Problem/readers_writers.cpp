#include "readers_writers.h"
#include <iostream>
#include <thread>
#include <chrono>

data::data()
        : Value(0), Data_Written(false), Stop(false),
          Current_Readers(0), Reads_After_Write(0) {}

void data::Read(int readerId) {
    std::unique_lock<std::mutex> lock(Main_Mutex);

    Can_Read_CV.wait(lock, [this]() {
        return Data_Written && !Stop;
    });

    ++Current_Readers;

    lock.unlock();

    std::cout << "Reader " << readerId << " is reading value: " << Value << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    lock.lock();
    --Current_Readers;
    ++Reads_After_Write;

    if (Reads_After_Write >= 3 && Current_Readers == 0) {
        Can_Write_CV.notify_one();
    }

    lock.unlock();
}

void data::Write(int writerId) {
    std::unique_lock<std::mutex> lock(Main_Mutex);

    Can_Write_CV.wait(lock, [this]() {
        return (!Data_Written || Reads_After_Write >= 3) && Current_Readers == 0;
    });

    RW_Mutex.lock();

    ++Value;
    Data_Written = true;
    Reads_After_Write = 0;

    std::cout << "Writer " << writerId << " wrote value: " << Value << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    RW_Mutex.unlock();

    Can_Read_CV.notify_all();
    lock.unlock();
}

void data::Shutdown() {
    std::lock_guard<std::mutex> lock(Main_Mutex);
    Stop = true;
    Can_Read_CV.notify_all();
    Can_Write_CV.notify_all();
}

// Reader Thread Function
reader::reader(int reader_ID, data& Shared_Data)
        : ID(reader_ID), Data(Shared_Data) {}

void reader::operator()() {
    while (true) {
        Data.Read(ID);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

// Writer Thread Function
writer::writer(int Writer_ID, data& Shared_Data)
        : ID(Writer_ID), Data(Shared_Data) {}

void writer::operator()() {
    while (true) {
        Data.Write(ID);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
