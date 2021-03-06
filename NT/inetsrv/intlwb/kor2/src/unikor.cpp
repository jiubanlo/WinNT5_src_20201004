// unikor.cpp
// Korean Unicode routines
// Copyright 1998-2000 Microsoft Corp.
//
// Modification History:
//  16 MAR 00  bhshin   porting for WordBreaker from uni_kor.c

#include "stdafx.h"
#include "unikor.h"

#pragma setlocale(".949")

// Hangul Jamo Map
// this table maps from the "conjoining jamo" area (u1100 - u11F9)
// to the compatibility Jamo area (u3131 - u318E)
//
// subtract HANGUL_JAMO_BASE (u1100) before indexing into this table
// make sure the the char is not > HANGUL_JAMO_MAX (u11F9) before indexing
//
// to build the complete Unicode character, add the value from this
// table to HANGUL_xJAMO_PAGE (u3100).
//
// 30JUN99  GaryKac  created
unsigned char g_rgchXJamoMap[] = {
    0x31,       // 1100 - 丑
    0x32,       // 1101 - 丐
    0x34,       // 1102 - 中
    0x37,       // 1103 - 之
    0x38,       // 1104 - 尹
    0x39,       // 1105 - 予
    0x41,       // 1106 - 仃
    0x42,       // 1107 - 仆
    0x43,       // 1108 - 仇
    0x45,       // 1109 - 今
    0x46,       // 110A - 介
    0x47,       // 110B - 仄
    0x48,       // 110C - 元
    0x49,       // 110D - 允
    0x4A,       // 110E - 內
    0x4B,       // 110F - 六

    0x4C,       // 1110 - 兮
    0x4D,       // 1111 - 公
    0x4E,       // 1112 - 冗
    0x64,       // 1113 - 中丑 - no match, use fill
    0x65,       // 1114 - 中中
    0x66,       // 1115 - 中之
    0x64,       // 1116 - 中仆 - no match
    0x64,       // 1117 - 之丑 - no match
    0x64,       // 1118 - 予中 - no match
    0x64,       // 1119 - 予予 - no match
    0x40,       // 111A - 什
    0x64,       // 111B - 予仄 - no match
    0x6E,       // 111C - 仃仆
    0x71,       // 111D - 仃仄
    0x72,       // 111E - 仆丑
    0x64,       // 111F - 仆中 - no match

    0x73,       // 1120 - 仆之
    0x44,       // 1121 - 仍
    0x74,       // 1122 - 仍丑
    0x75,       // 1123 - 仍之
    0x64,       // 1124 - 仍仆 - no match
    0x64,       // 1125 - 仍今 - no match
    0x64,       // 1126 - 仍元 - no match
    0x76,       // 1127 - 仆元
    0x64,       // 1128 - 仆內 - no match
    0x77,       // 1129 - 仆兮
    0x64,       // 112A - 仆公 - no match
    0x78,       // 112B - 仆仄
    0x79,       // 112C - 仇仄
    0x7A,       // 112D - 今丑
    0x7B,       // 112E - 今中
    0x7C,       // 112F - 今之

    0x64,       // 1130 - 今予 - no match
    0x64,       // 1131 - 今仄 - no match
    0x7D,       // 1132 - 今仆
    0x64,       // 1133 - 今仆丑 - no match
    0x64,       // 1134 - 今今今 - no match
    0x64,       // 1135 - 今仄 - no match
    0x7E,       // 1136 - 今元
    0x64,       // 1137 - 今內 - no match
    0x64,       // 1138 - 今六 - no match
    0x64,       // 1139 - 今兮 - no match
    0x64,       // 113A - 今公 - no match
    0x64,       // 113B - 今冗 - no match
    0x64,       // 113C - no match
    0x64,       // 113D - no match
    0x64,       // 113E - no match
    0x64,       // 113F - no match

    0x7F,       // 1140 - ^
    0x64,       // 1141 - 仄丑 - no match
    0x64,       // 1142 - 仄之 - no match
    0x64,       // 1143 - 仄仃 - no match
    0x64,       // 1144 - 仄仆 - no match
    0x82,       // 1145 - 仄今
    0x83,       // 1146 - 仄^
    0x84,       // 1147 - 仄仄
    0x64,       // 1148 - 仄元 - no match
    0x64,       // 1149 - 仄內 - no match
    0x64,       // 114A - 仄兮 - no match
    0x64,       // 114B - 仄公 - no match
    0x64,       // 114C - 仄 - no match
    0x64,       // 114D - 元仄 - no match
    0x64,       // 114E - no match
    0x64,       // 114F - no match

    0x64,       // 1150 - no match
    0x64,       // 1151 - no match
    0x64,       // 1152 - 內六 - no match
    0x64,       // 1153 - 內冗 - no match
    0x64,       // 1154 - no match
    0x64,       // 1155 - no match
    0x64,       // 1156 - 公仆 - no match
    0x84,       // 1157 - 公仄
    0x85,       // 1158 - 冗冗
    0x86,       // 1159 - 天仄
    0x64,       // 115A - unused
    0x64,       // 115B - unused
    0x64,       // 115C - unused
    0x64,       // 115D - unused
    0x64,       // 115E - unused
    0x64,       // 115F - fill

    0x64,       // 1160 - fill
    0x4F,       // 1161 - 凶
    0x50,       // 1162 - 分
    0x51,       // 1163 - 切
    0x52,       // 1164 - 刈
    0x53,       // 1165 - 勻
    0x54,       // 1166 - 勾
    0x55,       // 1167 - 勿
    0x56,       // 1168 - 化
    0x57,       // 1169 - 匹
    0x58,       // 116A - 午
    0x59,       // 116B - 升
    0x5A,       // 116C - 卅
    0x5B,       // 116D - 卞
    0x5C,       // 116E - 厄
    0x5D,       // 116F - 友

    0x5E,       // 1170 - 及
    0x5F,       // 1171 - 反
    0x60,       // 1172 - 壬
    0x61,       // 1173 - 天
    0x62,       // 1174 - 夫
    0x63,       // 1175 - 太
    0x64,       // 1176 - 凶匹 - no match
    0x64,       // 1177 - 凶厄 - no match
    0x64,       // 1178 - 切匹 - no match
    0x64,       // 1179 - 切卞 - no match
    0x64,       // 117A - 勻匹 - no match
    0x64,       // 117B - 勻厄 - no match
    0x64,       // 117C - 勻天 - no match
    0x64,       // 117D - 勿匹 - no match
    0x64,       // 117E - 勿厄 - no match
    0x64,       // 117F - 匹勻 - no match

    0x64,       // 1180 -  - no match
    0x64,       // 1181 -  - no match
    0x64,       // 1182 -  - no match
    0x64,       // 1183 -  - no match
    0x87,       // 1184 - 卞勿
    0x88,       // 1185 - 卞刈
    0x64,       // 1186 -  - no match
    0x64,       // 1187 -  - no match
    0x89,       // 1188 - 卞太
    0x64,       // 1189 -  - no match
    0x64,       // 118A -  - no match
    0x64,       // 118B -  - no match
    0x64,       // 118C -  - no match
    0x64,       // 118D -  - no match
    0x64,       // 118E -  - no match
    0x64,       // 118F -  - no match

    0x64,       // 1190 -  - no match
    0x8A,       // 1191 - 壬勿
    0x8B,       // 1192 - 壬化
    0x64,       // 1193 -  - no match
    0x8C,       // 1194 - 壬太
    0x64,       // 1195 -  - no match
    0x64,       // 1196 -  - no match
    0x64,       // 1197 -  - no match
    0x64,       // 1198 -  - no match
    0x64,       // 1199 -  - no match
    0x64,       // 119A -  - no match
    0x64,       // 119B -  - no match
    0x64,       // 119C -  - no match
    0x64,       // 119D -  - no match
    0x8D,       // 119E - .
    0x64,       // 119F -  - no match

    0x64,       // 11A0 - .厄 - no match
    0x8E,       // 11A1 - .太
    0x64,       // 11A2 - .. - no match
    0x64,       // 11A3 - unused
    0x64,       // 11A4 - unused
    0x64,       // 11A5 - unused
    0x64,       // 11A6 - unused
    0x64,       // 11A7 - unused
    0x31,       // 11A8 - 丑
    0x32,       // 11A9 - 丐
    0x33,       // 11AA - 不
    0x34,       // 11AB - 中
    0x35,       // 11AC - 丰
    0x36,       // 11AD - 丹
    0x37,       // 11AE - 之
    0x39,       // 11AF - 予

    0x3A,       // 11B0 - 云
    0x3B,       // 11B1 - 井
    0x3C,       // 11B2 - 互
    0x3D,       // 11B3 - 五
    0x3E,       // 11B4 - 亢
    0x3F,       // 11B5 - 仁
    0x40,       // 11B6 - 什
    0x41,       // 11B7 - 仃
    0x42,       // 11B8 - 仆
    0x44,       // 11B9 - 仍
    0x45,       // 11BA - 今
    0x46,       // 11BB - 介
    0x47,       // 11BC - 仄
    0x48,       // 11BD - 元
    0x4A,       // 11BE - 內
    0x4B,       // 11BF - 六

    0x4C,       // 11C0 - 兮
    0x4D,       // 11C1 - 公
    0x4E,       // 11C2 - 冗
    0x64,       // 11C3 - 丑予 - no match
    0x64,       // 11C4 - 不丑 - no match
    0x64,       // 11C5 - 中丑 - no match
    0x66,       // 11C6 - 中之
    0x67,       // 11C7 - 中今
    0x68,       // 11C8 - 中^
    0x64,       // 11C9 - 中兮 - no match
    0x64,       // 11CA - 之丑 - no match
    0x64,       // 11CB - 之予 - no match
    0x69,       // 11CC - 云今
    0x64,       // 11CD - 予中 - no match
    0x6A,       // 11CE - 予之
    0x64,       // 11CF - 予之冗 - no match

    0x64,       // 11D0 - 予予 - no match
    0x64,       // 11D1 - 井丑 - no match
    0x64,       // 11D2 - 井今 - no match
    0x6B,       // 11D3 - 互今
    0x64,       // 11D4 - 互冗 - no match
    0x64,       // 11D5 - 互仄 - no match
    0x64,       // 11D6 - 五今 - no match
    0x6C,       // 11D7 - 予^
    0x64,       // 11D8 - 予六 - no match
    0x6D,       // 11D9 - 予天仄
    0x64,       // 11DA - 仃丑 - no match
    0x64,       // 11DB - 仃予 - no match
    0x6E,       // 11DC - 仃仆
    0x6F,       // 11DD - 仃今
    0x64,       // 11DE - 仃今今 - no match
    0x70,       // 11DF - 仃^

    0x64,       // 11E0 - 仃內 - no match
    0x64,       // 11E1 - 仃冗 - no match
    0x71,       // 11E2 - 仃仄
    0x64,       // 11E3 - 仆予 - no match
    0x64,       // 11E4 - 仆公 - no match
    0x64,       // 11E5 - 仆冗 - no match
    0x78,       // 11E6 - 仆仄
    0x7A,       // 11E7 - 今丑
    0x7C,       // 11E8 - 今之
    0x64,       // 11E9 - 今予 - no match
    0x7D,       // 11EA - 今仆
    0x7F,       // 11EB - ^
    0x64,       // 11EC - 仄丑 - no match
    0x64,       // 11ED - 仄丑丑 - no match
    0x80,       // 11EE - 仄仄
    0x64,       // 11EF - 仄六 - no match

    0x81,       // 11F0 - 仄
    0x82,       // 11F1 - 仄今
    0x83,       // 11F2 - 仄^
    0x64,       // 11F3 - 公仆 - no match
    0x84,       // 11F4 - 公仄
    0x64,       // 11F5 - 冗中 - no match
    0x64,       // 11F6 - 冗予 - no match
    0x64,       // 11F7 - 冗仃 - no match
    0x64,       // 11F8 - 冗仆 - no match
    0x86,       // 11F9 - 天仄
    0x64,       // 11FA - unused
    0x64,       // 11FB - unused
    0x64,       // 11FC - unused
    0x64,       // 11FD - unused
    0x64,       // 11FE - unused
    0x64,       // 11FF - unused
};


