#include "hw/l3_DomainLayer.h"

bool Person::invariant() const
{
    return !_alias.empty();
}

Person::Person(const std::string & alias)
    : _alias(alias)
{
    assert(invariant());
}

Person::Person(const std::string &alias, std::vector<Visit> visits)
    : _alias(alias)
    , _visits(visits)
{
    assert(invariant());
}

const std::string &Person::getAlias() const
{
    return _alias;
}

void Person::setVisits(const std::vector<Visit> visits)
{
    std::lock_guard locker(_visits_mutex);
    _visits = visits;
}

void Person::addVisit(const Visit & visit)
{
    std::lock_guard locker(_visits_mutex);
    _visits.push_back(visit);
}

std::vector<Visit> Person::getVisits() const
{
    std::lock_guard locker(_visits_mutex);
    return _visits;
}

bool   Person::write(std::ostream& os)
{
    writeString(os, _alias);

    size_t number_of_visits = _visits.size();
    writeNumber(os, number_of_visits);

    for(const Visit & v : _visits)
    {
        writeNumber(os, v.getYear());
        writeNumber(os, v.getMonth());
        writeNumber(os, v.getDay());
    }

    return os.good();
}


std::shared_ptr<ICollectable> ItemCollector::read(std::istream& is)
{
    std::string alias       = readString(is, MAX_NAME_LENGTH);
    size_t number_of_visits = readNumber<size_t>(is);

    std::vector<Visit> v;

    v.reserve(number_of_visits);
    for(size_t i=0; i < number_of_visits; ++i)
    {
        int year = readNumber<int>(is);
        int month = readNumber<int>(is);
        int day = readNumber<int>(is);

        v.push_back(Visit(year, month, day));
    }

    std::shared_ptr<ICollectable> p = std::make_shared<Person>(alias, v);

    return p;
}

Person & ItemCollector::getPerson(size_t index)
{
    Person * p = static_cast<Person *>(getItem(index).get());

    assert(p != nullptr);

    return *p;
}
