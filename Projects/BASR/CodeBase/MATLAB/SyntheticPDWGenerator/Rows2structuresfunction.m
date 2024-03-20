function Rows2structuresfunction( )
h = figure('units','pixels','position',[500 500 200 50],'windowstyle','modal');
uicontrol('style','text','string','Initializing','units','pixels','position',[75 10 50 30]);
emittersInfo = xls2struct('Emitter/InitializationStructuresFile_Small.xlsx');
waitfor(emittersInfo);
delete(h);
[filename, pathname] = uigetfile({ '*.xlsx'}, 'Pick an excel file');

fullname = fullfile(pathname, filename);
% emittersInfocolumns= importdata(fullname);
[numData,textData,rawData] = xlsread(filename);
% totalEmitters = numData(15,1);
% temp=numData;
% numData = nan(390,totalEmitters);
% numData(1:size(temp,1),1:size(temp,2))=temp;
totalEmitters = numData(15,1);
temp=numData;
numData= nan(390,totalEmitters);
numData(1:size(temp,1),1:size(temp,2))=temp;
EmittersArray = repmat(emittersInfo,1,totalEmitters);
filename=filename(1:end-5);
if (fullname(end-9:end-5)=="_info"|| fullname(end-9:end-5)=="_Info" || fullname(end-9:end-5)=="_INFO")
	filename=filename(1:end-5);
