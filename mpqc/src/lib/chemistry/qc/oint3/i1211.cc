#include <chemistry/qc/oint3/build.h>
int sc::BuildIntV3::i1211(){
/* the cost is 198 */
double t1;
double t2;
double t3;
double t4;
double t5;
double t6;
double t7;
double t8;
double t9;
double t10;
double t11;
double t12;
double t13;
double t14;
double t15;
double t16;
double t17;
double t18;
double t19;
double t20;
double t21;
double t22;
double t23;
double t24;
double t25;
double t26;
double t27;
double t28;
double t29;
double t30;
double t31;
double t32;
double t33;
double t34;
double t35;
double t36;
double t37;
double t38;
t1=int_v_W0-int_v_p120;
double***restrictxx int_v_list0=int_v_list(0);
double**restrictxx int_v_list00=int_v_list0[0];
double*restrictxx int_v_list002=int_v_list00[2];
t2=t1*int_v_list002[0];
t3=int_v_p120-int_v_r10;
double*restrictxx int_v_list001=int_v_list00[1];
t4=t3*int_v_list001[0];
t5=t4+t2;
t2=0.5*int_v_ooze;
t4=t2*t5;
t5=int_v_W0-int_v_p340;
t6=t5*int_v_list002[0];
t7=int_v_p340-int_v_r30;
t8=t7*int_v_list001[0];
t9=t8+t6;
t6=int_v_zeta34*int_v_ooze;
t8=int_v_oo2zeta12*t6;
t6=(-1)*t8;
t8=t6*t9;
t10=t8+t4;
t11=t5*int_v_list001[0];
double*restrictxx int_v_list000=int_v_list00[0];
t12=t7*int_v_list000[0];
t13=t12+t11;
t11=int_v_oo2zeta12*t13;
t12=t11+t10;
t10=t2*int_v_list002[0];
double*restrictxx int_v_list003=int_v_list00[3];
t14=t5*int_v_list003[0];
t5=t7*int_v_list002[0];
t7=t5+t14;
t5=t1*t7;
t14=t5+t10;
t5=t3*t9;
t15=t5+t14;
t5=t1*t15;
t14=t5+t12;
t5=t2*int_v_list001[0];
t12=t1*t9;
t16=t12+t5;
t12=t3*t13;
t17=t12+t16;
double***restrictxx int_v_list1=int_v_list(1);
double**restrictxx int_v_list11=int_v_list1[1];
double*restrictxx int_v_list110=int_v_list11[0];
int_v_list110[8]=t17;
t12=t3*t17;
t16=t12+t14;
double***restrictxx int_v_list2=int_v_list(2);
double**restrictxx int_v_list21=int_v_list2[1];
double*restrictxx int_v_list210=int_v_list21[0];
int_v_list210[17]=t16;
t12=int_v_W2-int_v_p342;
t14=t12*int_v_list002[0];
t18=int_v_p342-int_v_r32;
t19=t18*int_v_list001[0];
t20=t19+t14;
t14=t6*t20;
t19=t12*int_v_list001[0];
t21=t18*int_v_list000[0];
t22=t21+t19;
t19=int_v_oo2zeta12*t22;
t21=t19+t14;
t23=t12*int_v_list003[0];
t12=t18*int_v_list002[0];
t18=t12+t23;
t12=t1*t18;
t23=t3*t20;
t24=t23+t12;
t12=t1*t24;
t23=t12+t21;
t12=t1*t20;
t25=t3*t22;
t26=t25+t12;
int_v_list110[7]=t26;
t12=t3*t26;
t25=t12+t23;
int_v_list210[16]=t25;
t12=int_v_W1-int_v_p341;
t23=t12*int_v_list002[0];
t27=int_v_p341-int_v_r31;
t28=t27*int_v_list001[0];
t29=t28+t23;
t23=t6*t29;
t6=t12*int_v_list001[0];
t28=t27*int_v_list000[0];
t30=t28+t6;
t6=int_v_oo2zeta12*t30;
t28=t6+t23;
t31=t12*int_v_list003[0];
t12=t27*int_v_list002[0];
t27=t12+t31;
t12=t1*t27;
t31=t3*t29;
t32=t31+t12;
t12=t1*t32;
t31=t12+t28;
t12=t1*t29;
t1=t3*t30;
t33=t1+t12;
int_v_list110[6]=t33;
t1=t3*t33;
t3=t1+t31;
int_v_list210[15]=t3;
t1=int_v_W2-int_v_p122;
t12=t1*t15;
t31=int_v_p122-int_v_r12;
t34=t31*t17;
t35=t34+t12;
int_v_list210[14]=t35;
t12=t1*t24;
t34=t4+t12;
t12=t31*t26;
t36=t12+t34;
int_v_list210[13]=t36;
t12=t1*t32;
t34=t31*t33;
t37=t34+t12;
int_v_list210[12]=t37;
t12=int_v_W1-int_v_p121;
t34=t15*t12;
t15=int_v_p121-int_v_r11;
t38=t15*t17;
t17=t38+t34;
int_v_list210[11]=t17;
t34=t12*t24;
t24=t15*t26;
t26=t24+t34;
int_v_list210[10]=t26;
t24=t12*t32;
t32=t4+t24;
t4=t15*t33;
t24=t4+t32;
int_v_list210[9]=t24;
t4=t11+t8;
t8=t1*t7;
t11=t31*t9;
t32=t11+t8;
t8=t1*t32;
t11=t8+t4;
t8=t1*t9;
t32=t31*t13;
t33=t32+t8;
int_v_list110[5]=t33;
t8=t31*t33;
t32=t8+t11;
int_v_list210[8]=t32;
t8=t1*int_v_list002[0];
t11=t31*int_v_list001[0];
t33=t11+t8;
t8=t2*t33;
t11=t14+t8;
t8=t19+t11;
t11=t1*t18;
t14=t10+t11;
t11=t31*t20;
t19=t11+t14;
t11=t1*t19;
t14=t11+t8;
t8=t1*t20;
t11=t5+t8;
t8=t31*t22;
t19=t8+t11;
int_v_list110[4]=t19;
t8=t31*t19;
t11=t8+t14;
int_v_list210[7]=t11;
t8=t1*t27;
t14=t31*t29;
t19=t14+t8;
t8=t1*t19;
t14=t28+t8;
t8=t1*t29;
t19=t31*t30;
t28=t19+t8;
int_v_list110[3]=t28;
t8=t31*t28;
t19=t8+t14;
int_v_list210[6]=t19;
t8=t12*t7;
t7=t15*t9;
t14=t7+t8;
t7=t1*t14;
t8=t12*t9;
t9=t15*t13;
t13=t9+t8;
int_v_list110[2]=t13;
t8=t31*t13;
t9=t8+t7;
int_v_list210[5]=t9;
t7=t12*t18;
t8=t15*t20;
t18=t8+t7;
t7=t1*t18;
t8=t12*int_v_list002[0];
t28=t15*int_v_list001[0];
t33=t28+t8;
t8=t2*t33;
t2=t8+t7;
t7=t12*t20;
t20=t15*t22;
t22=t20+t7;
int_v_list110[1]=t22;
t7=t31*t22;
t20=t7+t2;
int_v_list210[4]=t20;
t2=t12*t27;
t7=t10+t2;
t2=t15*t29;
t10=t2+t7;
t2=t1*t10;
t1=t12*t29;
t7=t5+t1;
t1=t15*t30;
t5=t1+t7;
int_v_list110[0]=t5;
t1=t31*t5;
t7=t1+t2;
int_v_list210[3]=t7;
t1=t12*t14;
t2=t4+t1;
t1=t15*t13;
t4=t1+t2;
int_v_list210[2]=t4;
t1=t12*t18;
t2=t21+t1;
t1=t15*t22;
t13=t1+t2;
int_v_list210[1]=t13;
t1=t23+t8;
t2=t6+t1;
t1=t12*t10;
t6=t1+t2;
t1=t15*t5;
t2=t1+t6;
int_v_list210[0]=t2;
return 1;}
