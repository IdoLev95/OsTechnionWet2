#include "Logger.hpp"


// Constructor implementation
Logger::Logger(string Path_to_logger,pthread_mutex_t mutex_lock_write,pthread_mutex_t mutex_lock_read)
: reader_writer_bank_list(mutex_lock_read,mutex_lock_write) {
    // Initialize mutex for thread safety
    if (pthread_mutex_init(&mutex_lock_write, nullptr) != 0) {
        throw std::runtime_error("Mutex initialization failed");
    }
    if (pthread_mutex_init(&mutex_lock_read, nullptr) != 0) {
            throw std::runtime_error("Mutex initialization failed");
        }
    path_to_logger = Path_to_logger;
}

// Destructor implementation
Logger::~Logger() {
}

// WriteToLogger implementation
void Logger::WriteToLogger(string str_to_write) {
    try {
        // Acquire writer lock
        reader_writer_bank_list.writer_locker();

        // Open log file
        std::ofstream log_file(path_to_logger, std::ios::app);
        if (!log_file.is_open()) {
            throw std::runtime_error("Failed to open log file");
        }

        // Write to log file
        log_file << str_to_write << std::endl;

        // Close file and release lock
        log_file.close();
        reader_writer_bank_list.writer_unlocker();
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing to log: " << e.what() << std::endl;
        reader_writer_bank_list.writer_unlocker();
    }
}
