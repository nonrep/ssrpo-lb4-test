#include "hw/l2_ApplicationLayer.h"

#include <algorithm>
#include <thread>

const int OUTPUT_LIMIT = 1000;

void Application::work()
{
    std::vector<std::string> args = split(_command);
    if (args.empty())
        return;

    // Используется для проверки заполнения очереди. 
    // Очередь слишком быстро опустошалась, пытался понять где тормозит.
    // std::this_thread::sleep_for(std::chrono::microseconds(10000));

    // count
    if (args[0] == "c" || args[0] == "count") {
        if (args.size() != 1) {
            _out.Output("Некорректное количество аргументов команды clean");
            return;
        }

        _out.Output(std::to_string(_col.getSize()));
        return;
    }

    // add alias
    if (args[0] == "a" || args[0] == "add") {
        if (args.size() != 2) {
            _out.Output("Некорректное количество аргументов команды add");
            return;
        }

        _col.addItem(std::make_shared<Person>(args[1].c_str()));
        return;
    }

    // add_visit person_no year month day
    if (args[0] == "av" || args[0] == "add_visit") {
        if (args.size() != 5) {
            _out.Output("Некорректное количество аргументов команды add_visit");
            return;
        }

        Person & p = _col.getPerson(stoul(args[1]));
        p.addVisit(Visit(stoi(args[2]),stoi(args[3]),stoi(args[4])));

        return;
    }

    // remove person_no
    if (args[0] == "r" || args[0] == "remove") {
        if (args.size() != 2) {
            _out.Output("Некорректное количество аргументов команды remove");
            return;
        }

        _col.removeItem(stoul(args[1]));
        return;
    }

    // update person_no alias
    if (args[0] == "u" || args[0] == "update") {
        if (args.size() != 3) {
            _out.Output("Некорректное количество аргументов команды update");
            return;
        }

        _col.updateItem(stoul(args[1]), std::make_shared<Person>(args[2].c_str()));
        return;
    }

    // view [lines_limit] [visits_limit]
    if (args[0] == "v" || args[0] == "view") {
        if (args.size() > 3) {
            _out.Output("Некорректное количество аргументов команды view");
            return;
        }

        size_t lines_limit = OUTPUT_LIMIT;
        if (args.size() > 1)
            lines_limit = stoul(args[1]);

        size_t visits_limit = OUTPUT_LIMIT;
        if (args.size() > 2)
            visits_limit = stoul(args[2]);

        size_t count = 0;
        for(size_t i=1; i <= _col.getSize(); ++i) {
            if (!_col.isRemoved(i)) {
                const Person & item = _col.getPerson(i);

                if (count < lines_limit) {
                    _out.Output("[" + std::to_string(i) + "] "
                            + item.getAlias() + " "
                            );

                    size_t visits_count = 0;
                    for(const Visit & v : item.getVisits()) {
                        if (visits_count < visits_limit)
                            _out.Output("\t" + std::to_string(v.getDay()) + "." + std::to_string(v.getMonth()) + "." + std::to_string(v.getYear()));
                        visits_count ++;
                    }

                    if (visits_count >= visits_limit)
                        _out.Output("\t... " + std::to_string(visits_count) + " визитов");
                }
                else if (count == lines_limit)
                    _out.Output("Выведено первые " + std::to_string(lines_limit) + " строк");
                count ++;
            }
        }

        _out.Output("Количество элементов в коллекции: " + std::to_string(count));
        return;
    }

    // report [lines_limit]
    if (args[0] == "rp" || args[0] == "report") {
        if (args.size() > 2) {
            _out.Output("Некорректное количество аргументов команды view");
            return;
        }

        size_t lines_limit = OUTPUT_LIMIT;
        if (args.size() == 2)
            lines_limit = stoul(args[1]);

        std::vector<std::pair<size_t,std::string>> report_set;

        for(size_t i=1; i <= _col.getSize(); ++i) {
            if (!_col.isRemoved(i)) {
                const Person & p = _col.getPerson(i);

                /// @todo Использование getVisits(), чтобы получить количество элементов не оптимально, 
                /// т.к. визиты каждый раз копируются во временный массив, а делать по ссылке нельзя.
                /// Нужно сделать метод в Person для получения количества визитов у посетителя
                if (p.getVisits().size() > 0)
                    report_set.push_back({p.getVisits().size(),p.getAlias()});
            }
        }

        sort(report_set.begin(), report_set.end(),
             [](std::pair<size_t,std::string> p1, std::pair<size_t,std::string> p2){
                return p1.first > p2.first;
            });

        size_t count = 0;
        for(const auto & [quantity,alias] : report_set) {
            if (count < lines_limit) 
                _out.Output(alias + " " + std::to_string(quantity));
            else if (count == lines_limit) {
                _out.Output("Выведено первые " + std::to_string(lines_limit) + " строк");
                break;
            }
            count ++;
        }
        _out.Output("Итого количество посетителей " + std::to_string(report_set.size()) + 
                    " из " + std::to_string(_col.getSize()) + " зарегистрировавшихся");
        return;
    }

    _out.Output("Недопустимая команда '" + args[0] + "'");
    return;
}

std::vector<std::string> Application::split(const std::string & str)
{
    std::vector<std::string> res;
    size_t                   start_pos = 0, 
                             end_pos   = 0;

    while(end_pos < str.size()) 
    {
        for(start_pos = end_pos; start_pos < str.size(); ++start_pos)
            if (str[start_pos] != ' ')
                break;

        if (start_pos == str.size())
            return res;

        for(end_pos = start_pos; end_pos < str.size(); ++end_pos)
            if (str[end_pos] == ' ')
                break;

        res.push_back(str.substr(start_pos,end_pos-start_pos));
    }

    return res;
}

