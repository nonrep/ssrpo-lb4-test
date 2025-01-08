#include "hw/l2_ApplicationLayer.h"
#include "tp/ThreadPool.h"

#include <iostream>
#include <string>
#include <cassert>


class TerminalOutput : public IOutput
{
public:
    virtual void Output(std::string s) const override final;
};


void TerminalOutput::Output(std::string s) const
{
    std::cout << s << std::endl;
}

int convertToInteger(const std::string & str)
try
{
    return std::stoi(str);
}
catch(...)
{
    return 0;
}

void performCommandsSimultaneously(std::istream & is, ItemCollector & col, const TerminalOutput & out, int number_of_threads)
{
    tp::ThreadPool tp(number_of_threads);
    tp.start();

    size_t number_of_commands = 0;
    for(std::string line; std::getline(is,line); ) {
        if (line.empty())
            break;

        tp.submit(new Application(col,line,out));
        number_of_commands ++;
    }

    std::cerr << "Выполняем пакет команд. Размер пула потоков: " << tp.size() 
              << ", остаток команд в очереди: " << tp.queue_length() 
              << " из " << number_of_commands
              << std::endl;
}

inline const std::string DATA_DEFAULT_NAME = "lab.data";

int main(int argc, char *argv[])
{
    ItemCollector  col;
    TerminalOutput out;
    std::string    data_file_name = DATA_DEFAULT_NAME;
    std::string    input_file_name;
    int            number_of_threads = -1;

    // Разбираем командную строку
    std::vector<std::string> arguments(argv + 1, argv + argc);
    for(const std::string & arg : arguments)
        if (std::to_string(convertToInteger(arg)) == arg)
            number_of_threads = convertToInteger(arg);
        else
            input_file_name = arg;

    // Соединение и загрузка хранилища
    col.loadCollection(data_file_name);

    // Работа с файлом команд через файл, а не пайп может быть полезна, если нужна отладка
    if (input_file_name.empty())
        performCommandsSimultaneously(std::cin,col,out,number_of_threads);
    else {
        std::ifstream ifs (input_file_name);
        if (!ifs) {
            out.Output("Ошибка при открытии файла команд '" + input_file_name + "'");
            return 1;
        }
        performCommandsSimultaneously(ifs,col,out,number_of_threads);
    }

    // Сохраняем данные в хранилище
    if (!col.saveCollection()) {
        out.Output("Ошибка при сохранении файла данных '" + col.data_file_name() + "'");
        return 1;
    }

    std::cout << "Выполнение команд завершено" << std::endl;
    return 0;
}