// decompose_jamo
//
// break the precomposed hangul syllables into the composite jamo
//
// Parameters:
//  wzDst        -> (WCHAR*) ptr to output buffer
//               <- (WCHAR*) expanded (decomposed) string
//  wzSrc        -> (WCHAR*) input string to expand
//  rgCharInfo   -> (CHAR_INFO*) ptr to CharInfo buffer
//               <- (char*) CharStart info for string
//  wzMaxDst     -> (int) size of output buffer
//
// Note: this code assumes that wzDst is large enough to hold the
// decomposed string.  it should be 3x the size of wzSrc.
//
// Result:
//  (void)
//
// 16MAR00  bhshin   porting for WordBreaker
void
decompose_jamo(WCHAR *wzDst, const WCHAR *wzSrc, CHAR_INFO_REC *rgCharInfo, int nMaxDst)
{
    const WCHAR *pwzS;
    WCHAR *pwzD, wch;
    CHAR_INFO_REC *pCharInfo = rgCharInfo;
    unsigned short nToken = 0;
    
    pwzS = wzSrc;
    pwzD = wzDst;
    for (; *pwzS != L'\0'; pwzS++, nToken++)
    {
        ATLASSERT(nMaxDst > 0);
        
		wch = *pwzS;

        if (fIsHangulSyllable(wch))
        {
            int nIndex = (wch - HANGUL_PRECOMP_BASE);
            int nL, nV, nT;
            WCHAR wchL, wchV, wchT;

            nL = nIndex / (NUM_JUNGSEONG * NUM_JONGSEONG);
            nV = (nIndex % (NUM_JUNGSEONG * NUM_JONGSEONG)) / NUM_JONGSEONG;
            nT = nIndex % NUM_JONGSEONG;

            // output L
            wchL = HANGUL_CHOSEONG + nL;
            *pwzD++ = wchL;
            pCharInfo->nToken = nToken;
            pCharInfo->fValidStart = 1;
            pCharInfo->fValidEnd = 0;
            pCharInfo++;

            // output V
            wchV = HANGUL_JUNGSEONG + nV;
            *pwzD++ = wchV;
            pCharInfo->nToken = nToken;
            pCharInfo->fValidStart = 0;
			if (nT != 0)
	            pCharInfo->fValidEnd = 0;	// 3-char syllable - not a valid end
			else
	            pCharInfo->fValidEnd = 1;	// 2-char syllable - mark end as valid
            pCharInfo++;

            // output T (if present)
            if (nT != 0)
            {
                wchT = HANGUL_JONGSEONG + (nT-1);
                *pwzD++ = wchT;
	            pCharInfo->nToken = nToken;
                pCharInfo->fValidStart = 0;
                pCharInfo->fValidEnd = 1;
                pCharInfo++;
            }
        }
        else
        {
            // just copy over the char
            *pwzD++ = *pwzS;
            pCharInfo->nToken = nToken;
            pCharInfo->fValidStart = 1;
            pCharInfo->fValidEnd = 1;
            pCharInfo++;
        }
    }
    *pwzD = L'\0';
    pCharInfo->nToken = nToken;
    pCharInfo++;
}


