#include "../../Precompiled.h"

#define TOLOWERU( c ) ( ( uint32_t ) ( ( ( c >= 'A' ) && ( c <= 'Z' ) )? c + 32 : c ) )
uint32_t MurmurHash2LowerCaseA(char const* pString, int len, uint32_t nSeed)
{
    char* p = (char*)MEM_STACKALLOC(len + 1);
    for (int i = 0; i < len; i++)
    {
        p[i] = TOLOWERU(pString[i]);
    }
    return MurmurHash2(p, len, nSeed);
}

uint32_t MurmurHash2LowerCaseB(char const* pString, uint32_t nSeed)
{
    return MurmurHash2LowerCaseA(pString, (uint32_t)CRT::StringLength(pString), nSeed);
}

uint32_t MurmurHash2(const void* key, int len, uint32_t seed)
{

    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    uint32_t h = seed ^ len;

    const unsigned char* data = (const unsigned char*)key;

    while (len >= 4)
    {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
        h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

uint64_t MurmurHash64A(const void* key, int len, uint64_t seed)
{
    const uint64_t m = BIG_CONSTANT(0xc6a4a7935bd1e995);
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const uint64_t* data = (const uint64_t*)key;
    const uint64_t* end = data + (len / 8);

    while (data != end)
    {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char* data2 = (const unsigned char*)data;

    switch (len & 7)
    {
    case 7: h ^= ((uint64_t)data2[6]) << 48;
    case 6: h ^= ((uint64_t)data2[5]) << 40;
    case 5: h ^= ((uint64_t)data2[4]) << 32;
    case 4: h ^= ((uint64_t)data2[3]) << 24;
    case 3: h ^= ((uint64_t)data2[2]) << 16;
    case 2: h ^= ((uint64_t)data2[1]) << 8;
    case 1: h ^= ((uint64_t)data2[0]);
        h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

uint64_t MurmurHash64B(const void* key, int len, uint64_t seed)
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    uint32_t h1 = ((uint32_t)seed) ^ len;
    uint32_t h2 = ((uint32_t)(seed >> 32));

    const uint32_t* data = (const uint32_t*)key;

    while (len >= 8)
    {
        uint32_t k1 = *data++;
        k1 *= m; k1 ^= k1 >> r; k1 *= m;
        h1 *= m; h1 ^= k1;
        len -= 4;

        uint32_t k2 = *data++;
        k2 *= m; k2 ^= k2 >> r; k2 *= m;
        h2 *= m; h2 ^= k2;
        len -= 4;
    }

    if (len >= 4)
    {
        uint32_t k1 = *data++;
        k1 *= m; k1 ^= k1 >> r; k1 *= m;
        h1 *= m; h1 ^= k1;
        len -= 4;
    }

    switch (len)
    {
    case 3: h2 ^= ((unsigned char*)data)[2] << 16;
    case 2: h2 ^= ((unsigned char*)data)[1] << 8;
    case 1: h2 ^= ((unsigned char*)data)[0];
        h2 *= m;
    };

    h1 ^= h2 >> 18; h1 *= m;
    h2 ^= h1 >> 22; h2 *= m;
    h1 ^= h2 >> 17; h1 *= m;
    h2 ^= h1 >> 19; h2 *= m;

    uint64_t h = h1;

    h = (h << 32) | h2;

    return h;
}

#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

uint32_t MurmurHash2A(const void* key, int len, uint32_t seed)
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    uint32_t l = len;

    const unsigned char* data = (const unsigned char*)key;

    uint32_t h = seed;

    while (len >= 4)
    {
        uint32_t k = *(uint32_t*)data;

        mmix(h, k);

        data += 4;
        len -= 4;
    }

    uint32_t t = 0;

    switch (len)
    {
    case 3: t ^= data[2] << 16;
    case 2: t ^= data[1] << 8;
    case 1: t ^= data[0];
    };

    mmix(h, t);
    mmix(h, l);

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

uint32_t MurmurHashNeutral2(const void* key, int len, uint32_t seed)
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    uint32_t h = seed ^ len;

    const unsigned char* data = (const unsigned char*)key;

    while (len >= 4)
    {
        uint32_t k;

        k = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
        h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

#define MIX(h,k,m) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

uint32_t MurmurHashAligned2(const void* key, int len, uint32_t seed)
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    const unsigned char* data = (const unsigned char*)key;

    uint32_t h = seed ^ len;

    int align = (uint64_t)data & 3;

    if (align && (len >= 4))
    {

        uint32_t t = 0, d = 0;

        switch (align)
        {
        case 1: t |= data[2] << 16;
        case 2: t |= data[1] << 8;
        case 3: t |= data[0];
        }

        t <<= (8 * align);

        data += 4 - align;
        len -= 4 - align;

        int sl = 8 * (4 - align);
        int sr = 8 * align;

        while (len >= 4)
        {
            d = *(uint32_t*)data;
            t = (t >> sr) | (d << sl);

            uint32_t k = t;

            MIX(h, k, m);

            t = d;

            data += 4;
            len -= 4;
        }

        d = 0;

        if (len >= align)
        {
            switch (align)
            {
            case 3: d |= data[2] << 16;
            case 2: d |= data[1] << 8;
            case 1: d |= data[0];
            }

            uint32_t k = (t >> sr) | (d << sl);
            MIX(h, k, m);

            data += align;
            len -= align;

            switch (len)
            {
            case 3: h ^= data[2] << 16;
            case 2: h ^= data[1] << 8;
            case 1: h ^= data[0];
                h *= m;
            };
        }
        else
        {
            switch (len)
            {
            case 3: d |= data[2] << 16;
            case 2: d |= data[1] << 8;
            case 1: d |= data[0];
            case 0: h ^= (t >> sr) | (d << sl);
                h *= m;
            }
        }

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    }
    else
    {
        while (len >= 4)
        {
            uint32_t k = *(uint32_t*)data;

            MIX(h, k, m);

            data += 4;
            len -= 4;
        }

        switch (len)
        {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
            h *= m;
        };

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    }
}