/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef L1CALOFEXBYTESTREAM_GFEXPOS_H
#define L1CALOFEXBYTESTREAM_GFEXPOS_H

#include <cstdint>

namespace LVL1::gFEXPos {

        // Header word constants
        constexpr unsigned int BLOCK_TYPE_BIT      = 28 ;
        constexpr unsigned int BLOCK_TYPE_MASK     = 0xf ;
        constexpr unsigned int HEADER_SIZE_BIT     = 22 ;
        constexpr unsigned int HEADER_SIZE_MASK    = 0x3 ;
        constexpr unsigned int ERROR_FLAG_BIT      = 12 ;
        constexpr unsigned int ERROR_FLAG_MASK     = 0x1 ;
        constexpr unsigned int DATA_SIZE_MASK      = 0xfff ;

        constexpr unsigned int  WORDS_PER_SLICE    = 14 ;// Number of words per slice is always 14

        
        //The following definitions reflect the position of the 32-bits words in the gFEX bytestream
        
        constexpr std::array<unsigned int, 2> TRAILER_POSITION        = { 6, 13 };//position of trailer in Jet/Global TOB dataframe, wihin he same slice 
        constexpr std::array<unsigned int, 5> JET_UNUSED_POSITION     = { 0, 4, 5, 11, 12 };//position of unused word in Jet TOB dataframe, wihin he same slice 
        constexpr std::array<unsigned int, 4> GLOBAL_UNUSED_POSITION  = { 4, 5, 11, 12 };//position of unused word in Global TOB dataframe, wihin he same slice 

        constexpr std::array<unsigned int, 4> GBLOCK_POSITION         = { 1, 2, 8, 9 };//position of gBlocks word in Jet TOB dataframe, within the same slice
        constexpr std::array<unsigned int, 2> GJET_POSITION           = { 3, 10 };//position of gBlocks word in Jet TOB dataframe, within the same slice
        constexpr unsigned int GRHO_POSITION                          = 7 ;//position of gRho word in Jet TOB dataframe, within the same slice
 
        constexpr unsigned int JWOJ_MHT_POSITION    = 0 ;//position of JwoJ MHT word in Global TOB dataframe, within the same slice
        constexpr unsigned int JWOJ_MST_POSITION    = 1 ;//position of JwoJ MST word in Global TOB dataframe, within the same slice
        constexpr unsigned int JWOJ_MET_POSITION    = 2 ;//position of JwoJ MET word in Global TOB dataframe, within the same slice
        constexpr unsigned int JWOJ_SCALAR_POSITION = 3 ;//position of JwoJ Scalar word in Global TOB dataframe, within the same slice

        constexpr unsigned int NC_MET_POSITION      = 7 ;//position of Noise Cut MET word in Global TOB dataframe, within the same slice
        constexpr unsigned int NC_SCALAR_POSITION   = 9;//position of Noise Cut Scalar word in Global TOB dataframe, within the same slice
 
        constexpr unsigned int RMS_MET_POSITION     = 8 ;//position of Rho+RMS MET word in Global TOB dataframe, within the same slice
        constexpr unsigned int RMS_SCALAR_POSITION  = 10;//position of Rho+RMS Scalar word in Global TOB dataframe, within the same slice



        constexpr uint32_t FPGA_A_INPUT_HEADER  = 0xa14002bc;
        constexpr uint32_t FPGA_B_INPUT_HEADER  = 0xb14002bc;
        constexpr uint32_t FPGA_C_INPUT_HEADER  = 0xc14002bc;

        constexpr int MAX_FIBERS   = 100; //maximum number of fibers in A, B and C
        constexpr int AB_FIBERS    = 80;  //Number of fibers A and B
        constexpr int C_FIBERS     = 50;  //Number of fibers C
        constexpr int MAX_E_FIELDS = 16;  //maximum number of gCaloTowers per fiber
        constexpr int MAX_FIELDS   = 20;  //maximum number of fields on fiber including BCID, etc.
        constexpr int ABC_ROWS     = 32;  //Number of rows in FPGA A, B, C
        constexpr int AB_COLUMNS   = 12;  //Number of columns in FPGA A and B
        constexpr int AB_TOWERS    = 384; //Total towers in FPGA => 12*32 = 384 towers


        constexpr std::array<unsigned int, 80>  GTRX_MAP_A_IND =  
        {
          88, 89, 93, 92, 94, 95, 90, 96, 97, 98, 99, 91, 76, 77, 79, 78,
          80, 81, 82, 83, 87, 84, 86, 85, 64, 65, 67, 66, 68, 69, 70, 71,
          75, 72, 74, 73, 52, 53, 55, 54, 56, 57, 58, 59, 63, 60, 62, 61,
          10,  9, 11,  8,  7,  6,  4,  5,  0,  3,  1,  2, 22, 21, 23, 20,
          34, 33, 35, 32, 31, 30, 28, 29, 24, 27, 25, 26, 47, 44, 45, 46
        };

