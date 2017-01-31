#include <string>
#include <map>
#include <cassert>
#include <iostream>
#include <functional>

namespace tlv {

#define TLVSET_TEMPLATE template<typename TAGTYPE, typename LENTYPE>
#define TLVSET_QUAL set<TAGTYPE, LENTYPE>
#define TLVITEM_QUAL item<TAGTYPE, LENTYPE>

TLVSET_TEMPLATE
class item;

template<typename TAGTYPE, typename LENTYPE>
class set: public std::multimap<TAGTYPE, TLVITEM_QUAL> {
public:
    set(const std::string&);
    set() {}

    auto parse(const std::string&) -> int;
    auto repr(std::ostream& out) const -> std::ostream&;

    template<typename TGT, typename IT>
    auto value(IT) const -> const TGT*;




private:
    std::string                     base{};
    bool                            parsed{false};

    template<typename TOF>
    const TOF* retrieve(std::string::iterator& i, size_t l = sizeof(TOF)) const;
    void _parse();
    void setBase(const std::string& d);
};

TLVSET_TEMPLATE
class item {
public:
    TAGTYPE     tag;
    LENTYPE     len;
    size_t      off;

    item(TAGTYPE t, LENTYPE l, size_t o):
        tag {t}, len {l}, off {o} {}

//        len {*(LENTYPE*)(base + off)},
//        val {base + off + sizeof(LENTYPE)} {}
};

#define BADF badformat(__LINE__)
class badformat: public std::runtime_error {
public:
    badformat(): std::runtime_error("bad format") {}
    badformat(int l): std::runtime_error(std::to_string(l)) {}
};

TLVSET_TEMPLATE
TLVSET_QUAL::set(const std::string& d) {
    setBase(d);
    _parse();
}

TLVSET_TEMPLATE
template<typename TOF>
auto TLVSET_QUAL::retrieve(std::string::iterator& i, size_t l) const -> const TOF* {
    assert(i >= base.begin() && i <= base.end());
    if (i == base.end())
        throw BADF;
    auto e = base.c_str() + base.size();
    const char* p = base.c_str() + (i - base.begin());
    if (e - p < l)
        throw BADF;
    const TOF* res = (const TOF*)p;
    i += l;
    return res;
}

TLVSET_TEMPLATE
auto TLVSET_QUAL::_parse() -> void {
    for (auto i = base.begin(); i != base.end(); ) {
        TAGTYPE t = *retrieve<TAGTYPE>(i);
        LENTYPE l = *retrieve<LENTYPE>(i);
        this->emplace(t, TLVITEM_QUAL{t, l, (size_t)std::distance(base.begin(), i)});
        retrieve<char>(i, l);
    }
    parsed = true;
}

TLVSET_TEMPLATE
auto TLVSET_QUAL::parse(const std::string& d) -> int {
    setBase(d);
    try {
        _parse();
    } catch(badformat& e) {
        return -1;
    }
    return 0;
}

TLVSET_TEMPLATE
auto TLVSET_QUAL::setBase(const std::string& d) -> void {
    this->clear();
    base = d;
}

TLVSET_TEMPLATE
auto TLVSET_QUAL::repr(std::ostream& out) const -> std::ostream& {
    for (const auto& r: *this)
        out << "tag " << r.second.tag << " len " << r.second.len << " off " << r.second.off << std::endl;
    return out;
}

TLVSET_TEMPLATE
template<typename TGT, typename IT>
auto TLVSET_QUAL::value(IT i) const -> const TGT* {
    if (i->second.len < sizeof(TGT))
        throw BADF;
    return (TGT*)(base.c_str() + i->second.off);
}
}