end
for i = 1:totalEmitters 
    % % Emitter Common Parameters
    EmittersArray(i).Type_Name = char(textData(2,i+1));
    EmittersArray(i).Sr_No  = numData(1,i);
    EmittersArray(i).RF = numData(2,i);
    %EmittersArray(i).Power = numData(3,i);
    EmittersArray(i).Power = 130;
    EmittersArray(i).PRI = numData(4,i);
    EmittersArray(i).PW = numData(5,i);
    EmittersArray(i).Time= numData(6,i);
    EmittersArray(i).f= numData(7,i);
    EmittersArray(i).AOA= numData(8,i);
    EmittersArray(i).pw= numData(9,i);
    EmittersArray(i).t= numData(10,i);
    EmittersArray(i).EmitterBlankPulses = numData(11,i);
    EmittersArray(i).CompositeBlankFlag = numData(12,i);
    EmittersArray(i).NoiseFlag = numData(13,i);
    EmittersArray(i).BandFilteringFlag = numData(14,i);
    EmittersArray(i).TotalEmitters = numData(15,i);
    EmittersArray(i).EmitterBlankingPresent = numData(16,i);

    % % Slide PRI DATA 
     
    EmittersArray(i).PRI_Start = numData(17,i);
    EmittersArray(i).PRI_Stop = numData(18,i);
    EmittersArray(i).PRI_Step = numData(19,i);
    EmittersArray(i).Cycle_Sin=0;
    if(numData(20,i)==1)
      EmittersArray(i).Modulation= 'Linear';
    elseif(numData(20,i)==2)
      EmittersArray(i).Modulation= 'sin';
      EmittersArray(i).Cycle_Sin=numData(363,i);
    elseif(numData(20,i)==3)
      EmittersArray(i).Modulation= 'negprislide';
     elseif(numData(20,i)==4)
      EmittersArray(i).Modulation= 'tri';
    else
      EmittersArray(i).Modulation='';
    end
    
    % % Stagger PRI Data
    
    EmittersArray(i).StaggerN = numData(21,i);
    EmittersArray(i).Stag_PRI1 = numData(22,i);
    EmittersArray(i).Stag_PRI2 = numData(23,i);
    EmittersArray(i).Stag_PRI3 = numData(24,i);
    EmittersArray(i).Stag_PRI4 = numData(25,i);
    EmittersArray(i).Stag_PRI5 = numData(26,i);
    EmittersArray(i).Stag_PRI6 = numData(27,i);
    EmittersArray(i).Stag_PRI7 = numData(28,i);
    EmittersArray(i).Stag_PRI8 = numData(29,i);
    EmittersArray(i).Stag_PRI9 = numData(30,i);
    EmittersArray(i).Stag_PRI10 = numData(31,i);
    EmittersArray(i).Stag_PRI11 = numData(32,i);
    EmittersArray(i).Stag_PRI12 = numData(33,i);
    EmittersArray(i).Stag_PRI13 = numData(34,i);
    EmittersArray(i).Stag_PRI14 = numData(35,i);
    EmittersArray(i).Stag_PRI15 = numData(36,i);
    EmittersArray(i).Stag_PRI16 = numData(37,i);
    EmittersArray(i).Stag_PRI17 = numData(38,i);
    EmittersArray(i).Stag_PRI18 = numData(39,i);
    EmittersArray(i).Stag_PRI19 = numData(40,i);
    EmittersArray(i).Stag_PRI20 = numData(41,i);
    EmittersArray(i).Stag_PRI21 = numData(42,i);
    EmittersArray(i).Stag_PRI22 = numData(43,i);
    EmittersArray(i).Stag_PRI23 = numData(44,i);
    EmittersArray(i).Stag_PRI24 = numData(45,i);
    EmittersArray(i).Stag_PRI25 = numData(46,i);
    EmittersArray(i).Stag_PRI26 = numData(47,i);
    EmittersArray(i).Stag_PRI27 = numData(48,i);
    EmittersArray(i).Stag_PRI28 = numData(49,i);
    EmittersArray(i).Stag_PRI29 = numData(50,i);
    EmittersArray(i).Stag_PRI30 = numData(51,i);
    EmittersArray(i).Stag_PRI31 = numData(52,i);
    EmittersArray(i).Stag_PRI32 = numData(53,i);

    EmittersArray(i).staggerDeltat1 = numData(54,i);
    EmittersArray(i).staggerDeltat2 = numData(55,i);
    EmittersArray(i).staggerDeltat3 = numData(56,i);
    EmittersArray(i).staggerDeltat4 = numData(57,i);
    EmittersArray(i).staggerDeltat5 = numData(58,i);
    EmittersArray(i).staggerDeltat6 = numData(59,i);
    EmittersArray(i).staggerDeltat7 = numData(60,i);
    EmittersArray(i).staggerDeltat8 = numData(61,i);
    EmittersArray(i).staggerDeltat9 = numData(62,i);
    EmittersArray(i).staggerDeltat10 = numData(63,i);
    EmittersArray(i).staggerDeltat11 = numData(64,i);
    EmittersArray(i).staggerDeltat12 = numData(65,i);
    EmittersArray(i).staggerDeltat13 = numData(66,i);
    EmittersArray(i).staggerDeltat14 = numData(67,i);
    EmittersArray(i).staggerDeltat15 = numData(68,i);
    EmittersArray(i).staggerDeltat16 = numData(69,i);
    EmittersArray(i).staggerDeltat17 = numData(70,i);
    EmittersArray(i).staggerDeltat18 = numData(71,i);
    EmittersArray(i).staggerDeltat19 = numData(72,i);
    EmittersArray(i).staggerDeltat20 = numData(73,i);
    EmittersArray(i).staggerDeltat21 = numData(74,i);
    EmittersArray(i).staggerDeltat22 = numData(75,i);
    EmittersArray(i).staggerDeltat23 = numData(76,i);
    EmittersArray(i).staggerDeltat24 = numData(77,i);
    EmittersArray(i).staggerDeltat25 = numData(78,i);
    EmittersArray(i).staggerDeltat26 = numData(79,i);
    EmittersArray(i).staggerDeltat27 = numData(80,i);
    EmittersArray(i).staggerDeltat28 = numData(81,i);
    EmittersArray(i).staggerDeltat29 = numData(82,i);
    EmittersArray(i).staggerDeltat30 = numData(83,i);
    EmittersArray(i).staggerDeltat31 = numData(84,i);
    EmittersArray(i).staggerDeltat32 = numData(85,i);

    % % Dwell Switch PRI Data

    EmittersArray(i).N  = numData(86,i);
    EmittersArray(i).pri1 = numData(87,i);
    EmittersArray(i).d_time1 =  numData(88,i);
    EmittersArray(i).pri2 = numData(89,i);
    EmittersArray(i).d_time2 =  numData(90,i);
    EmittersArray(i).pri3 = numData(91,i);
    EmittersArray(i).d_time3 =  numData(92,i);
    EmittersArray(i).pri4 = numData(93,i);
    EmittersArray(i).d_time4 =  numData(94,i);
    EmittersArray(i).pri5 = numData(95,i);
    EmittersArray(i).d_time5 =  numData(96,i);
    EmittersArray(i).pri6 = numData(97,i);
    EmittersArray(i).d_time6 =  numData(98,i);
    EmittersArray(i).pri7 = numData(99,i);
    EmittersArray(i).d_time7 =  numData(100,i);
    EmittersArray(i).pri8 = numData(101,i);
    EmittersArray(i).d_time8 =  numData(102,i);
    EmittersArray(i).pri9 = numData(103,i);
    EmittersArray(i).d_time9 =  numData(104,i);
    EmittersArray(i).pri10 = numData(105,i);
    EmittersArray(i).d_time10 =  numData(106,i);
    EmittersArray(i).pri11 = numData(107,i);
    EmittersArray(i).d_time11 =  numData(108,i);
    EmittersArray(i).pri12 = numData(109,i);
    EmittersArray(i).d_time12 =  numData(110,i);
    EmittersArray(i).pri13 = numData(111,i);
    EmittersArray(i).d_time13 =  numData(112,i);
    EmittersArray(i).pri14 = numData(113,i);
    EmittersArray(i).d_time14 =  numData(114,i);
    EmittersArray(i).pri15 = numData(115,i);
    EmittersArray(i).d_time15 =  numData(116,i);
    EmittersArray(i).pri16 = numData(117,i);
    EmittersArray(i).d_time16 =  numData(118,i);
    EmittersArray(i).pri17 = numData(119,i);
    EmittersArray(i).d_time17 =  numData(120,i);
    EmittersArray(i).pri18 = numData(121,i);
    EmittersArray(i).d_time18 =  numData(122,i);
    EmittersArray(i).pri19 = numData(123,i);
    EmittersArray(i).d_time19 =  numData(124,i);
    EmittersArray(i).pri20 = numData(125,i);
    EmittersArray(i).d_time20 =  numData(126,i);
    EmittersArray(i).pri21 = numData(127,i);
    EmittersArray(i).d_time21 =  numData(128,i);
    EmittersArray(i).pri22 = numData(129,i);
    EmittersArray(i).d_time22 =  numData(130,i);
    EmittersArray(i).pri23 = numData(131,i);
    EmittersArray(i).d_time23 =  numData(132,i);
    EmittersArray(i).pri24= numData(133,i);
    EmittersArray(i).d_time24 =  numData(134,i);
    EmittersArray(i).pri25 = numData(135,i);
    EmittersArray(i).d_time25 =  numData(136,i);
    EmittersArray(i).pri26 = numData(137,i);
    EmittersArray(i).d_time26 =  numData(138,i);
    EmittersArray(i).pri27 = numData(139,i);
    EmittersArray(i).d_time27 =  numData(140,i);
    EmittersArray(i).pri28 = numData(141,i);
    EmittersArray(i).d_time28 =  numData(142,i);
    EmittersArray(i).pri29 = numData(143,i);
    EmittersArray(i).d_time29 =  numData(144,i);
    EmittersArray(i).pri30= numData(145,i);
    EmittersArray(i).d_time30 =  numData(146,i);
    EmittersArray(i).pri31 = numData(147,i);
    EmittersArray(i).d_time31 =  numData(148,i);
    EmittersArray(i).pri32 = numData(149,i);
    EmittersArray(i).d_time32 =  numData(150,i);
    
    EmittersArray(i).switchDeltat1 = numData(151,i);
    EmittersArray(i).switchDeltat2 = numData(152,i);
    EmittersArray(i).switchDeltat3 = numData(153,i);
    EmittersArray(i).switchDeltat4 = numData(154,i);
    EmittersArray(i).switchDeltat5 = numData(155,i);
    EmittersArray(i).switchDeltat6 = numData(156,i);
    EmittersArray(i).switchDeltat7 = numData(157,i);
    EmittersArray(i).switchDeltat8 = numData(158,i);
    EmittersArray(i).switchDeltat9 = numData(159,i);
    EmittersArray(i).switchDeltat10 = numData(160,i);
    EmittersArray(i).switchDeltat11 = numData(161,i);
    EmittersArray(i).switchDeltat12 = numData(162,i);
    EmittersArray(i).switchDeltat13 = numData(163,i);
    EmittersArray(i).switchDeltat14 = numData(164,i);
    EmittersArray(i).switchDeltat15 = numData(165,i);
    EmittersArray(i).switchDeltat16 = numData(166,i);
    EmittersArray(i).switchDeltat17 = numData(167,i);
    EmittersArray(i).switchDeltat18 = numData(168,i);
    EmittersArray(i).switchDeltat19 = numData(169,i);
    EmittersArray(i).switchDeltat20 = numData(170,i);
    EmittersArray(i).switchDeltat21 = numData(171,i);
    EmittersArray(i).switchDeltat22 = numData(172,i);
    EmittersArray(i).switchDeltat23 = numData(173,i);
    EmittersArray(i).switchDeltat24 = numData(174,i);
    EmittersArray(i).switchDeltat25 = numData(175,i);
    EmittersArray(i).switchDeltat26 = numData(176,i);
    EmittersArray(i).switchDeltat27 = numData(177,i);
    EmittersArray(i).switchDeltat28 = numData(178,i);
    EmittersArray(i).switchDeltat29 = numData(179,i);
    EmittersArray(i).switchDeltat30 = numData(180,i);
    EmittersArray(i).switchDeltat31 = numData(181,i);
    EmittersArray(i).switchDeltat32 = numData(182,i);

    % % Frequency Step Information Data

    EmittersArray(i).Freq_StepN =  numData(183,i);
    EmittersArray(i).Freq_StepRF1 =  numData(184,i);
    EmittersArray(i).Freq_StepRF2 =  numData(185,i);
    EmittersArray(i).Freq_StepRF3 =  numData(186,i);
    EmittersArray(i).Freq_StepRF4 =  numData(187,i);
    EmittersArray(i).Freq_StepRF5 =  numData(188,i);
    EmittersArray(i).Freq_StepRF6 =  numData(189,i);
    EmittersArray(i).Freq_StepRF7 =  numData(190,i);
    EmittersArray(i).Freq_StepRF8 =  numData(191,i);
    EmittersArray(i).Freq_StepRF9 =  numData(192,i);
    EmittersArray(i).Freq_StepRF10 =  numData(193,i);
    EmittersArray(i).Freq_StepRF11 =  numData(194,i);
    EmittersArray(i).Freq_StepRF12 =  numData(195,i);
    EmittersArray(i).Freq_StepRF13 =  numData(196,i);
    EmittersArray(i).Freq_StepRF14 =  numData(197,i);
    EmittersArray(i).Freq_StepRF15 =  numData(198,i);
    EmittersArray(i).Freq_StepRF16 =  numData(199,i);

    EmittersArray(i).StepDeltaRF1 =  numData(200,i);
    EmittersArray(i).StepDeltaRF2 =  numData(201,i);
    EmittersArray(i).StepDeltaRF3 =  numData(202,i);
    EmittersArray(i).StepDeltaRF4 =  numData(203,i);
    EmittersArray(i).StepDeltaRF5 =  numData(204,i);
    EmittersArray(i).StepDeltaRF6 =  numData(205,i);
    EmittersArray(i).StepDeltaRF7 =  numData(206,i);
    EmittersArray(i).StepDeltaRF8 =  numData(207,i);
    EmittersArray(i).StepDeltaRF9 =  numData(208,i);
    EmittersArray(i).StepDeltaRF10 =  numData(209,i);
    EmittersArray(i).StepDeltaRF11 =  numData(210,i);
    EmittersArray(i).StepDeltaRF12 =  numData(211,i);
    EmittersArray(i).StepDeltaRF13 =  numData(212,i);
    EmittersArray(i).StepDeltaRF14 =  numData(213,i);
    EmittersArray(i).StepDeltaRF15 =  numData(214,i);
    EmittersArray(i).StepDeltaRF16 =  numData(215,i);

    % % Frequency Jump Information Data
    
    EmittersArray(i).Freq_JumpN =  numData(216,i);
    EmittersArray(i).Freq_JumpRF1 =  numData(217,i);
    EmittersArray(i).Freq_JumpRF2 =  numData(218,i);
    EmittersArray(i).Freq_JumpRF3 =  numData(219,i);
    EmittersArray(i).Freq_JumpRF4 =  numData(220,i);
    EmittersArray(i).Freq_JumpRF5 =  numData(221,i);
    EmittersArray(i).Freq_JumpRF6 =  numData(222,i);
    EmittersArray(i).Freq_JumpRF7 =  numData(223,i);
    EmittersArray(i).Freq_JumpRF8 =  numData(224,i);
    EmittersArray(i).Freq_JumpRF9 =  numData(225,i);
    EmittersArray(i).Freq_JumpRF10 =  numData(226,i);
    EmittersArray(i).Freq_JumpRF11 =  numData(227,i);
    EmittersArray(i).Freq_JumpRF12 =  numData(228,i);
    EmittersArray(i).Freq_JumpRF13 =  numData(229,i);
    EmittersArray(i).Freq_JumpRF14 =  numData(230,i);
    EmittersArray(i).Freq_JumpRF15 =  numData(231,i);
    EmittersArray(i).Freq_JumpRF16 =  numData(232,i);

    EmittersArray(i).JumpDeltaRF1 =  numData(233,i);
    EmittersArray(i).JumpDeltaRF2 =  numData(234,i);
    EmittersArray(i).JumpDeltaRF3 =  numData(235,i);
    EmittersArray(i).JumpDeltaRF4 =  numData(236,i);
    EmittersArray(i).JumpDeltaRF5 =  numData(237,i);
    EmittersArray(i).JumpDeltaRF6 =  numData(238,i);
    EmittersArray(i).JumpDeltaRF7 =  numData(239,i);
    EmittersArray(i).JumpDeltaRF8 =  numData(240,i);
    EmittersArray(i).JumpDeltaRF9 =  numData(241,i);
    EmittersArray(i).JumpDeltaRF10 =  numData(242,i);
    EmittersArray(i).JumpDeltaRF11 =  numData(243,i);
    EmittersArray(i).JumpDeltaRF12 =  numData(244,i);
    EmittersArray(i).JumpDeltaRF13 =  numData(245,i);
    EmittersArray(i).JumpDeltaRF14 =  numData(246,i);
    EmittersArray(i).JumpDeltaRF15 =  numData(247,i);
    EmittersArray(i).JumpDeltaRF16 =  numData(248,i);

    % % Frequency Sweep Information Data
    
    EmittersArray(i).Sweep_RFStart =  numData(249,i);
    EmittersArray(i).Sweep_RFStep =   numData(250,i);
    EmittersArray(i).Sweep_RFStop =   numData(251,i);
    if(numData(252,i)==1)
        EmittersArray(i).Freq_Sweep_Mod = 'Linear';
    elseif(numData(252,i)==2)
        EmittersArray(i).Freq_Sweep_Mod = 'sin';
		EmittersArray(i).Cycle_Sin=numData(363,i);
    elseif(numData(252,i)==3)
        EmittersArray(i).Freq_Sweep_Mod = 'negslide';
    elseif(numData(252,i)==4)
        EmittersArray(i).Freq_Sweep_Mod = 'tri';
    else
        EmittersArray(i).Freq_Sweep_Mod = '';
    end

    % % Frequency Switch Information Data
    
    EmittersArray(i).SwitchNRf  =  numData(253,i);
    EmittersArray(i).Switch_Rf1 =  numData(254,i);
    EmittersArray(i).Switch_RFt1 = numData(255,i);
    EmittersArray(i).Switch_Rf2 =  numData(256,i);
    EmittersArray(i).Switch_RFt2 = numData(257,i);
    EmittersArray(i).Switch_Rf3 =  numData(258,i);
    EmittersArray(i).Switch_RFt3 = numData(259,i);
    EmittersArray(i).Switch_Rf4 =  numData(260,i);
    EmittersArray(i).Switch_RFt4 = numData(261,i);
    EmittersArray(i).Switch_Rf5 =  numData(262,i);
    EmittersArray(i).Switch_RFt5 = numData(263,i);
    EmittersArray(i).Switch_Rf6 =  numData(264,i);
    EmittersArray(i).Switch_RFt6 = numData(265,i);
    EmittersArray(i).Switch_Rf7 =  numData(266,i);
    EmittersArray(i).Switch_RFt7 = numData(267,i);
    EmittersArray(i).Switch_Rf8 =  numData(268,i);
    EmittersArray(i).Switch_RFt8 = numData(269,i);
    EmittersArray(i).Switch_Rf9 =  numData(270,i);
    EmittersArray(i).Switch_RFt9 = numData(271,i);
    EmittersArray(i).Switch_Rf10 =  numData(272,i);
    EmittersArray(i).Switch_RFt10 = numData(273,i);
    EmittersArray(i).Switch_Rf11 =  numData(274,i);
    EmittersArray(i).Switch_RFt11 = numData(275,i);
    EmittersArray(i).Switch_Rf12 =  numData(276,i);
    EmittersArray(i).Switch_RFt12 = numData(277,i);
    EmittersArray(i).Switch_Rf13 =  numData(278,i);
    EmittersArray(i).Switch_RFt13 = numData(279,i);
    EmittersArray(i).Switch_Rf14 =  numData(280,i);
    EmittersArray(i).Switch_RFt14 = numData(281,i);
    EmittersArray(i).Switch_Rf15 =  numData(282,i);
    EmittersArray(i).Switch_RFt15 = numData(283,i);
    EmittersArray(i).Switch_Rf16 =  numData(284,i);
    EmittersArray(i).Switch_RFt16 = numData(285,i);

    EmittersArray(i).SwitchRFDeltat1 = numData(286,i);
    EmittersArray(i).SwitchRFDeltat2 = numData(287,i);
    EmittersArray(i).SwitchRFDeltat3 = numData(288,i);
    EmittersArray(i).SwitchRFDeltat4 = numData(289,i);
    EmittersArray(i).SwitchRFDeltat5 = numData(290,i);
    EmittersArray(i).SwitchRFDeltat6 = numData(291,i);
    EmittersArray(i).SwitchRFDeltat7 = numData(292,i);
    EmittersArray(i).SwitchRFDeltat8 = numData(293,i);
    EmittersArray(i).SwitchRFDeltat9 = numData(294,i);
    EmittersArray(i).SwitchRFDeltat10 =  numData(295,i);
    EmittersArray(i).SwitchRFDeltat11 =  numData(296,i);
    EmittersArray(i).SwitchRFDeltat12 =  numData(297,i);
    EmittersArray(i).SwitchRFDeltat13 =  numData(298,i);
    EmittersArray(i).SwitchRFDeltat14 =  numData(299,i);
    EmittersArray(i).SwitchRFDeltat15 =  numData(300,i);
    EmittersArray(i).SwitchRFDeltat16 =  numData(301,i);
    
    % % Unknown Data
    EmittersArray(i).RandonFreqNo = numData(302,i);
    EmittersArray(i).RanFreqSp1 = numData(303,i);
    EmittersArray(i).RanFreqSp2 = numData(304,i);
    EmittersArray(i).RanFreqSp3 = numData(305,i);
    EmittersArray(i).RanFreqSp4 = numData(306,i);
    EmittersArray(i).RanFreqSp5 = numData(307,i);
    EmittersArray(i).RanFreqSp6 = numData(308,i);
    EmittersArray(i).RanFreqSp7 = numData(309,i);
    EmittersArray(i).RanFreqSp8 = numData(310,i);
    EmittersArray(i).RanFreqSp9 = numData(311,i);
    EmittersArray(i).RanFreqSp10 = numData(312,i);

    % % Emitter Blanking Data    
    EmittersArray(i).EmitterBlankingpw =  numData(313,i);  
    EmittersArray(i).EmitterBlankingpri =  numData(314,i);
    EmittersArray(i).EmitterBlankingpulses =  numData(315,i);
    EmittersArray(i).EmitterBlankingStarttime = numData(316,i);
    if(numData(317,i)==1)
        EmittersArray(i).CompositeBlankingType ='Periodic';
    elseif (numData(317,i)==2)
        EmittersArray(i).CompositeBlankingType ='UserDefined';
    else
        EmittersArray(i).CompositeBlankingType ='';  
    end
    EmittersArray(i).Blankingperiod  = numData(318,i);
    EmittersArray(i).Blankingpw = numData(319,i);
    EmittersArray(i).BlankTimestart = numData(320,i);
    EmittersArray(i).BlankPulses = numData(321,i);
    EmittersArray(i).BlankSpots = numData(322,i);
    EmittersArray(i).Spot1Time = numData(323,i);
    EmittersArray(i).Spot1Duration = numData(324,i);
    EmittersArray(i).Spot2Time =  numData(325,i);
    EmittersArray(i).Spot2Duration =  numData(326,i);
    EmittersArray(i).Spot3Time = numData(327,i);
    EmittersArray(i).Spot3Duration = numData(328,i);
    EmittersArray(i).Spot4Time=  numData(329,i);
    EmittersArray(i).Spot4Duration = numData(330,i);
    EmittersArray(i).TotalTime =  numData(331,i);
    EmittersArray(i).EmitterStartTime = numData(332,i);
    EmittersArray(i).AntennaSwitchTime = numData(333,i);

    % % Additional Emitter Data
    EmittersArray(i).Stag_PRI33 = numData(334,i);
    EmittersArray(i).Stag_PRI34 = numData(335,i);
    EmittersArray(i).Stag_PRI35 = numData(336,i);
    EmittersArray(i).staggerDeltat33 = numData(337,i);
    EmittersArray(i).staggerDeltat34 = numData(338,i);
    EmittersArray(i).staggerDeltat35 = numData(339,i);

    EmittersArray(i).Switch_Rf17 =  numData(340,i);
    EmittersArray(i).Switch_RFt17 =  numData(341,i);
    EmittersArray(i).Switch_Rf18 =  numData(342,i);
    EmittersArray(i).Switch_RFt18 =  numData(343,i); %#ok<*AGROW>
    EmittersArray(i).SwitchRFDeltat17 =  numData(344,i);
    EmittersArray(i).SwitchRFDeltat18 =  numData(345,i);

    EmittersArray(i).Freq_JumpRF17 =  numData(346,i);
    EmittersArray(i).Freq_JumpRF18 =  numData(347,i);

    EmittersArray(i).JumpDeltaRF17 =  numData(348,i);
    EmittersArray(i).JumpDeltaRF18 =  numData(349,i);
    EmittersArray(i).Freq_StepRF17 = numData(350,i);
    EmittersArray(i).Freq_StepRF18 = numData(351,i);
    EmittersArray(i).StepDeltaRF17 = numData(352,i);
    EmittersArray(i).StepDeltaRF18 = numData(353,i);

    EmittersArray(i).pri33 = numData(354,i);
    EmittersArray(i).d_time33 = numData(355,i);
    EmittersArray(i).pri34 = numData(356,i);
    EmittersArray(i).d_time34 = numData(357,i);
    EmittersArray(i).pri35 = numData(358,i);
    EmittersArray(i).d_time35 = numData(359,i);

    EmittersArray(i).switchDeltat33 = numData(360,i);
    EmittersArray(i).switchDeltat34 = numData(361,i);
    EmittersArray(i).switchDeltat35 = numData(362,i);
    
    % % new addition by gmd on 10,Dec-18
    % % Emitter's Position Data
    if(size(numData,1)>368 ...
            && ~any(any(isnan(numData(368:376,i)))))
        emitterPosition         = numData(368:370,i);
        emitterVelocity         = numData(371:373,i);
        
        emitterScanType         = numData(374,i);
        emitterScanAzimuthStep  = numData(375,i);
        emitterScanElevationStep= numData(376,i);
        emitterRPM = numData(386,i);
        
        
    else    % if those parameters are not defined then allocate them by defualt
        emitterPosition         = [2000.*cosd(EmittersArray(i).AOA),2000.*sind(EmittersArray(i).AOA),0];
        emitterVelocity         = [0,0,0];
        emitterScanType         = 0;
        emitterScanAzimuthStep  = 0;
        emitterScanElevationStep= 0;
        
    end
    EmittersArray(i).EmitterPosition = reshape(emitterPosition,3,1);
    EmittersArray(i).EmitterVelocity = reshape(emitterVelocity,3,1);
    EmittersArray(i).EmitterScanType = emitterScanType;
     EmittersArray(i).EmitterRPM= emitterRPM;
    EmittersArray(i).EmitterScanAzStep = emitterScanAzimuthStep;
    EmittersArray(i).EmitterScanElStep = emitterScanElevationStep;
end

if(size(numData,1)>=382 ...
            && ~any(any(isnan(numData(377:382,1)))))
    airCraftPosition        = numData(377:379,1);
    airCraftVelocity        = numData(380:382,1);
else
    airCraftPosition        = [0,0,0];
    airCraftVelocity        = [0,0,0];
end
AirCraftParameters.AirCraftPosition  = reshape(airCraftPosition,3,1);
AirCraftParameters.AirCraftVelocity  = reshape(airCraftVelocity,3,1);
% Removal of Previous Run Data
% delete Data\*.*
outputNames=matfile('Data/OutputName.mat','writable',true);
outputNames.filename=filename;
parameterStructure=matfile('Data/EmitterArrays.mat','writable',true);
parameterStructure.totalTime=numData(331,1);
parameterStructure.EmittersArray=EmittersArray;
parameterStructure.totalEmitters=totalEmitters;
parameterStructure.AirCraftParameters=AirCraftParameters;
disp('Emitter Information Retrieval Finished');
end