// compose_jamo
//
// take the jamo chars and combine them into precomposed forms
//
// Parameters:
//  pwzDst  <- (WCHAR*) human-readable bit string
//  pwzSrc  -> (WCHAR*) string buffer to write output string
//  wzMaxDst -> (int) size of output buffer
//
// Result:
//  (int)  number of chars in output string
//
// 11APR00  bhshin   check output buffer overflow
// 16MAR00  bhshin   porting for WordBreaker
int
compose_jamo(WCHAR *wzDst, const WCHAR *wzSrc, int nMaxDst)
{
    const WCHAR *pwzS;
    WCHAR *pwzD, wchL, wchV, wchT, wchS;
    int nChars=0;

    pwzS = wzSrc;
    pwzD = wzDst;
    for (; *pwzS != L'\0';)
    {
        ATLASSERT(nChars < nMaxDst);

		// output buffer overflow
		if (nChars >= nMaxDst)
		{
			// make output string empty
			*wzDst = L'0';
			return 0;
		}
        
		wchL = *pwzS;
        wchV = *(pwzS+1);

        // if the L or V aren't valid, consume 1 char and continue
        if (!fIsChoSeong(wchL) || !fIsJungSeong(wchV))
        {
            if (fIsHangulJamo(wchL))
            {
                // convert from conjoining-jamo to compatibility-jamo
                wchS = g_rgchXJamoMap[wchL-HANGUL_JAMO_BASE];
                wchS += HANGUL_xJAMO_PAGE;
                *pwzD++ = wchS;
                pwzS++;
            }
            else
            {
                // just copy over the unknown char
                *pwzD++ = *pwzS++;
            }
            nChars++;
            continue;
        }

        wchL -= HANGUL_CHOSEONG;
        wchV -= HANGUL_JUNGSEONG;
        pwzS += 2;

        // calc (optional) T
        wchT = *pwzS;
        if (!fIsJongSeong(wchT))
            wchT = 0;
        else
        {
            wchT -= (HANGUL_JONGSEONG-1);
            pwzS++;
        }

        wchS = ((wchL * NUM_JUNGSEONG + wchV) * NUM_JONGSEONG) + wchT + HANGUL_PRECOMP_BASE;
        ATLASSERT(fIsHangulSyllable(wchS));
        
        *pwzD++ = wchS;
        nChars++;
    }
    *pwzD = L'\0';

    return nChars;
}

