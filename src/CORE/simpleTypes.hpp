#ifndef ITERATEDECKS_CORE_SIMPLETYPES_HPP
    #define ITERATEDECKS_CORE_SIMPLETYPES_HPP

    #include <vector>

    typedef	unsigned char UCHAR;
    typedef	unsigned int UINT;

    // that is usally the amount (i.e, the 3 in weaken 3), but also the card to summon in summon
    typedef UINT EFFECT_ARGUMENT;
    typedef UCHAR AbilityId;
    typedef UCHAR FactionId;

    typedef std::vector<UINT> VID;

#endif
