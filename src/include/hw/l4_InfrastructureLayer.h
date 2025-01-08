#ifndef HW_L4_INFRASTRUCTURE_LAYER_H
#define HW_L4_INFRASTRUCTURE_LAYER_H

#include <string>
#include <map>
#include <cassert>
#include <memory>
#include <mutex>

#include <fstream>

template<typename T>
T readNumber(std::istream& is)
{
    T result;
    is.read(reinterpret_cast<char *>(&result), sizeof(result));
    return result;
}

template<typename T>
void writeNumber(std::ostream& os, T i)
{
    os.write(reinterpret_cast<char *>(&i),sizeof(i));
}

std::string readString(std::istream& is, size_t max_string_length);
void writeString(std::ostream& os, const std::string& s);

class ICollectable
{
public:
    virtual ~ICollectable() = default;

    virtual bool write(std::ostream& os) = 0;
};

class CollectorData
{
    bool                          _removed_signs;
    std::shared_ptr<ICollectable> _item;

public:
    CollectorData() = delete;
    CollectorData(std::shared_ptr<ICollectable> item, bool removed_signs = false) 
        : _removed_signs(removed_signs)
        , _item(item)
    {}

    bool                          removed() const { return _removed_signs; }
    std::shared_ptr<ICollectable> item()    const { return _item; }

    void remove() { _removed_signs = true; }
};

class ACollector
{
    std::string                    _file_name;
    std::map<size_t,CollectorData> _items;
    mutable std::mutex             _mutex;
    size_t                         _max_index = 0;

public:
    virtual ~ACollector() = default;

    virtual std::shared_ptr<ICollectable> read(std::istream& is) = 0;

    size_t getSize() const 
    {
        std::lock_guard locker(_mutex);
        if (_items.empty())
            return 0;
        return _items.size();
    }

    std::shared_ptr<ICollectable> getItem(size_t index) const
    {
        std::lock_guard locker(_mutex);
        auto it = _items.find(index);
        if (it == _items.end())
            return std::shared_ptr<ICollectable>();
        return it->second.item();
    }

    bool isRemoved(size_t index) const
    {
        std::lock_guard locker(_mutex);
        auto it = _items.find(index);
        if (it == _items.end())
            return true;
        return it->second.removed();
    }

    size_t addItem(std::shared_ptr<ICollectable> item, bool removed=false)
    {
        std::lock_guard locker(_mutex);
        _max_index ++;
        _items.insert({_max_index,{item,removed}});
        return _max_index;
    }

    bool removeItem(size_t index)
    {
        std::lock_guard locker(_mutex);
        auto it = _items.find(index);
        if (it == _items.end())
            return false;
        it->second.remove();
        return true;
    }

    bool updateItem(size_t index, const std::shared_ptr<ICollectable> item)
    {
        std::lock_guard locker(_mutex);
        auto it = _items.find(index);
        if (it == _items.end())
            return false;
        it->second.item() = item;
        return true;
    }

    bool loadCollection(const std::string file_name);

    bool saveCollection() const;

    const std::string & data_file_name() const { return _file_name; }
};

#endif // HW_L4_INFRASTRUCTURE_LAYER_H
