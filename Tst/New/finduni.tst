LIB "tst.lib";
tst_init();
ring R = (0,T(1)), (x,y,z), (C,dp);
option(redSB);
ideal I =
z,
1234*x^3-y^2,
6922566050427*y^13-109314240502292757*x^2*y^10-92416728*x*y^11-7260831192055982627368914768*x*y^10+96932590666832755034752*x^2*y^8+1182619572*y^10+92913926542752043491720043032*y^9-1240407244100450595424448*x*y^7+(-4346590487508328781472*T(1)^2)*x+(-452098311926124992*T(1)^2),
5623534206*x^2*y^11-584916*x*y^11-71962097769*x*y^10+613497409283751614144*x^2*y^8-4779840592827222787722*y^10+75478462801334793235510896*x^2*y^7-7850678760129434148256*x*y^7+(-2861381721051424*T(1)^2);
I = std(I);
I;
"start finduni";
finduni(I);

tst_status(1);
$