// compose_length
//
// get the composed string length of input decomposed jamo
//
// Parameters:
//  wszInput  <- (const WCHAR*) input decomposed string (NULL terminated)
//
// Result:
//  (int)  number of chars in composed string
//
// 21MAR00  bhshin   created
int 
compose_length(const WCHAR *wszInput)
{
	const WCHAR *pwzInput;
	
	pwzInput = wszInput;
	
	int cch = 0;
	while (*pwzInput != L'\0')
	{
		if (!fIsChoSeong(*pwzInput) && !fIsJongSeong(*pwzInput))
			cch++;

		pwzInput++;
	}

	return cch;
}

// compose_length
//
// get the composed string length of input decomposed jamo
//
// Parameters:
//  wszInput  <- (const WCHAR*) input decomposed string (NULL terminated)
//  cchInput  <- (int) length of input string
//
// Result:
//  (int)  number of chars in composed string
//
// 15MAY00  bhshin   created
int 
compose_length(const WCHAR *wszInput, int cchInput)
{
	const WCHAR *pwzInput;
	
	pwzInput = wszInput;
	
	int cch = 0;
	int idxInput = 0;
	while (*pwzInput != L'\0' && idxInput < cchInput)
	{
		if (!fIsChoSeong(*pwzInput) && !fIsJongSeong(*pwzInput))
			cch++;

		pwzInput++;
		idxInput++;
	}

	return cch;
}