        constexpr std::array<unsigned int, 80>  GTRX_MAP_B_IND = 
        {
          88, 89, 93, 92, 94, 95, 90, 96, 91, 98, 99, 97, 76, 77, 79, 78,
          80, 81, 82, 83, 87, 84, 86, 85, 64, 65, 67, 66, 68, 69, 70, 71,
          75, 72, 74, 73, 52, 53, 55, 54, 56, 57, 58, 59, 63, 60, 62, 61,
          39, 47, 38, 45, 46, 44, 42, 43, 36, 41, 37, 40, 34, 33, 35, 32,
          22, 21, 23, 20, 19, 18, 16, 17, 12, 15, 13, 14, 10,  9, 11,  8
          
        };

        constexpr std::array<unsigned int, 50>  GTRX_MAP_C_IND =  
        {
          47, 46, 45, 44, 43,
          41, 40, 39, 38, 37, 36, 35, 34,
          23, 22, 20, 19,
          17, 16, 15, 14, 13, 12, 11, 10,
          95, 94, 93, 92, 91,
          89, 88, 87, 86, 85, 84, 83, 82,
          71, 70, 68, 67,
          65, 64, 63, 62, 61, 60, 59, 58
        };


        constexpr std::array<int, 100> AMPD_NFI = 
        {
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
          1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,
          1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2
        };

        constexpr std::array<int, 100> ACALO_TYPE = 
        {
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
          1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,
          1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2
        };


