#ifndef _TTS_GETCHARINFO_H_
#define _TTS_GETCHARINFO_H_

#include<string>
#include<map>

using namespace std;

typedef struct _PinyinInfo{
    char pinyin[8];
    char shengmu[6];
    char yunmu[6];
}PinyinInfo;

typedef struct _TtsLabelCharInfo{

    char ph1[6]; // tt ui n
    char ph2[6]; // tt ui n
    char ph3[6]; // tt ui n
    char phs[3][6];
    short num_ph;  //  当前syllable 包含phoneme的个数。

    // n^ong-y+ao=sh@1_1/A:1_2_4/B:1-2-1@1-1&3-3#3-3$8-1!3-5;10-1|2
    // /C:2+2+2/D:4_2/E:3+5@2+1&2+2#2+3/F:1_2/G:2


    // p1^p2-p3+p4=p5@p6_p7/A:a1_a2_a3/B:b1-b2-b3@b4-b5&b6-b7#b8-b9$b10-b11!b12-b13;b14-b15|b16
    // /C:c1+c2+c3/D:d1_d2/E:e1+e2@e3+e4&e5+e6#e7+e8/F:f1_f2/G:g1


    //////////////////////////////////////////////
    char pinyin[8];
    char shengmu[6];
    char yunmu[6];
    short yindiao; //1-5

    //p1^p2-p3+p4=p5@ 
        //  前后5个phoneme
    //@p6_p7:
        //  声韵母、清浊辅音
    // /A:a1_a2_a3/B:b1-b2:
        //  前后5个phoneme的声调 

    // 分别为 4-3-2-1 个 
    short CharInPwPos;  // 音节在韵律词位置（左数）
    short CharInPpPos;  // 音节在韵律短语位置（左数）
    short CharInIpPos;  // 音节在语调短语位置（左数）
    short CharInSentPos;// 音节在句子语位置（左数）

    short PwInPpPos;
    short PwInIpPos;
    short PwInSentPos;

    short PpInIpPos;    // 韵律短语在语调短语位置
    short PpInSentPos;

    short IpInSentPos;  // 语调短语在句子位置

    short CharInPwNum;  // 韵律词包含音节数
    short CharInPpNum;  // 韵律短语包含音节数
    short CharInIpNum;  // 语调短语包含音节数
    short CharInSentNum;// 句子包含音节数

    short PwInPpNum;    // 韵律短语包含韵律词个数
    short PwInIpNum;    // 语调短语包含韵律词个数
    short PwInSentNum;  // 句子包含韵律词个数

    short PpInIpNum;    // 语调短语包含韵律短语个数
    short PpInSentNum;  // 句子包含韵律短语个数

    short IpInSentNum;  // 句子包含语调短语个数


}TtsLabelCharInfo;

int get3ph(const TtsLabelCharInfo &cif, char *ph);
short getPinyinInfoID(char *pinyin);
void getShengYun(char *pinyin,char *shengmu,char *yunmu,short *yindiao);
int isQing(char *phone);



/*
 // tt ui n
 * */
short split_str(const char *line, char ** seg, short *nSeg);

/*
 * 按照最大匹配  将syllable 拆分成 phone 
 * */
int split_syll2phone(const string syll, string &phones);

int getPhones(const string syll, string &phones);

int TTS_Label_Init();


short getPinyinInfoID(char *pinyin);
void getShengYun(char *pinyin,char *shengmu,char *yunmu,short *yindiao);
int TtsLabel_ObtainLabelCharSeq(TtsLabelCharInfo * cif,char **pinyinSeq,short sNum,short * tag);
int PrintLabel(TtsLabelCharInfo * cif,short sNum,char *fname);

#endif
