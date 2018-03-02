function lab_live_view



    clc
    tcp = tcpip('127.0.0.1', 1234);
    tcp.InputBufferSize = 2^15;
    fopen(tcp);
    finishup = onCleanup(@()fclose(tcp));
    
    
    vehicles = struct('id',{},'observation',{},'command',{});
    
    clf
    hold on
    plot(...
            1e-3*[0.000000,18.187247,36.374490,54.561717,72.748889,90.935928,109.122712,127.309062,145.494736,163.679396,181.862604,200.043818,218.222392,236.397539,254.568342,272.733721,290.892421,309.043009,327.183882,345.313227,363.429025,381.529034,399.610762,417.671473,435.708185,453.717651,471.696339,489.640438,507.545821,525.408052,543.222384,560.983747,578.686728,596.325572,613.894165,631.386025,648.794305,666.111792,683.330884,700.443593,717.441540,734.315947,751.057637,767.657039,784.104179,800.388675,816.499748,832.426222,848.156529,863.678705,878.980404,894.048896,908.871082,923.433518,937.722422,951.723668,965.422808,978.805107,991.855522,1004.558790,1016.899427,1028.861729,1040.429779,1049.914453,1057.831076,1065.520252,1072.978521,1080.202523,1087.189008,1093.934832,1100.436959,1106.692462,1112.698525,1118.452443,1123.951627,1129.193603,1134.176011,1138.896609,1143.353271,1147.543992,1151.466884,1155.120182,1158.502243,1161.611543,1164.446684,1167.006389,1169.289506,1171.295006,1173.021988,1174.469675,1175.637415,1176.524681,1177.131075,1177.456323,1177.500280,1177.262925,1176.744366,1175.944835,1174.864693,1173.504426,1171.864646,1169.946091,1167.749625,1165.276236,1162.527038,1159.503268,1156.206287,1152.637580,1148.798751,1144.691530,1140.317764,1135.679425,1130.778598,1125.617489,1120.198420,1114.523833,1108.596280,1102.418431,1095.993066,1089.323077,1082.411464,1075.261341,1067.875925,1060.258541,1052.412617,1043.961410,1032.519851,1020.679046,1008.454925,995.863284,982.919704,969.639514,956.037798,942.129391,927.928843,913.450377,898.707900,883.714964,868.484764,853.030141,837.363571,821.497134,805.442528,789.211052,772.813606,756.260695,739.562422,722.728487,705.768187,688.690420,671.503693,654.216119,636.835418,619.368929,601.823608,584.206041,566.522462,548.778743,530.980403,513.132619,495.240239,477.307782,459.339471,441.339225,423.310665,405.257123,387.181668,369.087097,350.975973,332.850625,314.713150,296.565416,278.409101,260.245673,242.076435,223.902529,205.724933,187.544468,169.361830,151.177580,132.992180,114.806000,96.619327,78.432355,60.245217,42.058002,23.870759,5.683513,-7.618194,-18.699202,-29.780211,-40.861219,-51.942227,-63.023235,-74.104243,-85.185251,-96.266259,-107.347267,-118.428276,-129.509284,-140.590292,-151.671300,-162.752308,-173.833316,-184.914324,-195.995332,-207.076340,-218.157349,-229.238357,-240.319365,-251.400373,-262.481381,-273.562389,-284.643397,-295.724405,-306.805414,-317.886422,-328.967430,-340.048438,-351.129446,-362.210454,-373.291462,-384.372470,-395.453479,-406.534487,-417.615495,-428.696503,-439.777511,-450.858519,-461.939527,-473.020535,-484.101544,-495.182552,-506.263560,-517.344568,-528.425576,-539.506584,-550.587592,-561.668600,-572.749609,-583.830617,-594.911625,-605.992633,-617.073641,-628.154649,-639.235657,-650.316665,-661.397673,-672.478682,-683.559690,-696.417258,-714.604505,-732.791745,-750.978955,-769.166102,-787.353100,-805.539806,-823.726015,-841.911461,-860.095788,-878.278547,-896.459179,-914.636988,-932.811147,-950.980694,-969.144514,-987.301317,-1005.449644,-1023.587826,-1041.713990,-1059.826055,-1077.921725,-1095.998453,-1114.053459,-1132.083688,-1150.085810,-1168.056219,-1185.991027,-1203.886039,-1221.736753,-1239.538342,-1257.285639,-1274.973139,-1292.595002,-1310.145024,-1327.616643,-1345.002923,-1362.296544,-1379.489806,-1396.574623,-1413.542521,-1430.384623,-1447.091655,-1463.653942,-1480.061409,-1496.303575,-1512.369566,-1528.248104,-1543.927512,-1559.395732,-1574.640328,-1589.648488,-1604.407022,-1618.902400,-1633.120725,-1647.047790,-1660.669093,-1673.969836,-1686.934924,-1699.549017,-1711.796552,-1723.661774,-1735.128758,-1743.785184,-1751.645276,-1759.277046,-1766.677059,-1773.841982,-1780.768592,-1787.453773,-1793.894514,-1800.087916,-1806.031192,-1811.721665,-1817.156775,-1822.334075,-1827.251237,-1831.906045,-1836.296405,-1840.420341,-1844.275995,-1847.861633,-1851.175642,-1854.216529,-1856.982926,-1859.473587,-1861.687390,-1863.623341,-1865.280568,-1866.658324,-1867.755989,-1868.573070,-1869.109198,-1869.364132,-1869.337758,-1869.030087,-1868.441258,-1867.571536,-1866.421312,-1864.991105,-1863.281557,-1861.293438,-1859.027644,-1856.485193,-1853.667231,-1850.575026,-1847.209970,-1843.573577,-1839.667483,-1835.493448,-1831.053350,-1826.349188,-1821.383079,-1816.157258,-1810.674077,-1804.936004,-1798.945622,-1792.705629,-1786.218833,-1779.488152,-1772.516616,-1765.307363,-1757.863640,-1750.188795,-1742.286284,-1733.009772,-1721.466900,-1709.528778,-1697.211303,-1684.530226,-1671.501066,-1658.139090,-1644.459320,-1630.476529,-1616.205178,-1601.659419,-1586.853071,-1571.799596,-1556.512100,-1541.003336,-1525.285682,-1509.371121,-1493.271254,-1476.997279,-1460.559998,-1443.969814,-1427.236733,-1410.370351,-1393.379865,-1376.274076,-1359.061392,-1341.749828,-1324.347006,-1306.860172,-1289.296185,-1271.661546,-1253.962394,-1236.204517,-1218.393342,-1200.533969,-1182.631158,-1164.689353,-1146.712702,-1128.705050,-1110.669943,-1092.610646,-1074.530159,-1056.431219,-1038.316329,-1020.187763,-1002.047556,-983.897527,-965.739303,-947.574307,-929.403801,-911.228887,-893.050503,-874.869434,-856.686345,-838.501768,-820.316141,-802.129815,-783.943052,-765.756028,-747.568864,-729.381639,-711.194394,-693.007148,-681.482000,-670.400991,-659.319983,-648.238975,-637.157967,-626.076959,-614.995951,-603.914943,-592.833935,-581.752926,-570.671918,-559.590910,-548.509902,-537.428894,-526.347886,-515.266878,-504.185870,-493.104862,-482.023853,-470.942845,-459.861837,-448.780829,-437.699821,-426.618813,-415.537805,-404.456797,-393.375788,-382.294780,-371.213772,-360.132764,-349.051756,-337.970748,-326.889740,-315.808732,-304.727723,-293.646715,-282.565707,-271.484699,-260.403691,-249.322683,-238.241675,-227.160667,-216.079658,-204.998650,-193.917642,-182.836634,-171.755626,-160.674618,-149.593610,-138.512602,-127.431593,-116.350585,-105.269577,-94.188569,-83.107561,-72.026553,-60.945545,-49.864537,-38.783529,-27.702520,-16.621512,-5.540504,9.093624,27.280870,45.468107,63.655314,81.842431,100.029363,118.215954,136.402002,154.587218,172.771213,190.953498,209.133480,227.310447,245.483547,263.651784,281.813991,299.968825,318.114767,336.250117,354.372954,372.481153,390.572348,408.643925,426.693025,444.716539,462.711073,480.672963,498.598239,516.482620,534.321515,552.110018,569.842887,587.514540,605.119046,622.650106,640.101055,657.464864,674.734127,691.901051,708.957451,725.894750,742.703970,759.375738,775.900281,792.267422,808.466579,824.486773,840.316635,855.944396,871.357899,886.544611,901.491610,916.185622,930.613036,944.759898,958.611913,972.154497,985.372749,998.251512,1010.775414,1022.928860,1034.696037,1046.220676,1059.609380,1072.349478,1084.424683,1095.819532,1106.519447,1116.510752,1125.780681,1134.317373,1142.109898,1149.148290,1155.423542,1160.927630,1165.653524,1169.595179,1172.747544,1175.106590,1176.669299,1177.433672,1177.398734,1176.564528,1174.932120,1172.503596,1169.282070,1165.271662,1160.477497,1154.905701,1148.563407,1141.458722,1133.600745,1124.999533,1115.666075,1105.612299,1094.851068,1083.396152,1071.262206,1058.464766,1045.020189,1030.945653,1016.259158,1000.979505,985.126234,968.719646,951.780719,934.331093,916.393079,897.989643,879.144315,859.881230,840.225023,820.200811,799.834188,779.151224,758.178370,736.942479,715.470719,693.790531,671.929619,649.915955,627.777724,605.543234,583.240924,560.899308,541.150495,522.973425,504.820344,486.705081,468.640722,450.639630,432.713451,414.873104,397.128795,379.490042,361.965655,344.563788,327.291944,310.156974,293.165080,276.321860,259.632295,243.100790,226.731200,210.526828,194.490424,178.624221,162.929958,147.408882,132.061797,116.889066,101.890603,87.065902,72.414070,57.933823,43.623530,29.481220,15.504582,1.690970,-11.962544,-25.459195,-38.802473,-51.996120,-65.044123,-77.950721,-90.720360,-103.357701,-115.867594,-128.255060,-140.525294,-152.683661,-164.735676,-176.686992,-188.543394,-200.310774,-211.995134,-223.602585,-235.139334,-246.611667,-258.025949,-269.388605,-280.706113,-291.985008,-303.231876,-314.453332,-325.656019,-336.846600,-345.934643,-345.934662,-345.934681,-345.934699,-345.934718,-345.934737,-345.934755,-345.934774,-345.934793,-345.934811,-345.934830,-345.934848,-345.934867,-345.934886,-345.934904,-345.934923,-345.934942,-345.934960,-345.934979,-345.934997,-345.935016,-345.935035,-345.935053,-345.935072,-345.935091,-345.935109,-345.935128,-345.935147,-345.935165,-345.935184,-345.935202,-345.935221,-345.935240,-345.935258,-345.935277,-345.935296,-345.935314,-345.935333,-345.935352,-345.935370,-345.935389,-345.935407,-345.935426,-345.935445,-345.935463,-345.935482,-345.935501,-345.935519,-345.935538,-345.935556,-345.935575,-345.935594,-345.935612,-345.935631,-345.935650,-345.935668,-345.935687,-345.935706,-345.935724,-345.935743,-345.935761,-345.935780,-345.935799,-354.324682,-365.514727,-376.716459,-387.936545,-399.181635,-410.458344,-421.773246,-433.132880,-444.543741,-456.012257,-467.544801,-479.147661,-490.827038,-502.589043,-514.439698,-526.384907,-538.430466,-550.582036,-562.845126,-575.225103,-587.727184,-600.356408,-613.117644,-626.015563,-639.054614,-652.239033,-665.572830,-679.059781,-692.703396,-706.506925,-720.473311,-734.605194,-748.904908,-763.374470,-778.015534,-792.829412,-807.817016,-822.978854,-838.315029,-853.825235,-869.508700,-885.364211,-901.390058,-917.584019,-933.943360,-950.464834,-967.144635,-983.978398,-1000.961173,-1018.087391,-1035.350868,-1052.744804,-1070.261757,-1087.893618,-1105.631610,-1123.466257,-1141.387385,-1159.384117,-1177.444873,-1195.557333,-1213.708450,-1231.884440,-1254.214245,-1285.632029,-1316.945440,-1348.075290,-1378.942561,-1409.469128,-1439.577882,-1469.192729,-1498.238593,-1526.641947,-1554.330799,-1581.235009,-1607.286602,-1632.419760,-1656.570827,-1679.678580,-1701.684530,-1722.532889,-1742.170959,-1760.549138,-1777.620918,-1793.342981,-1807.675562,-1820.582318,-1832.030607,-1841.991515,-1850.439852,-1857.354173,-1862.716979,-1866.514691,-1868.737701,-1869.380392,-1868.441141,-1865.922320,-1861.830275,-1856.175414,-1848.972065,-1840.238426,-1829.996564,-1818.272416,-1805.095660,-1790.499686,-1774.521508,-1757.201509,-1738.583440,-1718.714417,-1697.644803,-1675.427911,-1652.120114,-1627.780355,-1602.470109,-1576.253384,-1549.196664,-1521.368377,-1492.839142,-1463.681153,-1433.968060,-1403.774965,-1373.178419,-1342.255929,-1311.085764,-1279.746776,-1248.318248,-1228.475211,-1210.302801,-1192.157908,-1174.054172,-1156.004499,-1138.021073,-1120.115355,-1102.298089,-1084.579305,-1066.968343,-1049.473847,-1032.103808,-1014.865565,-997.765808,-980.810580,-964.005331,-947.354889,-930.863524,-914.534950,-898.372334,-882.378288,-866.554929,-850.903862,-835.426226,-820.122713,-804.993574,-790.038614,-775.257230,-760.648429,-746.210837,-731.942737,-717.842075,-703.906454,-690.133153,-676.519168,-663.061198,-649.755688,-636.598836,-623.586591,-610.714657,-597.978537,-585.373520,-572.894711,-560.537046,-548.295289,-536.164031,-524.137727,-512.210687,-500.377097,-488.631039,-476.966483,-465.377293,-453.857240,-442.400019,-430.999248,-419.648489,-408.341249,-397.070980,-385.831086,-374.614946,-363.415910,-352.227314,-345.935801,-345.935782,-345.935764,-345.935745,-345.935726,-345.935708,-345.935689,-345.935670,-345.935652,-345.935633,-345.935615,-345.935596,-345.935577,-345.935559,-345.935540,-345.935521,-345.935503,-345.935484,-345.935466,-345.935447,-345.935428,-345.935410,-345.935391,-345.935372,-345.935354,-345.935335,-345.935316,-345.935298,-345.935279,-345.935261,-345.935242,-345.935223,-345.935205,-345.935186,-345.935167,-345.935149,-345.935130,-345.935111,-345.935093,-345.935074,-345.935056,-345.935037,-345.935018,-345.935000,-345.934981,-345.934962,-345.934944,-345.934925,-345.934907,-345.934888,-345.934869,-345.934851,-345.934832,-345.934813,-345.934795,-345.934776,-345.934757,-345.934739,-345.934720,-345.934702,-345.934683,-345.934664,-345.934646,-334.748986,-323.556644,-312.350950,-301.125235,-289.872858,-278.587216,-267.261742,-255.889908,-244.465234,-232.981306,-221.431769,-209.810357,-198.110893,-186.327287,-174.453542,-162.483784,-150.412247,-138.233308,-125.941493,-113.531474,-100.998072,-88.336294,-75.541317,-62.608524,-49.533519,-36.312123,-22.940379,-9.414583,4.268717,18.112696,32.120221,46.293855,60.635856,75.148151,89.832310,104.689550,119.720681,134.926110,150.305838,165.859448,181.586057,197.484332,213.552437,229.788019,246.188218,262.749653,279.468375,296.339880,313.359062,330.520199,347.816952,365.242367,382.788832,400.448075,418.211147,436.068395,454.009471,472.023323,490.098188,508.221565,526.380222,544.560190,559.975541,573.232050,586.479235,599.711135,612.921790,626.105255,639.255597,652.366900,665.433259,678.448795,691.407645,704.303977,717.131988,729.885907,742.559991,755.148534,767.645870,780.046373,792.344461,804.534603,816.611310,828.569145,840.402726,852.106725,863.675874,875.104969,886.388865,897.522482,908.500809,919.318901,929.971891,940.454986,950.763467,960.892694,970.838106,980.595227,990.159664,999.527113,1008.693361,1017.654278,1026.405833,1034.944085,1043.265190,1051.365404,1059.241082,1066.888679,1074.304752,1081.485963,1088.429078,1095.130973,1101.588633,1107.799150,1113.759729,1119.467687,1124.920454,1130.115576,1135.050714,1139.723649,1144.132276,1148.274611,1152.148789,1155.753066,],...
            1e-3*[0.000000,0.001413,0.011302,0.038144,0.090414,0.176590,0.305147,0.484559,0.723303,1.029850,1.412669,1.880228,2.440990,3.103408,3.875934,4.767006,5.785047,6.938464,8.235649,9.684970,11.294772,13.073366,15.029021,17.169962,19.504372,22.040378,24.786044,27.749367,30.938253,34.360512,38.023863,41.935923,46.104174,50.535982,55.238543,60.218886,65.483868,71.040170,76.894248,83.052356,89.520483,96.304342,103.409366,110.840709,118.603186,126.701288,135.139125,143.920391,153.048364,162.525905,172.355413,182.538789,193.077432,203.972158,215.223207,226.830238,238.792305,251.107780,263.774383,276.789086,290.148109,303.846917,317.880216,330.032338,340.669605,351.472426,362.435939,373.555208,384.825227,396.240927,407.797169,419.488750,431.310409,443.256822,455.322612,467.502350,479.790555,492.181695,504.670193,517.250425,529.916728,542.663403,555.484715,568.374891,581.328129,594.338598,607.400439,620.507776,633.654711,646.835325,660.043685,673.273846,686.519851,699.775737,713.035541,726.293296,739.543031,752.778784,765.994594,779.184514,792.342609,805.462957,818.539651,831.566805,844.538554,857.449059,870.292510,883.063129,895.755165,908.362906,920.880676,933.302839,945.623805,957.838031,969.940018,981.924318,993.785537,1005.518334,1017.117429,1028.577602,1039.893697,1051.060618,1062.073338,1072.926900,1083.616420,1094.621557,1108.758170,1122.562124,1136.027833,1149.150555,1161.926303,1174.351811,1186.424533,1198.142647,1209.504993,1220.511046,1231.160900,1241.455196,1251.395123,1260.982418,1270.219341,1279.108597,1287.653368,1295.857230,1303.724145,1311.258466,1318.464925,1325.348559,1331.914740,1338.169105,1344.117551,1349.766226,1355.121538,1360.190092,1364.978701,1369.494356,1373.744198,1377.735523,1381.475783,1384.972555,1388.233526,1391.266491,1394.079328,1396.679995,1399.076533,1401.277055,1403.289725,1405.122762,1406.784423,1408.283004,1409.626834,1410.824278,1411.883718,1412.813562,1413.622229,1414.318151,1414.909772,1415.405548,1415.813939,1416.143411,1416.402433,1416.599477,1416.743016,1416.841525,1416.903480,1416.937358,1416.951634,1416.954785,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954828,1416.954806,1416.952069,1416.938737,1416.906333,1416.846380,1416.750403,1416.609927,1416.416476,1416.161576,1415.836757,1415.433546,1414.943478,1414.358093,1413.668938,1412.867568,1411.945547,1410.894452,1409.705876,1408.371435,1406.882772,1405.231552,1403.409473,1401.408273,1399.219727,1396.835668,1394.247993,1391.448660,1388.429693,1385.183207,1381.701393,1377.976560,1374.001137,1369.767678,1365.268863,1360.497528,1355.446667,1350.109457,1344.479289,1338.549771,1332.314723,1325.768212,1318.904576,1311.718427,1304.204713,1296.358718,1288.176065,1279.652728,1270.785102,1261.569976,1252.004617,1242.086778,1231.814700,1221.187117,1210.203345,1198.863243,1187.167305,1175.116680,1162.713178,1149.959261,1136.858117,1123.413695,1109.630721,1095.514723,1084.278958,1073.599851,1062.756397,1051.753480,1040.596052,1029.289134,1017.837815,1006.247247,994.522651,982.669302,970.692539,958.597751,946.390381,934.075921,921.659917,909.147957,896.545675,883.858743,871.092870,858.253800,845.347313,832.379219,819.355357,806.281590,793.163800,780.007890,766.819782,753.605414,740.370734,727.121700,713.864275,700.604423,687.348114,674.101316,660.869990,647.660095,634.477575,621.328363,608.218375,595.153515,582.139663,569.182679,556.288394,543.462610,530.711100,518.039603,505.453824,492.959429,480.562043,468.267244,456.080565,444.007491,432.053457,420.223845,408.523981,396.959130,385.534495,374.255218,363.126379,352.152987,341.339980,330.692225,318.768267,304.714252,290.994367,277.613992,264.577668,251.889202,239.551680,227.567467,215.938208,204.664914,193.747941,183.187058,172.981487,163.129902,153.630429,144.480697,135.677876,127.218683,119.099445,111.316102,103.864203,96.738935,89.935179,83.447490,77.270168,71.397251,65.822527,60.539531,55.541610,50.821900,46.373375,42.188852,38.260997,34.582323,31.145228,27.941992,24.964801,22.205756,19.656878,17.310107,15.157315,13.190326,11.400908,9.780796,8.321685,7.015237,5.853081,4.826828,3.928068,3.148378,2.479324,1.912457,1.439321,1.051454,0.740388,0.497655,0.314782,0.183295,0.094719,0.040578,0.012395,0.001695,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000177,0.004768,0.022074,0.060570,0.128734,0.235041,0.387966,0.595985,0.867571,1.211196,1.635328,2.148430,2.758963,3.475380,4.306125,5.259629,6.344308,7.568561,8.940771,10.469290,12.162452,14.028545,16.075818,18.312474,20.746671,23.386501,26.240002,29.315124,32.619726,36.161572,39.948335,43.987561,48.286680,52.852975,57.693559,62.815375,68.225195,73.929591,79.934917,86.247303,92.872600,99.816379,107.083936,114.680268,122.610018,130.877499,139.486615,148.440850,157.743276,167.396535,177.402767,187.763637,198.480256,209.553150,220.982269,232.766980,244.905992,257.397362,270.238441,283.425823,296.955352,310.822117,325.226084,343.125231,361.491691,380.301988,399.532030,419.157211,439.152451,459.492199,480.150431,501.100710,522.316235,543.769844,565.434116,587.281371,609.283667,631.412836,653.640580,675.938435,698.277893,720.630411,742.967413,765.260298,787.480562,809.599750,831.589574,853.421935,875.068930,896.502848,917.696273,938.622078,959.253486,979.564133,999.528065,1019.119736,1038.314072,1057.086520,1075.413048,1093.270232,1110.635256,1127.485911,1143.800620,1159.558522,1174.739440,1189.323965,1203.293464,1216.630076,1229.316722,1241.337182,1252.676064,1263.318867,1273.251993,1282.462745,1290.939331,1298.670908,1305.647580,1311.860423,1317.301496,1321.963847,1325.841509,1328.929515,1331.223919,1332.721786,1333.421201,1333.374395,1332.784839,1331.680397,1330.070403,1327.964560,1325.372827,1322.305406,1318.772751,1314.785550,1310.354616,1305.490926,1300.205532,1294.509542,1288.414123,1281.930493,1275.069859,1267.843414,1260.262304,1252.337600,1244.080299,1235.501322,1226.611486,1217.421480,1207.941866,1198.183052,1188.155297,1177.868702,1167.333212,1156.558586,1145.554410,1134.330083,1122.894818,1111.257641,1099.427391,1087.412714,1075.222063,1062.863704,1050.345716,1037.675992,1024.862238,1011.911986,998.832581,985.631204,972.314873,958.890445,945.364614,931.743929,918.034790,904.243462,890.376092,876.438702,862.437200,848.377379,834.264944,820.105500,805.904583,791.667664,777.400144,763.107358,748.794609,734.467161,720.130254,708.478155,708.478131,708.478107,708.478083,708.478059,708.478035,708.478011,708.477988,708.477964,708.477940,708.477916,708.477892,708.477868,708.477844,708.477820,708.477796,708.477773,708.477749,708.477725,708.477701,708.477677,708.477653,708.477629,708.477605,708.477581,708.477558,708.477534,708.477510,708.477486,708.477462,708.477438,708.477414,708.477390,708.477366,708.477342,708.477319,708.477295,708.477271,708.477247,708.477223,708.477199,708.477175,708.477151,708.477127,708.477104,708.477080,708.477056,708.477032,708.477008,708.476984,708.476960,708.476936,708.476912,708.476889,708.476865,708.476841,708.476817,708.476793,708.476769,708.476745,708.476721,708.476697,708.476673,697.720823,683.383500,669.055312,654.741493,640.447299,626.178033,611.939044,597.735729,583.573538,569.458002,555.394721,541.389395,527.447824,513.575910,499.779658,486.065202,472.438798,458.906842,445.475877,432.152595,418.943836,405.856602,392.898061,380.075554,367.396601,354.868901,342.500334,330.298961,318.273039,306.431011,294.781512,283.333364,272.095583,261.077372,250.288118,239.737401,229.434969,219.390744,209.614815,200.117439,190.909019,182.000115,173.401411,165.123696,157.177871,149.574946,142.325996,135.442178,128.934680,122.814691,117.093397,111.781984,106.891595,102.433295,98.418068,94.856727,91.759917,89.138109,87.001584,85.360327,84.224029,83.602028,83.554672,84.839797,87.703445,92.138384,98.133335,105.673124,114.738707,125.307170,137.351731,150.841932,165.743544,182.018813,199.626620,218.522478,238.658535,259.983786,282.444221,305.982884,330.540278,356.054374,382.460608,409.692028,437.679743,466.352737,495.638499,525.463061,555.751000,586.425507,617.408995,648.622853,679.988086,711.425469,742.855543,774.198630,805.375406,836.306826,866.914547,897.121235,926.850575,956.027263,984.577356,1012.428573,1039.510307,1065.754108,1091.093689,1115.464918,1138.805976,1161.057819,1182.164011,1202.071178,1220.729040,1238.090412,1254.111243,1268.751005,1281.972579,1293.742508,1304.031062,1312.812237,1320.063760,1325.767247,1329.908279,1332.476379,1333.465050,1333.275769,1332.556640,1331.324916,1329.590040,1327.361782,1324.650160,1321.465441,1317.818140,1313.718987,1309.178831,1304.208677,1298.819590,1293.022694,1286.829168,1280.250239,1273.297103,1265.980949,1258.312906,1250.304025,1241.965285,1233.307585,1224.341708,1215.078315,1205.527931,1195.700927,1185.607520,1175.257777,1164.661594,1153.828689,1142.768601,1131.490684,1120.004102,1108.317835,1096.440676,1084.381220,1072.147874,1059.748858,1047.192203,1034.485756,1021.637179,1008.653956,995.543392,982.312625,968.968633,955.518233,941.968080,928.324687,914.594418,900.783509,886.898073,872.944106,858.927484,844.853980,830.729270,816.558941,802.348514,788.103441,773.829106,759.530834,745.213917,730.883613,716.545159,708.478146,708.478122,708.478098,708.478074,708.478050,708.478027,708.478003,708.477979,708.477955,708.477931,708.477907,708.477883,708.477859,708.477835,708.477812,708.477788,708.477764,708.477740,708.477716,708.477692,708.477668,708.477644,708.477620,708.477596,708.477573,708.477549,708.477525,708.477501,708.477477,708.477453,708.477429,708.477405,708.477381,708.477358,708.477334,708.477310,708.477286,708.477262,708.477238,708.477214,708.477190,708.477166,708.477143,708.477119,708.477095,708.477071,708.477047,708.477023,708.476999,708.476975,708.476951,708.476927,708.476904,708.476880,708.476856,708.476832,708.476808,708.476784,708.476760,708.476736,708.476712,708.476689,708.476665,694.135919,679.800389,665.475303,651.165896,636.877435,622.615234,608.384656,594.191108,580.040060,565.937059,551.887729,537.897793,523.973079,510.119513,496.343131,482.650096,469.046699,455.539371,442.134694,428.839396,415.660354,402.604615,389.679388,376.892057,364.250189,351.761526,339.433994,327.275703,315.294955,303.500242,291.900246,280.503838,269.320080,258.358220,247.627695,237.138129,226.899312,216.921208,207.213949,197.787831,188.653295,179.820936,171.301463,163.105694,155.244557,147.729086,140.570371,133.779583,127.367914,121.346550,115.726678,110.519482,105.736077,101.387522,97.484759,94.038560,91.059525,88.558084,86.544451,85.028539,84.019954,83.527943,83.521917,83.820799,84.400845,85.261795,86.403260,87.824727,89.525556,91.504983,93.762115,96.295937,99.105307,102.188962,105.545514,109.173453,113.071144,117.236834,121.668647,126.364589,131.322545,136.540285,142.015461,147.745607,153.728144,159.960378,166.439505,173.162610,180.126667,187.328540,194.764987,202.432660,210.328109,218.447780,226.788020,235.345074,244.115089,253.094118,262.278118,271.662958,281.244414,291.018172,300.979833,311.124912,321.448842,331.946978,342.614595,353.446892,364.438992,375.585946,386.882737,398.324280,409.905428,421.620968,433.465624,445.434067,457.520906,469.720702,482.027966,494.437159,506.942693,519.538940,532.220229,544.980854])
    
    
    h_scatter = scatter(0,0);
    axis equal
%     xlim([-1 1]*6)
%     ylim([-1 1]*6)
    
    
    while true
        messages = read_json_tcp(tcp);
        for message = messages
            % sort new messages
            if startsWith(message.topic, '/vehicle')
                id = message.topic(9:10);
                index = find(strcmp({vehicles.id},id));
                if isempty(index)
                    vehicles(end+1).id = id;
                    index = length(vehicles);
                end
                
                if endsWith(message.topic, 'command')
                    vehicles(index).command = message;
                elseif endsWith(message.topic, 'observation')
                    vehicles(index).observation = message;
                end
            end
        end
        
        % update visualization
        if ~isempty(messages) && ~isempty(vehicles)
            try
                x = arrayfun(@(i)vehicles(i).observation.message.pose.position.x,1:length(vehicles));
                y = arrayfun(@(i)vehicles(i).observation.message.pose.position.y,1:length(vehicles));
                set(h_scatter, 'XData', x);
                set(h_scatter, 'YData', y);
            end
            
        end
        
        drawnow
        pause(0.01);
    end
    
end