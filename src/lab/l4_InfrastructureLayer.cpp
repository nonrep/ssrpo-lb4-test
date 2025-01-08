#include "hw/l4_InfrastructureLayer.h"

std::string readString(std::istream& is, size_t max_string_length)
{
    uint16_t len = readNumber<uint16_t>(is);

    assert(len <= max_string_length);

    std::string str(len,' ');
    if (len > 0)
        is.read(str.data(), len);

    return str;
}

void writeString(std::ostream& os, const std::string& s)
{
    uint16_t len = s.length();
    writeNumber(os, len);
    os.write(s.data(), len);
}


bool ACollector::loadCollection(const std::string file_name)
{
    _file_name = file_name;
    std::ifstream ifs (file_name, std::ios_base::binary);
    if (!ifs)
        return false;

    size_t count = readNumber<size_t>(ifs);
    for(size_t i=0; i < count; ++i) {
        uint8_t removed = readNumber<uint8_t>(ifs);
        addItem(read(ifs), removed == 1);
    }

    return ifs.good();
}

bool ACollector::saveCollection() const
{
    std::ofstream ofs (_file_name, std::ios_base::binary);

    if (!ofs)
        return false;

    size_t items_quantity = _items.size();
    writeNumber<size_t>(ofs,items_quantity);

    for(auto & [index, data] : _items) {
        writeNumber<uint8_t>(ofs, data.removed() ? 1 : 0);
        data.item()->write(ofs);
    }

    return ofs.good();
}