        constexpr std::array<std::array<int, 16>, 100> AMPD_GTRN_ARR =
        {{
          {  3,  3,  1,  1, 15, 15, 13, 13,  2,  2,  0,  0, 12, 12, 14, 14}, //EMECC_1 
          { 27, 27, 25, 25, 39, 39, 37, 37, 26, 26, 24, 24, 36, 36, 38, 38}, //EMECC_2 
          { 51, 51, 49, 49, 63, 63, 61, 61, 50, 50, 48, 48, 60, 60, 62, 62}, //EMECC_3 
          { 75, 75, 73, 73, 87, 87, 85, 85, 74, 74, 72, 72, 84, 84, 86, 86}, //EMECC_4 
          { 99, 99, 97, 97,111,111,109,109, 98, 98, 96, 96,108,108,110,110}, //EMECC_5 
          {123,123,121,121,135,135,133,133,122,122,120,120,132,132,134,134}, //EMECC_6 
          {147,147,145,145,159,159,157,157,146,146,144,144,156,156,158,158}, //EMECC_7 
          {171,171,169,169,183,183,181,181,170,170,168,168,180,180,182,182}, //EMECC_8 
          {  7,  7,  5,  5, 19, 19, 17, 17,  6,  6,  4,  4, 16, 16, 18, 18}, //EMBC-EMECC_1 
          { 31, 31, 29, 29, 43, 43, 41, 41, 30, 30, 28, 28, 40, 40, 42, 42}, //EMBC-EMECC_2 
          { 55, 55, 53, 53, 67, 67, 65, 65, 54, 54, 52, 52, 64, 64, 66, 66}, //EMBC-EMECC_3 
          { 79, 79, 77, 77, 91, 91, 89, 89, 78, 78, 76, 76, 88, 88, 90, 90}, //EMBC-EMECC_4 
          {103,103,101,101,115,115,113,113,102,102,100,100,112,112,114,114}, //EMBC-EMECC_5 
          {127,127,125,125,139,139,137,137,126,126,124,124,136,136,138,138}, //EMBC-EMECC_6 
          {151,151,149,149,163,163,161,161,150,150,148,148,160,160,162,162}, //EMBC-EMECC_7 
          {175,175,173,173,187,187,185,185,174,174,172,172,184,184,186,186}, //EMBC-EMECC_8 
          { 11, 11,  9,  9, 23, 23, 21, 21, 10, 10,  8,  8, 20, 20, 22, 22}, //EMBC_1 
          { 35, 35, 33, 33, 47, 47, 45, 45, 34, 34, 32, 32, 44, 44, 46, 46}, //EMBC_2 
          { 59, 59, 57, 57, 71, 71, 69, 69, 58, 58, 56, 56, 68, 68, 70, 70}, //EMBC_3 
          { 83, 83, 81, 81, 95, 95, 93, 93, 82, 82, 80, 80, 92, 92, 94, 94}, //EMBC_4 
          {107,107,105,105,119,119,117,117,106,106,104,104,116,116,118,118}, //EMBC_5 
          {131,131,129,129,143,143,141,141,130,130,128,128,140,140,142,142}, //EMBC_6 
          {155,155,153,153,167,167,165,165,154,154,152,152,164,164,166,166}, //EMBC_7 
          {179,179,177,177,191,191,189,189,178,178,176,176,188,188,190,190}, //EMBC_8 
          {195,195,193,193,207,207,205,205,194,194,192,192,204,204,206,206}, //EMECC_9 
          {219,219,217,217,231,231,229,229,218,218,216,216,228,228,230,230}, //EMECC_10 
          {243,243,241,241,255,255,253,253,242,242,240,240,252,252,254,254}, //EMECC_11 
          {267,267,265,265,279,279,277,277,266,266,264,264,276,276,278,278}, //EMECC_12 
          {291,291,289,289,303,303,301,301,290,290,288,288,300,300,302,302}, //EMECC_13 
          {315,315,313,313,327,327,325,325,314,314,312,312,324,324,326,326}, //EMECC_14 
          {339,339,337,337,351,351,349,349,338,338,336,336,348,348,350,350}, //EMECC_15 
          {363,363,361,361,375,375,373,373,362,362,360,360,372,372,374,374}, //EMECC_16 
          {199,199,197,197,211,211,209,209,198,198,196,196,208,208,210,210}, //EMBC-EMECC_9 
          {223,223,221,221,235,235,233,233,222,222,220,220,232,232,234,234}, //EMBC-EMECC_10 
          {247,247,245,245,259,259,257,257,246,246,244,244,256,256,258,258}, //EMBC-EMECC_11 
          {271,271,269,269,283,283,281,281,270,270,268,268,280,280,282,282}, //EMBC-EMECC_12 
          {295,295,293,293,307,307,305,305,294,294,292,292,304,304,306,306}, //EMBC-EMECC_13 
          {319,319,317,317,331,331,329,329,318,318,316,316,328,328,330,330}, //EMBC-EMECC_14 
          {343,343,341,341,355,355,353,353,342,342,340,340,352,352,354,354}, //EMBC-EMECC_15 
          {367,367,365,365,379,379,377,377,366,366,364,364,376,376,378,378}, //EMBC-EMECC_16 
          {203,203,201,201,215,215,213,213,202,202,200,200,212,212,214,214}, //EMBC_9 
          {227,227,225,225,239,239,237,237,226,226,224,224,236,236,238,238}, //EMBC_10 
          {251,251,249,249,263,263,261,261,250,250,248,248,260,260,262,262}, //EMBC_11 
          {275,275,273,273,287,287,285,285,274,274,272,272,284,284,286,286}, //EMBC_12 
          {299,299,297,297,311,311,309,309,298,298,296,296,308,308,310,310}, //EMBC_13 
          {323,323,321,321,335,335,333,333,322,322,320,320,332,332,334,334}, //EMBC_14 
          {347,347,345,345,359,359,357,357,346,346,344,344,356,356,358,358}, //EMBC_15 
          {371,371,369,369,383,383,381,381,370,370,368,368,380,380,382,382}, //EMBC_16 
          {  5,  4, 16, 17, 29, 28, 40, 41, 53, 52, 64, 65, 77, 76, 88, 89}, //TREXC_4_1 
          {  7,  6, 18, 19, 31, 30, 42, 43, 55, 54, 66, 67, 79, 78, 90, 91}, //TREXC_5_1 
          {  9,  8, 20, 21, 33, 32, 44, 45, 57, 56, 68, 69, 81, 80, 92, 93}, //TREXC_6_1 
          { 11, 10, 22, 23, 35, 34, 46, 47, 59, 58, 70, 71, 83, 82, 94, 95}, //TREXC_7_1 
          {101,100,112,113,125,124,136,137,149,148,160,161,173,172,184,185}, //TREXC_4_2 
          {103,102,114,115,127,126,138,139,151,150,162,163,175,174,186,187}, //TREXC_5_2 
          {105,104,116,117,129,128,140,141,153,152,164,165,177,176,188,189}, //TREXC_6_2 
          {107,106,118,119,131,130,142,143,155,154,166,167,179,178,190,191}, //TREXC_7_2 
          {  0,  0,  3,  2,  1,  0,  0,  1,  1, 15, 14, 13, 12,  1, -1, -1}, //EMECC-HECC_1 
          {  2,  2, 27, 26, 25, 24,  2,  3,  3, 39, 38, 37, 36,  3, -1, -1}, //EMECC-HECC_1 
          {  4,  4, 51, 50, 49, 48,  4,  5,  5, 63, 62, 61, 60,  5, -1, -1}, //EMECC-HECC_2 
          {  6,  6, 75, 74, 73, 72,  6,  7,  7, 87, 86, 85, 84,  7, -1, -1}, //EMECC-HECC_2 
          {  8,  8, 99, 98, 97, 96,  8,  9,  9,111,110,109,108,  9, -1, -1}, //EMECC-HECC_3 
          { 10, 10,123,122,121,120, 10, 11, 11,135,134,133,132, 11, -1, -1}, //EMECC-HECC_3 
          { 12, 12,147,146,145,144, 12, 13, 13,159,158,157,156, 13, -1, -1}, //EMECC-HECC_4 
          { 14, 14,171,170,169,168, 14, 15, 15,183,182,181,180, 15, -1, -1}, //EMECC-HECC_4 
          {197,196,208,209,221,220,232,233,245,244,256,257,269,268,280,281}, //TREXC_4_3 
          {199,198,210,211,223,222,234,235,247,246,258,259,271,270,282,283}, //TREXC_5_3 
          {201,200,212,213,225,224,236,237,249,248,260,261,273,272,284,285}, //TREXC_6_3 
          {203,202,214,215,227,226,238,239,251,250,262,263,275,274,286,287}, //TREXC_7_3 
          {293,292,304,305,317,316,328,329,341,340,352,353,365,364,376,377}, //TREXC_4_4 
          {295,294,306,307,319,318,330,331,343,342,354,355,367,366,378,379}, //TREXC_5_4 
          {297,296,308,309,321,320,332,333,345,344,356,357,369,368,380,381}, //TREXC_6_4 
          {299,298,310,311,323,322,334,335,347,346,358,359,371,370,382,383}, //TREXC_7_4 
          { 16, 16,195,194,193,192, 16, 17, 17,207,206,205,204, 17, -1, -1}, //EMECC-HECC_5 
          { 18, 18,219,218,217,216, 18, 19, 19,231,230,229,228, 19, -1, -1}, //EMECC-HECC_5 
          { 20, 20,243,242,241,240, 20, 21, 21,255,254,253,252, 21, -1, -1}, //EMECC-HECC_6 
          { 22, 22,267,266,265,264, 22, 23, 23,279,278,277,276, 23, -1, -1}, //EMECC-HECC_6 
          { 24, 24,291,290,289,288, 24, 25, 25,303,302,301,300, 25, -1, -1}, //EMECC-HECC_7 
          { 26, 26,315,314,313,312, 26, 27, 27,327,326,325,324, 27, -1, -1}, //EMECC-HECC_7 
          { 28, 28,339,338,337,336, 28, 29, 29,351,350,349,348, 29, -1, -1}, //EMECC-HECC_8 
          { 30, 30,363,362,361,360, 30, 31, 31,375,374,373,372, 31, -1,-1} // EMECC-HECC_8 
        }};

