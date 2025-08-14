#pragma once
#include <iostream>
#include <string>

class Logger {
public:
    enum class Level {
        None = 0,
        Error,
        Warning,
        Info,
        Debug
    };

    explicit Logger(Level level = Level::None) : logLevel(level) {}

    void setLevel(Level level) { logLevel = level; }
    Level getLevel() const { return logLevel; }

    template<typename... Args>
    void log(Level level, Args&&... args) const {
        if (level <= logLevel) {
            (std::cout << ... << args) << std::endl;
        }
    }

    template<typename... Args>
    void debug(Args&&... args) const { log(Level::Debug, std::forward<Args>(args)...); }
    template<typename... Args>
    void info(Args&&... args)  const { log(Level::Info,  std::forward<Args>(args)...); }
    template<typename... Args>
    void warn(Args&&... args)  const { log(Level::Warning, std::forward<Args>(args)...); }
    template<typename... Args>
    void error(Args&&... args) const { log(Level::Error, std::forward<Args>(args)...); }

private:
    Level logLevel;
};
