// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
////////////////////////////////////////////////////////////////////////////
//
//  Class:    NLSDataTable
//
//  Author:   Yung-Shin Bala Lin (YSLin)
//
//  Purpose:  This class provides tables needed by CultureInfo and RegionInfo.
//            The table includes the data tables, hash tables, and collision
//            tables. The tables are generated by NLSTableBuilder.
//
//  Date:     April 15, 1999
//
////////////////////////////////////////////////////////////////////////////

#if _USE_NLS_PLUS_TABLE
// When NLS+ tables are used, we don't need these data.
#else
namespace System.Globalization {

    //All of these classes contain only static data and don't need to be serialized.
	using System;
    internal class NLSDataTable
    {
        internal static NLSDataItem[] _dataTable =
        {
            //               name     LCID WinLcid 639-1 639-2 regName 3166-13166-2 -%  % %sym
            new NLSDataItem( "\x0000", 0x0000, 0x0409, "en", "eng", null, "US", "USA", 0, 0, "%" ),

            new NLSDataItem( "ar"    , 0x0001, 0x0401, "ar", "ara", null, "SA", "SAU", 0, 0, "%" ),

            new NLSDataItem( "ar-sa" , 0x0401, 0x0401, "ar", "ara", "sa", "SA", "SAU", 0, 0, "%" ),

            new NLSDataItem( "ar-iq" , 0x0801, 0x0801, "ar", "ara", "iq", "IQ", "IRQ", 0, 0, "%" ),

            new NLSDataItem( "ar-eg" , 0x0c01, 0x0c01, "ar", "ara", "eg", "EG", "EGY", 0, 0, "%" ),

            new NLSDataItem( "ar-ly" , 0x1001, 0x1001, "ar", "ara", "ly", "LY", "LBY", 0, 0, "%" ),

            new NLSDataItem( "ar-dz" , 0x1401, 0x1401, "ar", "ara", "dz", "DZ", "DZA", 0, 0, "%" ),

            new NLSDataItem( "ar-ma" , 0x1801, 0x1801, "ar", "ara", "ma", "MA", "MAR", 0, 0, "%" ),

            new NLSDataItem( "ar-tn" , 0x1c01, 0x1c01, "ar", "ara", "tn", "TN", "TUN", 0, 0, "%" ),

            new NLSDataItem( "ar-om" , 0x2001, 0x2001, "ar", "ara", "om", "OM", "OMN", 0, 0, "%" ),

            new NLSDataItem( "ar-ye" , 0x2401, 0x2401, "ar", "ara", "ye", "YE", "YEM", 0, 0, "%" ),

            new NLSDataItem( "ar-sy" , 0x2801, 0x2801, "ar", "ara", "sy", "SY", "SYR", 0, 0, "%" ),

            new NLSDataItem( "ar-jo" , 0x2c01, 0x2c01, "ar", "ara", "jo", "JO", "JOR", 0, 0, "%" ),

            new NLSDataItem( "ar-lb" , 0x3001, 0x3001, "ar", "ara", "lb", "LB", "LBN", 0, 0, "%" ),

            new NLSDataItem( "ar-kw" , 0x3401, 0x3401, "ar", "ara", "kw", "KW", "KWT", 0, 0, "%" ),

            new NLSDataItem( "ar-ae" , 0x3801, 0x3801, "ar", "ara", "ae", "AE", "ARE", 0, 0, "%" ),

            new NLSDataItem( "ar-bh" , 0x3c01, 0x3c01, "ar", "ara", "bh", "BH", "BHR", 0, 0, "%" ),

            new NLSDataItem( "ar-qa" , 0x4001, 0x4001, "ar", "ara", "qa", "QA", "QAT", 0, 0, "%" ),

            new NLSDataItem( "bg"    , 0x0002, 0x0402, "bg", "bul", null, "BG", "BGR", 0, 0, "%" ),

            new NLSDataItem( "bg-bg" , 0x0402, 0x0402, "bg", "bul", "bg", "BG", "BGR", 0, 0, "%" ),

            new NLSDataItem( "ca"    , 0x0003, 0x0403, "ca", "cat", null, "ES", "ESP", 0, 0, "%" ),

            new NLSDataItem( "ca-es" , 0x0403, 0x0403, "ca", "cat", "es", "ES", "ESP", 0, 0, "%" ),

            new NLSDataItem( "zh"    , 0x0004, 0x0804, "zh", "zho", null, "TW", "TWN", 0, 0, "%" ),

            new NLSDataItem( "zh-tw" , 0x0404, 0x0404, "zh", "zho", "tw", "TW", "TWN", 0, 0, "%" ),

            new NLSDataItem( "zh-cn" , 0x0804, 0x0804, "zh", "zho", "cn", "CN", "CHN", 0, 0, "%" ),

            new NLSDataItem( "zh-hk" , 0x0c04, 0x0c04, "zh", "zho", "hk", "HK", "HKG", 0, 0, "%" ),

            new NLSDataItem( "zh-sg" , 0x1004, 0x1004, "zh", "zho", "sg", "SG", "SGP", 0, 0, "%" ),

            new NLSDataItem( "zh-mo" , 0x1404, 0x1404, "zh", "zho", "mo", "MO", "MCO", 0, 0, "%" ),

            new NLSDataItem( "cs"    , 0x0005, 0x0405, "cs", "ces", null, "CZ", "CZE", 0, 0, "%" ),

            new NLSDataItem( "cs-cz" , 0x0405, 0x0405, "cs", "ces", "cz", "CZ", "CZE", 0, 0, "%" ),

            new NLSDataItem( "da"    , 0x0006, 0x0406, "da", "dan", null, "DK", "DNK", 0, 0, "%" ),

            new NLSDataItem( "da-dk" , 0x0406, 0x0406, "da", "dan", "dk", "DK", "DNK", 0, 0, "%" ),

            new NLSDataItem( "de"    , 0x0007, 0x0407, "de", "deu", null, "DE", "DEU", 0, 0, "%" ),

            new NLSDataItem( "de-de" , 0x0407, 0x0407, "de", "deu", "de", "DE", "DEU", 0, 0, "%" ),

            new NLSDataItem( "de-ch" , 0x0807, 0x0807, "de", "deu", "ch", "CH", "CHE", 1, 1, "%" ),

            new NLSDataItem( "de-at" , 0x0c07, 0x0c07, "de", "deu", "at", "AT", "AUT", 0, 0, "%" ),

            new NLSDataItem( "de-lu" , 0x1007, 0x1007, "de", "deu", "lu", "LU", "LUX", 0, 0, "%" ),

            new NLSDataItem( "de-li" , 0x1407, 0x1407, "de", "deu", "li", "LI", "LIE", 0, 0, "%" ),

            new NLSDataItem( "el"    , 0x0008, 0x0408, "el", "ell", null, "GR", "GRC", 0, 0, "%" ),

            new NLSDataItem( "el-gr" , 0x0408, 0x0408, "el", "ell", "gr", "GR", "GRC", 0, 0, "%" ),

            new NLSDataItem( "en"    , 0x0009, 0x0409, "en", "eng", null, "US", "USA", 0, 0, "%" ),

            new NLSDataItem( "en-us" , 0x0409, 0x0409, "en", "eng", "us", "US", "USA", 0, 0, "%" ),

            new NLSDataItem( "en-gb" , 0x0809, 0x0809, "en", "eng", "gb", "GB", "GBR", 0, 0, "%" ),

            new NLSDataItem( "en-au" , 0x0c09, 0x0c09, "en", "eng", "au", "AU", "AUS", 0, 0, "%" ),

            new NLSDataItem( "en-ca" , 0x1009, 0x1009, "en", "eng", "ca", "CA", "CAN", 0, 0, "%" ),

            new NLSDataItem( "en-nz" , 0x1409, 0x1409, "en", "eng", "nz", "NZ", "NZL", 0, 0, "%" ),

            new NLSDataItem( "en-ie" , 0x1809, 0x1809, "en", "eng", "ie", "IE", "IRL", 0, 0, "%" ),

            new NLSDataItem( "en-za" , 0x1c09, 0x1c09, "en", "eng", "za", "ZA", "ZAF", 1, 1, "%" ),

            new NLSDataItem( "en-jm" , 0x2009, 0x2009, "en", "eng", "jm", "JM", "JAM", 0, 0, "%" ),

            new NLSDataItem( "en-cb" , 0x2409, 0x2409, "en", "eng", "cb", "CB", "CAR", 0, 0, "%" ),

            new NLSDataItem( "en-bz" , 0x2809, 0x2809, "en", "eng", "bz", "BZ", "BLZ", 0, 0, "%" ),

            new NLSDataItem( "en-tt" , 0x2c09, 0x2c09, "en", "eng", "tt", "TT", "TTO", 0, 0, "%" ),

            new NLSDataItem( "en-zw" , 0x3009, 0x3009, "en", "eng", "zw", "ZW", "ZWE", 0, 0, "%" ),

            new NLSDataItem( "en-ph" , 0x3409, 0x3409, "en", "eng", "ph", "PH", "PHL", 0, 0, "%" ),

            new NLSDataItem( "es"    , 0x000a, 0x040a, "es", "spa", null, "ES", "ESP", 0, 0, "%" ),

            new NLSDataItem( "es-es" , 0x040a, 0x040a, "es", "spa", null, "ES", "ESP", 0, 0, "%" ),

            new NLSDataItem( "es-mx" , 0x080a, 0x080a, "es", "spa", "mx", "MX", "MEX", 0, 0, "%" ),

            new NLSDataItem( "es-es2", 0x0c0a, 0x0c0a, "es", "spa", null, "ES", "ESP", 0, 0, "%" ),

            new NLSDataItem( "es-gt" , 0x100a, 0x100a, "es", "spa", "gt", "GT", "GTM", 0, 0, "%" ),

            new NLSDataItem( "es-cr" , 0x140a, 0x140a, "es", "spa", "cr", "CR", "CRI", 0, 0, "%" ),

            new NLSDataItem( "es-pa" , 0x180a, 0x180a, "es", "spa", "pa", "PA", "PAN", 0, 0, "%" ),

            new NLSDataItem( "es-do" , 0x1c0a, 0x1c0a, "es", "spa", "do", "DO", "DOM", 0, 0, "%" ),

            new NLSDataItem( "es-ve" , 0x200a, 0x200a, "es", "spa", "ve", "VE", "VEN", 0, 0, "%" ),

            new NLSDataItem( "es-co" , 0x240a, 0x240a, "es", "spa", "co", "CO", "COL", 0, 0, "%" ),

            new NLSDataItem( "es-pe" , 0x280a, 0x280a, "es", "spa", "pe", "PE", "PER", 0, 0, "%" ),

            new NLSDataItem( "es-ar" , 0x2c0a, 0x2c0a, "es", "spa", "ar", "AR", "ARG", 0, 0, "%" ),

            new NLSDataItem( "es-ec" , 0x300a, 0x300a, "es", "spa", "ec", "EC", "ECU", 0, 0, "%" ),

            new NLSDataItem( "es-cl" , 0x340a, 0x340a, "es", "spa", "cl", "CL", "CHL", 0, 0, "%" ),

            new NLSDataItem( "es-uy" , 0x380a, 0x380a, "es", "spa", "uy", "UY", "URY", 0, 0, "%" ),

            new NLSDataItem( "es-py" , 0x3c0a, 0x3c0a, "es", "spa", "py", "PY", "PRY", 0, 0, "%" ),

            new NLSDataItem( "es-bo" , 0x400a, 0x400a, "es", "spa", "bo", "BO", "BOL", 0, 0, "%" ),

            new NLSDataItem( "es-sv" , 0x440a, 0x440a, "es", "spa", "sv", "SV", "SLV", 0, 0, "%" ),

            new NLSDataItem( "es-hn" , 0x480a, 0x480a, "es", "spa", "hn", "HN", "HND", 0, 0, "%" ),

            new NLSDataItem( "es-ni" , 0x4c0a, 0x4c0a, "es", "spa", "ni", "NI", "NIC", 0, 0, "%" ),

            new NLSDataItem( "es-pr" , 0x500a, 0x500a, "es", "spa", "pr", "PR", "PRI", 0, 0, "%" ),

            new NLSDataItem( "fi"    , 0x000b, 0x040b, "fi", "fin", null, "FI", "FIN", 0, 0, "%" ),

            new NLSDataItem( "fi-fi" , 0x040b, 0x040b, "fi", "fin", "fi", "FI", "FIN", 0, 0, "%" ),

            new NLSDataItem( "fr"    , 0x000c, 0x040c, "fr", "fra", null, "FR", "FRA", 0, 0, "%" ),

            new NLSDataItem( "fr-fr" , 0x040c, 0x040c, "fr", "fra", "fr", "FR", "FRA", 0, 0, "%" ),

            new NLSDataItem( "fr-be" , 0x080c, 0x080c, "fr", "fra", "be", "BE", "BEL", 0, 0, "%" ),

            new NLSDataItem( "fr-ca" , 0x0c0c, 0x0c0c, "fr", "fra", null, "CA", "CAN", 0, 0, "%" ),

            new NLSDataItem( "fr-ch" , 0x100c, 0x100c, "fr", "fra", null, "CH", "CHE", 1, 1, "%" ),

            new NLSDataItem( "fr-lu" , 0x140c, 0x140c, "fr", "fra", null, "LU", "LUX", 0, 0, "%" ),

            new NLSDataItem( "fr-mc" , 0x180c, 0x180c, "fr", "fra", "mc", "MC", "MCO", 0, 0, "%" ),

            new NLSDataItem( "he"    , 0x000d, 0x040d, "iw", "iku", null, "IL", "ISR", 0, 0, "%" ),

            new NLSDataItem( "he-il" , 0x040d, 0x040d, "iw", "iku", "il", "IL", "ISR", 1, 1, "%" ),

            new NLSDataItem( "hu"    , 0x000e, 0x040e, "hu", "hun", null, "HU", "HUN", 0, 0, "%" ),

            new NLSDataItem( "hu-hu" , 0x040e, 0x040e, "hu", "hun", "hu", "HU", "HUN", 0, 0, "%" ),

            new NLSDataItem( "is"    , 0x000f, 0x040f, "is", "isl", null, "IS", "ISL", 0, 0, "%" ),

            new NLSDataItem( "is-is" , 0x040f, 0x040f, "is", "isl", "is", "IS", "ISL", 1, 1, "%" ),

            new NLSDataItem( "it"    , 0x0010, 0x0410, "it", "ita", null, "IT", "ITA", 0, 0, "%" ),

            new NLSDataItem( "it-it" , 0x0410, 0x0410, "it", "ita", "it", "IT", "ITA", 0, 0, "%" ),

            new NLSDataItem( "it-ch" , 0x0810, 0x0810, "it", "ita", null, "CH", "CHE", 1, 1, "%" ),

            new NLSDataItem( "ja"    , 0x0011, 0x0411, "ja", "jpn", null, "JP", "JPN", 0, 0, "%" ),

            new NLSDataItem( "ja-jp" , 0x0411, 0x0411, "ja", "jpn", "jp", "JP", "JPN", 0, 0, "%" ),

            new NLSDataItem( "ko"    , 0x0012, 0x0412, "ko", "kor", null, "KR", "KOR", 0, 0, "%" ),

            new NLSDataItem( "ko-kr" , 0x0412, 0x0412, "ko", "kor", "kr", "KR", "KOR", 0, 0, "%" ),

            new NLSDataItem( "nl"    , 0x0013, 0x0413, "nl", "nld", null, "NL", "NLD", 0, 0, "%" ),

            new NLSDataItem( "nl-nl" , 0x0413, 0x0413, "nl", "nld", "nl", "NL", "NLD", 0, 0, "%" ),

            new NLSDataItem( "nl-be" , 0x0813, 0x0813, "nl", "nld", null, "BE", "BEL", 0, 0, "%" ),

            new NLSDataItem( "no"    , 0x0014, 0x0414, "no", "nor", null, "NO", "NOR", 0, 0, "%" ),

            new NLSDataItem( "no-no" , 0x0414, 0x0414, "no", "nor", "no", "NO", "NOR", 0, 0, "%" ),

            new NLSDataItem( "no-bok", 0x0814, 0x0814, "no", "nor", null, "NO", "NOR", 0, 0, "%" ),

            new NLSDataItem( "pl"    , 0x0015, 0x0415, "pl", "pol", null, "PL", "POL", 0, 0, "%" ),

            new NLSDataItem( "pl-pl" , 0x0415, 0x0415, "pl", "pol", "pl", "PL", "POL", 0, 0, "%" ),

            new NLSDataItem( "pt"    , 0x0016, 0x0416, "pt", "por", null, "BR", "BRA", 0, 0, "%" ),

            new NLSDataItem( "pt-br" , 0x0416, 0x0416, "pt", "por", "br", "BR", "BRA", 1, 1, "%" ),

            new NLSDataItem( "pt-pt" , 0x0816, 0x0816, "pt", "por", "pt", "PT", "PRT", 0, 0, "%" ),

            new NLSDataItem( "ro"    , 0x0018, 0x0418, "ro", "ron", null, "RO", "ROM", 0, 0, "%" ),

            new NLSDataItem( "ro-ro" , 0x0418, 0x0418, "ro", "ron", "ro", "RO", "ROM", 0, 0, "%" ),

            new NLSDataItem( "ru"    , 0x0019, 0x0419, "ru", "rus", null, "RU", "RUS", 0, 0, "%" ),

            new NLSDataItem( "ru-ru" , 0x0419, 0x0419, "ru", "rus", "ru", "RU", "RUS", 0, 0, "%" ),

            new NLSDataItem( "hr"    , 0x001a, 0x041a, "hr", "hrv", null, "HR", "HRV", 0, 0, "%" ),

            new NLSDataItem( "hr-hr" , 0x041a, 0x041a, "hr", "hrv", "hr", "HR", "HRV", 0, 0, "%" ),

            new NLSDataItem( "sr"    , 0x081a, 0x081a, "sr", "srp", "sp", "SP", "SPB", 0, 0, "%" ),

            new NLSDataItem( "sr-sp" , 0x0c1a, 0x0c1a, "sr", "srp", null, "SP", "SPB", 0, 0, "%" ),

            new NLSDataItem( "sk"    , 0x001b, 0x041b, "sk", "slk", null, "SK", "SVK", 0, 0, "%" ),

            new NLSDataItem( "sk-sk" , 0x041b, 0x041b, "sk", "slk", "sk", "SK", "SVK", 0, 0, "%" ),

            new NLSDataItem( "sq"    , 0x001c, 0x041c, "sq", "sqi", null, "AL", "ALB", 0, 0, "%" ),

            new NLSDataItem( "sq-al" , 0x041c, 0x041c, "sq", "sqi", "al", "AL", "ALB", 0, 0, "%" ),

            new NLSDataItem( "sv"    , 0x001d, 0x041d, "sv", "swe", null, "SE", "SWE", 0, 0, "%" ),

            new NLSDataItem( "sv-se" , 0x041d, 0x041d, "sv", "swe", "se", "SE", "SWE", 0, 0, "%" ),

            new NLSDataItem( "sv-fi" , 0x081d, 0x081d, "sv", "swe", null, "FI", "FIN", 0, 0, "%" ),

            new NLSDataItem( "th"    , 0x001e, 0x041e, "th", "tha", null, "TH", "THA", 0, 0, "%" ),

            new NLSDataItem( "th-th" , 0x041e, 0x041e, "th", "tha", "th", "TH", "THA", 0, 0, "%" ),

            new NLSDataItem( "tr"    , 0x001f, 0x041f, "tr", "tgl", null, "TR", "TUR", 0, 0, "%" ),

            new NLSDataItem( "tr-tr" , 0x041f, 0x041f, "tr", "tgl", "tr", "TR", "TUR", 2, 2, "%" ),

            new NLSDataItem( "ur"    , 0x0020, 0x0420, "ur", "urd", null, "PK", "PAK", 0, 0, "%" ),

            new NLSDataItem( "ur-pk" , 0x0420, 0x0420, "ur", "urd", "pk", "PK", "PAK", 0, 0, "%" ),

            new NLSDataItem( "id"    , 0x0021, 0x0421, "id", "ina", null, "ID", "IDN", 0, 0, "%" ),

            new NLSDataItem( "id-id" , 0x0421, 0x0421, "id", "ina", "id", "ID", "IDN", 0, 0, "%" ),

            new NLSDataItem( "uk"    , 0x0022, 0x0422, "uk", "ukr", null, "UA", "UKR", 0, 0, "%" ),

            new NLSDataItem( "uk-ua" , 0x0422, 0x0422, "uk", "ukr", "ua", "UA", "UKR", 0, 0, "%" ),

            new NLSDataItem( "be"    , 0x0023, 0x0423, "be", "bel", null, "BY", "BLR", 0, 0, "%" ),

            new NLSDataItem( "be-by" , 0x0423, 0x0423, "be", "bel", "by", "BY", "BLR", 0, 0, "%" ),

            new NLSDataItem( "sl"    , 0x0024, 0x0424, "sl", "slv", null, "SI", "SVN", 0, 0, "%" ),

            new NLSDataItem( "sl-si" , 0x0424, 0x0424, "sl", "slv", "si", "SI", "SVN", 0, 0, "%" ),

            new NLSDataItem( "et"    , 0x0025, 0x0425, "et", "est", null, "EE", "EST", 0, 0, "%" ),

            new NLSDataItem( "et-ee" , 0x0425, 0x0425, "et", "est", "ee", "EE", "EST", 0, 0, "%" ),

            new NLSDataItem( "lv"    , 0x0026, 0x0426, "lv", "lav", null, "LV", "LVA", 0, 0, "%" ),

            new NLSDataItem( "lv-lv" , 0x0426, 0x0426, "lv", "lav", "lv", "LV", "LVA", 0, 0, "%" ),

            new NLSDataItem( "lt"    , 0x0027, 0x0427, "lt", "lit", null, "LT", "LTU", 0, 0, "%" ),

            new NLSDataItem( "lt-lt" , 0x0427, 0x0427, "lt", "lit", "lt", "LT", "LTU", 0, 0, "%" ),

            new NLSDataItem( "fa"    , 0x0029, 0x0429, "fa", "fas", null, "IR", "IRN", 0, 0, "%" ),

            new NLSDataItem( "fa-ir" , 0x0429, 0x0429, "fa", "fas", "ir", "IR", "IRN", 0, 0, "%" ),

            new NLSDataItem( "vi"    , 0x002a, 0x042a, "vi", "vie", null, "VN", "VNM", 0, 0, "%" ),

            new NLSDataItem( "vi-vn" , 0x042a, 0x042a, "vi", "vie", "vn", "VN", "VNM", 0, 0, "%" ),

            new NLSDataItem( "hy"    , 0x002b, 0x042b, "hy", "hye", null, "am", "ARM", 0, 0, "%" ),

            new NLSDataItem( "hy-am" , 0x042b, 0x042b, "hy", "hye", "am", "am", "ARM", 0, 0, "%" ),

            new NLSDataItem( "az"    , 0x002c, 0x042c, "az", "aze", null, "AZ", "AZE", 0, 0, "%" ),

            new NLSDataItem( "az-az" , 0x042c, 0x042c, "az", "aze", "az", "AZ", "AZE", 0, 0, "%" ),

            new NLSDataItem( "az-az2", 0x082c, 0x082c, "az", "aze", null, "AZ", "AZE", 0, 0, "%" ),

            new NLSDataItem( "eu"    , 0x002d, 0x042d, "eu", "eus", null, "ES", "ESP", 0, 0, "%" ),

            new NLSDataItem( "eu-es" , 0x042d, 0x042d, "eu", "eus", null, "ES", "ESP", 0, 0, "%" ),

            new NLSDataItem( "mk"    , 0x002f, 0x042f, "mk", "mkd", null, "MK", "MKD", 0, 0, "%" ),

            new NLSDataItem( "mk-mk" , 0x042f, 0x042f, "mk", "mkd", "mk", "MK", "MKD", 0, 0, "%" ),

            new NLSDataItem( "af"    , 0x0036, 0x0436, "af", "afr", null, "ZA", "ZAF", 0, 0, "%" ),

            new NLSDataItem( "af-za" , 0x0436, 0x0436, "af", "afr", null, "ZA", "ZAF", 1, 1, "%" ),

            new NLSDataItem( "ka"    , 0x0037, 0x0437, "ka", "kat", null, "GE", "GEO", 0, 0, "%" ),

            new NLSDataItem( "ka-ge" , 0x0437, 0x0437, "ka", "kat", "ge", "GE", "GEO", 0, 0, "%" ),

            new NLSDataItem( "fo"    , 0x0038, 0x0438, "fo", "fao", null, "FO", "FRO", 0, 0, "%" ),

            new NLSDataItem( "fo-fo" , 0x0438, 0x0438, "fo", "fao", "fo", "FO", "FRO", 0, 0, "%" ),

            new NLSDataItem( "hi"    , 0x0039, 0x0439, "hi", "hin", null, "IN", "IND", 0, 0, "%" ),

            new NLSDataItem( "hi-in" , 0x0439, 0x0439, "hi", "hin", "in", "IN", "IND", 0, 0, "%" ),

            new NLSDataItem( "ms"    , 0x003e, 0x043e, "ms", "msa", null, "MY", "MYS", 0, 0, "%" ),

            new NLSDataItem( "ms-my" , 0x043e, 0x043e, "ms", "msa", "my", "MY", "MYS", 0, 0, "%" ),

            new NLSDataItem( "ms-br" , 0x083e, 0x083e, "ms", "msa", "bn", "BN", "BRN", 0, 0, "%" ),

            new NLSDataItem( "kk"    , 0x003f, 0x043f, "kk", "kaz", null, "KZ", "KAZ", 0, 0, "%" ),

            new NLSDataItem( "kk-kz" , 0x043f, 0x043f, "kk", "kaz", "kz", "KZ", "KAZ", 0, 0, "%" ),

            new NLSDataItem( "sw"    , 0x0041, 0x0441, "sw", "swa", null, "KE", "KEN", 0, 0, "%" ),

            new NLSDataItem( "sw-ke" , 0x0441, 0x0441, "sw", "swa", "ke", "KE", "KEN", 0, 0, "%" ),

            new NLSDataItem( "uz"    , 0x0043, 0x0443, "uz", "uzb", null, "UZ", "UZB", 0, 0, "%" ),

            new NLSDataItem( "uz-uz" , 0x0443, 0x0443, "uz", "uzb", "uz", "UZ", "UZB", 0, 0, "%" ),

            new NLSDataItem( "uz-uz2", 0x0843, 0x0843, "uz", "uzb", null, "UZ", "UZS", 0, 0, "%" ),

            new NLSDataItem( "tt"    , 0x0044, 0x0444, "tt", "tat", null, "TA", "TAT", 0, 0, "%" ),

            new NLSDataItem( "tt-ta" , 0x0444, 0x0444, "tt", "tat", "ta", "TA", "TAT", 0, 0, "%" ),

            new NLSDataItem( "ta"    , 0x0049, 0x0449, "ta", "tam", null, "IN", "IND", 0, 0, "%" ),

            new NLSDataItem( "ta-in" , 0x0449, 0x0449, "ta", "tam", null, "IN", "IND", 0, 0, "%" ),

            new NLSDataItem( "mr"    , 0x004e, 0x044e, "mr", "mar", null, "IN", "IND", 0, 0, "%" ),

            new NLSDataItem( "mr-in" , 0x044e, 0x044e, "mr", "mar", null, "IN", "IND", 0, 0, "%" ),

            new NLSDataItem( "sa"    , 0x004f, 0x044f, "sa", "san", null, "IN", "IND", 0, 0, "%" ),

            new NLSDataItem( "sa-in" , 0x044f, 0x044f, "sa", "san", null, "IN", "IND", 0, 0, "%" ),

            new NLSDataItem( "hi2"   , 0x0057, 0x0457, "hi", "hin", null, "IN", "IND", 0, 0, "%" ),

            new NLSDataItem( "hi-kok", 0x0457, 0x0457, "hi", "hin", null, "IN", "IND", 0, 0, "%" ),
        };

