/* User:      williamc
   Date:      Fri May 31 04:50:45 1996
   Target:    CGCVIS
   Universe:  firnovis */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <math.h>

#if !defined(NO_FIX_SUPPORT)
/* include definitions for the fix runtime library */
#include "CGCrtlib.h"
#endif

#if !defined(COMPLEX_DATA)
#define COMPLEX_DATA 1
 typedef struct complex_data { double real; double imag; } complex; 
#endif
#define NUMTIMES (4000)

extern int main(int argc, char *argv[]);

/* main function */
int main(int argc, char *argv[]) {
    FILE* fp_5[1];
double value_7;
double output_0;
double output_1[1027];
int output_3;
double taps_8[1024];
int signalIn_4;
double signalOut_2;
int count_9;
double index_10;
hrtime_t start, end;
float    time1, time2, time3, meansampletime, runningtotal;

runningtotal=0.0;
count_9=0;
index_10=0.0;
taps_8[0]=-4.10485902015429e-05;
taps_8[1]=-6.27991234694974e-06;
taps_8[2]=-6.48044369959903e-06;
taps_8[3]=-6.43306093680702e-06;
taps_8[4]=-6.18549614648338e-06;
taps_8[5]=-5.65484231686277e-06;
taps_8[6]=-4.90690921899934e-06;
taps_8[7]=-3.87530545584888e-06;
taps_8[8]=-2.64796014539528e-06;
taps_8[9]=-1.1672977954277e-06;
taps_8[10]=4.65807896753724e-07;
taps_8[11]=2.31694868332586e-06;
taps_8[12]=4.26108402754801e-06;
taps_8[13]=6.34709045098423e-06;
taps_8[14]=8.32039368507842e-06;
taps_8[15]=1.03524342421933e-05;
taps_8[16]=1.22940783771573e-05;
taps_8[17]=1.40434090184937e-05;
taps_8[18]=1.55991108335413e-05;
taps_8[19]=1.68791662966105e-05;
taps_8[20]=1.78352625670506e-05;
taps_8[21]=1.84341526874985e-05;
taps_8[22]=1.8620373666481e-05;
taps_8[23]=1.8385658232471e-05;
taps_8[24]=1.76838523244811e-05;
taps_8[25]=1.65418756025426e-05;
taps_8[26]=1.49446580234431e-05;
taps_8[27]=1.29522377146441e-05;
taps_8[28]=1.0547471990519e-05;
taps_8[29]=7.81061368146095e-06;
taps_8[30]=4.81464937853888e-06;
taps_8[31]=1.61179663101019e-06;
taps_8[32]=-1.71099058383343e-06;
taps_8[33]=-5.0488099964773e-06;
taps_8[34]=-8.33200562555188e-06;
taps_8[35]=-1.14320117172685e-05;
taps_8[36]=-1.42802860000988e-05;
taps_8[37]=-1.67508780891144e-05;
taps_8[38]=-1.8790401581735e-05;
taps_8[39]=-2.02899545008248e-05;
taps_8[40]=-2.12122161797997e-05;
taps_8[41]=-2.1468559025605e-05;
taps_8[42]=-2.10593643057415e-05;
taps_8[43]=-1.99492623976596e-05;
taps_8[44]=-1.8153420151707e-05;
taps_8[45]=-1.56764509207828e-05;
taps_8[46]=-1.25983326776471e-05;
taps_8[47]=-8.94271823758291e-06;
taps_8[48]=-4.84159257829812e-06;
taps_8[49]=-3.55598111528911e-07;
taps_8[50]=4.35460134561526e-06;
taps_8[51]=9.1997758613628e-06;
taps_8[52]=1.39976988562167e-05;
taps_8[53]=1.86400351231313e-05;
taps_8[54]=2.293955568233e-05;
taps_8[55]=2.67955895093785e-05;
taps_8[56]=3.00342291050642e-05;
taps_8[57]=3.25639852375732e-05;
taps_8[58]=3.42424709615771e-05;
taps_8[59]=3.50278812403134e-05;
taps_8[60]=3.480685217763e-05;
taps_8[61]=3.35965664375177e-05;
taps_8[62]=3.13411648896224e-05;
taps_8[63]=2.8109965060684e-05;
taps_8[64]=2.39135836048846e-05;
taps_8[65]=1.88820742377107e-05;
taps_8[66]=1.30881149688615e-05;
taps_8[67]=6.71473953860341e-06;
taps_8[68]=-1.17037264134303e-07;
taps_8[69]=-7.17834724909024e-06;
taps_8[70]=-1.43097534215284e-05;
taps_8[71]=-2.12620548793177e-05;
taps_8[72]=-2.7867954158958e-05;
taps_8[73]=-3.38682355555699e-05;
taps_8[74]=-3.91128920398273e-05;
taps_8[75]=-4.33696616849924e-05;
taps_8[76]=-4.65182331293739e-05;
taps_8[77]=-4.83826209900949e-05;
taps_8[78]=-4.8900840057722e-05;
taps_8[79]=-4.79663045313616e-05;
taps_8[80]=-4.55925163241106e-05;
taps_8[81]=-4.17553956469138e-05;
taps_8[82]=-3.65577007156457e-05;
taps_8[83]=-3.00624410899258e-05;
taps_8[84]=-2.24545959348004e-05;
taps_8[85]=-1.3877697071816e-05;
taps_8[86]=-4.59567287529646e-06;
taps_8[87]=5.18679829143618e-06;
taps_8[88]=1.51543462296804e-05;
taps_8[89]=2.5056990636767e-05;
taps_8[90]=3.45622990628551e-05;
taps_8[91]=4.34110087077692e-05;
taps_8[92]=5.1279245294408e-05;
taps_8[93]=5.79368559112913e-05;
taps_8[94]=6.31055698540981e-05;
taps_8[95]=6.66200772604808e-05;
taps_8[96]=6.82784053777787e-05;
taps_8[97]=6.80055088543504e-05;
taps_8[98]=6.57040092489261e-05;
taps_8[99]=6.14126629665977e-05;
taps_8[100]=5.514917082034e-05;
taps_8[101]=4.70717816564067e-05;
taps_8[102]=3.73205710441116e-05;
taps_8[103]=2.61625193943218e-05;
taps_8[104]=1.3845146143102e-05;
taps_8[105]=7.31476844069375e-07;
taps_8[106]=-1.28556438219561e-05;
taps_8[107]=-2.64916086632523e-05;
taps_8[108]=-3.98144771103122e-05;
taps_8[109]=-5.23797097138412e-05;
taps_8[110]=-6.38304469071307e-05;
taps_8[111]=-7.37533381958204e-05;
taps_8[112]=-8.18448015994237e-05;
taps_8[113]=-8.77664368817071e-05;
taps_8[114]=-9.13131584318904e-05;
taps_8[115]=-9.22656949125147e-05;
taps_8[116]=-9.05495769367747e-05;
taps_8[117]=-8.60941735625944e-05;
taps_8[118]=-7.8982884121409e-05;
taps_8[119]=-6.93015185003471e-05;
taps_8[120]=-5.729574498826e-05;
taps_8[121]=-4.3206750661085e-05;
taps_8[122]=-2.74268262135782e-05;
taps_8[123]=-1.0328774980152e-05;
taps_8[124]=7.58371932386385e-06;
taps_8[125]=2.58462741782879e-05;
taps_8[126]=4.38903014506934e-05;
taps_8[127]=6.12164978440272e-05;
taps_8[128]=7.72517627161004e-05;
taps_8[129]=9.15218577429016e-05;
taps_8[130]=0.000103514009049596;
taps_8[131]=0.000112844827506834;
taps_8[132]=0.000119117395863559;
taps_8[133]=0.000122096575356086;
taps_8[134]=0.000121555042757421;
taps_8[135]=0.00011744476107651;
taps_8[136]=0.000109740663750113;
taps_8[137]=9.86032644392818e-05;
taps_8[138]=8.42211541515071e-05;
taps_8[139]=6.69624838747556e-05;
taps_8[140]=4.72127394342253e-05;
taps_8[141]=2.55211597474115e-05;
taps_8[142]=2.43231666674515e-06;
taps_8[143]=-2.13752740917339e-05;
taps_8[144]=-4.52590060477748e-05;
taps_8[145]=-6.84766230954042e-05;
taps_8[146]=-9.0365421235607e-05;
taps_8[147]=-0.000110202340589578;
taps_8[148]=-0.000127383266844889;
taps_8[149]=-0.000141287918298562;
taps_8[150]=-0.000151451878882252;
taps_8[151]=-0.000157432992333869;
taps_8[152]=-0.00015897502428116;
taps_8[153]=-0.000155870608549917;
taps_8[154]=-0.000148119679922636;
taps_8[155]=-0.000135783736050255;
taps_8[156]=-0.00011913646890242;
taps_8[157]=-9.85122010058052e-05;
taps_8[158]=-7.44471468065165e-05;
taps_8[159]=-4.75169638195416e-05;
taps_8[160]=-1.84746536238986e-05;
taps_8[161]=1.19187919042695e-05;
taps_8[162]=4.27686583101453e-05;
taps_8[163]=7.32148504001321e-05;
taps_8[164]=0.000102314148414001;
taps_8[165]=0.00012920928199096;
taps_8[166]=0.000153012914188822;
taps_8[167]=0.000172978178345116;
taps_8[168]=0.000188378166864465;
taps_8[169]=0.000198674605036415;
taps_8[170]=0.000203393087926507;
taps_8[171]=0.000202283729325368;
taps_8[172]=0.000195188458329326;
taps_8[173]=0.000182195822158416;
taps_8[174]=0.000163497985324821;
taps_8[175]=0.000139531279042549;
taps_8[176]=0.000110828712404391;
taps_8[177]=7.81471219204883e-05;
taps_8[178]=4.23097382147485e-05;
taps_8[179]=4.32534655904554e-06;
taps_8[180]=-3.47799657918898e-05;
taps_8[181]=-7.38481794592127e-05;
taps_8[182]=-0.000111765247139405;
taps_8[183]=-0.000147350724085271;
taps_8[184]=-0.000179534104140222;
taps_8[185]=-0.00020724637450465;
taps_8[186]=-0.000229595649189852;
taps_8[187]=-0.000245753852306883;
taps_8[188]=-0.000255124704130997;
taps_8[189]=-0.000257227641630487;
taps_8[190]=-0.00025185411132203;
taps_8[191]=-0.000238940109111693;
taps_8[192]=-0.000218713551981449;
taps_8[193]=-0.000191551612044901;
taps_8[194]=-0.000158117344180923;
taps_8[195]=-0.000119202893215909;
taps_8[196]=-7.58529027689147e-05;
taps_8[197]=-2.91977438236069e-05;
taps_8[198]=1.94368682287616e-05;
taps_8[199]=6.8705339092623e-05;
taps_8[200]=0.000117139770574355;
taps_8[201]=0.000163330817447082;
taps_8[202]=0.000205832360514755;
taps_8[203]=0.000243340888160399;
taps_8[204]=0.000274598721488349;
taps_8[205]=0.000298572369893355;
taps_8[206]=0.000314347366956299;
taps_8[207]=0.000321299230985803;
taps_8[208]=0.000318978249355968;
taps_8[209]=0.000307266422531577;
taps_8[210]=0.00028625083047742;
taps_8[211]=0.000256362285522925;
taps_8[212]=0.000218234913193012;
taps_8[213]=0.000172828714271459;
taps_8[214]=0.000121272555199706;
taps_8[215]=6.496972346656e-05;
taps_8[216]=5.42918960904014e-06;
taps_8[217]=-5.56420029317831e-05;
taps_8[218]=-0.0001165201485712;
taps_8[219]=-0.000175378925422957;
taps_8[220]=-0.000230476784616508;
taps_8[221]=-0.000280075510861557;
taps_8[222]=-0.000322624799478507;
taps_8[223]=-0.000356680406485794;
taps_8[224]=-0.00038107950818989;
taps_8[225]=-0.000394851922018956;
taps_8[226]=-0.000397382986797224;
taps_8[227]=-0.000388309319721726;
taps_8[228]=-0.000367665001227307;
taps_8[229]=-0.000335761067681751;
taps_8[230]=-0.000293311101538506;
taps_8[231]=-0.000241289912514808;
taps_8[232]=-0.00018104075190589;
taps_8[233]=-0.00011411453991483;
taps_8[234]=-4.23570007132143e-05;
taps_8[235]=3.22656750025608e-05;
taps_8[236]=0.000107596448628945;
taps_8[237]=0.00018147145939103;
taps_8[238]=0.000251659302297304;
taps_8[239]=0.000316049835340505;
taps_8[240]=0.000372593331651563;
taps_8[241]=0.000419487940729876;
taps_8[242]=0.000455113362865272;
taps_8[243]=0.000478206729681686;
taps_8[244]=0.000487783034209065;
taps_8[245]=0.000483295333669786;
taps_8[246]=0.000464535730003232;
taps_8[247]=0.000431772280662915;
taps_8[248]=0.000385629720810713;
taps_8[249]=0.000327199758585707;
taps_8[250]=0.000257898482996672;
taps_8[251]=0.000179552335674945;
taps_8[252]=9.42324802744343e-05;
taps_8[253]=4.32092321981464e-06;
taps_8[254]=-8.767511831997e-05;
taps_8[255]=-0.000179072552729365;
taps_8[256]=-0.000267208170849344;
taps_8[257]=-0.000349396898388738;
taps_8[258]=-0.00042313109947345;
taps_8[259]=-0.000486037343304923;
taps_8[260]=-0.000536068801636699;
taps_8[261]=-0.000571454000047465;
taps_8[262]=-0.00059087231298634;
taps_8[263]=-0.000593384717258721;
taps_8[264]=-0.000578587494368962;
taps_8[265]=-0.000546517273524351;
taps_8[266]=-0.000497777873071625;
taps_8[267]=-0.000433415556537857;
taps_8[268]=-0.00035501610059456;
taps_8[269]=-0.000264552506266698;
taps_8[270]=-0.000164451304552992;
taps_8[271]=-5.74148600651388e-05;
taps_8[272]=5.35363281003041e-05;
taps_8[273]=0.000165260820678015;
taps_8[274]=0.000274472050725655;
taps_8[275]=0.000377944758147115;
taps_8[276]=0.000472497996446331;
taps_8[277]=0.000555202965642309;
taps_8[278]=0.000623360531401895;
taps_8[279]=0.000674698903868529;
taps_8[280]=0.000707335516429796;
taps_8[281]=0.000719953592935128;
taps_8[282]=0.000711740595872378;
taps_8[283]=0.000682533101401086;
taps_8[284]=0.000632724915792309;
taps_8[285]=0.000563377120518508;
taps_8[286]=0.00047609090232215;
taps_8[287]=0.000373080573238073;
taps_8[288]=0.00025701333212609;
taps_8[289]=0.000131047922916085;
taps_8[290]=-1.35597195405922e-06;
taps_8[291]=-0.000136424843102925;
taps_8[292]=-0.000270283359622999;
taps_8[293]=-0.000398958803315089;
taps_8[294]=-0.000518602103401226;
taps_8[295]=-0.000625497920313323;
taps_8[296]=-0.000716280238486885;
taps_8[297]=-0.000787935055630905;
taps_8[298]=-0.000837999088160925;
taps_8[299]=-0.000864539945168113;
taps_8[300]=-0.000866326435817647;
taps_8[301]=-0.000842776838468573;
taps_8[302]=-0.000794090785776732;
taps_8[303]=-0.000721157299267711;
taps_8[304]=-0.000625645047182207;
taps_8[305]=-0.000509867270418223;
taps_8[306]=-0.000376828171582901;
taps_8[307]=-0.000230048447758953;
taps_8[308]=-7.35721489985449e-05;
taps_8[309]=8.82397238421628e-05;
taps_8[310]=0.000250732881833075;
taps_8[311]=0.000409187866775829;
taps_8[312]=0.000558859007898819;
taps_8[313]=0.000695210832369862;
taps_8[314]=0.000813957877061556;
taps_8[315]=0.000911291796250707;
taps_8[316]=0.000983906088391663;
taps_8[317]=0.00102919969715491;
taps_8[318]=0.00104527101623297;
taps_8[319]=0.00103108304299463;
taps_8[320]=0.00098641666492411;
taps_8[321]=0.000911987385643351;
taps_8[322]=0.00080934997400283;
taps_8[323]=0.000680962489080001;
taps_8[324]=0.000530039848959983;
taps_8[325]=0.000360568037868159;
taps_8[326]=0.000177109613141466;
taps_8[327]=-1.52252331489664e-05;
taps_8[328]=-0.000211009396037845;
taps_8[329]=-0.000404559581170206;
taps_8[330]=-0.000590189163242959;
taps_8[331]=-0.000762282899397457;
taps_8[332]=-0.000915553316738735;
taps_8[333]=-0.00104511239573034;
taps_8[334]=-0.00114671145774895;
taps_8[335]=-0.00121678986150735;
taps_8[336]=-0.00125268097172094;
taps_8[337]=-0.00125262039807831;
taps_8[338]=-0.00121590364476507;
taps_8[339]=-0.00114284009857819;
taps_8[340]=-0.00103485205127872;
taps_8[341]=-0.00089436892498331;
taps_8[342]=-0.000724862331421346;
taps_8[343]=-0.000530680311856522;
taps_8[344]=-0.000317023267568216;
taps_8[345]=-8.97245898939239e-05;
taps_8[346]=0.000144823346068166;
taps_8[347]=0.000379927505952292;
taps_8[348]=0.000608700947066885;
taps_8[349]=0.000824342584839133;
taps_8[350]=0.0010202552434078;
taps_8[351]=0.00119032474233865;
taps_8[352]=0.00132902606536383;
taps_8[353]=0.00143167854859802;
taps_8[354]=0.00149451886510745;
taps_8[355]=0.0015149093572486;
taps_8[356]=0.00149135913061003;
taps_8[357]=0.00142366976822282;
taps_8[358]=0.00131288934584293;
taps_8[359]=0.00116138479527815;
taps_8[360]=0.000972723557581556;
taps_8[361]=0.000751670300845109;
taps_8[362]=0.00050399665278481;
taps_8[363]=0.000236409435222172;
taps_8[364]=-4.37010375385551e-05;
taps_8[365]=-0.000328387151843177;
taps_8[366]=-0.000609438891492236;
taps_8[367]=-0.000878543772936451;
taps_8[368]=-0.00112759992452075;
taps_8[369]=-0.00134888461024465;
taps_8[370]=-0.00153535906830799;
taps_8[371]=-0.00168081615106879;
taps_8[372]=-0.0017801500221073;
taps_8[373]=-0.00182945753578819;
taps_8[374]=-0.00182624750775239;
taps_8[375]=-0.00176947298985087;
taps_8[376]=-0.00165966248281121;
taps_8[377]=-0.0014988683957662;
taps_8[378]=-0.00129070888238035;
taps_8[379]=-0.00104022752041877;
taps_8[380]=-0.000753844818780844;
taps_8[381]=-0.000439134232991035;
taps_8[382]=-0.000104692443930693;
taps_8[383]=0.000240153951790957;
taps_8[384]=0.000585538481486748;
taps_8[385]=0.000921395212219526;
taps_8[386]=0.00123768251241533;
taps_8[387]=0.00152473948086447;
taps_8[388]=0.00177351124860533;
taps_8[389]=0.00197588931923436;
taps_8[390]=0.0021249045492009;
taps_8[391]=0.00221501777918171;
taps_8[392]=0.00224225028179856;
taps_8[393]=0.00220439584413515;
taps_8[394]=0.00210106352007017;
taps_8[395]=0.00193379014505865;
taps_8[396]=0.00170597944716766;
taps_8[397]=0.00142290556496074;
taps_8[398]=0.00109154269222168;
taps_8[399]=0.000720461195292026;
taps_8[400]=0.000319557119545218;
taps_8[401]=-0.000100145877319413;
taps_8[402]=-0.000526847816955693;
taps_8[403]=-0.000948238621311922;
taps_8[404]=-0.00135189948754437;
taps_8[405]=-0.00172560407994248;
taps_8[406]=-0.00205773210474168;
taps_8[407]=-0.00233756542544168;
taps_8[408]=-0.00255567360235346;
taps_8[409]=-0.00270416409208601;
taps_8[410]=-0.002777000804462;
taps_8[411]=-0.00277017012303601;
taps_8[412]=-0.00268189929471029;
taps_8[413]=-0.00251271015717759;
taps_8[414]=-0.00226551228644611;
taps_8[415]=-0.00194552682984887;
taps_8[416]=-0.00156024458458877;
taps_8[417]=-0.00111921642029631;
taps_8[418]=-0.000633879508596764;
taps_8[419]=-0.000117226822089991;
taps_8[420]=0.000416478993259677;
taps_8[421]=0.000952131560493818;
taps_8[422]=0.00147414620797954;
taps_8[423]=0.00196694284908192;
taps_8[424]=0.00241534709048653;
taps_8[425]=0.00280508365077936;
taps_8[426]=0.00312316450570782;
taps_8[427]=0.00335834100183557;
taps_8[428]=0.00350142949122223;
taps_8[429]=0.00354567459721186;
taps_8[430]=0.00348696624375532;
taps_8[431]=0.00332407291306743;
taps_8[432]=0.00305871559221903;
taps_8[433]=0.00269564275278301;
taps_8[434]=0.00224253950489106;
taps_8[435]=0.00170993288050776;
taps_8[436]=0.00111093310111673;
taps_8[437]=0.000460975155465791;
taps_8[438]=-0.000222592556174237;
taps_8[439]=-0.000920905868588524;
taps_8[440]=-0.00161411426753779;
taps_8[441]=-0.00228187736465323;
taps_8[442]=-0.00290396437324447;
taps_8[443]=-0.00346079406971285;
taps_8[444]=-0.00393404571654105;
taps_8[445]=-0.00430718138461845;
taps_8[446]=-0.00456600461950203;
taps_8[447]=-0.00469910143373017;
taps_8[448]=-0.00469828659572174;
taps_8[449]=-0.00455890590772018;
taps_8[450]=-0.00428011744548165;
taps_8[451]=-0.00386501012411526;
taps_8[452]=-0.00332068300943833;
taps_8[453]=-0.00265815312377572;
taps_8[454]=-0.00189221480703878;
taps_8[455]=-0.00104113051724269;
taps_8[456]=-0.00012627684121803;
taps_8[457]=0.000828365585919089;
taps_8[458]=0.00179675123617038;
taps_8[459]=0.00275144052944547;
taps_8[460]=0.00366427435501059;
taps_8[461]=0.00450714785126018;
taps_8[462]=0.0052527534954686;
taps_8[463]=0.00587538430326645;
taps_8[464]=0.00635166912243049;
taps_8[465]=0.0066613254306348;
taps_8[466]=0.00678780665333652;
taps_8[467]=0.00671892522779786;
taps_8[468]=0.00644733708478563;
taps_8[469]=0.00597096624156978;
taps_8[470]=0.00529326335093911;
taps_8[471]=0.00442337916974131;
taps_8[472]=0.00337615603685074;
taps_8[473]=0.00217201995578836;
taps_8[474]=0.000836687596759469;
taps_8[475]=-0.000599225290970393;
taps_8[476]=-0.00210076975399763;
taps_8[477]=-0.00362931455356604;
taps_8[478]=-0.00514334559202617;
taps_8[479]=-0.00659932360704468;
taps_8[480]=-0.007952653125137;
taps_8[481]=-0.00915867235851367;
taps_8[482]=-0.0101737084002343;
taps_8[483]=-0.010956107574607;
taps_8[484]=-0.0114672816792802;
taps_8[485]=-0.0116726812083514;
taps_8[486]=-0.011542733117558;
taps_8[487]=-0.0110536604922463;
taps_8[488]=-0.010188220615185;
taps_8[489]=-0.00893628611676188;
taps_8[490]=-0.00729530625846509;
taps_8[491]=-0.00527058523078785;
taps_8[492]=-0.00287541467722181;
taps_8[493]=-0.000131010338607074;
taps_8[494]=0.00293370705868348;
taps_8[495]=0.00628252386422102;
taps_8[496]=0.00987249425133228;
taps_8[497]=0.0136546760297582;
taps_8[498]=0.0175749918235719;
taps_8[499]=0.0215752311501258;
taps_8[500]=0.0255941421293223;
taps_8[501]=0.0295686179378034;
taps_8[502]=0.0334349255296311;
taps_8[503]=0.0371299746641472;
taps_8[504]=0.04059257731491;
taps_8[505]=0.0437646887892564;
taps_8[506]=0.0465925854509202;
taps_8[507]=0.0490279687094751;
taps_8[508]=0.0510289558851041;
taps_8[509]=0.052560948602764;
taps_8[510]=0.053597347223733;
taps_8[511]=0.054120105655918;
taps_8[512]=0.054120105655918;
taps_8[513]=0.053597347223733;
taps_8[514]=0.052560948602764;
taps_8[515]=0.0510289558851041;
taps_8[516]=0.0490279687094751;
taps_8[517]=0.0465925854509202;
taps_8[518]=0.0437646887892564;
taps_8[519]=0.04059257731491;
taps_8[520]=0.0371299746641472;
taps_8[521]=0.0334349255296311;
taps_8[522]=0.0295686179378034;
taps_8[523]=0.0255941421293223;
taps_8[524]=0.0215752311501258;
taps_8[525]=0.0175749918235719;
taps_8[526]=0.0136546760297582;
taps_8[527]=0.00987249425133228;
taps_8[528]=0.00628252386422102;
taps_8[529]=0.00293370705868348;
taps_8[530]=-0.000131010338607074;
taps_8[531]=-0.00287541467722181;
taps_8[532]=-0.00527058523078785;
taps_8[533]=-0.00729530625846509;
taps_8[534]=-0.00893628611676188;
taps_8[535]=-0.010188220615185;
taps_8[536]=-0.0110536604922463;
taps_8[537]=-0.011542733117558;
taps_8[538]=-0.0116726812083514;
taps_8[539]=-0.0114672816792802;
taps_8[540]=-0.010956107574607;
taps_8[541]=-0.0101737084002343;
taps_8[542]=-0.00915867235851367;
taps_8[543]=-0.007952653125137;
taps_8[544]=-0.00659932360704468;
taps_8[545]=-0.00514334559202617;
taps_8[546]=-0.00362931455356604;
taps_8[547]=-0.00210076975399763;
taps_8[548]=-0.000599225290970393;
taps_8[549]=0.000836687596759469;
taps_8[550]=0.00217201995578836;
taps_8[551]=0.00337615603685074;
taps_8[552]=0.00442337916974131;
taps_8[553]=0.00529326335093911;
taps_8[554]=0.00597096624156978;
taps_8[555]=0.00644733708478563;
taps_8[556]=0.00671892522779786;
taps_8[557]=0.00678780665333652;
taps_8[558]=0.0066613254306348;
taps_8[559]=0.00635166912243049;
taps_8[560]=0.00587538430326645;
taps_8[561]=0.0052527534954686;
taps_8[562]=0.00450714785126018;
taps_8[563]=0.00366427435501059;
taps_8[564]=0.00275144052944547;
taps_8[565]=0.00179675123617038;
taps_8[566]=0.000828365585919089;
taps_8[567]=-0.00012627684121803;
taps_8[568]=-0.00104113051724269;
taps_8[569]=-0.00189221480703878;
taps_8[570]=-0.00265815312377572;
taps_8[571]=-0.00332068300943833;
taps_8[572]=-0.00386501012411526;
taps_8[573]=-0.00428011744548165;
taps_8[574]=-0.00455890590772018;
taps_8[575]=-0.00469828659572174;
taps_8[576]=-0.00469910143373017;
taps_8[577]=-0.00456600461950203;
taps_8[578]=-0.00430718138461845;
taps_8[579]=-0.00393404571654105;
taps_8[580]=-0.00346079406971285;
taps_8[581]=-0.00290396437324447;
taps_8[582]=-0.00228187736465323;
taps_8[583]=-0.00161411426753779;
taps_8[584]=-0.000920905868588524;
taps_8[585]=-0.000222592556174237;
taps_8[586]=0.000460975155465791;
taps_8[587]=0.00111093310111673;
taps_8[588]=0.00170993288050776;
taps_8[589]=0.00224253950489106;
taps_8[590]=0.00269564275278301;
taps_8[591]=0.00305871559221903;
taps_8[592]=0.00332407291306743;
taps_8[593]=0.00348696624375532;
taps_8[594]=0.00354567459721186;
taps_8[595]=0.00350142949122223;
taps_8[596]=0.00335834100183557;
taps_8[597]=0.00312316450570782;
taps_8[598]=0.00280508365077936;
taps_8[599]=0.00241534709048653;
taps_8[600]=0.00196694284908192;
taps_8[601]=0.00147414620797954;
taps_8[602]=0.000952131560493818;
taps_8[603]=0.000416478993259677;
taps_8[604]=-0.000117226822089991;
taps_8[605]=-0.000633879508596764;
taps_8[606]=-0.00111921642029631;
taps_8[607]=-0.00156024458458877;
taps_8[608]=-0.00194552682984887;
taps_8[609]=-0.00226551228644611;
taps_8[610]=-0.00251271015717759;
taps_8[611]=-0.00268189929471029;
taps_8[612]=-0.00277017012303601;
taps_8[613]=-0.002777000804462;
taps_8[614]=-0.00270416409208601;
taps_8[615]=-0.00255567360235346;
taps_8[616]=-0.00233756542544168;
taps_8[617]=-0.00205773210474168;
taps_8[618]=-0.00172560407994248;
taps_8[619]=-0.00135189948754437;
taps_8[620]=-0.000948238621311922;
taps_8[621]=-0.000526847816955693;
taps_8[622]=-0.000100145877319413;
taps_8[623]=0.000319557119545218;
taps_8[624]=0.000720461195292026;
taps_8[625]=0.00109154269222168;
taps_8[626]=0.00142290556496074;
taps_8[627]=0.00170597944716766;
taps_8[628]=0.00193379014505865;
taps_8[629]=0.00210106352007017;
taps_8[630]=0.00220439584413515;
taps_8[631]=0.00224225028179856;
taps_8[632]=0.00221501777918171;
taps_8[633]=0.0021249045492009;
taps_8[634]=0.00197588931923436;
taps_8[635]=0.00177351124860533;
taps_8[636]=0.00152473948086447;
taps_8[637]=0.00123768251241533;
taps_8[638]=0.000921395212219526;
taps_8[639]=0.000585538481486748;
taps_8[640]=0.000240153951790957;
taps_8[641]=-0.000104692443930693;
taps_8[642]=-0.000439134232991035;
taps_8[643]=-0.000753844818780844;
taps_8[644]=-0.00104022752041877;
taps_8[645]=-0.00129070888238035;
taps_8[646]=-0.0014988683957662;
taps_8[647]=-0.00165966248281121;
taps_8[648]=-0.00176947298985087;
taps_8[649]=-0.00182624750775239;
taps_8[650]=-0.00182945753578819;
taps_8[651]=-0.0017801500221073;
taps_8[652]=-0.00168081615106879;
taps_8[653]=-0.00153535906830799;
taps_8[654]=-0.00134888461024465;
taps_8[655]=-0.00112759992452075;
taps_8[656]=-0.000878543772936451;
taps_8[657]=-0.000609438891492236;
taps_8[658]=-0.000328387151843177;
taps_8[659]=-4.37010375385551e-05;
taps_8[660]=0.000236409435222172;
taps_8[661]=0.00050399665278481;
taps_8[662]=0.000751670300845109;
taps_8[663]=0.000972723557581556;
taps_8[664]=0.00116138479527815;
taps_8[665]=0.00131288934584293;
taps_8[666]=0.00142366976822282;
taps_8[667]=0.00149135913061003;
taps_8[668]=0.0015149093572486;
taps_8[669]=0.00149451886510745;
taps_8[670]=0.00143167854859802;
taps_8[671]=0.00132902606536383;
taps_8[672]=0.00119032474233865;
taps_8[673]=0.0010202552434078;
taps_8[674]=0.000824342584839133;
taps_8[675]=0.000608700947066885;
taps_8[676]=0.000379927505952292;
taps_8[677]=0.000144823346068166;
taps_8[678]=-8.97245898939239e-05;
taps_8[679]=-0.000317023267568216;
taps_8[680]=-0.000530680311856522;
taps_8[681]=-0.000724862331421346;
taps_8[682]=-0.00089436892498331;
taps_8[683]=-0.00103485205127872;
taps_8[684]=-0.00114284009857819;
taps_8[685]=-0.00121590364476507;
taps_8[686]=-0.00125262039807831;
taps_8[687]=-0.00125268097172094;
taps_8[688]=-0.00121678986150735;
taps_8[689]=-0.00114671145774895;
taps_8[690]=-0.00104511239573034;
taps_8[691]=-0.000915553316738735;
taps_8[692]=-0.000762282899397457;
taps_8[693]=-0.000590189163242959;
taps_8[694]=-0.000404559581170206;
taps_8[695]=-0.000211009396037845;
taps_8[696]=-1.52252331489664e-05;
taps_8[697]=0.000177109613141466;
taps_8[698]=0.000360568037868159;
taps_8[699]=0.000530039848959983;
taps_8[700]=0.000680962489080001;
taps_8[701]=0.00080934997400283;
taps_8[702]=0.000911987385643351;
taps_8[703]=0.00098641666492411;
taps_8[704]=0.00103108304299463;
taps_8[705]=0.00104527101623297;
taps_8[706]=0.00102919969715491;
taps_8[707]=0.000983906088391663;
taps_8[708]=0.000911291796250707;
taps_8[709]=0.000813957877061556;
taps_8[710]=0.000695210832369862;
taps_8[711]=0.000558859007898819;
taps_8[712]=0.000409187866775829;
taps_8[713]=0.000250732881833075;
taps_8[714]=8.82397238421628e-05;
taps_8[715]=-7.35721489985449e-05;
taps_8[716]=-0.000230048447758953;
taps_8[717]=-0.000376828171582901;
taps_8[718]=-0.000509867270418223;
taps_8[719]=-0.000625645047182207;
taps_8[720]=-0.000721157299267711;
taps_8[721]=-0.000794090785776732;
taps_8[722]=-0.000842776838468573;
taps_8[723]=-0.000866326435817647;
taps_8[724]=-0.000864539945168113;
taps_8[725]=-0.000837999088160925;
taps_8[726]=-0.000787935055630905;
taps_8[727]=-0.000716280238486885;
taps_8[728]=-0.000625497920313323;
taps_8[729]=-0.000518602103401226;
taps_8[730]=-0.000398958803315089;
taps_8[731]=-0.000270283359622999;
taps_8[732]=-0.000136424843102925;
taps_8[733]=-1.35597195405922e-06;
taps_8[734]=0.000131047922916085;
taps_8[735]=0.00025701333212609;
taps_8[736]=0.000373080573238073;
taps_8[737]=0.00047609090232215;
taps_8[738]=0.000563377120518508;
taps_8[739]=0.000632724915792309;
taps_8[740]=0.000682533101401086;
taps_8[741]=0.000711740595872378;
taps_8[742]=0.000719953592935128;
taps_8[743]=0.000707335516429796;
taps_8[744]=0.000674698903868529;
taps_8[745]=0.000623360531401895;
taps_8[746]=0.000555202965642309;
taps_8[747]=0.000472497996446331;
taps_8[748]=0.000377944758147115;
taps_8[749]=0.000274472050725655;
taps_8[750]=0.000165260820678015;
taps_8[751]=5.35363281003041e-05;
taps_8[752]=-5.74148600651388e-05;
taps_8[753]=-0.000164451304552992;
taps_8[754]=-0.000264552506266698;
taps_8[755]=-0.00035501610059456;
taps_8[756]=-0.000433415556537857;
taps_8[757]=-0.000497777873071625;
taps_8[758]=-0.000546517273524351;
taps_8[759]=-0.000578587494368962;
taps_8[760]=-0.000593384717258721;
taps_8[761]=-0.00059087231298634;
taps_8[762]=-0.000571454000047465;
taps_8[763]=-0.000536068801636699;
taps_8[764]=-0.000486037343304923;
taps_8[765]=-0.00042313109947345;
taps_8[766]=-0.000349396898388738;
taps_8[767]=-0.000267208170849344;
taps_8[768]=-0.000179072552729365;
taps_8[769]=-8.767511831997e-05;
taps_8[770]=4.32092321981464e-06;
taps_8[771]=9.42324802744343e-05;
taps_8[772]=0.000179552335674945;
taps_8[773]=0.000257898482996672;
taps_8[774]=0.000327199758585707;
taps_8[775]=0.000385629720810713;
taps_8[776]=0.000431772280662915;
taps_8[777]=0.000464535730003232;
taps_8[778]=0.000483295333669786;
taps_8[779]=0.000487783034209065;
taps_8[780]=0.000478206729681686;
taps_8[781]=0.000455113362865272;
taps_8[782]=0.000419487940729876;
taps_8[783]=0.000372593331651563;
taps_8[784]=0.000316049835340505;
taps_8[785]=0.000251659302297304;
taps_8[786]=0.00018147145939103;
taps_8[787]=0.000107596448628945;
taps_8[788]=3.22656750025608e-05;
taps_8[789]=-4.23570007132143e-05;
taps_8[790]=-0.00011411453991483;
taps_8[791]=-0.00018104075190589;
taps_8[792]=-0.000241289912514808;
taps_8[793]=-0.000293311101538506;
taps_8[794]=-0.000335761067681751;
taps_8[795]=-0.000367665001227307;
taps_8[796]=-0.000388309319721726;
taps_8[797]=-0.000397382986797224;
taps_8[798]=-0.000394851922018956;
taps_8[799]=-0.00038107950818989;
taps_8[800]=-0.000356680406485794;
taps_8[801]=-0.000322624799478507;
taps_8[802]=-0.000280075510861557;
taps_8[803]=-0.000230476784616508;
taps_8[804]=-0.000175378925422957;
taps_8[805]=-0.0001165201485712;
taps_8[806]=-5.56420029317831e-05;
taps_8[807]=5.42918960904014e-06;
taps_8[808]=6.496972346656e-05;
taps_8[809]=0.000121272555199706;
taps_8[810]=0.000172828714271459;
taps_8[811]=0.000218234913193012;
taps_8[812]=0.000256362285522925;
taps_8[813]=0.00028625083047742;
taps_8[814]=0.000307266422531577;
taps_8[815]=0.000318978249355968;
taps_8[816]=0.000321299230985803;
taps_8[817]=0.000314347366956299;
taps_8[818]=0.000298572369893355;
taps_8[819]=0.000274598721488349;
taps_8[820]=0.000243340888160399;
taps_8[821]=0.000205832360514755;
taps_8[822]=0.000163330817447082;
taps_8[823]=0.000117139770574355;
taps_8[824]=6.8705339092623e-05;
taps_8[825]=1.94368682287616e-05;
taps_8[826]=-2.91977438236069e-05;
taps_8[827]=-7.58529027689147e-05;
taps_8[828]=-0.000119202893215909;
taps_8[829]=-0.000158117344180923;
taps_8[830]=-0.000191551612044901;
taps_8[831]=-0.000218713551981449;
taps_8[832]=-0.000238940109111693;
taps_8[833]=-0.00025185411132203;
taps_8[834]=-0.000257227641630487;
taps_8[835]=-0.000255124704130997;
taps_8[836]=-0.000245753852306883;
taps_8[837]=-0.000229595649189852;
taps_8[838]=-0.00020724637450465;
taps_8[839]=-0.000179534104140222;
taps_8[840]=-0.000147350724085271;
taps_8[841]=-0.000111765247139405;
taps_8[842]=-7.38481794592127e-05;
taps_8[843]=-3.47799657918898e-05;
taps_8[844]=4.32534655904554e-06;
taps_8[845]=4.23097382147485e-05;
taps_8[846]=7.81471219204883e-05;
taps_8[847]=0.000110828712404391;
taps_8[848]=0.000139531279042549;
taps_8[849]=0.000163497985324821;
taps_8[850]=0.000182195822158416;
taps_8[851]=0.000195188458329326;
taps_8[852]=0.000202283729325368;
taps_8[853]=0.000203393087926507;
taps_8[854]=0.000198674605036415;
taps_8[855]=0.000188378166864465;
taps_8[856]=0.000172978178345116;
taps_8[857]=0.000153012914188822;
taps_8[858]=0.00012920928199096;
taps_8[859]=0.000102314148414001;
taps_8[860]=7.32148504001321e-05;
taps_8[861]=4.27686583101453e-05;
taps_8[862]=1.19187919042695e-05;
taps_8[863]=-1.84746536238986e-05;
taps_8[864]=-4.75169638195416e-05;
taps_8[865]=-7.44471468065165e-05;
taps_8[866]=-9.85122010058052e-05;
taps_8[867]=-0.00011913646890242;
taps_8[868]=-0.000135783736050255;
taps_8[869]=-0.000148119679922636;
taps_8[870]=-0.000155870608549917;
taps_8[871]=-0.00015897502428116;
taps_8[872]=-0.000157432992333869;
taps_8[873]=-0.000151451878882252;
taps_8[874]=-0.000141287918298562;
taps_8[875]=-0.000127383266844889;
taps_8[876]=-0.000110202340589578;
taps_8[877]=-9.0365421235607e-05;
taps_8[878]=-6.84766230954042e-05;
taps_8[879]=-4.52590060477748e-05;
taps_8[880]=-2.13752740917339e-05;
taps_8[881]=2.43231666674515e-06;
taps_8[882]=2.55211597474115e-05;
taps_8[883]=4.72127394342253e-05;
taps_8[884]=6.69624838747556e-05;
taps_8[885]=8.42211541515071e-05;
taps_8[886]=9.86032644392818e-05;
taps_8[887]=0.000109740663750113;
taps_8[888]=0.00011744476107651;
taps_8[889]=0.000121555042757421;
taps_8[890]=0.000122096575356086;
taps_8[891]=0.000119117395863559;
taps_8[892]=0.000112844827506834;
taps_8[893]=0.000103514009049596;
taps_8[894]=9.15218577429016e-05;
taps_8[895]=7.72517627161004e-05;
taps_8[896]=6.12164978440272e-05;
taps_8[897]=4.38903014506934e-05;
taps_8[898]=2.58462741782879e-05;
taps_8[899]=7.58371932386385e-06;
taps_8[900]=-1.0328774980152e-05;
taps_8[901]=-2.74268262135782e-05;
taps_8[902]=-4.3206750661085e-05;
taps_8[903]=-5.729574498826e-05;
taps_8[904]=-6.93015185003471e-05;
taps_8[905]=-7.8982884121409e-05;
taps_8[906]=-8.60941735625944e-05;
taps_8[907]=-9.05495769367747e-05;
taps_8[908]=-9.22656949125147e-05;
taps_8[909]=-9.13131584318904e-05;
taps_8[910]=-8.77664368817071e-05;
taps_8[911]=-8.18448015994237e-05;
taps_8[912]=-7.37533381958204e-05;
taps_8[913]=-6.38304469071307e-05;
taps_8[914]=-5.23797097138412e-05;
taps_8[915]=-3.98144771103122e-05;
taps_8[916]=-2.64916086632523e-05;
taps_8[917]=-1.28556438219561e-05;
taps_8[918]=7.31476844069375e-07;
taps_8[919]=1.3845146143102e-05;
taps_8[920]=2.61625193943218e-05;
taps_8[921]=3.73205710441116e-05;
taps_8[922]=4.70717816564067e-05;
taps_8[923]=5.514917082034e-05;
taps_8[924]=6.14126629665977e-05;
taps_8[925]=6.57040092489261e-05;
taps_8[926]=6.80055088543504e-05;
taps_8[927]=6.82784053777787e-05;
taps_8[928]=6.66200772604808e-05;
taps_8[929]=6.31055698540981e-05;
taps_8[930]=5.79368559112913e-05;
taps_8[931]=5.1279245294408e-05;
taps_8[932]=4.34110087077692e-05;
taps_8[933]=3.45622990628551e-05;
taps_8[934]=2.5056990636767e-05;
taps_8[935]=1.51543462296804e-05;
taps_8[936]=5.18679829143618e-06;
taps_8[937]=-4.59567287529646e-06;
taps_8[938]=-1.3877697071816e-05;
taps_8[939]=-2.24545959348004e-05;
taps_8[940]=-3.00624410899258e-05;
taps_8[941]=-3.65577007156457e-05;
taps_8[942]=-4.17553956469138e-05;
taps_8[943]=-4.55925163241106e-05;
taps_8[944]=-4.79663045313616e-05;
taps_8[945]=-4.8900840057722e-05;
taps_8[946]=-4.83826209900949e-05;
taps_8[947]=-4.65182331293739e-05;
taps_8[948]=-4.33696616849924e-05;
taps_8[949]=-3.91128920398273e-05;
taps_8[950]=-3.38682355555699e-05;
taps_8[951]=-2.7867954158958e-05;
taps_8[952]=-2.12620548793177e-05;
taps_8[953]=-1.43097534215284e-05;
taps_8[954]=-7.17834724909024e-06;
taps_8[955]=-1.17037264134303e-07;
taps_8[956]=6.71473953860341e-06;
taps_8[957]=1.30881149688615e-05;
taps_8[958]=1.88820742377107e-05;
taps_8[959]=2.39135836048846e-05;
taps_8[960]=2.8109965060684e-05;
taps_8[961]=3.13411648896224e-05;
taps_8[962]=3.35965664375177e-05;
taps_8[963]=3.480685217763e-05;
taps_8[964]=3.50278812403134e-05;
taps_8[965]=3.42424709615771e-05;
taps_8[966]=3.25639852375732e-05;
taps_8[967]=3.00342291050642e-05;
taps_8[968]=2.67955895093785e-05;
taps_8[969]=2.293955568233e-05;
taps_8[970]=1.86400351231313e-05;
taps_8[971]=1.39976988562167e-05;
taps_8[972]=9.1997758613628e-06;
taps_8[973]=4.35460134561526e-06;
taps_8[974]=-3.55598111528911e-07;
taps_8[975]=-4.84159257829812e-06;
taps_8[976]=-8.94271823758291e-06;
taps_8[977]=-1.25983326776471e-05;
taps_8[978]=-1.56764509207828e-05;
taps_8[979]=-1.8153420151707e-05;
taps_8[980]=-1.99492623976596e-05;
taps_8[981]=-2.10593643057415e-05;
taps_8[982]=-2.1468559025605e-05;
taps_8[983]=-2.12122161797997e-05;
taps_8[984]=-2.02899545008248e-05;
taps_8[985]=-1.8790401581735e-05;
taps_8[986]=-1.67508780891144e-05;
taps_8[987]=-1.42802860000988e-05;
taps_8[988]=-1.14320117172685e-05;
taps_8[989]=-8.33200562555188e-06;
taps_8[990]=-5.0488099964773e-06;
taps_8[991]=-1.71099058383343e-06;
taps_8[992]=1.61179663101019e-06;
taps_8[993]=4.81464937853888e-06;
taps_8[994]=7.81061368146095e-06;
taps_8[995]=1.0547471990519e-05;
taps_8[996]=1.29522377146441e-05;
taps_8[997]=1.49446580234431e-05;
taps_8[998]=1.65418756025426e-05;
taps_8[999]=1.76838523244811e-05;
taps_8[1000]=1.8385658232471e-05;
taps_8[1001]=1.8620373666481e-05;
taps_8[1002]=1.84341526874985e-05;
taps_8[1003]=1.78352625670506e-05;
taps_8[1004]=1.68791662966105e-05;
taps_8[1005]=1.55991108335413e-05;
taps_8[1006]=1.40434090184937e-05;
taps_8[1007]=1.22940783771573e-05;
taps_8[1008]=1.03524342421933e-05;
taps_8[1009]=8.32039368507842e-06;
taps_8[1010]=6.34709045098423e-06;
taps_8[1011]=4.26108402754801e-06;
taps_8[1012]=2.31694868332586e-06;
taps_8[1013]=4.65807896753724e-07;
taps_8[1014]=-1.1672977954277e-06;
taps_8[1015]=-2.64796014539528e-06;
taps_8[1016]=-3.87530545584888e-06;
taps_8[1017]=-4.90690921899934e-06;
taps_8[1018]=-5.65484231686277e-06;
taps_8[1019]=-6.18549614648338e-06;
taps_8[1020]=-6.43306093680702e-06;
taps_8[1021]=-6.48044369959903e-06;
taps_8[1022]=-6.27991234694974e-06;
taps_8[1023]=-4.10485902015429e-05;
value_7=0.0;
output_0 = 0.0;
{int i; for(i=0;i<1027;i++) output_1[i] = 0.0;}
output_3 = 0;
signalIn_4 = 0;
signalOut_2 = 0.0;
    if(!(fp_5[0] = fopen("CGCVIS_temp_60","w")))
    {
        fprintf(stderr,"ERROR: cannot open output file for Xgraph star.\n");
	exit(1);
    }
{ int sdfLoopCounter_11;for (sdfLoopCounter_11=0; sdfLoopCounter_11 < NUMTIMES; sdfLoopCounter_11++) {
	{  /* star firnovis.singen1.Ramp1 (class CGCRamp) */
	output_0 = value_7;
	value_7 += 0.628318530717958;
	}
	{  /* star firnovis.singen1.Sin1 (class CGCSin) */
	output_1[output_3] = sin(output_0);
	output_3 += 1;
	if (output_3 >= 1027)
		output_3 -= 1027;
	}
	{  /* star firnovis.FIR1 (class CGCFIR) */
	int i,fred,limit;
	double out, tap;
	/* phase keeps track of which phase of the filter coefficients is used.
	   Starting phase depends on the decimationPhase state. */
	start = gethrtime();
	out = 0.0;

	/* Compute the inner product. */
	/*
	fred = signalIn_4;
	for (i = 0; i <= signalIn_4; i++ ) {
	  tap = taps_8[i];
	  out += tap * output_1[fred];
	  fred --;
	}
	fred = 1026;
	for ( ; i < 1024; i++ ) {
	  tap = taps_8[i];
	  out += tap * output_1[fred];
	  fred --;
	}
	*/

	fred = signalIn_4;
	limit = 1024;
	if (signalIn_4 < 1024) {
	  limit = signalIn_4;
	}
	for (i = 0; i <= limit; i++ ) {
	  tap = taps_8[i];
	  out += tap * output_1[fred];
	  fred--;
	  /*
	  if ( fred < 0 ) {
	    break;
	  }
	  */
	}
	/*	i++; */
	fred = 1026;
	for ( ; i < 1024; i++ ) {
	  tap = taps_8[i];
	  out += tap * output_1[fred];
	  fred--;
	}

	/*
	fred = signalIn_4;
	for (i = 0; i < 1024; i++ ) {
	  tap = taps_8[i];
	  out += tap * output_1[fred];
	  fred--;
	  if ( fred < 0 ) {
	    fred += 1027;
	  }
	}
	*/
	/*
	for (i = 0; i < 1024; i++) {
	  tap = taps_8[i];
	  out += tap * output_1[(signalIn_4-(i)+1027)%1027];
	}
	*/

	signalOut_2 = out;
	
	end = gethrtime();
	time3 = (float) (end - start);
	runningtotal+=time3;
	signalIn_4 += 1;
	if (signalIn_4 >= 1027)
		signalIn_4 -= 1027;
	}
	
	{  /* star firnovis.XMgraph1 (class CGCXMgraph) */
	if (++count_9 >= 0) {
		fprintf(fp_5[0],"%g %g\n",index_10,signalOut_2);
	}
	index_10 += 1.0;
	
	}}}
  meansampletime = (float) runningtotal/(NUMTIMES)/1000;
  printf("mean sample time =%f microsec\n",meansampletime);
 /* end repeat, depth 0*/
    { int i;
    for (i = 0; i < 1; i++) fclose(fp_5[i]);
    system("( pxgraph -t 'X graph' -bb -tk =800x400 CGCVIS_temp_60 ; /bin/rm -f CGCVIS_temp_60) &");
    }

return 1;
}