        constexpr std::array<std::array<int, 20>, 4> AMPD_DSTRT_ARR =
        {{          
          {11,15,27,31,75,79,91,95,107,111,139,143,155,159,171,175,199,207,214,223}, //f0 - f47  EMEC,EMB
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223}, 
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223},  // f56 - f63, f72 - f79 EMECHEC
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223}  // unsued    
        }};

        constexpr std::array<std::array<char, 20>, 4> AMPD_DTYP_ARR = 
        {{
  
          {0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,
           0b1111,0b1000,0b1010,0b0101}, //f0      EMEC,EMB
   
          {0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,
           0b1111, 0b1111,0b1010,0b0101}, //f48/f64 TREX
   
          {0b0010, 0b0110, 0b1011, 0b1011, 0b1011, 0b1011, 0b0011 ,  
           0b0010, 0b0110, 0b1011, 0b1011, 0b1011, 0b1011, 0b0011 , 
           0b1111, 0b1111, 
           0b1111, 0b1000, 0b1010,0b0101} //f56 EMEC/HEC
 
        }};


        constexpr std::array<int, 100> AMSK =
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};


        constexpr std::array<int, 100> BMPD_NFI = 
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,
         1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2};


        constexpr std::array<int, 100> BCALO_TYPE = 
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,
         1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2};


        constexpr std::array<std::array<int, 16>, 100> BMPD_GTRN_ARR =
        {{
          {  8,  8, 10, 10, 20, 20, 22, 22,  9,  9, 11, 11, 23, 23, 21, 21},  // EMECA_1 
          { 32, 32, 34, 34, 44, 44, 46, 46, 33, 33, 35, 35, 47, 47, 45, 45},  //EMECA_2 
          { 56, 56, 58, 58, 68, 68, 70, 70, 57, 57, 59, 59, 71, 71, 69, 69},  //EMECA_3 
          { 80, 80, 82, 82, 92, 92, 94, 94, 81, 81, 83, 83, 95, 95, 93, 93},  //EMECA_4 
          {104,104,106,106,116,116,118,118,105,105,107,107,119,119,117,117},  //EMECA_5 
          {128,128,130,130,140,140,142,142,129,129,131,131,143,143,141,141},  //EMECA_6 
          {152,152,154,154,164,164,166,166,153,153,155,155,167,167,165,165},  //EMECA_7 
          {176,176,178,178,188,188,190,190,177,177,179,179,191,191,189,189},  //EMECA_8 
          {  4,  4,  6,  6, 16, 16, 18, 18,  5,  5,  7,  7, 19, 19, 17, 17},  //EMBA-EMECA_1 
          { 28, 28, 30, 30, 40, 40, 42, 42, 29, 29, 31, 31, 43, 43, 41, 41},  //EMBA-EMECA_2 
          { 52, 52, 54, 54, 64, 64, 66, 66, 53, 53, 55, 55, 67, 67, 65, 65},  //EMBA-EMECA_3 
          { 76, 76, 78, 78, 88, 88, 90, 90, 77, 77, 79, 79, 91, 91, 89, 89},  //EMBA-EMECA_4 
          {100,100,102,102,112,112,114,114,101,101,103,103,115,115,113,113},  //EMBA-EMECA_5 
          {124,124,126,126,136,136,138,138,125,125,127,127,139,139,137,137},  //EMBA-EMECA_6 
          {148,148,150,150,160,160,162,162,149,149,151,151,163,163,161,161},  //EMBA-EMECA_7 
          {172,172,174,174,184,184,186,186,173,173,175,175,187,187,185,185},  //EMBA-EMECA_8 
          {  0,  0,  2,  2, 12, 12, 14, 14,  1,  1,  3,  3, 15, 15, 13, 13},  //EMBA_1 
          { 24, 24, 26, 26, 36, 36, 38, 38, 25, 25, 27, 27, 39, 39, 37, 37},  //EMBA_2 
          { 48, 48, 50, 50, 60, 60, 62, 62, 49, 49, 51, 51, 63, 63, 61, 61},  //EMBA_3 
          { 72, 72, 74, 74, 84, 84, 86, 86, 73, 73, 75, 75, 87, 87, 85, 85},  //EMBA_4 
          { 96, 96, 98, 98,108,108,110,110, 97, 97, 99, 99,111,111,109,109},  //EMBA_5 
          {120,120,122,122,132,132,134,134,121,121,123,123,135,135,133,133},  //EMBA_6 
          {144,144,146,146,156,156,158,158,145,145,147,147,159,159,157,157},  //EMBA_7 
          {168,168,170,170,180,180,182,182,169,169,171,171,183,183,181,181},  //EMBA_8 
          {200,200,202,202,212,212,214,214,201,201,203,203,215,215,213,213},  //EMECA_9 
          {224,224,226,226,236,236,238,238,225,225,227,227,239,239,237,237},  //EMECA_10 
          {248,248,250,250,260,260,262,262,249,249,251,251,263,263,261,261},  //EMECA_11 
          {272,272,274,274,284,284,286,286,273,273,275,275,287,287,285,285},  //EMECA_12 
          {296,296,298,298,308,308,310,310,297,297,299,299,311,311,309,309},  //EMECA_13 
          {320,320,322,322,332,332,334,334,321,321,323,323,335,335,333,333},  //EMECA_14 
          {344,344,346,346,356,356,358,358,345,345,347,347,359,359,357,357},  //EMECA_15 
          {368,368,370,370,380,380,382,382,369,369,371,371,383,383,381,381},  //EMECA_16 
          {196,196,198,198,208,208,210,210,197,197,199,199,211,211,209,209},  //EMBA-EMECA_9 
          {220,220,222,222,232,232,234,234,221,221,223,223,235,235,233,233},  //EMBA-EMECA_10 
          {244,244,246,246,256,256,258,258,245,245,247,247,259,259,257,257},  //EMBA-EMECA_11 
          {268,268,270,270,280,280,282,282,269,269,271,271,283,283,281,281},  //EMBA-EMECA_12 
          {292,292,294,294,304,304,306,306,293,293,295,295,307,307,305,305},  //EMBA-EMECA_13 
          {316,316,318,318,328,328,330,330,317,317,319,319,331,331,329,329},  //EMBA-EMECA_14 
          {340,340,342,342,352,352,354,354,341,341,343,343,355,355,353,353},  //EMBA-EMECA_15 
          {364,364,366,366,376,376,378,378,365,365,367,367,379,379,377,377},  //EMBA-EMECA_16 
          {192,192,194,194,204,204,206,206,193,193,195,195,207,207,205,205},  //EMBA_9 
          {216,216,218,218,228,228,230,230,217,217,219,219,231,231,229,229},  //EMBA_10 
          {240,240,242,242,252,252,254,254,241,241,243,243,255,255,253,253},  //EMBA_11 
          {264,264,266,266,276,276,278,278,265,265,267,267,279,279,277,277},  //EMBA_12 
          {288,288,290,290,300,300,302,302,289,289,291,291,303,303,301,301},  //EMBA_13 
          {312,312,314,314,324,324,326,326,313,313,315,315,327,327,325,325},  //EMBA_14 
          {336,336,338,338,348,348,350,350,337,337,339,339,351,351,349,349},  //EMBA_15 
          {360,360,362,362,372,372,374,374,361,361,363,363,375,375,373,373},  //EMBA_16 
          {  6,  7, 19, 18, 30, 31, 43, 42, 54, 55, 67, 66, 78, 79, 91, 90},  //TREXA_B_1 
          {  4,  5, 17, 16, 28, 29, 41, 40, 52, 53, 65, 64, 76, 77, 89, 88},  //TREXA_A_1 
          {  2,  3, 15, 14, 26, 27, 39, 38, 50, 51, 63, 62, 74, 75, 87, 86},  //TREXA_9_1 
          {  0,  1, 13, 12, 24, 25, 37, 36, 48, 49, 61, 60, 72, 73, 85, 84},  //TREXA_8_1 
          {102,103,115,114,126,127,139,138,150,151,163,162,174,175,187,186},  //TREXA_B_2 
          {100,101,113,112,124,125,137,136,148,149,161,160,172,173,185,184},  //TREXA_A_2 
          { 98, 99,111,110,122,123,135,134,146,147,159,158,170,171,183,182},  //TREXA_9_2 
          { 96, 97,109,108,120,121,133,132,144,145,157,156,168,169,181,180},  //TREXA_8_2 
          {  0,  0,  8,  9, 10, 11,  0,  1,  1, 20, 21, 22, 23,  1, -1, -1},  //EMECA-HECA_1 
          {  2,  2, 32, 33, 34, 35,  2,  3,  3, 44, 45, 46, 47,  3, -1, -1},  //EMECA-HECA_1 
          {  4,  4, 56, 57, 58, 59,  4,  5,  5, 68, 69, 70, 71,  5, -1, -1},  //EMECA-HECA_2 
          {  6,  6, 80, 81, 82, 83,  6,  7,  7, 92, 93, 94, 95,  7, -1, -1},  //EMECA-HECA_2 
          {  8,  8,104,105,106,107,  8,  9,  9,116,117,118,119,  9, -1, -1},  //EMECA-HECA_3 
          { 10, 10,128,129,130,131, 10, 11, 11,140,141,142,143, 11, -1, -1},  //EMECA-HECA_3 
          { 12, 12,152,153,154,155, 12, 13, 13,164,165,166,167, 13, -1, -1},  //EMECA-HECA_4 
          { 14, 14,176,177,178,179, 14, 15, 15,188,189,190,191, 15, -1, -1},  //EMECA-HECA_4 
          {198,199,211,210,222,223,235,234,246,247,259,258,270,271,283,282},  //TREXA_B_3 
          {196,197,209,208,220,221,233,232,244,245,257,256,268,269,281,280},  //TREXA_A_3 
          {194,195,207,206,218,219,231,230,242,243,255,254,266,267,279,278},  //TREXA_9_3 
          {192,193,205,204,216,217,229,228,240,241,253,252,264,265,277,276},  //TREXA_8_3 
          {294,295,307,306,318,319,331,330,342,343,355,354,366,367,379,378},  //TREXA_B_4 
          {292,293,305,304,316,317,329,328,340,341,353,352,364,365,377,376},  //TREXA_A_4 
          {290,291,303,302,314,315,327,326,338,339,351,350,362,363,375,374},  //TREXA_9_4 
          {288,289,301,300,312,313,325,324,336,337,349,348,360,361,373,372},  //TREXA_8_4 
          { 16, 16,200,201,202,203, 16, 17, 17,212,213,214,215, 17, -1, -1},  //EMECA-HECA_5 
          { 18, 18,224,225,226,227, 18, 19, 19,236,237,238,239, 19, -1, -1},  //EMECA-HECA_5 
          { 20, 20,248,249,250,251, 20, 21, 21,260,261,262,263, 21, -1, -1},  //EMECA-HECA_6 
          { 22, 22,272,273,274,275, 22, 23, 23,284,285,286,287, 23, -1, -1},  //EMECA-HECA_6 
          { 24, 24,296,297,298,299, 24, 25, 25,308,309,310,311, 25, -1, -1},  //EMECA-HECA_7 
          { 26, 26,320,321,322,323, 26, 27, 27,332,333,334,335, 27, -1, -1},  //EMECA-HECA_7 
          { 28, 28,344,345,346,347, 28, 29, 29,356,357,358,359, 29, -1, -1},  //EMECA-HECA_8 
          { 30, 30,368,369,370,371, 30, 31, 31,380,381,382,383, 31, -1, -1} // EMECA-HECA_8 
    
        }};

        constexpr std::array<std::array<int, 20>, 4> BMPD_DSTRT_ARR= 
        {{
          {11,15,27,31,75,79,91,95,107,111,139,143,155,159,171,175,199,207,214,223}, //f0 - f47  EMEC,EMB
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223}, 
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223},  // f56 - f63, f72 - f79 EMECHEC
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223}  // unsued
    
        }};

        constexpr std::array<std::array<char, 20>, 4> BMPD_DTYP_ARR =
        {{
          {0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,0b0000,0b0100,
           0b1001,0b1000,0b1010,0b0101}, //f0  EMEC,EMB
          {0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,0b0001,
           0b1001,0b1111,0b1010,0b0101}, //f48 TREX
          {0b0010,0b0010,0b0011,0b1011,0b1011,0b1011,0b1011,0b0110,0b0011,0b1011,0b1011,0b1011,0b1011,0b0110,0b1111,0b1111,
           0b1111,0b1001,0b1000,0b0101}, //f56 EMEC/HEC
          
          {0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,
           0b1111,0b1111,0b1111,0b1111} // unused in A&B
          
        }};

        constexpr std::array<int, 100> BMSK =
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; 


        constexpr std::array<int, 100> CMPD_NFI =
        {1,1,2,1,1,0,0,0,0,0,0,0,0,3,3,3,
         3,0,0,0,0,0,0,0,0,1,1,2,1,1,0,0,
         0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,
         0,0,
         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //unsed in C 
         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //unsed in C 
         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1   //unsed in C
         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1   //unsed in C
         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1   //unsed in C
        };

        constexpr std::array<int, 100> CCALO_TYPE=     
        {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
          3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
          3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
          3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
          3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
          3,3,3,3};

        constexpr std::array<std::array<int, 16>, 100> CMPD_GTRN_ARR = 
        {{
          //--EMEC/HEC
          //    --neg eta ("C" side) 16
          {2, 14, 3, 15, 4, 16, 5, 17, 2, 14, 3, 15, 4, 16, 5, 17},
          {26, 38, 27, 39, 28, 40, 29, 41, 26, 38, 27, 39, 28, 40, 29, 41},
          {50, 62, 51, 63, 52, 64, 53, 65, 50, 62, 51, 63, 52, 64, 53, 65},
          {74, 86, 75, 87, 76, 88, 77, 89, 74, 86, 75, 87, 76, 88, 77, 89},
          {98, 110, 99, 111, 100, 112, 101, 113, 98, 110, 99, 111, 100, 112, 101, 113},
          {122, 134, 123, 135, 124, 136, 125, 137, 122, 134, 123, 135, 124, 136, 125, 137},
          {146, 158, 147, 159, 148, 160, 149, 161, 146, 158, 147, 159, 148, 160, 149, 161},
          {170, 182, 171, 183, 172, 184, 173, 185, 170, 182, 171, 183, 172, 184, 173, 185},
          {194, 206, 195, 207, 196, 208, 197, 209, 194, 206, 195, 207, 196, 208, 197, 209},
          {218, 230, 219, 231, 220, 232, 221, 233, 218, 230, 219, 231, 220, 232, 221, 233},
          {242, 254, 243, 255, 244, 256, 245, 257, 242, 254, 243, 255, 244, 256, 245, 257},
          {266, 278, 267, 279, 268, 280, 269, 281, 266, 278, 267, 279, 268, 280, 269, 281},
          {290, 302, 291, 303, 292, 304, 293, 305, 290, 302, 291, 303, 292, 304, 293, 305},
          {314, 326, 315, 327, 316, 328, 317, 329, 314, 326, 315, 327, 316, 328, 317, 329},
          {338, 350, 339, 351, 340, 352, 341, 353, 338, 350, 339, 351, 340, 352, 341, 353},
          {362, 374, 363, 375, 364, 376, 365, 377, 362, 374, 363, 375, 364, 376, 365, 377},
          //      --pos eta {"A" side) 16
          {6, 18, 7, 19, 8, 20, 9, 21, 6, 18, 7, 19, 8, 20, 9, 21},
          {30, 42, 31, 43, 32, 44, 33, 45, 30, 42, 31, 43, 32, 44, 33, 45},
          {54, 66, 55, 67, 56, 68, 57, 69, 54, 66, 55, 67, 56, 68, 57, 69},
          {78, 90, 79, 91, 80, 92, 81, 93, 78, 90, 79, 91, 80, 92, 81, 93},
          {102, 114, 103, 115, 104, 116, 105, 117, 102, 114, 103, 115, 104, 116, 105, 117},
          {126, 138, 127, 139, 128, 140, 129, 141, 126, 138, 127, 139, 128, 140, 129, 141},
          {150, 162, 151, 163, 152, 164, 153, 165, 150, 162, 151, 163, 152, 164, 153, 165},
          {174, 186, 175, 187, 176, 188, 177, 189, 174, 186, 175, 187, 176, 188, 177, 189},
          {198, 210, 199, 211, 200, 212, 201, 213, 198, 210, 199, 211, 200, 212, 201, 213},
          {222, 234, 223, 235, 224, 236, 225, 237, 222, 234, 223, 235, 224, 236, 225, 237},
          {246, 258, 247, 259, 248, 260, 249, 261, 246, 258, 247, 259, 248, 260, 249, 261},
          {270, 282, 271, 283, 272, 284, 273, 285, 270, 282, 271, 283, 272, 284, 273, 285},
          {294, 306, 295, 307, 296, 308, 297, 309, 294, 306, 295, 307, 296, 308, 297, 309},
          {318, 330, 319, 331, 320, 332, 321, 333, 318, 330, 319, 331, 320, 332, 321, 333},
          {342, 354, 343, 355, 344, 356, 345, 357, 342, 354, 343, 355, 344, 356, 345, 357},
          {366, 378, 367, 379, 368, 380, 369, 381, 366, 378, 367, 379, 368, 380, 369, 381},
          //--FCAL1
          //    --neg eta ("C" side) 5
          {12, 36, 60, 84, 1, 25, 49, 73, 13, 37, 61, 85, 0, 0, 0, 0},
          {108, 132, 156, 180, 97, 121, 145, 169, 109, 133, 157, 181, 0, 0, 0, 0},
          {0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240, 264, 288, 312, 336, 360},
          {204, 228, 252, 276, 193, 217, 241, 265, 205, 229, 253, 277, 0, 0, 0, 0},
          {300, 324, 348, 372, 289, 313, 337, 361, 301, 325, 349, 373, 0, 0, 0, 0},
          //--pos eta {"A" side) 5
          {22, 46, 70, 94, 11, 35, 59, 83, 23, 47, 71, 95, 0, 0, 0, 0},
          {118, 142, 166, 190, 107, 131, 155, 179, 119, 143, 167, 191, 0, 0, 0, 0},
          {10, 34, 58, 82, 106, 130, 154, 178, 202, 226, 250, 274, 298, 322, 346, 370},
          {214, 238, 262, 286, 203, 227, 251, 275, 215, 239, 263, 287, 0, 0, 0, 0},
          {310, 334, 358, 382, 299, 323, 347, 371, 311, 335, 359, 383, 0, 0, 0, 0},
          //--FCAL2
          //--neg eta {"C" side) 4
          {0, 24, 48, 72, 12, 36, 60, 84, 1, 25, 49, 73, 13, 37, 61, 85},
          {96, 120, 144, 168, 108, 132, 156, 180, 97, 121, 145, 169, 109, 133, 157, 181},
          {192, 216, 240, 264, 204, 228, 252, 276, 193, 217, 241, 265, 205, 229, 253, 277},
          {288, 312, 336, 360, 300, 324, 348, 372, 289, 313, 337, 361, 301, 325, 349, 373},
          //--pos eta {"A" side) 4
          {10, 34, 58, 82, 22, 46, 70, 94, 11, 35, 59, 83, 23, 47, 71, 95},
          {106, 130, 154, 178, 118, 142, 166, 190, 107, 131, 155, 179, 119, 143, 167, 191},
          {202, 226, 250, 274, 214, 238, 262, 286, 203, 227, 251, 275, 215, 239, 263, 287},
          {298, 322, 346, 370, 310, 334, 358, 382, 299, 323, 347, 371, 311, 335, 359, 383},
          // total to here 32 + 10 + 8 = 50 
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
  
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
  
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // unused
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1} // unused 
        }};

        constexpr std::array<std::array<int, 20>, 4> CMPD_DSTRT_ARR = 
        {{
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223},
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223},
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223},
          {11,23,35,47,59,71,83,95,107,119,131,143,155,167,179,191,199,207,214,223},
        }};

        constexpr std::array<std::array<char, 20>, 4> CMPD_DTYP_ARR = 
        {{
          {0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,
           0b1001,0b1000,0b1010,0b0101}, // EMECHEC
          {0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b1111,0b1111,0b1111,0b1111,
           0b1001,0b1000,0b1010,0b0101}, // FCAL1
          {0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,0b0010,
           0b1001,0b1000,0b1010,0b0101}, // FCAL1 f1
          {0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,0b0011,
           0b1001,0b1000,0b1010,0b0101}  // FCAL2
        }};


        constexpr std::array<int, 100> CMSK =
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1
        }; 


} // namespace LVL1::gFEXPos

#endif // L1CALOFEXBYTESTREAM_GFEXPOS_H
