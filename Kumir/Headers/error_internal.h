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
/*
The text below in russian at UTF-8 encoding.

Список внутренних ошибок КУМИР.

Номера внутренних ошибок - шестизначные и имеют вид 99aabb,

где aa обозночает cpp-файл:
 01 - draw_isp.cpp
 02 - event.cpp
 03 - int_proga.cpp
 04 - internal_func.cpp
 05 - main.cpp
 06 - robot_isp.cpp
 07 - run.cpp
 08 - synax.cpp
 09 - text_analiz.cpp
 10 - tools.cpp
 
bb обозначает порядковый номер ошибки.

Имена констант для ошибок имеют вид INT_*_&&,
где *:
 DI - для draw_isp.cpp
  E - для event.cpp
 IP - для int_proga.cpp
 IF - для internal_func.cpp
  M - для main.cpp
 RI - robot_isp.cpp
  R - run.cpp
  S - synax.cpp
 TA - text_analiz.cpp
  T - tools.cpp

&& - двузначный номер ошибки, совпадающий с bb

*/


// for int_proga.cpp

#define INT_IP_01 990301
#define INT_IP_02 990302

// for text_analiz.cpp
#define INT_TA_01 990901
#define INT_TA_02 990902
#define INT_TA_03 990903
#define INT_TA_04 990904
#define INT_TA_05 990905
#define INT_TA_06 990906
#define INT_TA_07 990907
#define INT_TA_08 990908
#define INT_TA_09 990909
#define INT_TA_10 990910
#define INT_TA_11 990911
#define INT_TA_12 990912
#define INT_TA_13 990913
#define INT_TA_14 990914

#define INT_R_01 990701
#define INT_R_02 990702
#define INT_R_03 990703
#define INT_R_04 990704
#define INT_R_05 990705
#define INT_R_06 990706
#define INT_R_07 990707
#define INT_R_08 990708
#define INT_R_09 990709
#define INT_R_10 990710