        ////////////////////////////////////////////////////////////////////////
        //
        // Get a NLSDataItem in NLSDataTable._dataTable by given a culture name.
        //
        ////////////////////////////////////////////////////////////////////////

        internal static int GetDataFromCultureName(String name)
        {

            BCLDebug.Assert(name!=null,"name!=null");

            for (int i = 0; i < _dataTable.Length; i++) {
                if (name.Equals(_dataTable[i].name)) {
                    return (i);
                }
            }

            return (-1);
        }


        ////////////////////////////////////////////////////////////////////////
        //
        // Get a NLSDataItem in NLSDataTable._dataTable by given a LCID.
        //
        ////////////////////////////////////////////////////////////////////////

        internal static int GetDataFromCultureID(int culture)
        {
            for (int i = 0; i < _dataTable.Length; i++) {
                if (culture == _dataTable[i].CultureID) {
                    return (i);
                }
            }

            return (-1);
        }

        ////////////////////////////////////////////////////////////////////////
        //
        // Get a NLSDataItem in NLSDataTable._dataTable by given a LCID.
        //
        ////////////////////////////////////////////////////////////////////////

        internal static int GetDataFromRegionID(int region)
        {
            for (int i = 0; i < _dataTable.Length; i++) {
                if (region == _dataTable[i].CultureID) {
                    if (_dataTable[i].RegionName != null) {
                        return (i);
                    } else {
                        return (-1);
                    }
                }
            }

            return (-1);
        }
        
        ////////////////////////////////////////////////////////////////////////
        //
        // Given a region name in the format of "us", get the NLSDataItem for that
        // region.
        //
        ////////////////////////////////////////////////////////////////////////

        internal static int GetDataFromRegionName(String name)
        {
            int len = _dataTable.Length;
            for (int i = 0; i < len; i++)
            {
                if (name.Equals(_dataTable[i].RegionName))
                {
                    return (i);
                }
            }
            
            return (-1);
        }
    }
}
#endif
