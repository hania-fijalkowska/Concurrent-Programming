#ifndef CONCURRENT_PROGRAMMING_READERS_WRITERS_H
#define CONCURRENT_PROGRAMMING_READERS_WRITERS_H

#include <mutex>
#include <condition_variable>

class data {
private:
    int Value;
    bool Data_Written;
    bool Stop;

    int Current_Readers;
    int Reads_After_Write;

    std::mutex Mutex;
    std::condition_variable Can_Read_CV;
    std::condition_variable Can_Write_CV;

public:
    data();

    void Read(int readerId);
    void Write(int writerId);

    bool IsStopped() const;
    void StopSimulation();
};

class reader {
private:
    int ID;
    data& Data;

public:
    reader(int reader_ID, data& Shared_Data);
    void operator()();
};

class writer {
private:
    int ID;
    data& Data;

public:
    writer(int Writer_ID, data& Shared_Data);
    void operator()();
};

#endif
