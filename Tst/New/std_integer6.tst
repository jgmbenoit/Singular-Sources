LIB "tst.lib"; tst_init();


// Example 6
ring rng = (integer),(x,y,z),dp;

ideal I = 242*x^3*z+y^2*z-y*z+11*y,
y^3*z^2-y^2*z^2+11*y^2*z+484,
y^4*z-y^3*z-10648*x^3+11*y^3-44*y^2+44*y,
x^3*y*z^2-2,
11*x^3*y^2*z+484*x^3+2*y^2-2*y,
117128*x^6-121*x^3*y^3+968*x^3*y^2-968*x^3*y+2*y^4-4*y^3+2*y^2,
121*x^6*z^3+y*z-z+11,
2178*x^2*y^2*z^3-1452*x^2*y*z^3+15972*x^2*y*z^2,
1452*x^2*y^3*z^2-1452*x^2*y^2*z^2+7986*x^2*y^2*z,
-726*x^3*y^2*z^2+484*x^3*y*z^2+y^4*z^2-5324*x^3*y*z-y^3*z^2+22*y^3*z-11*y^2*z+121*y^2,
2904*x^2*y^3*z^2-2178*x^2*y^2*z^2+23958*x^2*y^2*z+351384*x^2,
726*x^2*y^4*z-726*x^2*y^3*z+7730448*x^5+31944*x^2*y^2-31944*x^2*y,
-968*x^3*y^3*z+726*x^3*y^2*z-2*y^5*z-7986*x^3*y^2+4*y^4*z+21296*x^3*y-22*y^4-2*y^3*z-10648*x^3+110*y^3-132*y^2+44*y,
95832*x^2*y^2*z^2-63888*x^2*y*z^2+702768*x^2*y*z,
63888*x^2*y^3*z-63888*x^2*y^2*z+351384*x^2*y^2,
-5*y^6*z^2+9*y^5*z^2-88*y^5*z-4*y^4*z^2+253*y^4*z-363*y^4-264*y^3*z+968*y^3+88*y^2*z-484*y^2,
726*x^5*z^3-6*x^2*y^2*z^3+3*x^2*y*z^3-33*x^2*y*z^2,
726*x^5*y*z^2-3*x^2*y^3*z^2+3*x^2*y^2*z^2,
-242*x^6*z^2+3*x^3*y^2*z^2-x^3*y*z^2+22*x^3*y*z,
-9*x^2*y^3*z^4+6*x^2*y^2*z^4-66*x^2*y^2*z^3,
-6*x^2*y^4*z^3+6*x^2*y^3*z^3-33*x^2*y^3*z^2,
4*x^3*y^3*z^3-2*x^3*y^2*z^3+33*x^3*y^2*z^2,
-12*x^2*y^4*z^3+9*x^2*y^3*z^3-31944*x^5*z^2-99*x^2*y^3*z^2+264*x^2*y^2*z^2-132*x^2*y*z^2,
-3*x^2*y^5*z^2+3*x^2*y^4*z^2-63888*x^5*y*z,
7*x^3*y^4*z^2-5*x^3*y^3*z^2+66*x^3*y^3*z-176*x^3*y^2*z+88*x^3*y*z,
15972*x^5*y*z^2-66*x^2*y^3*z^2+33*x^2*y^2*z^2-363*x^2*y^2*z-15972*x^2,
-33*x^2*y^4*z+33*x^2*y^3*z-351384*x^5-1452*x^2*y^2+1452*x^2*y,
-5324*x^6*y*z+11*x^3*y^2*z+121*x^3*y^2-968*x^3*y+484*x^3-4*y^3+6*y^2-2*y,
-99*x^2*y^4*z^3+66*x^2*y^3*z^3-726*x^2*y^3*z^2-4356*x^2*y^2*z^2+2904*x^2*y*z^2-31944*x^2*y*z,
-66*x^2*y^5*z^2+66*x^2*y^4*z^2-363*x^2*y^4*z-2904*x^2*y^3*z+2904*x^2*y^2*z-15972*x^2*y^2,
-11*x^3*y^4*z^2+22*x^3*y^3*z^2-8*y^4*z+12*y^3*z-44*y^3-4*y^2*z+22*y^2,
-132*x^2*y^5*z^2+99*x^2*y^4*z^2-702768*x^5*y*z-1089*x^2*y^4*z-2904*x^2*y^3*z+2904*x^2*y^2*z-47916*x^2*y^2,
-33*x^2*y^6*z+33*x^2*y^5*z-351384*x^5*y^2-1452*x^2*y^4+1452*x^2*y^3,
22*x^3*y^5*z-11*x^3*y^4*z+363*x^3*y^4-968*x^3*y^3+484*x^3*y^2-4*y^5+6*y^4-2*y^3,
33*x^5*y^2*z^3-1452*x^5*z^2+12*x^2*y^2*z^2-6*x^2*y*z^2,
-33*x^5*y^3*z^2-2904*x^5*y*z,
-33*x^6*y^2*z^2-8*x^3*y^2*z+4*x^3*y*z,
-263538*x^5*y^2*z+726*x^2*y^4*z-363*x^2*y^3*z-7730448*x^5+3993*x^2*y^3-31944*x^2*y^2+31944*x^2*y,
-170069856*x^8+87846*x^5*y^3-1405536*x^5*y^2+363*x^2*y^5+1405536*x^5*y-3267*x^2*y^4+5808*x^2*y^3-2904*x^2*y^2,
87846*x^6*y^2-468512*x^6*y+363*x^3*y^4+234256*x^6-4235*x^3*y^3+5808*x^3*y^2-8*y^5-1936*x^3*y+20*y^4-16*y^3+4*y^2,
-2108304*x^5*y^2*z^2+1089*x^2*y^5*z^2+1405536*x^5*y*z^2-9438*x^2*y^4*z^2-15460896*x^5*y*z+7986*x^2*y^4*z+14520*x^2*y^3*z^2-63888*x^2*y^3*z-5808*x^2*y^2*z^2+63888*x^2*y^2*z,
-1405536*x^5*y^3*z+726*x^2*y^6*z+1405536*x^5*y^2*z-6534*x^2*y^5*z-7730448*x^5*y^2+3993*x^2*y^5+11616*x^2*y^4*z-31944*x^2*y^4-5808*x^2*y^3*z+31944*x^2*y^3,
726*x^3*y^5*z-4598*x^3*y^4*z+3993*x^3*y^4+5808*x^3*y^3*z-16*y^6*z-21296*x^3*y^3-1936*x^3*y^2*z+40*y^5*z+10648*x^3*y^2-88*y^5-32*y^4*z+132*y^4+8*y^3*z-44*y^3,
-2811072*x^5*y^3*z+1452*x^2*y^6*z+2108304*x^5*y^2*z-12705*x^2*y^5*z-11595672*x^5*y^2+11979*x^2*y^5+20328*x^2*y^4*z-127776*x^2*y^4-8712*x^2*y^3*z+111804*x^2*y^3,
-702768*x^5*y^4+363*x^2*y^7+702768*x^5*y^3-3267*x^2*y^6+5808*x^2*y^5-2904*x^2*y^4,
363*x^3*y^6-2299*x^3*y^5+2904*x^3*y^4-8*y^7-968*x^3*y^3+20*y^6-16*y^5+4*y^4,
-702768*x^8*z^2-726*x^5*y^3*z^2+2904*x^5*y^2*z^2+24*x^2*y^4*z^2-36*x^2*y^3*z^2+12*x^2*y^2*z^2,
-1405536*x^8*y*z+726*x^5*y^4*z-5808*x^5*y^3*z+5808*x^5*y^2*z,
726*x^6*y^3*z-3872*x^6*y^2*z+1936*x^6*y*z-16*x^3*y^4*z+24*x^3*y^3*z-8*x^3*y^2*z,
-15460896*x^8*y*z-3993*x^5*y^4*z+31944*x^5*y^2*z+264*x^2*y^5*z-527076*x^5*y^2-396*x^2*y^4*z+1452*x^2*y^4+132*x^2*y^3*z-726*x^2*y^3,
-7730448*x^8*y^2+3993*x^5*y^5-31944*x^5*y^4+31944*x^5*y^3,
3993*x^6*y^4-21296*x^6*y^3+10648*x^6*y^2-88*x^3*y^5+132*x^3*y^4-44*x^3*y^3,
-1452*x^5*y*z^4+726*x^5*z^4-7986*x^5*z^3+726*x^2*z^2,
87846*x^8*z^3-726*x^5*y^2*z^3+726*x^5*y*z^3+726*x^2*y*z-726*x^2*z,
726*x^6*y*z^3-363*x^6*z^3+3993*x^6*z^2-242*x^3*z+y^2*z-2*y*z+11*y+z-11,
-2178*x^5*y^2*z^5+1452*x^5*y*z^5-15972*x^5*y*z^4,
-1452*x^5*y^3*z^4+1452*x^5*y^2*z^4-7986*x^5*y^2*z^3,
1089*x^6*y^2*z^4-726*x^6*y*z^4+7986*x^6*y*z^3+y^3*z^2-3*y^2*z^2+11*y^2*z+2*y*z^2-22*y*z,
-2904*x^5*y^3*z^4+2178*x^5*y^2*z^4-23958*x^5*y^2*z^3+63888*x^5*y*z^3-31944*x^5*z^3-31944*x^2*z,
-726*x^5*y^4*z^3+726*x^5*y^3*z^3-11595672*x^8*z^2-31944*x^2*y+31944*x^2,
1452*x^6*y^3*z^3-1089*x^6*y^2*z^3+11979*x^6*y^2*z^2-31944*x^6*y*z^2+15972*x^6*z^2+3*y^4*z-6*y^3*z+33*y^3+3*y^2*z-121*y^2+132*y-44,
-726*x^8*z^5+3*x^2*y*z^3,
-363*x^8*y*z^4+3*x^2*y^2*z^2-3*x^2*y*z^2,
363*x^9*z^4-x^3*y*z^2-x^3*z^2,
-15972*x^8*y*z^4-2904*x^5*y*z^3+1452*x^5*z^3+33*x^2*y^2*z^2+1452*x^2*z,
3993*x^8*y^2*z^3+527076*x^8*z^2+33*x^2*y^3*z-33*x^2*y^2*z+1452*x^2*y-1452*x^2,
7986*x^9*y*z^3+1452*x^6*y*z^2-726*x^6*z^2+11*x^3*y^2*z-22*x^3*y*z+4*y^2-6*y+2,
263538*x^8*y^2*z^3-1405536*x^8*y*z^3+702768*x^8*z^3-5808*x^5*y^3*z^3+8712*x^5*y^2*z^3-2904*x^5*y*z^3+702768*x^5*z-363*x^2*y^3*z+2904*x^2*y^2*z-2904*x^2*y*z,
255104784*x^11*z^2-131769*x^8*y^3*z^2+1054152*x^8*y^2*z^2-1054152*x^8*y*z^2+702768*x^5*y-363*x^2*y^4-702768*x^5+3267*x^2*y^3-5808*x^2*y^2+2904*x^2*y,
-131769*x^9*y^2*z^2+702768*x^9*y*z^2-351384*x^9*z^2+2904*x^6*y^3*z^2-4356*x^6*y^2*z^2+1452*x^6*y*z^2-363*x^3*y^3+2299*x^3*y^2-2904*x^3*y+8*y^4+968*x^3-20*y^3+16*y^2-4*y;
ideal stdI = std(I);
stdI;


tst_status(1);$