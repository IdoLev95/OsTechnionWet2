#include "Logger.hpp"


// Constructor implementation
Logger::Logger(string Path_to_logger,pthread_mutex_t* mutex_lock_write,pthread_mutex_t* mutex_lock_read)
: reader_writer_logger(mutex_lock_read,mutex_lock_write) {
    // Initialize mutex for thread safety

    reader_writer_logger.SetLocks(mutex_lock_read,mutex_lock_write);
    path_to_logger = Path_to_logger;
    log_file.open(path_to_logger.c_str(), std::ios::out | std::ios::app);

}

// Destructor implementation
Logger::~Logger() {
}

// WriteToLogger implementation
void Logger::WriteToLogger(string str_to_write) {
    try {
//        cout << "here" << endl;
    	// Acquire writer lock
        reader_writer_logger.writer_locker();
        if (log_file.is_open()) {
                log_file << str_to_write << std::endl;
            } else {
                std::cerr << "Error: Log file is not open!" << std::endl;
            }
        /*
        // Open log file
        std::ofstream log_file(path_to_logger, std::ios::app);
        if (!log_file.is_open()) {
        	reader_writer_logger.writer_unlocker();
        	cout<<"There is an error"<<endl;

            throw std::runtime_error("Failed to open log file");
        }

        // Write to log file
        log_file << str_to_write << std::endl;

        // Close file and release lock
        log_file.close();
        */
        reader_writer_logger.writer_unlocker();
    }
    catch (const std::exception& e) {
    	reader_writer_logger.writer_unlocker();
    	std::cerr << "Error writing to log: " << e.what() << std::endl;

    }
    //reader_writer_logger.writer_unlocker();
  //  cout << "wow" << endl;
}


// EraseLoggerContent: Clears all content in the log file.
void Logger::EraseLoggerContent() {
    // Lock for writing since we are modifying the file
    reader_writer_logger.writer_locker(); // Lock for thread-safe write operation

    // Open the log file in **truncate mode** to clear its content
    std::ofstream log_file(path_to_logger, std::ofstream::out | std::ofstream::trunc);

    if (!log_file.is_open()) {
        // Throw an exception if the file cannot be opened
        throw std::runtime_error("Error: Unable to open log file for erasing content.");
    }

    // Close the file immediately after truncation
    //log_file.close();

    // Unlock after finishing the write operation
    reader_writer_logger.writer_unlocker(); // Unlock the writer lock
}
