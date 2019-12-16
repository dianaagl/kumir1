/****************************************************************************
**
** Copyright (C) 2004-2008 NIISI RAS. All rights reserved.
**
** This file is part of the KuMir.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef __ERROR_H // NEW V.Y. 01.09
#define __ERROR_H

#include "error_internal.h"

#define ISP_READ_ERROR  1215

// ================== for run.cpp

// error codes for internal errors value started from 8001

#define RUN_DIV_ZERO	8001
#define RUN_NO_VALUE	8002
#define RUN_NO_ALG	8003
#define RUN_MAX_COUNT	8004
#define RUN_MANY_IMPL	8006
#define RUN_SYN_ERR	8007
#define RUN_BAD_DEFINITION	8008
#define RUN_BAD_COUNT	8009
#define RUN_TYPE_MISS_1	8010
#define RUN_NO_IMPL	8011
#define RUN_BAD_NAME	8012
#define RUN_WRONG_USE	8013
#define RUN_NO_EXPR	8014
#define RUN_TYPE_MISS_2	8015
#define RUN_BAD_OPER	8016
#define RUN_BAD_INDEX	8017
#define RUN_TOO_COMPLEX	8018
#define RUN_ARRAY_BOUND	8019
#define RUN_FALSE_UTV	8020
#define RUN_NEG_INDEX	8021
#define RUN_NEG_INDEX_2	8022
#define RUN_NEG_INDEX_3	8023
#define RUN_ARRAY_BOUND_2	8024
#define RUN_ARRAY_BOUND_3	8025
#define RUN_MANY_CALLS	8026
#define RUN_CONST_INPUT	8027
#define RUN_INPUT_INT	8028
#define RUN_INPUT_RES	8029
#define RUN_INPUT_FLOAT	8030
#define RUN_INPUT_BOOL	8031
#define RUN_INPUT_CHAR	8032
#define RUN_END_BEFORE_START	8033
#define RUN_END_BEFORE_START_2	8034
#define RUN_END_BEFORE_START_3	8035
#define RUN_BAD_BOUND_DEF	8036
#define RUN_STRING_BOUND_1 8037
#define RUN_STRING_BOUND_2 8038

#define RUN_CHARECT_EMPTY 8039
#define RUN_CHARECT_IS_STRING 8040

#define RUN_STRING_NULL_INDEX 8041
#define RUN_STRING_BAD_SLICE 8042


#define RUN_DIM_MISMATCH 8043
#define CALC_OVERFLOW 8044
#define RUN_INTPOWER_LESS_ZERO 8045
#define RUN_FRACTIONAL_POWER 8046

#define RUN_NO_MASS_VALUE 8047

#define CALC_FLOAT_OVERFLOW 8048
#define DRAW_BAD_COLOR 8049
#define RUN_BAD_DANO 8050
#define RUN_BAD_NADO 8051

#define RUN_ROOT_OF_NEGATIVE 8052
#define RUN_BAG_INVERSE_TRIGONOMETRIC_ARGUMENT 8053
#define RUN_BAG_TYPE_FROM_ISP 8053
//ROBOT 8060-8070
#define ROBOT_STENA 8061
#define ROBOT_IS_EDITOR 8062

#define RUN_SYMBOL_NOT_ALLOWED 8063





// ================== for internal_func.cpp

#define INTFUN_LN_ZERO 8101
#define INTFUN_LG_ZERO 8102
#define INTFUN_MOD_BAD_Y 8103
#define INTFUN_DIV_BAD_Y 8104
#define INTFUN_UNICODE_NOT_CHARECT 8105
#define INTFUN_NOT_KOI8_RANGE 8106

#define RUN_CONST_ARRAY_OUT_OF_BOUNDS 8107


// ================== for text_analiz.cpp

#define SYNTAX_ERR       11
#define PARAM_COUNT_ERR  13
#define PARAM_TYPE_ERR   14
#define WRONG_USE        7701		//MR-31
#define SET_PARAM_ERR         99

#define ISP_NOT_EXISTS 900001

// for testOperSkobka

#define TOS_ISP_IN_ALG      1101
#define TOS_ISP_IN_ISP      1102
#define TOS_ALG_IN_ALG      1103
#define TOS_IF_NOT_IN_ALG   1104
#define TOS_VIBOR_NO_IN_ALG 1105
#define TOS_NO_ALG_DEFINED  1106
#define TOS_BEGIN_BAD_SEAT  1107
#define TOS_NO_IF_BELOW     1108
#define TOS_THEN_BAD_SEAT   1109
#define TOS_WRONG_ELSE      1110
#define TOS_ELSE_BAD_SEAT   1111
#define TOS_WRONG_CASE      1112
#define TOS_CASE_BAD_SEAT   1113
#define TOS_WRONG_TERM      1114
#define TOS_TERM_BAD_SEAT   1115
#define TOS_WRONG_REQR      1116
#define TOS_REQR_BAD_SEAT   1117
#define TOS_WRONG_VALD      1118
#define TOS_VALD_BAD_SEAT   1119
#define TOS_END_NO_BEGIN    1120
#define TOS_END_BAD_SEAT    1121
#define TOS_CE_NO_CB        1122
#define TOS_CE_BAD_SEAT     1123
#define TOS_WRONG_FIN       1124
#define TOS_FIN_BAD_SEAT    1125
#define TOS_ISP_NO_BEGIN    1126
#define TOS_ENDISP_BAD_SEAT 1127
#define TOS_OUT_OF_ALG      1128
#define TOS_ALG_NO_IMPL     1129
#define TOS_BEGIN_NO_END    1130
#define TOS_CB_NO_CE        1131
#define TOS_ELSE_NO_ALL     1132
#define TOS_IF_NO_THEN      1133
#define TOS_THEN_NO_ALL     1134
#define TOS_SWITCH_NO_CASE  1135
#define TOS_CASE_NO_ALL     1136
#define TOS_VALD_NO_BEGIN   1137
#define TOS_TERM_NO_BEGIN   1138
#define TOS_REQR_NO_BEGIN   1139
#define TOS_ISP_NO_END      1140
#define TOS_NO_CLOSE        1141
#define TOS_ENDISP_CLOSE_B  1142
#define TOS_ENDISP_CLOSE_CB 1143
#define TOS_ENDISP_CLOSE_A  1144
#define TOS_ENDISP_CLOSE_E  1145
#define TOS_ENDISP_CLOSE_T  1146
#define TOS_ENDISP_CLOSE_C  1147
#define TOS_ENDISP_CLOSE_S  1148
#define TOS_ENDISP_CLOSE_IF 1149
#define TOS_ENDISP_CLOSE_VD 1150
#define TOS_ENDISP_CLOSE_TR 1151
#define TOS_ENDISP_CLOSE_RQ 1152
#define TOS_END_CLOSE_CB    1153
#define TOS_END_CLOSE_A     1154
#define TOS_END_CLOSE_E     1155
#define TOS_END_CLOSE_T     1156
#define TOS_END_CLOSE_C     1157
#define TOS_END_CLOSE_S     1158
#define TOS_END_CLOSE_IF    1159
#define TOS_END_CLOSE_VD    1160
#define TOS_END_CLOSE_TR    1161
#define TOS_END_CLOSE_RQ    1162
#define TOS_END_CLOSE_ISP   1163
#define TOS_CE_CLOSE_B      1164
#define TOS_CE_CLOSE_A      1165
#define TOS_CE_CLOSE_E      1166
#define TOS_CE_CLOSE_T      1167
#define TOS_CE_CLOSE_C      1168
#define TOS_CE_CLOSE_S      1169
#define TOS_CE_CLOSE_IF     1170
#define TOS_CE_CLOSE_VD     1171
#define TOS_CE_CLOSE_TR     1172
#define TOS_CE_CLOSE_RQ     1173
#define TOS_CE_CLOSE_ISP    1174
#define TOS_FIN_CLOSE_B     1175
#define TOS_FIN_CLOSE_CB    1176
#define TOS_FIN_CLOSE_A     1177

#define TOS_FIN_CLOSE_S     1179
#define TOS_FIN_CLOSE_IF    1180
#define TOS_FIN_CLOSE_VD    1181
#define TOS_FIN_CLOSE_TR    1182
#define TOS_FIN_CLOSE_RQ    1183
#define TOS_FIN_CLOSE_ISP   1184
#define TOS_CB_NOT_IN_ALG   1185
#define TOS_CEC_NO_NO_CB    1186
#define TOS_CEC_CLOSE_ISP   1187
#define TOS_CEC_CLOSE_B     1188
#define TOS_CEC_CLOSE_A     1189
#define TOS_CEC_CLOSE_E     1190
#define TOS_CEC_CLOSE_T     1191
#define TOS_CEC_CLOSE_C     1192
#define TOS_CEC_CLOSE_S     1193
#define TOS_CEC_CLOSE_IF    1194
#define TOS_CEC_CLOSE_VD    1195
#define TOS_CEC_CLOSE_TR    1196
#define TOS_CEC_CLOSE_RQ    1197
#define TOS_CEC_BAD_SEAT    1198
#define TOS_DOC_NO_ALG			1199
#define TOS_DOC_BAD_SEAT    11100
#define TOS_EXTRA_MAIN_ISP	11101
#define TOS_VARS_AFTER_ALG	11102
#define TOS_CONSTRUCTOR_EXIT 11103
#define TOS_CONSTRUCTOR_INPUT 11104
#define TOS_NADO_BEZ_DANO	11105

#define TOS_PROLOG_IF 12001
#define TOS_PROLOG_THEN 12002
#define TOS_PROLOG_FI 12003
#define TOS_PROLOG_ELSE 12004
#define TOS_PROLOG_SWITCH 12005
#define TOS_PROLOG_CASE 12006
#define TOS_PROLOG_ESAC 12007
#define TOS_PROLOG_EXIT 12008
#define TOS_PROLOG_FOR 12009
#define TOS_PROLOG_ROF 12010

#define TOS_AFTER_ISP 12011

#define TOS_NO_END_AFTER_IF_THEN 12012
#define TOS_LINES_BETWEEN_ALG_AND_BEGIN 12013
#define TOS_DOUBLE_NACH 12014
#define TOS_NADO_OUT_OF_ALG 12015
#define TOS_DANO_OUT_OF_ALG 12016
#define TOS_NACH_OUT_OF_ALG 12017

#define TOS_NO_CLOSE_NC 12018
#define TOS_NO_CLOSE_TO 12019
#define TOS_NO_CLOSE_PRI 12020
#define TOS_NO_CLOSE_VIBOR 12021
#define TOS_NO_CLOSE_ELSE 12022
#define TOS_NO_CLOSE_IF 12023

#define TOS_LINE_AFTER_IF 12024
#define TOS_LINE_AFTER_ALG 12025
#define TOS_LINE_AFTER_SWITCH 12026





// for parce_sip
#define ISP_NO_NAME         1211
#define ENDISP_THRASH       1212
#define ISP_USE_NOFILE       1214
#define ISP_INTERNAL_ERROR   1216
#define ISP_USE_WRONG_POS    1217
#define ISP_RELOAD           1218
#define MANY_EXTERNAL_ISPS   1219
#define ISP_NAME_CONFLICT    1220

// for analiz

#define ANALIZ_MANY_PRISV 1202
#define NOT_IN_ALG 1203


// for parce_symbol

#define SYMB_MANY_SEMICOLON 1301
#define SYMB_ALREADY_DECLARED 1302
#define SYMB_NO_VAR 1303
#define SYMB_MASS_INSTEAD_SIMPLE 1304
#define SYMB_HAS_EXT_ALG 1305
#define SYMB_HAS_INT_ALG 1306
#define SYMB_DECLARED_ISP 1307
#define SYMB_NO_NAME 1308
#define SYMB_EXTRA_COMMA 1309
#define SYMB_DECL_IN_LOOP 1310


// for parce_func
#define FUNC_WRONG_SKOB_NUMB 2201
#define FUNC_WRONG_SKOB_POS 2202
#define FUNC_TRASH 2203
#define FUNC_KVAD_OTKR 2204
#define FUNC_KVAD_ZAKR 2205
#define FUNC_NET_IMENI 2206
#define FUNC_BAD_MASS 2207
#define FUNC_NET_IMENI_2 2208
#define FUNC_ALG_EXIST 2209
#define FUNC_BAD_NAME 2210
#define FUNC_NOARG 2211
#define FUNC_NO_TYPE_PAR 2212
#define FUNC_MASS_SKOB 2215
#define FUNC_NET_GRANITS 2217
#define FUNC_BIG_RAZM 2218
#define FUNC_NO_DVOET 2219
#define FUNC_BAD_TYPE 2220
#define FUNC_BAD_TYPE_2 2221
#define FUNC_ARG_EXISTS 2222
#define FUNC_NAME_USED 2223
#define FUNC_NAME_USED_2 2224
#define FUNC_EXTRA_ARGS 2225
#define FUNC_MUST_NONE 2226
#define FUNC_INNER_SK 2227
#define FUNC_DUPLICATE_ARG_NAME 2229
#define FUNC_INNER_QSK 2230
#define FUNC_NO_RIGHT_BOUND 2231
#define FUNC_BOUND_NOT_INT 2233
#define FUNC_EXTRA_BOUND 2234
#define FUNC_NO_OPEN_QSK 2235
#define FUNC_MASS_ALREADY_DECLARED 22351
#define FUNC_NO_TYPE_SPECIFIED 2236
#define FUNC_MASS_NO_BOUNDS 2237
#define FUNC_MASS_FOR_SIMPLE 2238
#define FUNC_EXTRA_ARGS_FIRST 2239
#define FUNC_NO_CLOSE_BRACKET 2240
#define FUNC_NO_TYPE 2241
#define FUNC_RETURN_MASS 2243
#define FUNC_STARTS_WITH_FOR 2244
#define FUNC_STARTS_WITH_WHILE 2245
#define FUNC_ALG_INSTEAD_ARG 2246
#define FUNC_EQUAL_SIGN_IN_NAME 2247
#define FUNC_NO_COMMA_BETWEEN_GROUPS 2248
#define FUNC_ARRAY_PARAM_IN_FIRST_ALG 2249

// for testOperSkobka

#define BAD_OPEN_KVAD_SKOBKA 2102
#define PAR_SYM_SQ_OTKR 2103
#define BAD_KVAD_PAIR 2106
#define SEMICOLON 2109
#define TRASH_END 2107
#define LARGE_RAZM 2108
#define NO_KVAD_CLOSE 2105
#define NO_KVAD_OPEN 2104
#define BAD_LEFT_BOUND 2110
#define BAD_RIGHT_BOUND 2111
#define BAD_KAVICHKA 2112

#define FATAL_OUT_OF_MEM 2101
#define UNKN_ALG 1

// for parce_prisv

#define PRISV_NO_LEFT 2301
#define NO_ZAKR_KVAD_LEFT 2302
#define BAD_KVAD_LEFT 2303
#define TRASH_LEFT 2304
#define BAD_INDEX_SKOB_LEFT 2305
#define NO_IMPL_2306 2306
#define PRISV_CONST 2307
#define PRISV_LEFT_CANT_INDEX 2308
#define PRISV_LEFT_NO_INDEX 2309
#define PRISV_NO_RIGHT 2310
#define TYPE_MISS_2308 2311
#define WRONG_ARG_NUMB_LEFT 2312
#define TYPE_MISS_2313 2313
#define PRISV_END_ZAP 2314
#define PRISV_CONST_1 2315


#define PRISV_MASS_SLICE 2384

#define PRISV_INT_REAL 112385
#define PRISV_INT_CHAR 112386
#define PRISV_INT_STRING 112387
#define PRISV_INT_BOOL 112388
#define PRISV_INT_MASSINT 112389
#define PRISV_INT_MASSREAL 112390
#define PRISV_INT_MASSCHAR 112391
#define PRISV_INT_MASSSTRING 112392
#define PRISV_INT_MASSBOOL 112393

#define PRISV_REAL_CHAR 112394
#define PRISV_REAL_STRING 112395
#define PRISV_REAL_BOOL 112396
#define PRISV_REAL_MASSINT 112397
#define PRISV_REAL_MASSREAL 112398
#define PRISV_REAL_MASSCHAR 112399
#define PRISV_REAL_MASSSTRING 1123100
#define PRISV_REAL_MASSBOOL 1123101

#define PRISV_CHAR_REAL 1123102
#define PRISV_CHAR_STRING 1123103
#define PRISV_CHAR_BOOL 1123104
#define PRISV_CHAR_MASSINT 1123105
#define PRISV_CHAR_MASSREAL 1123106
#define PRISV_CHAR_MASSCHAR 1123107
#define PRISV_CHAR_MASSSTRING 1123108
#define PRISV_CHAR_MASSBOOL 1123109
#define PRISV_CHAR_INT 1123110

#define PRISV_BOOL_REAL 1123111
#define PRISV_BOOL_CHAR 1123112
#define PRISV_BOOL_STRING 1123113
#define PRISV_BOOL_INT 1123114
#define PRISV_BOOL_MASSINT 1123115
#define PRISV_BOOL_MASSREAL 1123116
#define PRISV_BOOL_MASSCHAR 1123117
#define PRISV_BOOL_MASSSTRING 1123118
#define PRISV_BOOL_MASSBOOL 1123119

#define PRISV_STR_REAL 1123120
#define PRISV_STR_CHAR 1123121
#define PRISV_STR_BOOL 1123122
#define PRISV_STR_INT 1123123
#define PRISV_STR_MASSINT 1123124
#define PRISV_STR_MASSREAL 1123125
#define PRISV_STR_MASSCHAR 1123126
#define PRISV_STR_MASSSTRING 1123127
#define PRISV_STR_MASSBOOL 1123128

#define PRISV_EXTRA_RETURN 1123129

#define PRISV_LEFT_FUNC 1123130
#define PRISV_LEFT_IND_NOT_SIMPLE 1123131

#define PRISV_RETURN_NO_FUNCTION 1123132

#define PRISV_MASS_ARG_NOT_INTEGER 1123133

#define PRISV_LEFT_IS_ARG 1123134
#define PRISV_NO_MASS_NAME 1123135


#define PRISV_MASS_ARG_EMPTY 1123136

#define PRISV_MASS 1123137
#define PRISV_COMPLEX 11223138

#define PRISV_NONE 11223140

#define PRISV_LEFT_ARRAY 11223141
#define PRISV_RIGHT_ARRAY 11223142

// for parceVariables

#define PV_NO_TYPE 20101
#define PV_NO_NAME 20102
#define PV_EXTRA_BOUNDS 20103
#define PV_NO_BOUNDS 20104
#define PV_BIG_RAZM 20105
#define PV_RECURSION_L 20106
#define PV_BAD_LEFT 20107
#define PV_BAD_RIGHT 20108
#define PV_RECURSION_R 20109
#define PV_EXTRA_RES 20110
#define PV_EXTRA_ARG 20111
#define PV_EXTRA_ARGRES 20112
#define PV_NO_CLOSED_QB 20113
#define PV_NO_OPEN_QB 20114
#define PV_NO_LEFT_BOUND 20115
#define PV_NO_RIGHT_BOUND 20116
#define PV_BOUND_REDECL 20117
#define PV_COMMA_INSTEAD_SEMICOLON 20118
#define PV_REDECLARATION 20119
#define PV_REDECLARATION_GLOBAL 201220
#define PV_REDECLARATION_ALG 201221
#define PV_LITCONST 201222
#define PV_ASSIGNMENT 201223
#define PV_ZNACH_NAME 201224
#define PV_NO_CLOSE_BRACKET 201225
#define PV_CONSTANT_IN_RES 201226
#define PV_REGULAR_TYPE_MISMATCH 201227
#define PV_CONST_EXTRA_OPEN_BRACKET 201228
#define PV_CONST_EXTRA_CLOSE_BRACKET 201229
#define PV_CONST_ELEM_TYPE_MISMATCH 201230
#define PV_CONST_DIMENSION_MISMATCH 201231
#define PV_CONST_IS_EMPTY 201232
#define PV_NOT_A_CONSTANT 201233
#define PV_CONSTANT_NOT_TYPED 201234


// for parce_cicle

#define CIKL_NO_KEYWORD 2601
#define CIKL_NO_LOOP_VAR 2602
#define CIKL_NO_IMPL 2603
#define CIKL_CONST 2604
#define CIKL_PARAM_TYPE_ERR 2605
#define CIKL_NO_DO 2606
#define CIKL_DO_NE_TAM 2607
#define CIKL_TYPE_MISS_1 2608
#define CIKL_TYPE_MISS_2 2609
#define CIKL_TYPE_MISS_3 2610
#define CIKL_SKOLKO_RAZ 2611
#define CIKL_TRASH_RAZ 2612
#define CIKL_TYPE_MISS_4 2613
#define CIKL_TYPE_MISS_5 2614
#define CIKL_NO_CONDITION 2615
#define CICK_EMPTY_FROM 2616
#define CICK_EMPTY_STEP 2617
#define CICK_EMPTY_TO 2618
#define CICK_NO_FROM 2619

// for func_find

#define VYZ_BAD_SYM_1 2701
#define VYZ_NO_IMPL_1 2702
#define VYZ_NO_PROC_1 2703
#define VYZ_BAD_SYM_2 2704
#define VYZ_NO_IMPL_2 2705
#define VYZ_NO_PROC_2 2706
#define VYZ_LISH_ZAKR 2707
#define VYZ_LISH_OTKR 2708
#define VYZ_BAD_LAST 2709
#define VYZ_BAD_ARG_NUM 2710
#define VYZ_WRONG_USE_1 2711
#define VYZ_TYPE_MISS_1 2712
#define VYZ_NO_ARGS 2713
#define VYZ_MASS_BAD_RAZM 2714
#define VYZ_RES_NOT_SIMPLE 2715
#define VYZ_ARGRES_NOT_SIMPLE 2716
#define VYZ_EMPTY_BRAKCETS 2717
#define VYZ_PARAM_COUNT_MISMATCH 2718
#define VYZ_EMPTY_ARG 2719
#define VYZ_BAD_ALG 2720
#define VYZ_ARG_AS_RES 2721
#define VYZ_NO_ALG_NAME 2722
#define VYZ_ARG_AS_ARGRES 2723
#define VYZ_TRASH 2724

// for parce_vvod

#define VVOD_EMPTY 2401
#define VVOD_BAD_NAME 2402
#define VVOD_NO_IMPL 2403
#define VVOD_BAD_KVAD_ZAKR 2404
#define VVOD_BAD_NAME_2 2405
#define VVOD_NO_IMPL_2 2406
#define VVOD_CANT_INDEX 2407
#define VVOD_NO_MASS 2408
#define VVOD_CONST 2409
#define VVOD_TYPE_MISS_1 2410
#define VVOD_TYPE_MISS_2 2411
#define VVOD_END_ZAP 2412
#define VVOD_MASS 2413
#define VVOD_MANY_SYMBOLS 2414
#define VVOD_NON_STRING_CONST 2415
#define VVOD_EXPRESSION 2416
#define VVOD_NO_SPACE 2417
#define VVOD_ARG	2418
// for parce_vyvod

#define VYVOD_BAD_SKOBKA_1 2501
#define VYVOD_BAD_SKOBKA_2 2502
#define VYVOD_END_ZAP 2503
#define VYVOD_EMPTY 2504
#define VYVOD_MASS 2505
#define VYVOD_NO_SPACE 2506

// for NewGet
#define GNVR_BAD_EXP_LEFT 4101
#define GNVR_BAD_EXP_RIGHT 4102
#define GNVR_BAD_EXP_E2 4103
#define GNVR_BAD_EXP_LEFT_2 4104
#define GNVR_BAD_EXP_RIGHT_2 4105
#define GNVR_NO_ARRAY_NAME 4106
#define GNVR_NOT_END 4107
#define GNVR_MANY_NOT 4108
#define GNVR_BAD_SYM_1 4109
#define GNVR_NO_IMPL 4110
#define GNVR_NOT_LOG 4111
#define GNVR_BAD_SYM_2 4112
#define GNVR_NO_IMPL_2 4113
#define GNVR_NOT_LOG_2 4114
#define GNVR_NO_ARG 4115
#define GNVR_NOT_LOG_3 4116
#define GNVR_BAD_SYM_3 4117
#define GNVR_NO_IMPL_3 4118
#define GNVR_CANT_INDEX 4119
#define GNVR_EXTRA_COMMA 4120
#define GNVR_USE_RES 4121
#define GNVR_BIG_INT_CONST 4122
#define GNVR_BIG_FLOAT_CONST 4123
#define GNVR_MANY_QOUTES_IN_BLOCK 4124
#define GNVR_MISSED_OPERATOR 4125
#define GNVR_PROCEDURE_RETVAL_ACCESS 4126
#define GNVR_USE_BAD_ALG 4127
#define GNVR_ALG_INSTEAD_OF_FUNCTION 4128
#define GNVR_BAD_HEX_CONST 4129
#define GNVR_BRACKET_MISMATCH 4130

// for findIncSkobka

#define FIS_VYREZ_3 4209

#define FIS_1 4210
#define FIS_2 4211
#define FIS_3 4212
#define FIS_4 4213
#define FIS_5 4214
#define FIS_6 4215
#define FIS_7 4216
#define FIS_8 4217
#define FIS_9 4218
#define FIS_10 4219
#define FIS_11 4220
#define FIS_12 4221
#define FIS_13 4222
#define FIS_14 4223
#define FIS_15 4224
#define FIS_16 4225
#define FIS_17 4226
#define FIS_18 4227
#define FIS_19 4228
#define FIS_20 4229
#define FIS_21 4230


// for getSimpleVirajType
#define GSV_NO_IMPL_1 4301
#define GSV_ONLY_OPER 4302
#define GSV_TWO_OPER 4303
#define GSV_ZNAK_V_KONCE 4304
#define GSV_NO_IMPL_2 4305
#define GSV_NOT_UNARY 4306
#define GSV_EXTRA_OPER 4307
#define GSV_ZNAK_V_KONCE_2 4308

// for getObjectType
#define GOT_ARRAY_1 4401
#define GOT_ARRAY_2 4402
#define GOT_ARRAY_3 4403
#define GOT_STRING_1 4404
#define GOT_STRING_2 4405
#define GOT_STRING_3 4406
#define GOT_FUNC_1 4407
#define GOT_TYPE_MISS 4408
#define GOT_BAD_RES 4409
#define GOT_INTERNAL_1 4481
#define GOT_INTERNAL_2 4482
#define GOT_INTERNAL_3 4483
#define GOT_INTERNAL_4 4484
#define GOT_INTERNAL_5 4485
#define GOT_INTERNAL_6 4486
#define GOT_INTERNAL_7 4487
#define GOT_INTERNAL_8 4488
#define GOT_BAD_SKOBK 4489
#define GOT_BAD_DIM 4490
#define GOT_RECURSIVE_TABLE 4491
#define GOT_RES_PARAM_NOT_SIMPLE 4492
#define GOT_ARG_PARAM_AS_RES 4493
#define GOT_ARG_PARAM_AS_ARGRES 4494

// for ResType
#define RESTP_UNARY_INT 4501
#define RESTP_UNARY_REAL 4502
#define RESTP_UNARY_BOOL 4503
#define RESTP_UNARY 4504
#define RESTP_INT 4505
#define RESTP_BOOL 4506
#define RESTP_REAL 4507
#define RESTP_STRING 4508
#define RESTP_CHAR 4509
#define RESTP_FINAL 4510

#define RESTP_INT_INT_OR 4511
#define RESTP_INT_INT_AND 4512
#define RESTP_INT_INT_XOR 4513
#define RESTP_INT_REAL_OR 4514
#define RESTP_INT_REAL_AND 4515
#define RESTP_INT_REAL_XOR 4516

#define RESTP_INT_BOOL_PLUS 4517
#define RESTP_INT_BOOL_MINUS 4518
#define RESTP_INT_BOOL_MUL 4519
#define RESTP_INT_BOOL_DIV 4520
#define RESTP_INT_BOOL_LESS 4521
#define RESTP_INT_BOOL_GREAT 4522
#define RESTP_INT_BOOL_EQ 4523
#define RESTP_INT_BOOL_POW 4524
#define RESTP_INT_BOOL_NEQ 4525
#define RESTP_INT_BOOL_LEQ 4526
#define RESTP_INT_BOOL_GEQ 4527
#define RESTP_INT_BOOL_OR 4528
#define RESTP_INT_BOOL_XOR 4529
#define RESTP_INT_BOOL_AND 4530

#define RESTP_INT_CHAR_PLUS 4531
#define RESTP_INT_CHAR_MINUS 4532
#define RESTP_INT_CHAR_MUL 4533
#define RESTP_INT_CHAR_DIV 4534
#define RESTP_INT_CHAR_LESS 4535
#define RESTP_INT_CHAR_GREAT 4536
#define RESTP_INT_CHAR_EQ 4537
#define RESTP_INT_CHAR_POW 4538
#define RESTP_INT_CHAR_NEQ 4539
#define RESTP_INT_CHAR_LEQ 4540
#define RESTP_INT_CHAR_GEQ 4541
#define RESTP_INT_CHAR_OR 4542
#define RESTP_INT_CHAR_XOR 4543
#define RESTP_INT_CHAR_AND 4544

#define RESTP_INT_STR_PLUS 4545
#define RESTP_INT_STR_MINUS 4546
#define RESTP_INT_STR_MUL 4547
#define RESTP_INT_STR_DIV 4548
#define RESTP_INT_STR_LESS 4549
#define RESTP_INT_STR_GREAT 4550
#define RESTP_INT_STR_EQ 4551
#define RESTP_INT_STR_POW 4552
#define RESTP_INT_STR_NEQ 4553
#define RESTP_INT_STR_LEQ 4554
#define RESTP_INT_STR_GEQ 4555
#define RESTP_INT_STR_OR 4556
#define RESTP_INT_STR_XOR 4557
#define RESTP_INT_STR_AND 4558

#define RESTP_BOOL_BOOL_PLUS 4559
#define RESTP_BOOL_BOOL_MINUS 4560
#define RESTP_BOOL_BOOL_MUL 4561
#define RESTP_BOOL_BOOL_DIV 4562
#define RESTP_BOOL_BOOL_LESS 4563
#define RESTP_BOOL_BOOL_GREAT 4564
#define RESTP_BOOL_BOOL_EQ 4565
#define RESTP_BOOL_BOOL_POW 4566
#define RESTP_BOOL_BOOL_NEQ 4567
#define RESTP_BOOL_BOOL_LEQ 4568
#define RESTP_BOOL_BOOL_GEQ 4569
#define RESTP_BOOL_BOOL_OR 4570
#define RESTP_BOOL_BOOL_XOR 4571
#define RESTP_BOOL_BOOL_AND 4572

#define RESTP_BOOL_INT_PLUS 4573
#define RESTP_BOOL_INT_MINUS 4574
#define RESTP_BOOL_INT_MUL 4575
#define RESTP_BOOL_INT_DIV 4576
#define RESTP_BOOL_INT_LESS 4577
#define RESTP_BOOL_INT_GREAT 4578
#define RESTP_BOOL_INT_EQ 4579
#define RESTP_BOOL_INT_POW 4580
#define RESTP_BOOL_INT_NEQ 4581
#define RESTP_BOOL_INT_LEQ 4582
#define RESTP_BOOL_INT_GEQ 4583
#define RESTP_BOOL_INT_OR 4584
#define RESTP_BOOL_INT_XOR 4585
#define RESTP_BOOL_INT_AND 4586

#define RESTP_BOOL_REAL_PLUS 4587
#define RESTP_BOOL_REAL_MINUS 4588
#define RESTP_BOOL_REAL_MUL 4589
#define RESTP_BOOL_REAL_DIV 45490
#define RESTP_BOOL_REAL_LESS 4591
#define RESTP_BOOL_REAL_GREAT 4592
#define RESTP_BOOL_REAL_EQ 4593
#define RESTP_BOOL_REAL_POW 4594
#define RESTP_BOOL_REAL_NEQ 4595
#define RESTP_BOOL_REAL_LEQ 4596
#define RESTP_BOOL_REAL_GEQ 4597
#define RESTP_BOOL_REAL_OR 4598
#define RESTP_BOOL_REAL_XOR 4599
#define RESTP_BOOL_REAL_AND 45100

#define RESTP_BOOL_CHAR_PLUS 45101
#define RESTP_BOOL_CHAR_MINUS 45102
#define RESTP_BOOL_CHAR_MUL 45103
#define RESTP_BOOL_CHAR_DIV 45104
#define RESTP_BOOL_CHAR_LESS 45105
#define RESTP_BOOL_CHAR_GREAT 45106
#define RESTP_BOOL_CHAR_EQ 45107
#define RESTP_BOOL_CHAR_POW 45108
#define RESTP_BOOL_CHAR_NEQ 45109
#define RESTP_BOOL_CHAR_LEQ 45110
#define RESTP_BOOL_CHAR_GEQ 45111
#define RESTP_BOOL_CHAR_OR 45112
#define RESTP_BOOL_CHAR_XOR 45113
#define RESTP_BOOL_CHAR_AND 45114

#define RESTP_BOOL_STR_PLUS 45115
#define RESTP_BOOL_STR_MINUS 45116
#define RESTP_BOOL_STR_MUL 45117
#define RESTP_BOOL_STR_DIV 45118
#define RESTP_BOOL_STR_LESS 45119
#define RESTP_BOOL_STR_GREAT 45120
#define RESTP_BOOL_STR_EQ 45121
#define RESTP_BOOL_STR_POW 45122
#define RESTP_BOOL_STR_NEQ 45123
#define RESTP_BOOL_STR_LEQ 45124
#define RESTP_BOOL_STR_GEQ 45125
#define RESTP_BOOL_STR_OR 45126
#define RESTP_BOOL_STR_XOR 45127
#define RESTP_BOOL_STR_AND 45128

#define RESTP_REAL_REAL_OR 45129
#define RESTP_REAL_REAL_XOR 45130
#define RESTP_REAL_REAL_AND 45131

#define RESTP_REAL_INT_OR 45132
#define RESTP_REAL_INT_XOR 45133
#define RESTP_REAL_INT_AND 45134

#define RESTP_REAL_STR_PLUS 45135
#define RESTP_REAL_STR_MINUS 45136
#define RESTP_REAL_STR_MUL 45137
#define RESTP_REAL_STR_DIV 45138
#define RESTP_REAL_STR_LESS 45139
#define RESTP_REAL_STR_GREAT 45140
#define RESTP_REAL_STR_EQ 45141
#define RESTP_REAL_STR_POW 45142
#define RESTP_REAL_STR_NEQ 45143
#define RESTP_REAL_STR_LEQ 45144
#define RESTP_REAL_STR_GEQ 45145
#define RESTP_REAL_STR_OR 45146
#define RESTP_REAL_STR_XOR 45147
#define RESTP_REAL_STR_AND 45148

#define RESTP_REAL_CHAR_PLUS 45149
#define RESTP_REAL_CHAR_MINUS 45150
#define RESTP_REAL_CHAR_MUL 45151
#define RESTP_REAL_CHAR_DIV 45152
#define RESTP_REAL_CHAR_LESS 45153
#define RESTP_REAL_CHAR_GREAT 45154
#define RESTP_REAL_CHAR_EQ 45155
#define RESTP_REAL_CHAR_POW 45156
#define RESTP_REAL_CHAR_NEQ 45157
#define RESTP_REAL_CHAR_LEQ 45158
#define RESTP_REAL_CHAR_GEQ 45159
#define RESTP_REAL_CHAR_OR 45160
#define RESTP_REAL_CHAR_XOR 45161
#define RESTP_REAL_CHAR_AND 45162

#define RESTP_REAL_BOOL_PLUS 45163
#define RESTP_REAL_BOOL_MINUS 45164
#define RESTP_REAL_BOOL_MUL 45165
#define RESTP_REAL_BOOL_DIV 45166
#define RESTP_REAL_BOOL_LESS 45167
#define RESTP_REAL_BOOL_GREAT 45168
#define RESTP_REAL_BOOL_EQ 45169
#define RESTP_REAL_BOOL_POW 45170
#define RESTP_REAL_BOOL_NEQ 45171
#define RESTP_REAL_BOOL_LEQ 45172
#define RESTP_REAL_BOOL_GEQ 45173
#define RESTP_REAL_BOOL_OR 45174
#define RESTP_REAL_BOOL_XOR 45175
#define RESTP_REAL_BOOL_AND 45176

#define RESTP_STR_STR_MINUS 45177
#define RESTP_STR_STR_MUL 45178
#define RESTP_STR_STR_DIV 45179
#define RESTP_STR_STR_POW 45180
#define RESTP_STR_STR_OR 45181
#define RESTP_STR_STR_XOR 45182
#define RESTP_STR_STR_AND 45183

#define RESTP_STR_CHAR_MINUS 45184
#define RESTP_STR_CHAR_MUL 45185
#define RESTP_STR_CHAR_DIV 45186
#define RESTP_STR_CHAR_POW 45187
#define RESTP_STR_CHAR_OR 45188
#define RESTP_STR_CHAR_XOR 45189
#define RESTP_STR_CHAR_AND 45190

#define RESTP_STR_BOOL_PLUS 45191
#define RESTP_STR_BOOL_MINUS 45192
#define RESTP_STR_BOOL_MUL 45193
#define RESTP_STR_BOOL_DIV 45194
#define RESTP_STR_BOOL_LESS 45195
#define RESTP_STR_BOOL_GREAT 45196
#define RESTP_STR_BOOL_EQ 45197
#define RESTP_STR_BOOL_POW 45198
#define RESTP_STR_BOOL_NEQ 45199
#define RESTP_STR_BOOL_LEQ 45200
#define RESTP_STR_BOOL_GEQ 45201
#define RESTP_STR_BOOL_OR 45202
#define RESTP_STR_BOOL_XOR 45203
#define RESTP_STR_BOOL_AND 45204

#define RESTP_STR_INT_PLUS 45205
#define RESTP_STR_INT_MINUS 45206
#define RESTP_STR_INT_MUL 45207
#define RESTP_STR_INT_DIV 45208
#define RESTP_STR_INT_LESS 45209
#define RESTP_STR_INT_GREAT 45210
#define RESTP_STR_INT_EQ 45211
#define RESTP_STR_INT_POW 45212
#define RESTP_STR_INT_NEQ 45213
#define RESTP_STR_INT_LEQ 45214
#define RESTP_STR_INT_GEQ 45215
#define RESTP_STR_INT_OR 45216
#define RESTP_STR_INT_XOR 45217
#define RESTP_STR_INT_AND 45218

#define RESTP_STR_REAL_PLUS 45219
#define RESTP_STR_REAL_MINUS 45220
#define RESTP_STR_REAL_MUL 45221
#define RESTP_STR_REAL_DIV 45222
#define RESTP_STR_REAL_LESS 45223
#define RESTP_STR_REAL_GREAT 45224
#define RESTP_STR_REAL_EQ 45225
#define RESTP_STR_REAL_POW 45226
#define RESTP_STR_REAL_NEQ 45227
#define RESTP_STR_REAL_LEQ 45228
#define RESTP_STR_REAL_GEQ 45229
#define RESTP_STR_REAL_OR 45230
#define RESTP_STR_REAL_XOR 45231
#define RESTP_STR_REAL_AND 45232

#define RESTP_CHAR_STR_MINUS 45233
#define RESTP_CHAR_STR_MUL 45234
#define RESTP_CHAR_STR_DIV 45235
#define RESTP_CHAR_STR_POW 45236
#define RESTP_CHAR_STR_OR 45237
#define RESTP_CHAR_STR_XOR 45238
#define RESTP_CHAR_STR_AND 45239

#define RESTP_CHAR_CHAR_MINUS 45240
#define RESTP_CHAR_CHAR_MUL 45241
#define RESTP_CHAR_CHAR_DIV 45242
#define RESTP_CHAR_CHAR_POW 45243
#define RESTP_CHAR_CHAR_OR 45244
#define RESTP_CHAR_CHAR_XOR 45245
#define RESTP_CHAR_CHAR_AND 45246

#define RESTP_CHAR_BOOL_PLUS 45247
#define RESTP_CHAR_BOOL_MINUS 45248
#define RESTP_CHAR_BOOL_MUL 45249
#define RESTP_CHAR_BOOL_DIV 45250
#define RESTP_CHAR_BOOL_LESS 45251
#define RESTP_CHAR_BOOL_GREAT 45252
#define RESTP_CHAR_BOOL_EQ 45253
#define RESTP_CHAR_BOOL_POW 45254
#define RESTP_CHAR_BOOL_NEQ 45255
#define RESTP_CHAR_BOOL_LEQ 45256
#define RESTP_CHAR_BOOL_GEQ 45257
#define RESTP_CHAR_BOOL_OR 45258
#define RESTP_CHAR_BOOL_XOR 45259
#define RESTP_CHAR_BOOL_AND 45260

#define RESTP_CHAR_INT_PLUS 45261
#define RESTP_CHAR_INT_MINUS 45262
#define RESTP_CHAR_INT_MUL 45263
#define RESTP_CHAR_INT_DIV 45264
#define RESTP_CHAR_INT_LESS 45265
#define RESTP_CHAR_INT_GREAT 45266
#define RESTP_CHAR_INT_EQ 45267
#define RESTP_CHAR_INT_POW 45268
#define RESTP_CHAR_INT_NEQ 45269
#define RESTP_CHAR_INT_LEQ 45270
#define RESTP_CHAR_INT_GEQ 45271
#define RESTP_CHAR_INT_OR 45272
#define RESTP_CHAR_INT_XOR 45273
#define RESTP_CHAR_INT_AND 45274

#define RESTP_CHAR_REAL_PLUS 45275
#define RESTP_CHAR_REAL_MINUS 45276
#define RESTP_CHAR_REAL_MUL 45277
#define RESTP_CHAR_REAL_DIV 45278
#define RESTP_CHAR_REAL_LESS 45279
#define RESTP_CHAR_REAL_GREAT 45280
#define RESTP_CHAR_REAL_EQ 45281
#define RESTP_CHAR_REAL_POW 45282
#define RESTP_CHAR_REAL_NEQ 45283
#define RESTP_CHAR_REAL_LEQ 45284
#define RESTP_CHAR_REAL_GEQ 45285
#define RESTP_CHAR_REAL_OR 45286
#define RESTP_CHAR_REAL_XOR 45287
#define RESTP_CHAR_REAL_AND 45288

#define RESTP_INT_BN_AND 45289
#define RESTP_INT_BN_OR  45290
#define RESTP_INT_BN_XOR 45291

#define RESTP_INT_BL_MORE 45292
#define RESTP_INT_BL_LESS 45293
#define RESTP_INT_BL_EQ 45294
#define RESTP_INT_BL_NEQ 45295
#define RESTP_INT_BL_GEQ 45296
#define RESTP_INT_BL_LEQ 45297
#define RESTP_INT_BL_AND 45298
#define RESTP_INT_BL_OR  45299
#define RESTP_INT_BL_XOR 45300

#define RESTP_REAL_BN_AND 45301
#define RESTP_REAL_BN_OR  45302
#define RESTP_REAL_BN_XOR 45303

#define RESTP_REAL_BL_MORE 45304
#define RESTP_REAL_BL_LESS 45305
#define RESTP_REAL_BL_EQ 45306
#define RESTP_REAL_BL_NEQ 45307
#define RESTP_REAL_BL_GEQ 45308
#define RESTP_REAL_BL_LEQ 45309
#define RESTP_REAL_BL_AND 45310
#define RESTP_REAL_BL_OR  45311
#define RESTP_REAL_BL_XOR 45312

#define RESTP_BN_REAL_AND 45313
#define RESTP_BN_REAL_XOR 45314
#define RESTP_BN_REAL_OR 45315

#define RESTP_BN_INT_AND 45316
#define RESTP_BN_INT_XOR 45317
#define RESTP_BN_INT_OR 45318

#define RESTP_BN_STR_PLUS 45319
#define RESTP_BN_STR_MINUS 45320
#define RESTP_BN_STR_MUL 45321
#define RESTP_BN_STR_DIV 45322
#define RESTP_BN_STR_LESS 45323
#define RESTP_BN_STR_GREAT 45324
#define RESTP_BN_STR_EQ 45325
#define RESTP_BN_STR_POW 45326
#define RESTP_BN_STR_NEQ 45327
#define RESTP_BN_STR_LEQ 45328
#define RESTP_BN_STR_GEQ 45329
#define RESTP_BN_STR_OR 45330
#define RESTP_BN_STR_XOR 45331
#define RESTP_BN_STR_AND 45332

#define RESTP_BN_CHAR_PLUS 45333
#define RESTP_BN_CHAR_MINUS 45334
#define RESTP_BN_CHAR_MUL 45335
#define RESTP_BN_CHAR_DIV 45336
#define RESTP_BN_CHAR_LESS 45337
#define RESTP_BN_CHAR_GREAT 45338
#define RESTP_BN_CHAR_EQ 45339
#define RESTP_BN_CHAR_POW 45340
#define RESTP_BN_CHAR_NEQ 45341
#define RESTP_BN_CHAR_LEQ 45342
#define RESTP_BN_CHAR_GEQ 45343
#define RESTP_BN_CHAR_OR 45344
#define RESTP_BN_CHAR_XOR 45345
#define RESTP_BN_CHAR_AND 45346

#define RESTP_BN_BOOL_LESS 45347
#define RESTP_BN_BOOL_GREAT 45348
#define RESTP_BN_BOOL_EQ 45349
#define RESTP_BN_BOOL_NEQ 45350
#define RESTP_BN_BOOL_LEQ 45351
#define RESTP_BN_BOOL_GEQ 45352

#define RESTP_BN_BL_LESS 45353
#define RESTP_BN_BL_MORE 45354
#define RESTP_BN_BL_EQ 45355
#define RESTP_BN_BL_NEQ 45356
#define RESTP_BN_BL_GEQ 45357
#define RESTP_BN_BL_LEQ 45358

#define RESTP_BN_CHAR_MORE 45359
#define RESTP_BN_STR_MORE 45360
#define RESTP_BN_BOOL_MORE 45361

#define RESTP_BL_BN_LESS 45362
#define RESTP_BL_BN_MORE 45363
#define RESTP_BL_BN_EQ 45364
#define RESTP_BL_BN_NEQ 45365
#define RESTP_BL_BN_LEQ 45367
#define RESTP_BL_BN_GEQ 45368

#define RESTP_BL_BOOL_LESS 45369
#define RESTP_BL_BOOL_GREAT 45370
#define RESTP_BL_BOOL_EQ 45371
#define RESTP_BL_BOOL_NEQ 45372
#define RESTP_BL_BOOL_LEQ 45373
#define RESTP_BL_BOOL_GEQ 45374

#define RESTP_BL_CHAR_OR 45375
#define RESTP_BL_CHAR_XOR 45376
#define RESTP_BL_CHAR_AND 45377
				
#define RESTP_BL_STR_OR 45378
#define RESTP_BL_STR_XOR 45379
#define RESTP_BL_STR_AND 45380
					
#define RESTP_BL_REAL_PLUS 45381
#define RESTP_BL_REAL_MINUS 45382
#define RESTP_BL_REAL_MUL 45383
#define RESTP_BL_REAL_DIV 45384
#define RESTP_BL_REAL_LESS 45385
#define RESTP_BL_REAL_GREAT 45386
#define RESTP_BL_REAL_EQ 45387
#define RESTP_BL_REAL_POW 45388
#define RESTP_BL_REAL_NEQ 45389
#define RESTP_BL_REAL_LEQ 45390
#define RESTP_BL_REAL_GEQ 45391
#define RESTP_BL_REAL_OR 45392
#define RESTP_BL_REAL_XOR 45393
#define RESTP_BL_REAL_AND 45394
				
#define RESTP_BL_INT_PLUS 45395
#define RESTP_BL_INT_MINUS 45396
#define RESTP_BL_INT_MUL 45397
#define RESTP_BL_INT_DIV 45398
#define RESTP_BL_INT_LESS 45399
#define RESTP_BL_INT_GREAT 45400
#define RESTP_BL_INT_EQ 45401
#define RESTP_BL_INT_POW 45402
#define RESTP_BL_INT_NEQ 45403
#define RESTP_BL_INT_LEQ 45404
#define RESTP_BL_INT_GEQ 45405
#define RESTP_BL_INT_OR 45406
#define RESTP_BL_INT_XOR 45407
#define RESTP_BL_INT_AND 45408

#define RESTP_CHAR_BL_MORE 45409
#define RESTP_CHAR_BL_LESS 45410
#define RESTP_CHAR_BL_EQ 45411

#define RESTP_CHAR_BL_NEQ 45412
#define RESTP_CHAR_BL_GEQ 45413
#define RESTP_CHAR_BL_LEQ 45414

#define RESTP_CHAR_BL_OR 45415
#define RESTP_CHAR_BL_XOR 45416
#define RESTP_CHAR_BL_AND 45417

#define RESTP_CHAR_BN_OR 45418
#define RESTP_CHAR_BN_XOR 45419
#define RESTP_CHAR_BN_AND 45420

#define RESTP_STR_BL_MORE 45421
#define RESTP_STR_BL_LESS 45422
#define RESTP_STR_BL_EQ 45423

#define RESTP_STR_BL_NEQ 45424
#define RESTP_STR_BL_GEQ 45425
#define RESTP_STR_BL_LEQ 45426

#define RESTP_STR_BL_OR 45427
#define RESTP_STR_BL_XOR 45428
#define RESTP_STR_BL_AND 45429

#define RESTP_STR_BN_OR 45430
#define RESTP_STR_BN_XOR 45431
#define RESTP_STR_BN_AND 45432

#define RESTP_ASSIGN_INSTEAD_EQ 45433

// for parce_if
#define IF_NO_CONDITION_1 3501
#define IF_NO_CONDITION_2 3502
#define IF_NO_CONDITION_3 3503
#define IF_BAD_CONDITION 3504
#define IF_TYPE_MISS 3505

// for parce_kc_pri

#define KCPRI_NO_CONDITION_1 3401
#define KCPRI_NO_CONDITION_2 3402
#define KCPRI_NO_CONDITION_3 3403
#define KCPRI_TYPE_MISS 3404

// for parce_nado
#define NADO_NOFUNCT 3301
#define NADO_TYPE_MISS 3302

// for parce_dano
#define DANO_NOFUNCT 3201
#define DANO_TYPE_MISS 3202

// for parce_utv
#define UTV_NO_CONDITION_1 3101
#define UTV_NO_CONDITION_2 3102
#define UTV_NO_CONDITION_3 3103
#define UTV_TYPE_MISS 5504

// for TRASH
#define VYBOR_TRASH 5601
#define TO_TRASH 5602
#define INACHE_TRASH 5603
#define VSE_TRASH 5604
#define KON_TRASH 5605
#define KC_TRASH 5606
#define NACH_TRASH 5607
#define EXIT_TRASH 5608
#define NS_TRASH 5609


// for test_name
#define TN_BAD_NAME_1 6101
#define TN_BAD_NAME_2 6102
#define TN_BAD_NAME_3 6103
#define TN_BAD_NAME_4 6104
#define TN_BAD_NAME_1A 6101
#define TN_BAD_NAME_5 6105


// for parce_pri
#define PRI_NO_CONDITION_1	3601
#define PRI_NO_CONDITION_2	3602
#define PRI_NO_CONDITION_3	3603
#define PRI_NO_DVOET	3604
#define PRI_TYPE_MISS	3605

// for parce_isp
#define ISP_EXISTS 3701

// for file input/output

#define RT_ERR_NO_FILE_ISP 7001
#define RT_ERR_FILE_NO_EXISTS 7002
#define RT_ERR_FILE_REOPEN 7003
#define RT_ERR_FILE_NO_ACCESS 7004
#define RT_ERR_FILE_NOT_OPENED 7005
#define RT_ERR_READ_AT_EOF 7006
#define RT_ERR_OPENED_FOR_WRITE_ONLY 7007
#define RT_ERR_OPENED_FOR_READ_ONLY 7008
#define RT_INTERNAL_FILE_ERROR 7009
#define RT_FILE_NO_KEY 7010    
#define F_VVOD_KEY_ERROR 7011  
#define F_VVOD_NO_PEREM 7012
#define F_CANT_RESET_WRITE 7013
#define F_KEY_NOT_INT 7014
#define F_FILE_ALREADY_EXIST 7015
#define F_FILE_CANT_CREATE 7016
#define F_FILE_WRONG_TYPE 7017
#define BAD_ASSIGN 7018
#define TRASH_AFTER_ASSIGN 7019
// for old-style I/O functions

#define IO_FILE_NO_EXISTS 1


#define ISP_INT_ERROR 65535

// include file errors
#define INCLUDE_NO_NAME 67001
#define INCLUDE_EXTRA_LEXEM 67002
#define INCLUDE_INVALID_LEXEM 67003
#define INCLUDE_FILE_NOT_EXISTS 67004
#define INCLUDE_NSLOOKUP_ERROR 67005
#define INCLUDE_CONNECTION_ERRROR 67006
#define INCLUDE_HTTP_401 67007
#define INCLUDE_HTTP_404 67008
#define INCLUDE_FILE_READ_ERROR 67009
#define INCLUDE_CONTAINS_ERRORS 67010
#define INCLUDE_INVALID_URL 67011
#define INCLUDE_HTTP_NOT_SUPPORTED 67012
#define INCLUDE_CONTAINS_NORM_ERRORS 67013

#define ASSIGN_IO_WONG_ARGS_COUNT 68001
#define ASSIGN_IO_WRONG_FIRST_ARGUMENT 68002
#define ASSIGN_IO_WRONG_SECOND_ARGUMENT 68003
#define ASSIGN_NO_ARGUMENTS 68004

#define BAD_COLOR 3000001

#endif
