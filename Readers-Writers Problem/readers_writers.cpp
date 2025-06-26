#include "readers_writers.h"
#include <iostream>
#include <thread>
#include <chrono>

data::data()
        : Value(0), Data_Written(false), Stop(false),
          Current_Readers(0), Reads_After_Write(0) {}

bool data::IsStopped() const {
    return Stop;
}

void data::Read(int readerId) {
    std::unique_lock<std::mutex> lock(Mutex);

    // Czekaj aż dane będą zapisane i program nie będzie zatrzymany
    Can_Read_CV.wait(lock, [this]() {
        return Data_Written && !Stop;
    });

    // Rozpoczęcie czytania — zwiększamy licznik czytelników
    ++Current_Readers;

    lock.unlock();

    // Sekcja czytająca — bez blokady!
    std::cout << "Reader " << readerId << " is reading value: " << Value << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    lock.lock();

    // Kończenie czytania
    --Current_Readers;
    ++Reads_After_Write;

    // Jeśli 3 czytelników przeczytało i nie ma aktywnych czytelników — można pisać
    if (Reads_After_Write >= 3 && Current_Readers == 0) {
        Can_Write_CV.notify_one();
    }

    lock.unlock();
}

void data::Write(int writerId) {
    std::unique_lock<std::mutex> lock(Mutex);

    // Czekaj, aż można pisać: nie ma aktywnych czytelników i
    // albo dane nie były jeszcze zapisane (pierwszy zapis),
    // albo zostały przeczytane przez 3 czytelników
    Can_Write_CV.wait(lock, [this]() {
        return (!Data_Written || Reads_After_Write >= 3) && Current_Readers == 0;
    });

    // Zapis danych
    ++Value;
    Data_Written = true;
    Reads_After_Write = 0;

    std::cout << "Writer " << writerId << " wrote value: " << Value << std::endl;

    lock.unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    // Powiadom czytelników, że dane są gotowe do odczytu
    Can_Read_CV.notify_all();
}

// Reader Thread Function
reader::reader(int reader_ID, data& Shared_Data)
        : ID(reader_ID), Data(Shared_Data) {}

void reader::operator()() {
    while (true) {
        Data.Read(ID);
        if (Data.IsStopped()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

// Writer Thread Function
writer::writer(int Writer_ID, data& Shared_Data)
        : ID(Writer_ID), Data(Shared_Data) {}

void writer::operator()() {
    while (true) {
        Data.Write(ID);
        if (Data.IsStopped()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void data::StopSimulation() {
    std::unique_lock<std::mutex> lock(Mutex);
    Stop = true;
    Can_Read_CV.notify_all();
    Can_Write_CV.notify_all();
}
