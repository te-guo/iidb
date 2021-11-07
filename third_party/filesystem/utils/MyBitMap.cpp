#include "MyBitMap.h"
unsigned char h[61];


int MyBitMap::getIndex(uint k)
{
    return h[_hash(k)];
}
int MyBitMap::findLeftOne() {
    int i = _findLeftOne(rootLevel, rootIndex, 0, rootBit);
    /*
    for (i = 0; i < size;++i){
        if (data[i] !=0)break;
    }*/
    //cout << "nPosi " << i << " " << getLeafData(i) << endl;
    //cout << i << endl;
    //cout << data[0] << endl;
    uint lb = lowbit(getLeafData(i));
    int index = h[_hash(lb)];
    return (i << BIAS) + index;
}
int MyBitMap::_findLeftOne(int level, int offset, int pos, int prevLevelCap) {
    uint lb = lowbit(inner[offset + pos]);
    int index = h[_hash(lb)];
    /*if (level == 0) {
        cout << "level0:" << index << " " << pos << endl;
    }*/
    int nPos = (pos << BIAS) + index;
    if (level == 0) {
    //	cout << "npos " << nPos << endl;
        return nPos;
    }
    return _findLeftOne(level - 1, offset - prevLevelCap, nPos, (prevLevelCap << BIAS));
}

void MyBitMap::initConst() {
    for (int i = 0; i < 32; ++ i) {
        unsigned int k = (1 << i);
        h[_hash(k)] = i;
    }
}