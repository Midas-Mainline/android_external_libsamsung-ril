/*
 * This list was generated from:
 * http://en.wikipedia.org/wiki/Mobile_country_code
 *
 * Date: 07/08/2014 12:54:48
 * Copyright: Wikipedia Contributors, Creative Commons
 * Attribution-ShareAlike License
 */

#ifndef _PLMN_LIST_H_
#define _PLMN_LIST_H_

#include <stdlib.h>

struct plmn_list_entry {
	unsigned int mcc;
	unsigned int mnc;
	char *operator_long;
	char *operator_short;
};

struct plmn_list_entry plmn_list[] = {
	{ 1, 1, "TEST", "TEST" },
	{ 000, 00, "null", "null" },

	// Abkhazia

	{ 289, 67, "Aquafon", "Aquafon" },
	{ 289, 88, "A-Mobile", "A-Mobile" },

	// Afghanistan - AF

	{ 412, 1, "AWCC", "AWCC" },
	{ 412, 20, "Roshan", "Roshan" },
	{ 412, 40, "MTN", "MTN" },
	{ 412, 50, "Etisalat", "Etisalat" },
	{ 412, 55, "WASEL", "WASEL" },
	{ 412, 80, "Salaam", "Salaam" },
	{ 412, 88, "Salaam", "Salaam" },

	// Albania - AL

	{ 276, 1, "AMC", "AMC" },
	{ 276, 2, "Vodafone", "Vodafone" },
	{ 276, 3, "Eagle Mobile", "Eagle Mobile" },
	{ 276, 4, "Plus Communication", "Plus Communication" },

	// Algeria - DZ

	{ 603, 1, "Mobilis", "Mobilis" },
	{ 603, 2, "Djezzy", "Djezzy" },
	{ 603, 3, "Ooredoo", "Ooredoo" },

	// American Samoa (United States of America) - AS

	{ 544, 11, "Bluesky", "Bluesky" },

	// Andorra - AD

	{ 213, 3, "Mobiland", "Mobiland" },

	// Angola - AO

	{ 631, 2, "UNITEL", "UNITEL" },
	{ 631, 4, "MOVICEL", "MOVICEL" },

	// Anguilla (United Kingdom) - AI

	{ 365, 10, "Weblinks Limited", "Weblinks Limited" },
	{ 365, 840, "LIME", "LIME" },

	// Antigua and Barbuda - AG

	{ 344, 30, "APUA", "APUA" },
	{ 344, 50, "Digicel", "Digicel" },
	{ 344, 920, "LIME", "LIME" },
	{ 344, 930, "AT&T Wireless", "AT&T Wireless" },

	// Argentina - AR

	{ 722, 10, "Movistar", "Movistar" },
	{ 722, 20, "Nextel", "Nextel" },
	{ 722, 40, "Globalstar", "Globalstar" },
	{ 722, 70, "Movistar", "Movistar" },
	{ 722, 310, "Claro", "Claro" },
	{ 722, 320, "Claro", "Claro" },
	{ 722, 330, "Claro", "Claro" },
	{ 722, 340, "Personal", "Personal" },
	{ 722, 350, "PORT-HABLE", "PORT-HABLE" },

	// Armenia - AM

	{ 283, 1, "Beeline", "Beeline" },
	{ 283, 4, "Karabakh Telecom", "Karabakh Telecom" },
	{ 283, 5, "VivaCell-MTS", "VivaCell-MTS" },
	{ 283, 10, "Orange", "Orange" },

	// Aruba (Kingdom of the Netherlands) - AW

	{ 363, 1, "SETAR", "SETAR" },
	{ 363, 2, "Digicel", "Digicel" },

	// Australia - AU

	{ 505, 1, "Telstra", "Telstra" },
	{ 505, 2, "Optus", "Optus" },
	{ 505, 3, "Vodafone", "Vodafone" },
	{ 505, 4, "Department of Defence", "Department of Defence" },
	{ 505, 5, "Ozitel", "Ozitel" },
	{ 505, 6, "3", "3" },
	{ 505, 7, "Vodafone", "Vodafone" },
	{ 505, 8, "One.Tel", "One.Tel" },
	{ 505, 9, "Airnet", "Airnet" },
	{ 505, 10, "Norfolk Is.", "Norfolk Is." },
	{ 505, 11, "Telstra", "Telstra" },
	{ 505, 12, "3", "3" },
	{ 505, 13, "Railcorp", "Railcorp" },
	{ 505, 14, "AAPT", "AAPT" },
	{ 505, 15, "3GIS", "3GIS" },
	{ 505, 16, "VicTrack", "VicTrack" },
	{ 505, 17, "Vidid Wireless Pty. Ltd.", "Vidid Wireless Pty. Ltd." },
	{ 505, 18, "Pactel", "Pactel" },
	{ 505, 19, "Lycamobile", "Lycamobile" },
	{ 505, 20, "Ausgrid Corporation", "Ausgrid Corporation" },
	{ 505, 21, "Queensland Rail Limited", "Queensland Rail Limited" },
	{ 505, 22, "iiNet Ltd", "iiNet Ltd" },
	{ 505, 23, "Challenge Networks Pty. Ltd.", "Challenge Networks Pty. Ltd." },
	{ 505, 24, "Advanced Communications Technologies Pty. Ltd.", "Advanced Communications Technologies Pty. Ltd." },
	{ 505, 25, "Pilbara Iron Company Services Pty Ltd", "Pilbara Iron Company Services Pty Ltd" },
	{ 505, 26, "Dialogue Communications Pty. Ltd.", "Dialogue Communications Pty. Ltd." },
	{ 505, 27, "Nexium Telecommunications", "Nexium Telecommunications" },
	{ 505, 28, "RCOM International Pty Ltd", "RCOM International Pty Ltd" },
	{ 505, 38, "Crazy John's", "Crazy John's" },
	{ 505, 62, "NBN", "NBN" },
	{ 505, 68, "NBN", "NBN" },
	{ 505, 71, "Telstra", "Telstra" },
	{ 505, 72, "Telstra", "Telstra" },
	{ 505, 88, "Localstar Holding Pty. Ltd.", "Localstar Holding Pty. Ltd." },
	{ 505, 90, "Optus", "Optus" },
	{ 505, 99, "One.Tel", "One.Tel" },

	// Austria - AT

	{ 232, 1, "A1 TA", "A1 TA" },
	{ 232, 2, "A1 Telekom Austria", "A1 Telekom Austria" },
	{ 232, 3, "T-Mobile AT", "T-Mobile AT" },
	{ 232, 4, "T-Mobile AT", "T-Mobile AT" },
	{ 232, 5, "Orange AT", "Orange AT" },
	{ 232, 6, "Orange AT", "Orange AT" },
	{ 232, 7, "tele.ring", "tele.ring" },
	{ 232, 9, "Tele2Mobil", "Tele2Mobil" },
	{ 232, 10, "3AT", "3AT" },
	{ 232, 11, "bob", "bob" },
	{ 232, 12, "yesss!", "yesss!" },
	{ 232, 14, "Hutchison Drei Austria", "Hutchison Drei Austria" },
	{ 232, 15, "Vectone", "Vectone" },
	{ 232, 16, "Hutchison Drei Austria", "Hutchison Drei Austria" },

	// Azerbaijan - AZ

	{ 400, 1, "Azercell", "Azercell" },
	{ 400, 2, "Bakcell", "Bakcell" },
	{ 400, 3, "FONEX", "FONEX" },
	{ 400, 4, "Nar Mobile", "Nar Mobile" },

	// Bahamas - BS

	{ 364, 39, "BaTelCo", "BaTelCo" },

	// Bahrain - BH

	{ 426, 1, "Batelco", "Batelco" },
	{ 426, 2, "zain BH", "zain BH" },
	{ 426, 3, "Civil Aviation Authority", "Civil Aviation Authority" },
	{ 426, 4, "VIVA Bahrain", "VIVA Bahrain" },
	{ 426, 5, "Batelco", "Batelco" },

	// Bangladesh - BD

	{ 470, 1, "Grameenphone", "Grameenphone" },
	{ 470, 2, "Robi", "Robi" },
	{ 470, 3, "Banglalink", "Banglalink" },
	{ 470, 4, "TeleTalk", "TeleTalk" },
	{ 470, 5, "Citycell", "Citycell" },
	{ 470, 6, "Beeong3G Warid Telekom", "Beeong3G Warid Telekom" },
	{ 470, 7, "Airtel", "Airtel" },

	// Barbados - BB

	{ 342, 600, "LIME", "LIME" },
	{ 342, 750, "Digicel", "Digicel" },
	{ 342, 820, "Sunbeach Communications", "Sunbeach Communications" },

	// Belarus - BY

	{ 257, 1, "velcom", "velcom" },
	{ 257, 2, "MTS", "MTS" },
	{ 257, 3, "DIALLOG", "DIALLOG" },
	{ 257, 4, "life:)", "life:)" },
	{ 257, 5, "Beltelecom", "Beltelecom" },
	{ 257, 6, "Belorussian Cloud Technologies", "Belorussian Cloud Technologies" },
	{ 257, 501, "BelCel JV", "BelCel JV" },

	// Belgium - BE

	{ 206, 1, "Proximus", "Proximus" },
	{ 206, 2, "N.M.B.S.", "N.M.B.S." },
	{ 206, 5, "Telenet", "Telenet" },
	{ 206, 6, "Lycamobile", "Lycamobile" },
	{ 206, 7, "Vectone Mobile", "Vectone Mobile" },
	{ 206, 9, "Voxbone", "Voxbone" },
	{ 206, 10, "Mobistar", "Mobistar" },
	{ 206, 15, "Elephant Talk Communications Schweiz GmbH", "Elephant Talk Communications Schweiz GmbH" },
	{ 206, 20, "BASE", "BASE" },

	// Belize - BZ

	{ 702, 67, "DigiCell", "DigiCell" },
	{ 702, 68, "INTELCO", "INTELCO" },
	{ 702, 99, "Smart", "Smart" },

	// Benin - BJ

	{ 616, 1, "Libercom", "Libercom" },
	{ 616, 2, "Moov", "Moov" },
	{ 616, 3, "MTN", "MTN" },
	{ 616, 4, "BBCOM", "BBCOM" },
	{ 616, 4, "BLK", "BLK" },
	{ 616, 5, "Glo", "Glo" },

	// Bermuda - BM

	{ 310, 59, "Cellular One", "Cellular One" },
	{ 338, 50, "Digicel Bermuda", "Digicel Bermuda" },
	{ 350, 1, "Digicel Bermuda", "Digicel Bermuda" },
	{ 350, 2, "Mobility", "Mobility" },

	// Bhutan - BT

	{ 402, 11, "B-Mobile", "B-Mobile" },
	{ 402, 77, "TashiCell", "TashiCell" },

	// Bolivia - BO

	{ 736, 1, "Nuevatel", "Nuevatel" },
	{ 736, 2, "Entel", "Entel" },
	{ 736, 3, "Tigo", "Tigo" },

	// Bosnia and Herzegovina - BA

	{ 218, 3, "HT-ERONET", "HT-ERONET" },
	{ 218, 5, "m:tel", "m:tel" },
	{ 218, 90, "BH Mobile", "BH Mobile" },

	// Botswana - BW

	{ 652, 1, "Mascom", "Mascom" },
	{ 652, 2, "Orange", "Orange" },
	{ 652, 4, "BTC Mobile", "BTC Mobile" },

	// Brazil - BR

	{ 724, 2, "TIM", "TIM" },
	{ 724, 3, "TIM", "TIM" },
	{ 724, 4, "TIM", "TIM" },
	{ 724, 5, "Claro BR", "Claro BR" },
	{ 724, 55, "Sercomtel", "Sercomtel" },
	{ 724, 6, "Vivo", "Vivo" },
	{ 724, 10, "Vivo", "Vivo" },
	{ 724, 11, "Vivo", "Vivo" },
	{ 724, 15, "CTBC Celular", "CTBC Celular" },
	{ 724, 16, "Brasil Telecom GSM", "Brasil Telecom GSM" },
	{ 724, 23, "Vivo", "Vivo" },
	{ 724, 31, "Oi", "Oi" },
	{ 724, 32, "CTBC Celular", "CTBC Celular" },
	{ 724, 33, "CTBC Celular", "CTBC Celular" },
	{ 724, 34, "CTBC Celular", "CTBC Celular" },
	{ 724, 39, "Nextel", "Nextel" },

	// British Virgin Islands (United Kingdom) - VG

	{ 348, 170, "LIME", "LIME" },
	{ 348, 370, "BVI Cable TV Ltd", "BVI Cable TV Ltd" },
	{ 348, 570, "CCT Boatphone", "CCT Boatphone" },
	{ 348, 770, "Digicel", "Digicel" },

	// Brunei - BN

	{ 528, 1, "Jabatan Telekom Brunei", "Jabatan Telekom Brunei" },
	{ 528, 2, "B-Mobile", "B-Mobile" },
	{ 528, 11, "DSTCom", "DSTCom" },

	// Bulgaria - BG

	{ 284, 1, "M-Tel", "M-Tel" },
	{ 284, 3, "Vivacom", "Vivacom" },
	{ 284, 5, "GLOBUL", "GLOBUL" },
	{ 284, 7, "НКЖИ", "НКЖИ" },
	{ 284, 9, "COMPATEL LIMITED", "COMPATEL LIMITED" },
	{ 284, 11, "Bulsatcom", "Bulsatcom" },
	{ 284, 13, "Max Telecom", "Max Telecom" },

	// Burkina Faso - BF

	{ 613, 1, "Telmob", "Telmob" },
	{ 613, 2, "Airtel", "Airtel" },
	{ 613, 3, "Telecel Faso", "Telecel Faso" },

	// Burundi - BI

	{ 642, 1, "Spacetel", "Spacetel" },
	{ 642, 2, "Tempo", "Tempo" },
	{ 642, 3, "Onatel", "Onatel" },
	{ 642, 7, "Smart Mobile", "Smart Mobile" },
	{ 642, 8, "HiTs Telecom", "HiTs Telecom" },
	{ 642, 82, "Leo", "Leo" },

	// Cambodia - KH

	{ 456, 1, "Cellcard", "Cellcard" },
	{ 456, 2, "Smart", "Smart" },
	{ 456, 3, "qb", "qb" },
	{ 456, 4, "qb", "qb" },
	{ 456, 5, "Latelz Company Limited", "Latelz Company Limited" },
	{ 456, 6, "Latelz Company Limited", "Latelz Company Limited" },
	{ 456, 8, "Cellcard", "Cellcard" },
	{ 456, 9, "Beeline", "Beeline" },
	{ 456, 11, "Excell", "Excell" },
	{ 456, 18, "Cellcard", "Cellcard" },

	// Cameroon - CM

	{ 624, 1, "MTN Cameroon", "MTN Cameroon" },
	{ 624, 2, "Orange", "Orange" },

	// Canada - CA

	{ 302, 220, "Telus", "Telus" },
	{ 302, 221, "Telus", "Telus" },
	{ 302, 222, "Telus", "Telus" },
	{ 302, 250, "ALO", "ALO" },
	{ 302, 270, "EastLink", "EastLink" },
	{ 302, 290, "Airtel Wireless", "Airtel Wireless" },
	{ 302, 320, "Mobilicity", "Mobilicity" },
	{ 302, 340, "Execulink", "Execulink" },
	{ 302, 350, "FIRST", "FIRST" },
	{ 302, 360, "MiKe", "MiKe" },
	{ 302, 361, "Telus", "Telus" },
	{ 302, 370, "Fido", "Fido" },
	{ 302, 380, "DMTS", "DMTS" },
	{ 302, 480, "SSi Connexions", "SSi Connexions" },
	{ 302, 490, "WIND Mobile", "WIND Mobile" },
	{ 302, 500, "Videotron", "Videotron" },
	{ 302, 510, "Videotron", "Videotron" },
	{ 302, 530, "Keewatinook Okimacinac", "Keewatinook Okimacinac" },
	{ 302, 560, "Lynx Mobility", "Lynx Mobility" },
	{ 302, 570, "LightSquared", "LightSquared" },
	{ 302, 590, "Quadro Communication", "Quadro Communication" },
	{ 302, 610, "Bell", "Bell" },
	{ 302, 620, "ICE Wireless", "ICE Wireless" },
	{ 302, 630, "Aliant Mobility", "Aliant Mobility" },
	{ 302, 640, "Bell", "Bell" },
	{ 302, 652, "BC Tel Mobility (Telus)", "BC Tel Mobility (Telus)" },
	{ 302, 653, "Telus", "Telus" },
	{ 302, 655, "MTS", "MTS" },
	{ 302, 656, "TBay", "TBay" },
	{ 302, 657, "Telus", "Telus" },
	{ 302, 660, "MTS", "MTS" },
	{ 302, 670, "CityTel Mobility", "CityTel Mobility" },
	{ 302, 680, "SaskTel", "SaskTel" },
	{ 302, 690, "Bell", "Bell" },
	{ 302, 701, "MB Tel Mobility", "MB Tel Mobility" },
	{ 302, 702, "MT&T Mobility (Aliant)", "MT&T Mobility (Aliant)" },
	{ 302, 703, "New Tel Mobility (Aliant)", "New Tel Mobility (Aliant)" },
	{ 302, 710, "Globalstar", "Globalstar" },
	{ 302, 720, "Rogers Wireless", "Rogers Wireless" },
	{ 302, 730, "TerreStar Solutions", "TerreStar Solutions" },
	{ 302, 740, "Shaw Telecom G.P.", "Shaw Telecom G.P." },
	{ 302, 750, "SaskTel", "SaskTel" },
	{ 302, 760, "Public Mobile Inc.", "Public Mobile Inc." },
	{ 302, 770, "Rural Com", "Rural Com" },
	{ 302, 780, "SaskTel", "SaskTel" },
	{ 302, 860, "Telus", "Telus" },
	{ 302, 880, "Bell / Telus / SaskTel", "Bell / Telus / SaskTel" },
	{ 302, 940, "Wightman Telecom", "Wightman Telecom" },

	// Cape Verde - CV

	{ 625, 1, "CVMOVEL", "CVMOVEL" },
	{ 625, 2, "T+", "T+" },

	// Cayman Islands (United Kingdom) - KY

	{ 346, 140, "LIME", "LIME" },
	{ 346, 50, "Digicel", "Digicel" },

	// Central African Republic - CF

	{ 623, 1, "CTP", "CTP" },
	{ 623, 2, "TC", "TC" },
	{ 623, 3, "Orange", "Orange" },
	{ 623, 4, "Nationlink", "Nationlink" },

	// Chad - TD

	{ 622, 1, "Airtel", "Airtel" },
	{ 622, 2, "Tchad Mobile", "Tchad Mobile" },
	{ 622, 2, "Tawali", "Tawali" },
	{ 622, 3, "Tigo", "Tigo" },
	{ 622, 4, "Salam", "Salam" },

	// Chile - CL

	{ 730, 1, "entel", "entel" },
	{ 730, 2, "movistar", "movistar" },
	{ 730, 3, "Claro", "Claro" },
	{ 730, 4, "Nextel", "Nextel" },
	{ 730, 5, "Multikom S.A.", "Multikom S.A." },
	{ 730, 6, "Blue Two Chile S.A.", "Blue Two Chile S.A." },
	{ 730, 7, "Virgin Mobile", "Virgin Mobile" },
	{ 730, 8, "VTR Móvil", "VTR Móvil" },
	{ 730, 9, "Nextel", "Nextel" },
	{ 730, 10, "entel", "entel" },
	{ 730, 11, "Celupago S.A.", "Celupago S.A." },
	{ 730, 12, "Telestar Móvil S.A.", "Telestar Móvil S.A." },
	{ 730, 13, "Tribe Mobile Chile SPA", "Tribe Mobile Chile SPA" },
	{ 730, 14, "Netline Telefónica Móvil Ltda", "Netline Telefónica Móvil Ltda" },
	{ 730, 15, "Cibeles Telecom S.A.", "Cibeles Telecom S.A." },
	{ 730, 99, "Will", "Will" },

	// China - CN

	{ 460, 1, "China Unicom", "China Unicom" },
	{ 460, 2, "China Mobile", "China Mobile" },
	{ 460, 3, "China Telecom", "China Telecom" },
	{ 460, 4, "Global Star Satellite", "Global Star Satellite" },
	{ 460, 5, "China Telecom", "China Telecom" },
	{ 460, 6, "China Unicom", "China Unicom" },
	{ 460, 7, "China Mobile", "China Mobile" },
	{ 460, 20, "China Tietong", "China Tietong" },

	// Christmas Island (Australia) - CX


	// Cocos Islands (Australia) - CC


	// Colombia - CO

	{ 732, 1, "Colombia Telecomunicaciones S.A.", "Colombia Telecomunicaciones S.A." },
	{ 732, 2, "Edatel", "Edatel" },
	{ 732, 20, "Emtelsa", "Emtelsa" },
	{ 732, 99, "EMCALI", "EMCALI" },
	{ 732, 101, "Claro", "Claro" },
	{ 732, 102, "movistar", "movistar" },
	{ 732, 103, "Tigo", "Tigo" },
	{ 732, 111, "Tigo", "Tigo" },
	{ 732, 123, "movistar", "movistar" },
	{ 732, 130, "AVANTEL", "AVANTEL" },
	{ 732, 123, "Virgin Mobile Colombia", "Virgin Mobile Colombia" },

	// Comoros - KM

	{ 654, 1, "Comoros Telecom", "Comoros Telecom" },

	// Republic of the Congo - CG

	{ 629, 1, "Airtel", "Airtel" },
	{ 629, 7, "Warid Telecom", "Warid Telecom" },
	{ 629, 10, "Libertis Telecom", "Libertis Telecom" },

	// Cook Islands (New Zealand) - CK

	{ 548, 1, "Telecom Cook", "Telecom Cook" },

	// Costa Rica - CR

	{ 712, 1, "Kolbi ICE", "Kolbi ICE" },
	{ 712, 2, "Kolbi ICE", "Kolbi ICE" },
	{ 712, 3, "Claro", "Claro" },
	{ 712, 4, "movistar", "movistar" },
	{ 712, 20, "'''full'''móvil", "'''full'''móvil" },

	// Croatia - HR

	{ 219, 1, "T-Mobile", "T-Mobile" },
	{ 219, 2, "Tele2", "Tele2" },
	{ 219, 10, "Vip", "Vip" },

	// Cuba - CU

	{ 368, 1, "CUBACEL", "CUBACEL" },

	// Cyprus - CY

	{ 280, 1, "Cytamobile-Vodafone", "Cytamobile-Vodafone" },
	{ 280, 10, "MTN", "MTN" },
	{ 280, 20, "PrimeTel", "PrimeTel" },
	{ 280, 22, "lemontel", "lemontel" },

	// Czech Republic - CZ

	{ 230, 1, "T-Mobile", "T-Mobile" },
	{ 230, 2, "O2", "O2" },
	{ 230, 3, "Vodafone", "Vodafone" },
	{ 230, 4, "U:fon", "U:fon" },
	{ 230, 5, "TRAVEL TELEKOMMUNIKATION, s.r.o.", "TRAVEL TELEKOMMUNIKATION, s.r.o." },
	{ 230, 6, "OSNO TELECOMUNICATION, s.r.o.", "OSNO TELECOMUNICATION, s.r.o." },
	{ 230, 7, "ASTELNET, s.r.o.", "ASTELNET, s.r.o." },
	{ 230, 8, "Compatel s.r.o.", "Compatel s.r.o." },
	{ 230, 98, "Správa železniční dopravní cesty, s.o.", "Správa železniční dopravní cesty, s.o." },
	{ 230, 99, "Vodafone", "Vodafone" },

	// Democratic Republic of the Congo - CD

	{ 630, 1, "Vodacom", "Vodacom" },
	{ 630, 2, "airtel", "airtel" },
	{ 630, 4, "Cellco", "Cellco" },
	{ 630, 5, "Supercell", "Supercell" },
	{ 630, 10, "Libertis Telecom", "Libertis Telecom" },
	{ 630, 86, "Orange", "Orange" },
	{ 630, 88, "YTT", "YTT" },
	{ 630, 89, "Tigo", "Tigo" },
	{ 630, 90, "Africell", "Africell" },

	// Denmark (Kingdom of Denmark) - DK

	{ 238, 1, "TDC", "TDC" },
	{ 238, 2, "Telenor", "Telenor" },
	{ 238, 3, "End2End", "End2End" },
	{ 238, 4, "NextGen Mobile Ltd", "NextGen Mobile Ltd" },
	{ 238, 5, "TetraNet", "TetraNet" },
	{ 238, 6, "3", "3" },
	{ 238, 7, "Vectone Mobile", "Vectone Mobile" },
	{ 238, 8, "Voxbone", "Voxbone" },
	{ 238, 9, "SINE", "SINE" },
	{ 238, 10, "TDC", "TDC" },
	{ 238, 11, "SINE", "SINE" },
	{ 238, 12, "Lycamobile", "Lycamobile" },
	{ 238, 13, "Compatel Limited", "Compatel Limited" },
	{ 238, 20, "Telia", "Telia" },
	{ 238, 23, "GSM-R DK", "GSM-R DK" },
	{ 238, 28, "CoolTEL ApS", "CoolTEL ApS" },
	{ 238, 30, "Interactive digital media GmbH", "Interactive digital media GmbH" },
	{ 238, 40, "Ericsson Danmark A/S", "Ericsson Danmark A/S" },
	{ 238, 43, "MobiWeb Limited", "MobiWeb Limited" },
	{ 238, 66, "TT-Netværket P/S", "TT-Netværket P/S" },
	{ 238, 77, "Telenor", "Telenor" },

	// Djibouti - DJ

	{ 638, 1, "Evatis", "Evatis" },

	// Dominica - DM

	{ 366, 20, "Digicel", "Digicel" },
	{ 366, 110, "Cable &  Wireless", "Cable &  Wireless" },

	// Dominican Republic - DO

	{ 370, 1, "Orange", "Orange" },
	{ 370, 2, "Claro", "Claro" },
	{ 370, 3, "Tricom", "Tricom" },
	{ 370, 4, "Viva", "Viva" },

	// East Timor - TL

	{ 514, 1, "Telin Telkomcel", "Telin Telkomcel" },
	{ 514, 2, "Timor Telecom", "Timor Telecom" },
	{ 514, 3, "Viettel Timor-Leste", "Viettel Timor-Leste" },

	// Ecuador - EC

	{ 740, 1, "Claro", "Claro" },
	{ 740, 2, "CNT Mobile", "CNT Mobile" },

	// Egypt - EG

	{ 602, 1, "Mobinil", "Mobinil" },
	{ 602, 2, "Vodafone", "Vodafone" },
	{ 602, 3, "Etisalat", "Etisalat" },

	// El Salvador - SV

	{ 706, 1, "CTE Telecom Personal, Claro", "CTE Telecom Personal, Claro" },
	{ 706, 2, "digicel", "digicel" },
	{ 706, 3, "Tigo", "Tigo" },
	{ 706, 4, "movistar", "movistar" },

	// Equatorial Guinea - GQ

	{ 627, 1, "Orange GQ", "Orange GQ" },
	{ 627, 3, "Hits GQ", "Hits GQ" },

	// Eritrea - ER

	{ 657, 1, "Eritel", "Eritel" },

	// Estonia - EE

	{ 248, 1, "EMT", "EMT" },
	{ 248, 2, "Elisa", "Elisa" },
	{ 248, 3, "Tele 2", "Tele 2" },
	{ 248, 4, "OY Top Connect", "OY Top Connect" },
	{ 248, 5, "AS Bravocom Mobiil", "AS Bravocom Mobiil" },
	{ 248, 6, "Progroup Holding", "Progroup Holding" },
	{ 248, 7, "Televõrgu AS", "Televõrgu AS" },
	{ 248, 8, "VIVEX", "VIVEX" },
	{ 248, 9, "Bravo Telecom", "Bravo Telecom" },
	{ 248, 71, "Siseministeerium (Ministry of Interior)", "Siseministeerium (Ministry of Interior)" },

	// Ethiopia - ET

	{ 636, 1, "ETH-MTN", "ETH-MTN" },

	// Faroe Islands (Kingdom of Denmark) - FO

	{ 288, 1, "Faroese Telecom", "Faroese Telecom" },
	{ 288, 2, "Vodafone", "Vodafone" },
	{ 288, 3, "Edge Mobile Sp/F", "Edge Mobile Sp/F" },

	// Fiji - FJ

	{ 542, 1, "Vodafone", "Vodafone" },
	{ 542, 2, "Digicel", "Digicel" },
	{ 542, 3, "Telecom Fiji Ltd", "Telecom Fiji Ltd" },

	// Finland - FI

	{ 244, 3, "DNA", "DNA" },
	{ 244, 4, "AINA", "AINA" },
	{ 244, 5, "Elisa", "Elisa" },
	{ 244, 7, "Nokia", "Nokia" },
	{ 244, 8, "Nokia", "Nokia" },
	{ 244, 9, "Finnet Group / Nokia Solutions and Networks", "Finnet Group / Nokia Solutions and Networks" },
	{ 244, 10, "TDC", "TDC" },
	{ 244, 11, "Vectone Mobile", "Vectone Mobile" },
	{ 244, 12, "DNA", "DNA" },
	{ 244, 13, "DNA", "DNA" },
	{ 244, 14, "Ålcom", "Ålcom" },
	{ 244, 15, "SAMK", "SAMK" },
	{ 244, 16, "Oy Finland Tele2 AB", "Oy Finland Tele2 AB" },
	{ 244, 21, "Saunalahti", "Saunalahti" },
	{ 244, 25, "Datame", "Datame" },
	{ 244, 26, "Compatel", "Compatel" },
	{ 244, 29, "SCNL Truphone", "SCNL Truphone" },
	{ 244, 30, "Vectone Mobile", "Vectone Mobile" },
	{ 244, 31, "Ukko Mobile", "Ukko Mobile" },
	{ 244, 32, "Voxbone", "Voxbone" },
	{ 244, 91, "Sonera", "Sonera" },

	// France - FR

	{ 208, 1, "Orange", "Orange" },
	{ 208, 2, "Orange", "Orange" },
	{ 208, 3, "MobiquiThings", "MobiquiThings" },
	{ 208, 4, "Sisteer", "Sisteer" },
	{ 208, 5, "Globalstar Europe", "Globalstar Europe" },
	{ 208, 6, "Globalstar Europe", "Globalstar Europe" },
	{ 208, 7, "Globalstar Europe", "Globalstar Europe" },
	{ 208, 8, "Completel Mobile", "Completel Mobile" },
	{ 208, 9, "SFR", "SFR" },
	{ 208, 10, "SFR", "SFR" },
	{ 208, 11, "SFR", "SFR" },
	{ 208, 13, "SFR", "SFR" },
	{ 208, 14, "RFF", "RFF" },
	{ 208, 15, "Free Mobile", "Free Mobile" },
	{ 208, 16, "Free Mobile", "Free Mobile" },
	{ 208, 17, "LEGOS", "LEGOS" },
	{ 208, 18, "Voxbone", "Voxbone" },
	{ 208, 20, "Bouygues", "Bouygues" },
	{ 208, 21, "Bouygues", "Bouygues" },
	{ 208, 22, "Transatel Mobile", "Transatel Mobile" },
	{ 208, 23, "Virgin Mobile (MVNO)", "Virgin Mobile (MVNO)" },
	{ 208, 24, "MobiquiThings", "MobiquiThings" },
	{ 208, 25, "LycaMobile", "LycaMobile" },
	{ 208, 26, "NRJ Mobile (MVNO)", "NRJ Mobile (MVNO)" },
	{ 208, 27, "Afone", "Afone" },
	{ 208, 28, "Astrium SAS", "Astrium SAS" },
	{ 208, 29, "Société International Mobile Communication", "Société International Mobile Communication" },
	{ 208, 30, "Symacom", "Symacom" },
	{ 208, 31, "Vectone", "Vectone" },
	{ 208, 88, "Bouygues", "Bouygues" },
	{ 208, 89, "Omer Telecom Ltd", "Omer Telecom Ltd" },
	{ 208, 90, "Images & Réseaux", "Images & Réseaux" },
	{ 208, 91, "Orange S.A.", "Orange S.A." },
	{ 208, 92, "Com4Innov", "Com4Innov" },
	{ 208, 93, "TDF", "TDF" },

	// French Guiana (France) - GF


	// French Polynesia (France) - PF

	{ 547, 10, "Mara Telecom", "Mara Telecom" },
	{ 547, 15, "Vodafone", "Vodafone" },
	{ 547, 20, "Vini", "Vini" },

	// French Southern Territories (France) - TF


	// Gabon - GA

	{ 628, 1, "Libertis", "Libertis" },
	{ 628, 2, "Moov", "Moov" },
	{ 628, 3, "Airtel", "Airtel" },
	{ 628, 4, "Azur", "Azur" },
	{ 628, 5, "RAG", "RAG" },

	// Gambia - GM

	{ 607, 1, "Gamcel", "Gamcel" },
	{ 607, 2, "Africel", "Africel" },
	{ 607, 3, "Comium", "Comium" },
	{ 607, 4, "QCell", "QCell" },

	// Georgia - GE

	{ 282, 1, "Geocell", "Geocell" },
	{ 282, 2, "MagtiCom", "MagtiCom" },
	{ 282, 3, "MagtiCom", "MagtiCom" },
	{ 282, 4, "Beeline", "Beeline" },
	{ 282, 5, "Silknet", "Silknet" },
	{ 282, 6, "JSC Compatel", "JSC Compatel" },
	{ 282, 7, "GlobalCell Ltd", "GlobalCell Ltd" },

	// Germany - DE

	{ 262, 1, "Telekom", "Telekom" },
	{ 262, 2, "Vodafone", "Vodafone" },
	{ 262, 3, "E-Plus", "E-Plus" },
	{ 262, 4, "Vodafone", "Vodafone" },
	{ 262, 5, "E-Plus", "E-Plus" },
	{ 262, 6, "T-Mobile", "T-Mobile" },
	{ 262, 7, "O2", "O2" },
	{ 262, 8, "O2", "O2" },
	{ 262, 9, "Vodafone", "Vodafone" },
	{ 262, 10, "DB Netz AG", "DB Netz AG" },
	{ 262, 11, "O2", "O2" },
	{ 262, 12, "Dolphin Telecom / sipgate", "Dolphin Telecom / sipgate" },
	{ 262, 13, "Mobilcom Multimedia", "Mobilcom Multimedia" },
	{ 262, 14, "Group 3G UMTS", "Group 3G UMTS" },
	{ 262, 15, "Airdata", "Airdata" },
	{ 262, 16, "Telogic ex. Vistream", "Telogic ex. Vistream" },
	{ 262, 17, "E-Plus", "E-Plus" },
	{ 262, 18, "NetCologne", "NetCologne" },
	{ 262, 19, "Inquam Deutschland", "Inquam Deutschland" },
	{ 262, 20, "OnePhone", "OnePhone" },
	{ 262, 41, "First Telecom GmbH", "First Telecom GmbH" },
	{ 262, 42, "CCC Event", "CCC Event" },
	{ 262, 43, "LYCA", "LYCA" },
	{ 262, 60, "DB Telematik", "DB Telematik" },
	{ 262, 76, "Siemens AG", "Siemens AG" },
	{ 262, 77, "E-Plus", "E-Plus" },
	{ 262, 78, "T-Mobile", "T-Mobile" },
	{ 262, 79, "ng4T GmbH", "ng4T GmbH" },
	{ 262, 92, "Nash Technologies", "Nash Technologies" },
	{ 262, 901, "Debitel", "Debitel" },

	// Ghana - GH

	{ 620, 1, "MTN", "MTN" },
	{ 620, 2, "Vodafone", "Vodafone" },
	{ 620, 3, "tiGO", "tiGO" },
	{ 620, 4, "Expresso", "Expresso" },
	{ 620, 6, "Airtel", "Airtel" },
	{ 620, 7, "Globacom", "Globacom" },
	{ 620, 11, "Netafrique Dot Com Ltd", "Netafrique Dot Com Ltd" },

	// Gibraltar (United Kingdom) - GI

	{ 266, 1, "GibTel", "GibTel" },
	{ 266, 6, "CTS Mobile", "CTS Mobile" },
	{ 266, 9, "Shine", "Shine" },

	// Greece - GR

	{ 202, 1, "Cosmote", "Cosmote" },
	{ 202, 2, "Cosmote", "Cosmote" },
	{ 202, 3, "OTE", "OTE" },
	{ 202, 4, "EDISY", "EDISY" },
	{ 202, 5, "Vodafone", "Vodafone" },
	{ 202, 6, "Cosmoline", "Cosmoline" },
	{ 202, 7, "AMD Telecom", "AMD Telecom" },
	{ 202, 9, "Wind", "Wind" },
	{ 202, 10, "Wind", "Wind" },

	// Greenland (Kingdom of Denmark) - GL

	{ 290, 1, "TELE Greenland A/S", "TELE Greenland A/S" },

	// Grenada - GD

	{ 352, 30, "Digicel", "Digicel" },
	{ 352, 110, "Cable &  Wireless", "Cable &  Wireless" },

	// Guadeloupe (France) - GP

	{ 340, 1, "Orange", "Orange" },
	{ 340, 2, "Outremer", "Outremer" },
	{ 340, 3, "Telcell", "Telcell" },
	{ 340, 8, "Dauphin", "Dauphin" },
	{ 340, 20, "Digicel", "Digicel" },

	// Guam (United States of America) - GU

	{ 310, 32, "IT&E Wireless", "IT&E Wireless" },
	{ 310, 33, "Guam Telephone Authority", "Guam Telephone Authority" },
	{ 310, 140, "mPulse", "mPulse" },
	{ 311, 250, "i CAN_GSM", "i CAN_GSM" },
	{ 310, 370, "Docomo", "Docomo" },
	{ 310, 470, "Docomo", "Docomo" },

	// Guatemala - GT

	{ 704, 1, "Claro", "Claro" },
	{ 704, 2, "Tigo", "Tigo" },
	{ 704, 3, "movistar", "movistar" },

	// Guernsey (United Kingdom) - GG

	{ 234, 3, "Airtel Vodafone", "Airtel Vodafone" },
	{ 234, 50, "Wave Telecom", "Wave Telecom" },
	{ 234, 55, "Sure Mobile", "Sure Mobile" },

	// Guinea - GN

	{ 611, 1, "Orange S.A.", "Orange S.A." },
	{ 611, 2, "Sotelgui", "Sotelgui" },
	{ 611, 3, "Telecel Guinee", "Telecel Guinee" },
	{ 611, 4, "MTN", "MTN" },
	{ 611, 5, "Cellcom", "Cellcom" },

	// Guinea-Bissau - GW

	{ 632, 1, "Guinetel", "Guinetel" },
	{ 632, 2, "MTN Areeba", "MTN Areeba" },
	{ 632, 3, "Orange", "Orange" },
	{ 632, 7, "Guinetel", "Guinetel" },

	// Guyana - GY

	{ 738, 1, "Digicel", "Digicel" },
	{ 738, 2, "GT&T Cellink Plus", "GT&T Cellink Plus" },

	// Haiti - HT

	{ 372, 1, "Voila", "Voila" },
	{ 372, 2, "Digicel", "Digicel" },
	{ 372, 3, "Natcom", "Natcom" },

	// Honduras - HN

	{ 708, 1, "Claro", "Claro" },
	{ 708, 2, "Tigo", "Tigo" },
	{ 708, 30, "Hondutel", "Hondutel" },
	{ 708, 40, "Digicel", "Digicel" },

	// Hong Kong - HK

	{ 454, 1, "CITIC Telecom 1616", "CITIC Telecom 1616" },
	{ 454, 2, "CSL Limited", "CSL Limited" },
	{ 454, 3, "3", "3" },
	{ 454, 4, "3 (2G)", "3 (2G)" },
	{ 454, 5, "3 (CDMA)", "3 (CDMA)" },
	{ 454, 6, "SmarTone", "SmarTone" },
	{ 454, 7, "China Unicom (Hong Kong) Limited", "China Unicom (Hong Kong) Limited" },
	{ 454, 8, "Truphone", "Truphone" },
	{ 454, 9, "China Motion Telecom", "China Motion Telecom" },
	{ 454, 10, "New World Mobility", "New World Mobility" },
	{ 454, 11, "China-Hong Kong Telecom", "China-Hong Kong Telecom" },
	{ 454, 12, "CMCC HK", "CMCC HK" },
	{ 454, 13, "CMCC HK", "CMCC HK" },
	{ 454, 14, "Hutchison Telecom", "Hutchison Telecom" },
	{ 454, 15, "SmarTone Mobile Communications Limited", "SmarTone Mobile Communications Limited" },
	{ 454, 16, "PCCW Mobile (2G)", "PCCW Mobile (2G)" },
	{ 454, 17, "SmarTone Mobile Communications Limited", "SmarTone Mobile Communications Limited" },
	{ 454, 18, "CSL Limited", "CSL Limited" },
	{ 454, 19, "PCCW Mobile (3G/4G)", "PCCW Mobile (3G/4G)" },
	{ 454, 22, "P Plus", "P Plus" },
	{ 454, 29, "PCCW Mobile (CDMA)", "PCCW Mobile (CDMA)" },

	// Hungary - HU

	{ 216, 1, "Telenor", "Telenor" },
	{ 216, 20, "Telenor", "Telenor" },
	{ 216, 30, "T-Mobile", "T-Mobile" },
	{ 216, 70, "Vodafone", "Vodafone" },
	{ 216, 71, "UPC Hungary", "UPC Hungary" },
	{ 216, 99, "MAV GSM-R", "MAV GSM-R" },

	// Iceland - IS

	{ 274, 1, "Síminn", "Síminn" },
	{ 274, 2, "Vodafone", "Vodafone" },
	{ 274, 3, "Vodafone", "Vodafone" },
	{ 274, 4, "Viking", "Viking" },
	{ 274, 6, "Núll níu ehf", "Núll níu ehf" },
	{ 274, 7, "IceCell", "IceCell" },
	{ 274, 8, "On-waves", "On-waves" },
	{ 274, 11, "Nova", "Nova" },
	{ 274, 12, "Tal", "Tal" },

	// India - IN

	{ 404, 1, "Vodafone IN", "Vodafone IN" },
	{ 404, 2, "AirTel", "AirTel" },
	{ 404, 3, "AirTel", "AirTel" },
	{ 404, 4, "IDEA", "IDEA" },
	{ 404, 5, "Vodafone IN", "Vodafone IN" },
	{ 404, 7, "IDEA", "IDEA" },
	{ 404, 9, "Reliance", "Reliance" },
	{ 404, 10, "AirTel", "AirTel" },
	{ 404, 11, "Vodafone IN", "Vodafone IN" },
	{ 404, 12, "IDEA", "IDEA" },
	{ 404, 13, "Vodafone IN", "Vodafone IN" },
	{ 404, 14, "IDEA", "IDEA" },
	{ 404, 15, "Vodafone IN", "Vodafone IN" },
	{ 404, 16, "Airtel", "Airtel" },
	{ 404, 17, "AIRCEL", "AIRCEL" },
	{ 404, 18, "Reliance", "Reliance" },
	{ 404, 19, "IDEA", "IDEA" },
	{ 404, 20, "Vodafone IN", "Vodafone IN" },
	{ 404, 21, "Loop Mobile", "Loop Mobile" },
	{ 404, 22, "IDEA", "IDEA" },
	{ 404, 24, "IDEA", "IDEA" },
	{ 404, 25, "AIRCEL", "AIRCEL" },
	{ 404, 27, "Vodafone IN", "Vodafone IN" },
	{ 404, 28, "AIRCEL", "AIRCEL" },
	{ 404, 29, "AIRCEL", "AIRCEL" },
	{ 404, 30, "Vodafone IN", "Vodafone IN" },
	{ 404, 31, "AirTel", "AirTel" },
	{ 404, 34, "CellOne", "CellOne" },
	{ 404, 36, "Reliance", "Reliance" },
	{ 404, 37, "Aircel", "Aircel" },
	{ 404, 38, "CellOne", "CellOne" },
	{ 404, 40, "AirTel", "AirTel" },
	{ 404, 41, "Aircel", "Aircel" },
	{ 404, 42, "Aircel", "Aircel" },
	{ 404, 43, "Vodafone IN", "Vodafone IN" },
	{ 404, 44, "IDEA", "IDEA" },
	{ 404, 45, "Airtel", "Airtel" },
	{ 404, 46, "Vodafone IN", "Vodafone IN" },
	{ 404, 48, "Dishnet Wireless", "Dishnet Wireless" },
	{ 404, 49, "Airtel", "Airtel" },
	{ 404, 50, "Reliance", "Reliance" },
	{ 404, 51, "CellOne", "CellOne" },
	{ 404, 52, "Reliance", "Reliance" },
	{ 404, 53, "CellOne", "CellOne" },
	{ 404, 54, "CellOne", "CellOne" },
	{ 404, 55, "CellOne", "CellOne" },
	{ 404, 56, "IDEA", "IDEA" },
	{ 404, 57, "CellOne", "CellOne" },
	{ 404, 58, "CellOne", "CellOne" },
	{ 404, 59, "CellOne", "CellOne" },
	{ 404, 60, "Vodafone IN", "Vodafone IN" },
	{ 404, 62, "CellOne", "CellOne" },
	{ 404, 64, "CellOne", "CellOne" },
	{ 404, 66, "CellOne", "CellOne" },
	{ 404, 67, "Reliance", "Reliance" },
	{ 404, 68, "DOLPHIN", "DOLPHIN" },
	{ 404, 69, "DOLPHIN", "DOLPHIN" },
	{ 404, 70, "AirTel", "AirTel" },
	{ 404, 71, "CellOne", "CellOne" },
	{ 404, 72, "CellOne", "CellOne" },
	{ 404, 73, "CellOne", "CellOne" },
	{ 404, 74, "CellOne", "CellOne" },
	{ 404, 75, "CellOne", "CellOne" },
	{ 404, 76, "CellOne", "CellOne" },
	{ 404, 77, "CellOne", "CellOne" },
	{ 404, 78, "Idea Cellular Ltd", "Idea Cellular Ltd" },
	{ 404, 79, "CellOne", "CellOne" },
	{ 404, 80, "CellOne", "CellOne" },
	{ 404, 81, "CellOne", "CellOne" },
	{ 404, 82, "Idea", "Idea" },
	{ 404, 83, "Reliance", "Reliance" },
	{ 404, 84, "Vodafone IN", "Vodafone IN" },
	{ 404, 85, "Reliance", "Reliance" },
	{ 404, 86, "Vodafone IN", "Vodafone IN" },
	{ 404, 87, "IDEA", "IDEA" },
	{ 404, 88, "Vodafone IN", "Vodafone IN" },
	{ 404, 89, "Idea", "Idea" },
	{ 404, 90, "AirTel", "AirTel" },
	{ 404, 91, "AIRCEL", "AIRCEL" },
	{ 404, 92, "AirTel", "AirTel" },
	{ 404, 93, "AirTel", "AirTel" },
	{ 404, 94, "AirTel", "AirTel" },
	{ 404, 95, "AirTel", "AirTel" },
	{ 404, 96, "AirTel", "AirTel" },
	{ 404, 97, "AirTel", "AirTel" },
	{ 404, 98, "AirTel", "AirTel" },
	{ 405, 1, "Reliance", "Reliance" },
	{ 405, 25, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 26, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 27, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 28, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 29, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 3, "Reliance", "Reliance" },
	{ 405, 30, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 31, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 32, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 33, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 34, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 35, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 36, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 37, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 38, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 39, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 4, "Reliance", "Reliance" },
	{ 405, 41, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 42, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 43, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 44, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 45, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 46, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 47, "TATA DOCOMO", "TATA DOCOMO" },
	{ 405, 5, "Reliance", "Reliance" },
	{ 405, 6, "Reliance", "Reliance" },
	{ 405, 7, "Reliance", "Reliance" },
	{ 405, 8, "Reliance", "Reliance" },
	{ 405, 9, "Reliance", "Reliance" },
	{ 405, 10, "Reliance", "Reliance" },
	{ 405, 11, "Reliance", "Reliance" },
	{ 405, 12, "Reliance", "Reliance" },
	{ 405, 13, "Reliance", "Reliance" },
	{ 405, 14, "Reliance", "Reliance" },
	{ 405, 15, "Reliance", "Reliance" },
	{ 405, 17, "Reliance", "Reliance" },
	{ 405, 18, "Reliance", "Reliance" },
	{ 405, 19, "Reliance", "Reliance" },
	{ 405, 20, "Reliance", "Reliance" },
	{ 405, 21, "Reliance", "Reliance" },
	{ 405, 22, "Reliance", "Reliance" },
	{ 405, 23, "Reliance", "Reliance" },
	{ 405, 51, "AirTel", "AirTel" },
	{ 405, 52, "AirTel", "AirTel" },
	{ 405, 53, "AirTel", "AirTel" },
	{ 405, 54, "AirTel", "AirTel" },
	{ 405, 55, "Airtel", "Airtel" },
	{ 405, 56, "AirTel", "AirTel" },
	{ 405, 66, "Vodafone IN", "Vodafone IN" },
	{ 405, 67, "Vodafone IN", "Vodafone IN" },
	{ 405, 70, "IDEA", "IDEA" },
	{ 405, 750, "Vodafone IN", "Vodafone IN" },
	{ 405, 751, "Vodafone IN", "Vodafone IN" },
	{ 405, 752, "Vodafone IN", "Vodafone IN" },
	{ 405, 753, "Vodafone IN", "Vodafone IN" },
	{ 405, 754, "Vodafone IN", "Vodafone IN" },
	{ 405, 755, "Vodafone IN", "Vodafone IN" },
	{ 405, 756, "Vodafone IN", "Vodafone IN" },
	{ 405, 799, "IDEA", "IDEA" },
	{ 405, 800, "AIRCEL", "AIRCEL" },
	{ 405, 801, "AIRCEL", "AIRCEL" },
	{ 405, 802, "AIRCEL", "AIRCEL" },
	{ 405, 803, "AIRCEL", "AIRCEL" },
	{ 405, 804, "AIRCEL", "AIRCEL" },
	{ 405, 805, "AIRCEL", "AIRCEL" },
	{ 405, 806, "AIRCEL", "AIRCEL" },
	{ 405, 807, "AIRCEL", "AIRCEL" },
	{ 405, 808, "AIRCEL", "AIRCEL" },
	{ 405, 809, "AIRCEL", "AIRCEL" },
	{ 405, 810, "AIRCEL", "AIRCEL" },
	{ 405, 811, "AIRCEL", "AIRCEL" },
	{ 405, 812, "AIRCEL", "AIRCEL" },
	{ 405, 819, "Uninor", "Uninor" },
	{ 405, 818, "Uninor", "Uninor" },
	{ 405, 820, "Uninor", "Uninor" },
	{ 405, 821, "Uninor", "Uninor" },
	{ 405, 822, "Uninor", "Uninor" },
	{ 405, 824, "Videocon Datacom", "Videocon Datacom" },
	{ 405, 827, "Videocon Datacom", "Videocon Datacom" },
	{ 405, 834, "Videocon Datacom", "Videocon Datacom" },
	{ 405, 844, "Uninor", "Uninor" },
	{ 405, 840, "Jio", "Jio" },
	{ 405, 845, "IDEA", "IDEA" },
	{ 405, 846, "IDEA", "IDEA" },
	{ 405, 847, "IDEA", "IDEA" },
	{ 405, 848, "IDEA", "IDEA" },
	{ 405, 849, "IDEA", "IDEA" },
	{ 405, 850, "IDEA", "IDEA" },
	{ 405, 851, "IDEA", "IDEA" },
	{ 405, 852, "IDEA", "IDEA" },
	{ 405, 853, "IDEA", "IDEA" },
	{ 405, 854, "Jio", "Jio" },
	{ 405, 855, "Jio", "Jio" },
	{ 405, 856, "Jio", "Jio" },
	{ 405, 857, "Jio", "Jio" },
	{ 405, 858, "Jio", "Jio" },
	{ 405, 859, "Jio", "Jio" },
	{ 405, 860, "Jio", "Jio" },
	{ 405, 861, "Jio", "Jio" },
	{ 405, 862, "Jio", "Jio" },
	{ 405, 863, "Jio", "Jio" },
	{ 405, 864, "Jio", "Jio" },
	{ 405, 865, "Jio", "Jio" },
	{ 405, 866, "Jio", "Jio" },
	{ 405, 867, "Jio", "Jio" },
	{ 405, 868, "Jio", "Jio" },
	{ 405, 869, "Jio", "Jio" },
	{ 405, 870, "Jio", "Jio" },
	{ 405, 871, "Jio", "Jio" },
	{ 405, 872, "Jio", "Jio" },
	{ 405, 873, "Jio", "Jio" },
	{ 405, 874, "Jio", "Jio" },
	{ 405, 875, "Uninor", "Uninor" },
	{ 405, 880, "Uninor", "Uninor" },
	{ 405, 881, "S Tel", "S Tel" },
	{ 405, 908, "IDEA", "IDEA" },
	{ 405, 909, "IDEA", "IDEA" },
	{ 405, 910, "IDEA", "IDEA" },
	{ 405, 911, "IDEA", "IDEA" },
	{ 405, 912, "Etisalat DB(cheers)", "Etisalat DB(cheers)" },
	{ 405, 913, "Etisalat DB(cheers)", "Etisalat DB(cheers)" },
	{ 405, 914, "Etisalat DB(cheers)", "Etisalat DB(cheers)" },
	{ 405, 917, "Etisalat DB(cheers)", "Etisalat DB(cheers)" },
	{ 405, 927, "Uninor", "Uninor" },
	{ 405, 929, "Uninor", "Uninor" },

	// Indonesia - ID

	{ 510, 1, "INDOSAT", "INDOSAT" },
	{ 510, 3, "StarOne", "StarOne" },
	{ 510, 7, "TelkomFlexi", "TelkomFlexi" },
	{ 510, 8, "AXIS", "AXIS" },
	{ 510, 9, "SMARTFREN", "SMARTFREN" },
	{ 510, 10, "Telkomsel", "Telkomsel" },
	{ 510, 11, "XL", "XL" },
	{ 510, 20, "TELKOMMobile", "TELKOMMobile" },
	{ 510, 21, "IM3", "IM3" },
	{ 510, 27, "Ceria", "Ceria" },
	{ 510, 28, "Fren/Hepi", "Fren/Hepi" },
	{ 510, 89, "3", "3" },
	{ 510, 99, "Esia", "Esia" },
	{ 510, 995, "Komselindo", "Komselindo" },
	{ 510, 996, "Komselindo", "Komselindo" },

	// Iran - IR

	{ 432, 11, "IR-MCI", "IR-MCI" },
	{ 432, 14, "TKC", "TKC" },
	{ 432, 19, "MTCE", "MTCE" },
	{ 432, 20, "Rightel", "Rightel" },
	{ 432, 32, "Taliya", "Taliya" },
	{ 432, 35, "Irancell", "Irancell" },
	{ 432, 70, "TCI", "TCI" },
	{ 432, 93, "Iraphone", "Iraphone" },

	// Iraq - IQ

	{ 418, 5, "Asia Cell", "Asia Cell" },
	{ 418, 8, "SanaTel", "SanaTel" },
	{ 418, 20, "Zain", "Zain" },
	{ 418, 30, "Zain", "Zain" },
	{ 418, 40, "Korek", "Korek" },
	{ 418, 45, "Mobitel", "Mobitel" },
	{ 418, 62, "Itisaluna", "Itisaluna" },
	{ 418, 92, "Omnnea", "Omnnea" },

	// Ireland - IE

	{ 272, 1, "Vodafone", "Vodafone" },
	{ 272, 2, "O2", "O2" },
	{ 272, 3, "Meteor", "Meteor" },
	{ 272, 4, "Access Telecom", "Access Telecom" },
	{ 272, 5, "3", "3" },
	{ 272, 7, "eMobile", "eMobile" },
	{ 272, 9, "Clever Communications", "Clever Communications" },
	{ 272, 11, "Tesco Mobile", "Tesco Mobile" },
	{ 272, 13, "Lycamobile", "Lycamobile" },

	// Isle of Man (United Kingdom) - IM

	{ 234, 9, "Sure Mobile", "Sure Mobile" },
	{ 234, 58, "Pronto GSM", "Pronto GSM" },

	// Israel - IL

	{ 425, 1, "Orange", "Orange" },
	{ 425, 2, "Cellcom", "Cellcom" },
	{ 425, 3, "Pelephone", "Pelephone" },
	{ 425, 7, "Hot Mobile", "Hot Mobile" },
	{ 425, 8, "Golan Telecom", "Golan Telecom" },
	{ 425, 11, "365 Telecom", "365 Telecom" },
	{ 425, 12, "Free Telecom", "Free Telecom" },
	{ 425, 13, "Ituran Cellular Communications", "Ituran Cellular Communications" },
	{ 425, 14, "Youphone", "Youphone" },
	{ 425, 15, "Home Cellular", "Home Cellular" },
	{ 425, 16, "Rami Levy", "Rami Levy" },
	{ 425, 17, "Gale Phone", "Gale Phone" },
	{ 425, 18, "Cellact Communications", "Cellact Communications" },
	{ 425, 19, "Azi Communications Ltd.", "Azi Communications Ltd." },
	{ 425, 20, "Bezeq Ltd", "Bezeq Ltd" },
	{ 425, 21, "B.I.P. Communications Ltd.", "B.I.P. Communications Ltd." },

	// Italy - IT

	{ 222, 1, "TIM", "TIM" },
	{ 222, 2, "Elsacom", "Elsacom" },
	{ 222, 4, "Intermatica", "Intermatica" },
	{ 222, 5, "Telespazio", "Telespazio" },
	{ 222, 7, "Noverca", "Noverca" },
	{ 222, 10, "Vodafone", "Vodafone" },
	{ 222, 30, "RFI", "RFI" },
	{ 222, 34, "BT Italia", "BT Italia" },
	{ 222, 35, "Lyca Italy", "Lyca Italy" },
	{ 222, 77, "IPSE 2000", "IPSE 2000" },
	{ 222, 88, "Wind", "Wind" },
	{ 222, 98, "Blu", "Blu" },
	{ 222, 99, "3 Italia", "3 Italia" },

	// Ivory Coast - CI

	{ 612, 1, "Cora de Comstar", "Cora de Comstar" },
	{ 612, 2, "Moov", "Moov" },
	{ 612, 3, "Orange", "Orange" },
	{ 612, 4, "KoZ", "KoZ" },
	{ 612, 5, "MTN", "MTN" },
	{ 612, 6, "ORICEL", "ORICEL" },

	// Jamaica - JM

	{ 338, 20, "LIME", "LIME" },
	{ 338, 50, "Digicel", "Digicel" },
	{ 338, 110, "LIME", "LIME" },
	{ 338, 180, "LIME", "LIME" },

	// Japan - JP

	{ 440, 1, "NTT docomo", "NTT docomo" },
	{ 440, 2, "NTT docomo", "NTT docomo" },
	{ 440, 3, "NTT docomo", "NTT docomo" },
	{ 440, 4, "SoftBank", "SoftBank" },
	{ 440, 6, "SoftBank", "SoftBank" },
	{ 440, 7, "KDDI", "KDDI" },
	{ 440, 8, "KDDI", "KDDI" },
	{ 440, 9, "NTT docomo", "NTT docomo" },
	{ 440, 10, "NTT docomo", "NTT docomo" },
	{ 440, 11, "NTT docomo", "NTT docomo" },
	{ 440, 12, "NTT docomo", "NTT docomo" },
	{ 440, 13, "NTT docomo", "NTT docomo" },
	{ 440, 14, "NTT docomo", "NTT docomo" },
	{ 440, 15, "NTT docomo", "NTT docomo" },
	{ 440, 16, "NTT docomo", "NTT docomo" },
	{ 440, 17, "NTT docomo", "NTT docomo" },
	{ 440, 18, "NTT docomo", "NTT docomo" },
	{ 440, 19, "NTT docomo", "NTT docomo" },
	{ 440, 20, "SoftBank", "SoftBank" },
	{ 440, 21, "NTT docomo", "NTT docomo" },
	{ 440, 22, "NTT docomo", "NTT docomo" },
	{ 440, 23, "NTT docomo", "NTT docomo" },
	{ 440, 24, "NTT docomo", "NTT docomo" },
	{ 440, 25, "NTT docomo", "NTT docomo" },
	{ 440, 26, "NTT docomo", "NTT docomo" },
	{ 440, 27, "NTT docomo", "NTT docomo" },
	{ 440, 28, "NTT docomo", "NTT docomo" },
	{ 440, 29, "NTT docomo", "NTT docomo" },
	{ 440, 30, "NTT docomo", "NTT docomo" },
	{ 440, 31, "NTT docomo", "NTT docomo" },
	{ 440, 32, "NTT docomo", "NTT docomo" },
	{ 440, 33, "NTT docomo", "NTT docomo" },
	{ 440, 34, "NTT docomo", "NTT docomo" },
	{ 440, 35, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 36, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 37, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 38, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 39, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 40, "SoftBank", "SoftBank" },
	{ 440, 41, "SoftBank", "SoftBank" },
	{ 440, 42, "SoftBank", "SoftBank" },
	{ 440, 43, "SoftBank", "SoftBank" },
	{ 440, 44, "SoftBank", "SoftBank" },
	{ 440, 45, "SoftBank", "SoftBank" },
	{ 440, 46, "SoftBank", "SoftBank" },
	{ 440, 47, "SoftBank", "SoftBank" },
	{ 440, 48, "SoftBank", "SoftBank" },
	{ 440, 49, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 50, "KDDI", "KDDI" },
	{ 440, 51, "KDDI", "KDDI" },
	{ 440, 52, "KDDI", "KDDI" },
	{ 440, 53, "KDDI", "KDDI" },
	{ 440, 54, "KDDI", "KDDI" },
	{ 440, 55, "KDDI", "KDDI" },
	{ 440, 56, "KDDI", "KDDI" },
	{ 440, 58, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 60, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 61, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 62, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 63, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 64, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 65, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 66, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 67, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 68, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 69, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 70, "au", "au" },
	{ 440, 71, "KDDI", "KDDI" },
	{ 440, 72, "KDDI", "KDDI" },
	{ 440, 73, "KDDI", "KDDI" },
	{ 440, 74, "KDDI", "KDDI" },
	{ 440, 75, "KDDI", "KDDI" },
	{ 440, 76, "KDDI", "KDDI" },
	{ 440, 77, "KDDI", "KDDI" },
	{ 440, 78, "Okinawa Cellular Telephone", "Okinawa Cellular Telephone" },
	{ 440, 79, "KDDI", "KDDI" },
	{ 440, 80, "TU-KA", "TU-KA" },
	{ 440, 81, "TU-KA", "TU-KA" },
	{ 440, 82, "TU-KA", "TU-KA" },
	{ 440, 83, "TU-KA", "TU-KA" },
	{ 440, 84, "TU-KA", "TU-KA" },
	{ 440, 85, "TU-KA", "TU-KA" },
	{ 440, 86, "TU-KA", "TU-KA" },
	{ 440, 87, "NTT DoCoMo", "NTT DoCoMo" },
	{ 440, 88, "KDDI", "KDDI" },
	{ 440, 89, "KDDI", "KDDI" },
	{ 440, 90, "SoftBank", "SoftBank" },
	{ 440, 92, "SoftBank", "SoftBank" },
	{ 440, 93, "SoftBank", "SoftBank" },
	{ 440, 94, "SoftBank", "SoftBank" },
	{ 440, 95, "SoftBank", "SoftBank" },
	{ 440, 96, "SoftBank", "SoftBank" },
	{ 440, 97, "SoftBank", "SoftBank" },
	{ 440, 98, "SoftBank", "SoftBank" },
	{ 440, 99, "NTT DoCoMo", "NTT DoCoMo" },

	// Jersey (United Kingdom) - JE

	{ 234, 3, "Airtel Vodafone", "Airtel Vodafone" },
	{ 234, 50, "JT", "JT" },
	{ 234, 55, "Sure Mobile", "Sure Mobile" },

	// Jordan - JO

	{ 416, 1, "zain JO", "zain JO" },
	{ 416, 2, "XPress Telecom", "XPress Telecom" },
	{ 416, 3, "Umniah", "Umniah" },
	{ 416, 77, "Orange", "Orange" },

	// Kazakhstan - KZ

	{ 401, 1, "Beeline", "Beeline" },
	{ 401, 2, "Kcell", "Kcell" },
	{ 401, 7, "Dalacom", "Dalacom" },
	{ 401, 8, "Kazakhtelecom", "Kazakhtelecom" },
	{ 401, 77, "Tele2.kz", "Tele2.kz" },

	// Kenya - KE

	{ 639, 2, "Safaricom", "Safaricom" },
	{ 639, 3, "Airtel", "Airtel" },
	{ 639, 5, "yu", "yu" },
	{ 639, 7, "Orange Kenya", "Orange Kenya" },

	// Kiribati - KI

	{ 545, 1, "Kiribati - TSKL", "Kiribati - TSKL" },
	{ 545, 9, "Kiribati - Frigate Net", "Kiribati - Frigate Net" },

	// North Korea - KP

	{ 467, 5, "Koryolink", "Koryolink" },
	{ 467, 193, "SunNet", "SunNet" },

	// South Korea - KR

	{ 450, 2, "KT", "KT" },
	{ 450, 3, "Power 017", "Power 017" },
	{ 450, 4, "KT", "KT" },
	{ 450, 5, "SKTelecom", "SKTelecom" },
	{ 450, 6, "LG U+", "LG U+" },
	{ 450, 8, "olleh", "olleh" },
	{ 450, 11, "SKTelecom", "SKTelecom" },

	// Kosovo - XK

	{ 212, 1, "Vala", "Vala" },
	{ 212, 1, "Z Mobile", "Z Mobile" },
	{ 293, 41, "IPKO", "IPKO" },
	{ 293, 41, "D3 Mobile", "D3 Mobile" },

	// Kuwait - KW

	{ 419, 2, "zain KW", "zain KW" },
	{ 419, 3, "Wataniya", "Wataniya" },
	{ 419, 4, "Viva", "Viva" },

	// Kyrgyzstan - KG

	{ 437, 1, "Beeline", "Beeline" },
	{ 437, 3, "Fonex", "Fonex" },
	{ 437, 5, "MegaCom", "MegaCom" },
	{ 437, 9, "O!", "O!" },

	// Laos - LA

	{ 457, 1, "LTC", "LTC" },
	{ 457, 2, "ETL", "ETL" },
	{ 457, 3, "Unitel", "Unitel" },
	{ 457, 8, "Beeline", "Beeline" },

	// Latvia - LV

	{ 247, 1, "LMT", "LMT" },
	{ 247, 2, "Tele2", "Tele2" },
	{ 247, 3, "TRIATEL", "TRIATEL" },
	{ 247, 4, "Beta Telecom", "Beta Telecom" },
	{ 247, 5, "Bite", "Bite" },
	{ 247, 6, "Rigatta", "Rigatta" },
	{ 247, 7, "MTS", "MTS" },
	{ 247, 8, "IZZI", "IZZI" },
	{ 247, 9, "Camel Mobile", "Camel Mobile" },

	// Lebanon - LB

	{ 415, 1, "Alfa", "Alfa" },
	{ 415, 3, "mtc touch", "mtc touch" },
	{ 415, 5, "Ogero Mobile", "Ogero Mobile" },

	// Lesotho - LS

	{ 651, 1, "Vodacom", "Vodacom" },
	{ 651, 2, "Econet Ezi-cel", "Econet Ezi-cel" },

	// Liberia - LR

	{ 618, 1, "Lonestar Cell", "Lonestar Cell" },
	{ 618, 2, "Libercell", "Libercell" },
	{ 618, 4, "Comium", "Comium" },
	{ 618, 7, "Cellcom", "Cellcom" },
	{ 618, 20, "LIBTELCO", "LIBTELCO" },

	// Libya - LY

	{ 606, 1, "Madar", "Madar" },
	{ 606, 2, "Al-Jeel Phone", "Al-Jeel Phone" },
	{ 606, 3, "Libya Phone", "Libya Phone" },
	{ 606, 6, "Hatef Libya", "Hatef Libya" },

	// Liechtenstein - LI

	{ 295, 1, "Swisscom", "Swisscom" },
	{ 295, 2, "Orange", "Orange" },
	{ 295, 5, "FL1", "FL1" },
	{ 295, 6, "Cubic Telecom", "Cubic Telecom" },
	{ 295, 77, "Alpmobil", "Alpmobil" },

	// Lithuania - LT

	{ 246, 1, "Omnitel", "Omnitel" },
	{ 246, 2, "BITĖ", "BITĖ" },
	{ 246, 3, "Tele 2", "Tele 2" },
	{ 246, 5, "LitRail", "LitRail" },
	{ 246, 6, "Mediafon", "Mediafon" },

	// Luxembourg - LU

	{ 270, 1, "POST", "POST" },
	{ 270, 2, "MTX Connect S.a.r.l.", "MTX Connect S.a.r.l." },
	{ 270, 10, "Blue Communications", "Blue Communications" },
	{ 270, 77, "Tango", "Tango" },
	{ 270, 78, "Interactive digital media GmbH", "Interactive digital media GmbH" },
	{ 270, 99, "Orange", "Orange" },

	// Macau (People's Republic of China) - MO

	{ 455, 1, "CTM", "CTM" },
	{ 455, 2, "China Telecom", "China Telecom" },
	{ 455, 3, "3", "3" },
	{ 455, 4, "CTM", "CTM" },
	{ 455, 5, "3", "3" },
	{ 455, 6, "SmarTone", "SmarTone" },

	// Macedonia - MK

	{ 294, 1, "T-Mobile MK", "T-Mobile MK" },
	{ 294, 2, "ONE", "ONE" },
	{ 294, 3, "Vip MK", "Vip MK" },

	// Madagascar - MG

	{ 646, 1, "Airtel", "Airtel" },
	{ 646, 2, "Orange", "Orange" },
	{ 646, 3, "Sacel", "Sacel" },
	{ 646, 4, "Telma", "Telma" },

	// Malawi - MW

	{ 650, 1, "TNM", "TNM" },
	{ 650, 10, "Airtel", "Airtel" },

	// Malaysia - MY

	{ 502, 1, "ATUR 450", "ATUR 450" },
	{ 502, 10, "DiGi Telecommunications", "DiGi Telecommunications" },
	{ 502, 11, "TM Homeline", "TM Homeline" },
	{ 502, 12, "Maxis", "Maxis" },
	{ 502, 13, "Celcom", "Celcom" },
	{ 502, 14, "Telekom Malaysia Berhad for PSTN SMS", "Telekom Malaysia Berhad for PSTN SMS" },
	{ 502, 150, "Tune Talk", "Tune Talk" },
	{ 502, 151, "Baraka Telecom Sdn Bhd (MVNE)", "Baraka Telecom Sdn Bhd (MVNE)" },
	{ 502, 152, "Yes", "Yes" },
	{ 502, 156, "Altel", "Altel" },
	{ 502, 16, "DiGi", "DiGi" },
	{ 502, 17, "Hotlink", "Hotlink" },
	{ 502, 18, "U Mobile", "U Mobile" },
	{ 502, 18, "TM Homeline", "TM Homeline" },
	{ 502, 19, "Celcom", "Celcom" },
	{ 502, 20, "Electcoms Wireless Sdn Bhd", "Electcoms Wireless Sdn Bhd" },
	{ 502, 12, "Kartu As", "Kartu As" },

	// Maldives - MV

	{ 472, 1, "Dhiraagu", "Dhiraagu" },
	{ 472, 2, "Ooredoo", "Ooredoo" },

	// Mali - ML

	{ 610, 1, "Malitel", "Malitel" },
	{ 610, 2, "Orange", "Orange" },

	// Malta - MT

	{ 278, 1, "Vodafone", "Vodafone" },
	{ 278, 21, "GO", "GO" },
	{ 278, 77, "Melita", "Melita" },

	// Marshall Islands - MH

	{ 551, 1, "Marshall Islands National Telecommunications Authority (MINTA)", "Marshall Islands National Telecommunications Authority (MINTA)" },

	// Martinique (France) - MQ

	{ 340, 1, "Orange", "Orange" },
	{ 340, 2, "Outremer", "Outremer" },
	{ 340, 20, "Digicel", "Digicel" },

	// Mauritania - MR

	{ 609, 1, "Mattel", "Mattel" },
	{ 609, 2, "Chinguitel", "Chinguitel" },
	{ 609, 10, "Mauritel", "Mauritel" },

	// Mauritius - MU

	{ 617, 1, "Orange", "Orange" },
	{ 617, 2, "MTML", "MTML" },
	{ 617, 10, "Emtel", "Emtel" },

	// Mayotte (France) - YT


	// Mexico - MX

	{ 334, 10, "Nextel", "Nextel" },
	{ 334, 20, "Telcel", "Telcel" },
	{ 334, 30, "movistar", "movistar" },
	{ 334, 30, "Virgin Mobile", "Virgin Mobile" },
	{ 334, 40, "Iusacell / Unefon", "Iusacell / Unefon" },
	{ 334, 50, "Iusacell", "Iusacell" },
	{ 334, 90, "Nextel", "Nextel" },

	// Federated States of Micronesia - FM

	{ 550, 1, "FSMTC", "FSMTC" },

	// Moldova - MD

	{ 259, 1, "Orange", "Orange" },
	{ 259, 2, "Moldcell", "Moldcell" },
	{ 259, 3, "IDC", "IDC" },
	{ 259, 3, "Unité", "Unité" },
	{ 259, 4, "Eventis", "Eventis" },
	{ 259, 5, "Unité", "Unité" },
	{ 259, 99, "Unité", "Unité" },

	// Monaco - MC

	{ 212, 1, "Office des Telephones", "Office des Telephones" },

	// Mongolia - MN

	{ 428, 88, "Unitel", "Unitel" },
	{ 428, 91, "Skytel", "Skytel" },
	{ 428, 98, "G.Mobile", "G.Mobile" },
	{ 428, 99, "MobiCom", "MobiCom" },

	// Montenegro - ME

	{ 297, 1, "Telenor", "Telenor" },
	{ 297, 2, "T-Mobile", "T-Mobile" },
	{ 297, 3, "m:tel CG", "m:tel CG" },
	{ 297, 4, "T-Mobile", "T-Mobile" },

	// Montserrat (United Kingdom) - MS

	{ 354, 860, "Cable & Wireless", "Cable & Wireless" },

	// Morocco - MA

	{ 604, 1, "IAM", "IAM" },
	{ 604, 2, "Wana Corporate", "Wana Corporate" },
	{ 604, 5, "INWI", "INWI" },

	// Mozambique - MZ

	{ 643, 1, "mCel", "mCel" },
	{ 643, 3, "Movitel", "Movitel" },
	{ 643, 4, "Vodacom", "Vodacom" },

	// Myanmar - MM

	{ 414, 1, "MPT", "MPT" },
	{ 414, 5, "Ooredoo", "Ooredoo" },
	{ 414, 6, "NA", "NA" },

	// Namibia - NA

	{ 649, 1, "MTC", "MTC" },
	{ 649, 2, "switch", "switch" },
	{ 649, 3, "TN Mobile", "TN Mobile" },

	// Nauru - NR

	{ 536, 2, "Digicel", "Digicel" },

	// Nepal - NP

	{ 429, 1, "Namaste / NT Mobile", "Namaste / NT Mobile" },
	{ 429, 2, "Ncell", "Ncell" },
	{ 429, 3, "Sky/C-Phone", "Sky/C-Phone" },
	{ 429, 4, "SmartCell", "SmartCell" },

	// Netherlands (Kingdom of the Netherlands) - NL

	{ 204, 1, "RadioAccess Network Services BV", "RadioAccess Network Services BV" },
	{ 204, 2, "Tele2", "Tele2" },
	{ 204, 3, "Voiceworks", "Voiceworks" },
	{ 204, 4, "Vodafone", "Vodafone" },
	{ 204, 5, "Elephant Talk Communications Premium Rate Services", "Elephant Talk Communications Premium Rate Services" },
	{ 204, 6, "Vectone Mobile Delight Mobile", "Vectone Mobile Delight Mobile" },
	{ 204, 7, "Teleena (MVNE)", "Teleena (MVNE)" },
	{ 204, 8, "KPN", "KPN" },
	{ 204, 9, "Lycamobile", "Lycamobile" },
	{ 204, 10, "KPN", "KPN" },
	{ 204, 11, "VoipIT B.V.", "VoipIT B.V." },
	{ 204, 12, "Telfort", "Telfort" },
	{ 204, 13, "Unica Installatietechniek B.V.", "Unica Installatietechniek B.V." },
	{ 204, 14, "6GMOBILE B.V.", "6GMOBILE B.V." },
	{ 204, 15, "Ziggo", "Ziggo" },
	{ 204, 16, "T-Mobile (BEN)", "T-Mobile (BEN)" },
	{ 204, 17, "Intercity Zakelijk", "Intercity Zakelijk" },
	{ 204, 18, "UPC Nederland B.V.", "UPC Nederland B.V." },
	{ 204, 19, "Mixe Communication Solutions B.V.", "Mixe Communication Solutions B.V." },
	{ 204, 20, "T-Mobile", "T-Mobile" },
	{ 204, 21, "ProRail B.V.", "ProRail B.V." },
	{ 204, 22, "Ministerie van Defensie", "Ministerie van Defensie" },
	{ 204, 23, "ASPIDER Solutions Nederland B.V.", "ASPIDER Solutions Nederland B.V." },
	{ 204, 24, "Private Mobility Nederland B.V.", "Private Mobility Nederland B.V." },
	{ 204, 25, "CapX B.V.", "CapX B.V." },
	{ 204, 26, "SpeakUp B.V.", "SpeakUp B.V." },
	{ 204, 27, "Breezz Nederland B.V.", "Breezz Nederland B.V." },
	{ 204, 28, "Lancelot B.V.", "Lancelot B.V." },
	{ 204, 60, "Nextgen Mobile Ltd", "Nextgen Mobile Ltd" },
	{ 204, 61, "BodyTrace Netherlands B.V.", "BodyTrace Netherlands B.V." },
	{ 204, 62, "Voxbone", "Voxbone" },
	{ 204, 64, "Zetacom B.V.", "Zetacom B.V." },
	{ 204, 65, "AGMS Netherlands B.V.", "AGMS Netherlands B.V." },
	{ 204, 66, "Utility Connect B.V.", "Utility Connect B.V." },
	{ 204, 67, "RadioAccess B.V.", "RadioAccess B.V." },
	{ 204, 68, "Roamware (Netherlands) B.V.", "Roamware (Netherlands) B.V." },
	{ 204, 69, "KPN Mobile The Netherlands B.V.", "KPN Mobile The Netherlands B.V." },

	// Former Netherlands Antilles (Kingdom of the Netherlands) - AN

	{ 362, 51, "Telcell", "Telcell" },
	{ 362, 69, "Digicel", "Digicel" },
	{ 362, 91, "UTS", "UTS" },
	{ 362, 94, "Bayòs", "Bayòs" },
	{ 362, 95, "MIO", "MIO" },

	// New Caledonia (France) - NC

	{ 546, 1, "Mobilis", "Mobilis" },

	// New Zealand - NZ

	{ 530, 1, "Vodafone", "Vodafone" },
	{ 530, 2, "Telecom", "Telecom" },
	{ 530, 3, "Woosh", "Woosh" },
	{ 530, 4, "TelstraClear", "TelstraClear" },
	{ 530, 5, "XT Mobile Network", "XT Mobile Network" },
	{ 530, 6, "Skinny", "Skinny" },
	{ 530, 24, "2degrees", "2degrees" },

	// Nicaragua - NI

	{ 710, 21, "Claro", "Claro" },
	{ 710, 30, "movistar", "movistar" },
	{ 710, 73, "SERCOM", "SERCOM" },

	// Niger - NE

	{ 614, 1, "SahelCom", "SahelCom" },
	{ 614, 2, "Airtel", "Airtel" },
	{ 614, 3, "Moov", "Moov" },
	{ 614, 4, "Orange", "Orange" },

	// Nigeria - NG

	{ 621, 20, "Airtel", "Airtel" },
	{ 621, 25, "Visafone", "Visafone" },
	{ 621, 30, "MTN", "MTN" },
	{ 621, 40, "M-Tel", "M-Tel" },
	{ 621, 50, "Glo", "Glo" },
	{ 621, 60, "Etisalat", "Etisalat" },

	// Niue - NU

	{ 555, 1, "Telecom Niue", "Telecom Niue" },

	// Norfolk Island - NF

	{ 505, 10, "Norfolk Telecom", "Norfolk Telecom" },

	// Northern Mariana Islands - MP


	// Norway - NO

	{ 242, 1, "Telenor", "Telenor" },
	{ 242, 2, "NetCom", "NetCom" },
	{ 242, 3, "Teletopia", "Teletopia" },
	{ 242, 4, "Tele2", "Tele2" },
	{ 242, 5, "Network Norway", "Network Norway" },
	{ 242, 6, "Ice", "Ice" },
	{ 242, 7, "Ventelo", "Ventelo" },
	{ 242, 8, "TDC", "TDC" },
	{ 242, 9, "Com4", "Com4" },
	{ 242, 11, "SystemNet", "SystemNet" },
	{ 242, 12, "Telenor", "Telenor" },
	{ 242, 20, "Jernbaneverket AS", "Jernbaneverket AS" },
	{ 242, 21, "Jernbaneverket AS", "Jernbaneverket AS" },
	{ 242, 23, "Lyca", "Lyca" },
	{ 242, 24, "Mobile Norway AS", "Mobile Norway AS" },

	// Oman - OM

	{ 422, 2, "Oman Mobile", "Oman Mobile" },
	{ 422, 3, "Nawras", "Nawras" },

	// Pakistan - PK

	{ 410, 1, "Mobilink", "Mobilink" },
	{ 410, 3, "Ufone", "Ufone" },
	{ 410, 4, "Zong", "Zong" },
	{ 410, 5, "SCO Mobile", "SCO Mobile" },
	{ 410, 6, "Telenor", "Telenor" },
	{ 410, 7, "Warid Pakistan", "Warid Pakistan" },

	// Palau - PW

	{ 552, 1, "PNCC", "PNCC" },
	{ 552, 80, "Palau Mobile", "Palau Mobile" },

	// Palestine, State of - PS

	{ 425, 5, "Jawwal", "Jawwal" },
	{ 425, 6, "Wataniya", "Wataniya" },

	// Panama - PA

	{ 714, 1, "Cable & Wireless", "Cable & Wireless" },
	{ 714, 2, "movistar", "movistar" },
	{ 714, 3, "Claro", "Claro" },
	{ 714, 4, "Digicel", "Digicel" },

	// Papua New Guinea - PG

	{ 537, 1, "bmobile", "bmobile" },
	{ 537, 3, "Digicel", "Digicel" },

	// Paraguay - PY

	{ 744, 1, "VOX", "VOX" },
	{ 744, 2, "Claro/Hutchison", "Claro/Hutchison" },
	{ 744, 4, "Tigo", "Tigo" },
	{ 744, 5, "Personal", "Personal" },
	{ 744, 6, "Copaco", "Copaco" },

	// Peru - PE

	{ 716, 6, "Movistar", "Movistar" },
	{ 716, 7, "NEXTEL", "NEXTEL" },
	{ 716, 10, "Claro (TIM)", "Claro (TIM)" },
	{ 716, 15, "Viettel Mobile", "Viettel Mobile" },
	{ 716, 17, "NEXTEL", "NEXTEL" },

	// Philippines - PH

	{ 515, 1, "Islacom", "Islacom" },
	{ 515, 2, "Globe", "Globe" },
	{ 515, 3, "Smart", "Smart" },
	{ 515, 5, "Sun", "Sun" },
	{ 515, 11, "PLDT via ACeS Philippines", "PLDT via ACeS Philippines" },
	{ 515, 18, "Cure", "Cure" },
	{ 515, 24, "ABS-CBN", "ABS-CBN" },
	{ 515, 88, "Nextel", "Nextel" },

	// Pitcairn Islands (United Kingdom) - PN


	// Poland - PL

	{ 260, 1, "Plus", "Plus" },
	{ 260, 2, "T-Mobile", "T-Mobile" },
	{ 260, 3, "Orange", "Orange" },
	{ 260, 4, "CenterNet S.A.", "CenterNet S.A." },
	{ 260, 5, "Polska Telefonia Komórkowa Centertel Sp. z o.o.", "Polska Telefonia Komórkowa Centertel Sp. z o.o." },
	{ 260, 6, "Play", "Play" },
	{ 260, 7, "Netia", "Netia" },
	{ 260, 8, "E-Telko Sp. z o.o.", "E-Telko Sp. z o.o." },
	{ 260, 9, "Lycamobile", "Lycamobile" },
	{ 260, 10, "Sferia", "Sferia" },
	{ 260, 11, "Nordisk Polska", "Nordisk Polska" },
	{ 260, 12, "Cyfrowy Polsat", "Cyfrowy Polsat" },
	{ 260, 13, "Sferia", "Sferia" },
	{ 260, 14, "Sferia", "Sferia" },
	{ 260, 15, "CenterNet", "CenterNet" },
	{ 260, 16, "Mobyland", "Mobyland" },
	{ 260, 17, "Aero2", "Aero2" },
	{ 260, 32, "Voxbone", "Voxbone" },
	{ 260, 34, "T-Mobile Polska S.A.", "T-Mobile Polska S.A." },
	{ 260, 98, "P4 Sp. z o.o.", "P4 Sp. z o.o." },

	// Portugal - PT

	{ 268, 1, "Vodafone", "Vodafone" },
	{ 268, 3, "Optimus", "Optimus" },
	{ 268, 4, "LycaMobile", "LycaMobile" },
	{ 268, 6, "TMN", "TMN" },
	{ 268, 7, "Vectonemobile - Delightmobile", "Vectonemobile - Delightmobile" },
	{ 268, 21, "Zapp", "Zapp" },

	// Puerto Rico - PR

	{ 330, 110, "Claro Puerto Rico", "Claro Puerto Rico" },
	{ 330, 120, "tmobile", "tmobile" },

	// Qatar - QA

	{ 427, 1, "ooredoo", "ooredoo" },
	{ 427, 2, "Vodafone", "Vodafone" },
	{ 427, 5, "Ministry of Interior", "Ministry of Interior" },
	{ 427, 6, "Ministry of Interior", "Ministry of Interior" },

	// Réunion (France) - RE

	{ 647, 1, "BJT Partners", "BJT Partners" },
	{ 647, 2, "Outremer", "Outremer" },
	{ 647, 10, "SFR Reunion", "SFR Reunion" },

	// Romania - RO

	{ 226, 1, "Vodafone", "Vodafone" },
	{ 226, 2, "Romtelecom", "Romtelecom" },
	{ 226, 3, "Cosmote", "Cosmote" },
	{ 226, 4, "Cosmote/Zapp", "Cosmote/Zapp" },
	{ 226, 5, "Digi.Mobil", "Digi.Mobil" },
	{ 226, 6, "Cosmote/Zapp", "Cosmote/Zapp" },
	{ 226, 10, "Orange", "Orange" },
	{ 226, 15, "Idilis", "Idilis" },

	// Russian Federation - RU

	{ 250, 1, "MTS", "MTS" },
	{ 250, 2, "MegaFon", "MegaFon" },
	{ 250, 3, "NCC", "NCC" },
	{ 250, 4, "Sibchallenge", "Sibchallenge" },
	{ 250, 5, "ETK", "ETK" },
	{ 250, 6, "CJSC Saratov System of Cellular Communications", "CJSC Saratov System of Cellular Communications" },
	{ 250, 7, "SMARTS", "SMARTS" },
	{ 250, 9, "Skylink", "Skylink" },
	{ 250, 10, "DTC", "DTC" },
	{ 250, 11, "Yota", "Yota" },
	{ 250, 12, "Baykalwestcom", "Baykalwestcom" },
	{ 250, 12, "Akos", "Akos" },
	{ 250, 13, "KUGSM", "KUGSM" },
	{ 250, 14, "MegaFon", "MegaFon" },
	{ 250, 15, "SMARTS", "SMARTS" },
	{ 250, 16, "NTC", "NTC" },
	{ 250, 17, "Utel", "Utel" },
	{ 250, 18, "Osnova Telecom", "Osnova Telecom" },
	{ 250, 19, "INDIGO", "INDIGO" },
	{ 250, 20, "Tele2", "Tele2" },
	{ 250, 23, "Mobicom - Novosibirsk", "Mobicom - Novosibirsk" },
	{ 250, 28, "Beeline", "Beeline" },
	{ 250, 35, "MOTIV", "MOTIV" },
	{ 250, 38, "Tambov GSM", "Tambov GSM" },
	{ 250, 39, "Rostelecom", "Rostelecom" },
	{ 250, 44, "Stavtelesot / North Caucasian GSM", "Stavtelesot / North Caucasian GSM" },
	{ 250, 50, "MTS", "MTS" },
	{ 250, 91, "Sonic Duo", "Sonic Duo" },
	{ 250, 92, "Primtelefon", "Primtelefon" },
	{ 250, 93, "Telecom XXI", "Telecom XXI" },
	{ 250, 99, "Beeline", "Beeline" },

	// Rwanda - RW

	{ 635, 10, "MTN", "MTN" },
	{ 635, 12, "Rwandatel", "Rwandatel" },
	{ 635, 13, "Tigo", "Tigo" },
	{ 635, 14, "Airtel", "Airtel" },
	{ 635, 17, "Olleh Rwanda Networks", "Olleh Rwanda Networks" },

	// Saint Kitts and Nevis - KN

	{ 356, 50, "Digicel", "Digicel" },
	{ 356, 70, "Chippie", "Chippie" },
	{ 356, 110, "LIME", "LIME" },

	// Saint Lucia - LC

	{ 358, 50, "Digicel", "Digicel" },
	{ 358, 110, "Cable &  Wireless", "Cable &  Wireless" },

	// Saint Pierre and Miquelon (France) - PM

	{ 308, 1, "Ameris", "Ameris" },
	{ 308, 2, "GLOBALTEL", "GLOBALTEL" },

	// Saint Vincent and the Grenadines - VC

	{ 360, 70, "Digicel", "Digicel" },
	{ 360, 100, "Cingular Wireless", "Cingular Wireless" },
	{ 360, 110, "Lime", "Lime" },

	// Samoa - WS

	{ 549, 1, "Digicel", "Digicel" },
	{ 549, 27, "Bluesky", "Bluesky" },

	// San Marino - SM

	{ 292, 1, "PRIMA", "PRIMA" },

	// Sao Tome and Principe - ST

	{ 626, 1, "CSTmovel", "CSTmovel" },

	// Saudi Arabia - SA

	{ 420, 1, "Al Jawal (STC )", "Al Jawal (STC )" },
	{ 420, 3, "Mobily", "Mobily" },
	{ 420, 4, "Zain SA", "Zain SA" },
	{ 420, 21, "RGSM", "RGSM" },

	// Senegal - SN

	{ 608, 1, "Orange", "Orange" },
	{ 608, 2, "Tigo", "Tigo" },
	{ 608, 3, "Expresso", "Expresso" },
	{ 608, 4, "CSU-SA", "CSU-SA" },

	// Serbia - RS

	{ 220, 1, "Telenor", "Telenor" },
	{ 220, 2, "Telenor", "Telenor" },
	{ 220, 3, "mt:s", "mt:s" },
	{ 220, 5, "VIP", "VIP" },

	// Seychelles - SC

	{ 633, 1, "Cable & Wireless", "Cable & Wireless" },
	{ 633, 2, "Mediatech International", "Mediatech International" },
	{ 633, 10, "Airtel", "Airtel" },

	// Sierra Leone - SL

	{ 619, 1, "Airtel", "Airtel" },
	{ 619, 3, "Africell", "Africell" },
	{ 619, 4, "Comium", "Comium" },
	{ 619, 5, "Africell", "Africell" },
	{ 619, 6, "SierraTel", "SierraTel" },
	{ 619, 25, "Mobitel", "Mobitel" },

	// Singapore - SG

	{ 525, 1, "SingTel", "SingTel" },
	{ 525, 2, "SingTel-G18", "SingTel-G18" },
	{ 525, 3, "M1", "M1" },
	{ 525, 5, "StarHub", "StarHub" },
	{ 525, 6, "StarHub", "StarHub" },
	{ 525, 7, "SingTel", "SingTel" },
	{ 525, 12, "Grid", "Grid" },

	// Slovakia - SK

	{ 231, 1, "Orange", "Orange" },
	{ 231, 2, "Telekom", "Telekom" },
	{ 231, 3, "Unient Communications", "Unient Communications" },
	{ 231, 4, "T-Mobile", "T-Mobile" },
	{ 231, 6, "O2", "O2" },
	{ 231, 99, "ŽSR", "ŽSR" },

	// Slovenia - SI

	{ 293, 31, "Mobitel", "Mobitel" },
	{ 293, 40, "Si.mobil", "Si.mobil" },
	{ 293, 41, "Mobitel", "Mobitel" },
	{ 293, 51, "Mobitel", "Mobitel" },
	{ 293, 64, "T-2", "T-2" },
	{ 293, 70, "Tušmobil", "Tušmobil" },

	// Solomon Islands - SB

	{ 540, 1, "BREEZE", "BREEZE" },
	{ 540, 2, "BeMobile", "BeMobile" },

	// Somalia - SO

	{ 637, 1, "Telesom", "Telesom" },
	{ 637, 4, "SomaCyber", "SomaCyber" },
	{ 637, 4, "Somafone", "Somafone" },
	{ 637, 10, "Nationlink", "Nationlink" },
	{ 637, 50, "Hormuud", "Hormuud" },
	{ 637, 30, "Golis", "Golis" },
	{ 637, 57, "Unittel", "Unittel" },
	{ 637, 60, "Nationlink Telecom", "Nationlink Telecom" },
	{ 637, 71, "Somtel", "Somtel" },
	{ 637, 82, "Telcom", "Telcom" },

	// South Africa - ZA

	{ 655, 1, "Vodacom", "Vodacom" },
	{ 655, 2, "Telkom Mobile / 8.ta / Telekom SA", "Telkom Mobile / 8.ta / Telekom SA" },
	{ 655, 6, "Sentech", "Sentech" },
	{ 655, 7, "Cell C", "Cell C" },
	{ 655, 10, "MTN", "MTN" },
	{ 655, 11, "South African Police Service Gauteng", "South African Police Service Gauteng" },
	{ 655, 13, "Neotel", "Neotel" },
	{ 655, 14, "Neotel", "Neotel" },
	{ 655, 19, "iBurst", "iBurst" },
	{ 655, 21, "Cape Town Metropolitan Council", "Cape Town Metropolitan Council" },
	{ 655, 30, "Bokamoso Consortium", "Bokamoso Consortium" },
	{ 655, 31, "Karabo Telecoms (Pty) Ltd.", "Karabo Telecoms (Pty) Ltd." },
	{ 655, 32, "Ilizwi Telecommunications", "Ilizwi Telecommunications" },
	{ 655, 33, "Thinta Thinta Telecommunications", "Thinta Thinta Telecommunications" },
	{ 655, 50, "Ericsson South Africa (Pty) Ltd", "Ericsson South Africa (Pty) Ltd" },
	{ 655, 51, "Integrat (Pty) Ltd", "Integrat (Pty) Ltd" },

	// South Sudan - SS

	{ 659, 2, "MTN", "MTN" },
	{ 659, 3, "Gemtel", "Gemtel" },
	{ 659, 4, "Vivacell", "Vivacell" },
	{ 659, 6, "Zain", "Zain" },
	{ 659, 7, "Sudani", "Sudani" },

	// Spain - ES

	{ 214, 1, "Vodafone", "Vodafone" },
	{ 214, 3, "Orange", "Orange" },
	{ 214, 4, "Yoigo", "Yoigo" },
	{ 214, 5, "TME", "TME" },
	{ 214, 6, "Vodafone", "Vodafone" },
	{ 214, 7, "movistar", "movistar" },
	{ 214, 8, "Euskaltel", "Euskaltel" },
	{ 214, 9, "Orange", "Orange" },
	{ 214, 15, "BT", "BT" },
	{ 214, 16, "TeleCable", "TeleCable" },
	{ 214, 17, "Móbil R", "Móbil R" },
	{ 214, 18, "ONO", "ONO" },
	{ 214, 19, "Simyo", "Simyo" },
	{ 214, 20, "Fonyou", "Fonyou" },
	{ 214, 21, "Jazztel", "Jazztel" },
	{ 214, 22, "DigiMobil", "DigiMobil" },
	{ 214, 23, "Barablu", "Barablu" },
	{ 214, 24, "Eroski", "Eroski" },
	{ 214, 25, "Lycamobile", "Lycamobile" },
	{ 214, 28, "Consorcio de Telecomunicaciones Avanzadas, S.A.", "Consorcio de Telecomunicaciones Avanzadas, S.A." },
	{ 214, 29, "NEO-SKY 2002, S.A.", "NEO-SKY 2002, S.A." },
	{ 214, 30, "Compatel Limited", "Compatel Limited" },
	{ 214, 31, "Red Digital De Telecomunicaciones de las Islas Baleares, S.L.", "Red Digital De Telecomunicaciones de las Islas Baleares, S.L." },
	{ 214, 32, "Tuenti", "Tuenti" },

	// Sri Lanka - LK

	{ 413, 1, "Mobitel", "Mobitel" },
	{ 413, 2, "Dialog", "Dialog" },
	{ 413, 3, "Etisalat", "Etisalat" },
	{ 413, 5, "Airtel", "Airtel" },
	{ 413, 8, "Hutch", "Hutch" },

	// Sudan - SD

	{ 634, 1, "Zain SD", "Zain SD" },
	{ 634, 2, "MTN", "MTN" },
	{ 634, 5, "Canar Telecom", "Canar Telecom" },
	{ 634, 7, "Sudani One", "Sudani One" },
	{ 634, 9, "Privet Network", "Privet Network" },

	// Suriname - SR

	{ 746, 2, "Telesur", "Telesur" },
	{ 746, 3, "Digicel", "Digicel" },
	{ 746, 4, "Intelsur N.V. / UTS N.V.", "Intelsur N.V. / UTS N.V." },

	// Swaziland - SZ

	{ 653, 10, "Swazi MTN", "Swazi MTN" },

	// Sweden - SE

	{ 240, 1, "Telia", "Telia" },
	{ 240, 2, "3", "3" },
	{ 240, 3, "Net 1", "Net 1" },
	{ 240, 4, "SWEDEN", "SWEDEN" },
	{ 240, 5, "Sweden 3G", "Sweden 3G" },
	{ 240, 6, "Telenor", "Telenor" },
	{ 240, 7, "Tele2", "Tele2" },
	{ 240, 8, "Telenor", "Telenor" },
	{ 240, 9, "djuice", "djuice" },
	{ 240, 10, "Spring Mobil", "Spring Mobil" },
	{ 240, 11, "Lindholmen Science Park AB", "Lindholmen Science Park AB" },
	{ 240, 12, "Lycamobile", "Lycamobile" },
	{ 240, 13, "Alltele Företag Sverige AB", "Alltele Företag Sverige AB" },
	{ 240, 14, "TDC Sverige AB", "TDC Sverige AB" },
	{ 240, 15, "Wireless Maingate Nordic AB", "Wireless Maingate Nordic AB" },
	{ 240, 16, "42 Telecom AB", "42 Telecom AB" },
	{ 240, 17, "Götalandsnätet AB", "Götalandsnätet AB" },
	{ 240, 18, "Generic Mobile Systems Sweden AB", "Generic Mobile Systems Sweden AB" },
	{ 240, 19, "Vectone Mobile / Delight Mobile", "Vectone Mobile / Delight Mobile" },
	{ 240, 20, "Wireless Maingate Messaging Services AB", "Wireless Maingate Messaging Services AB" },
	{ 240, 21, "MobiSir", "MobiSir" },
	{ 240, 22, "EuTel AB", "EuTel AB" },
	{ 240, 23, "Infobip Limited", "Infobip Limited" },
	{ 240, 24, "Sweden 2G", "Sweden 2G" },
	{ 240, 25, "Digitel Mobile Srl", "Digitel Mobile Srl" },
	{ 240, 26, "Beepsend AB", "Beepsend AB" },
	{ 240, 27, "Fogg Mobile AB", "Fogg Mobile AB" },
	{ 240, 28, "CoolTEL Aps", "CoolTEL Aps" },
	{ 240, 29, "Mercury International Carrier Services", "Mercury International Carrier Services" },
	{ 240, 30, "NextGen Mobile Ltd.", "NextGen Mobile Ltd." },
	{ 240, 31, "Mobimax AB", "Mobimax AB" },
	{ 240, 32, "Compatel Limited", "Compatel Limited" },
	{ 240, 33, "Mobile Arts AB", "Mobile Arts AB" },
	{ 240, 34, "Pro Net Telecommunications Services Ltd.", "Pro Net Telecommunications Services Ltd." },
	{ 240, 35, "42 Telecom LTD", "42 Telecom LTD" },
	{ 240, 36, "interactive digital media GmbH", "interactive digital media GmbH" },
	{ 240, 37, "CLX Networks AB", "CLX Networks AB" },
	{ 240, 38, "Voxbone", "Voxbone" },
	{ 240, 39, "iCentrex Sweden AB", "iCentrex Sweden AB" },
	{ 240, 40, "ReWiCom Scandinavia AB", "ReWiCom Scandinavia AB" },
	{ 240, 41, "Shyam Telecom UK Ltd.", "Shyam Telecom UK Ltd." },
	{ 240, 42, "Telenor Connexion AB", "Telenor Connexion AB" },
	{ 240, 43, "MobiWeb Ltd.", "MobiWeb Ltd." },
	{ 240, 44, "Limitless Mobile AB", "Limitless Mobile AB" },

	// Switzerland - CH

	{ 228, 1, "Swisscom", "Swisscom" },
	{ 228, 2, "Sunrise", "Sunrise" },
	{ 228, 3, "Orange", "Orange" },
	{ 228, 5, "Togewanet AG (Comfone)", "Togewanet AG (Comfone)" },
	{ 228, 6, "SBB-CFF-FFS", "SBB-CFF-FFS" },
	{ 228, 7, "IN&Phone", "IN&Phone" },
	{ 228, 8, "Tele4u", "Tele4u" },
	{ 228, 9, "Comfone", "Comfone" },
	{ 228, 12, "Sunrise", "Sunrise" },
	{ 228, 50, "3G Mobile AG", "3G Mobile AG" },
	{ 228, 51, "BebbiCell AG", "BebbiCell AG" },
	{ 228, 52, "Barablu", "Barablu" },
	{ 228, 53, "UPC", "UPC" },
	{ 228, 54, "Lyca Mobile", "Lyca Mobile" },
	{ 228, 99, "Swisscom", "Swisscom" },

	// Syria - SY

	{ 417, 1, "Syriatel", "Syriatel" },
	{ 417, 2, "MTN", "MTN" },

	// Taiwan - TW

	{ 466, 1, "FarEasTone", "FarEasTone" },
	{ 466, 5, "APTG", "APTG" },
	{ 466, 6, "FarEasTone", "FarEasTone" },
	{ 466, 11, "Chunghwa LDM", "Chunghwa LDM" },
	{ 466, 88, "FarEasTone", "FarEasTone" },
	{ 466, 89, "VIBO", "VIBO" },
	{ 466, 92, "Chungwa", "Chungwa" },
	{ 466, 93, "MobiTai", "MobiTai" },
	{ 466, 97, "Taiwan Mobile", "Taiwan Mobile" },
	{ 466, 99, "TransAsia", "TransAsia" },

	// Tajikistan - TJ

	{ 436, 1, "Tcell", "Tcell" },
	{ 436, 2, "Tcell", "Tcell" },
	{ 436, 3, "Megafon Tajikistan", "Megafon Tajikistan" },
	{ 436, 4, "Babilon-M", "Babilon-M" },
	{ 436, 5, "Beeline", "Beeline" },
	{ 436, 12, "Tcell", "Tcell" },

	// Tanzania - TZ

	{ 640, 2, "tiGO", "tiGO" },
	{ 640, 3, "Zantel", "Zantel" },
	{ 640, 4, "Vodacom", "Vodacom" },
	{ 640, 5, "Airtel", "Airtel" },
	{ 640, 6, "Dovetel Limited", "Dovetel Limited" },
	{ 640, 7, "Tanzania Telecommunication Company LTD (TTCL)", "Tanzania Telecommunication Company LTD (TTCL)" },
	{ 640, 8, "Benson Informatics Limited", "Benson Informatics Limited" },
	{ 640, 9, "ExcellentCom Tanzania Limited", "ExcellentCom Tanzania Limited" },
	{ 640, 11, "SmileCom", "SmileCom" },

	// Thailand - TH

	{ 520, 1, "AIS", "AIS" },
	{ 520, 2, "CAT CDMA", "CAT CDMA" },
	{ 520, 3, "AIS 3G", "AIS 3G" },
	{ 520, 4, "truemove H 4G LTE", "truemove H 4G LTE" },
	{ 520, 5, "dtac TriNet", "dtac TriNet" },
	{ 520, 10, "?", "?" },
	{ 520, 15, "TOT 3G", "TOT 3G" },
	{ 520, 18, "dtac", "dtac" },
	{ 520, 23, "AIS GSM 1800", "AIS GSM 1800" },
	{ 520, 25, "WE PCT", "WE PCT" },
	{ 520, 47, "Telephone Organization of Thailand (TOT)", "Telephone Organization of Thailand (TOT)" },
	{ 520, 99, "truemove", "truemove" },

	// Togo - TG

	{ 615, 1, "Togo Cell", "Togo Cell" },
	{ 615, 3, "Moov", "Moov" },

	// Tonga - TO

	{ 539, 1, "U-Call", "U-Call" },
	{ 539, 43, "Shoreline Communication", "Shoreline Communication" },
	{ 539, 88, "Digicel", "Digicel" },

	// Trinidad and Tobago - TT

	{ 374, 12, "bmobile", "bmobile" },
	{ 374, 130, "Digicel", "Digicel" },

	// Tunisia - TN

	{ 605, 1, "Orange", "Orange" },
	{ 605, 2, "Tunicell", "Tunicell" },
	{ 605, 3, "Tunisiana", "Tunisiana" },

	// Turkey - TR

	{ 286, 1, "Turkcell", "Turkcell" },
	{ 286, 2, "Vodafone", "Vodafone" },
	{ 286, 3, "Avea", "Avea" },
	{ 286, 4, "Aycell", "Aycell" },

	// Turkmenistan - TM

	{ 438, 1, "MTS (BARASH Communication)", "MTS (BARASH Communication)" },
	{ 438, 2, "TM-Cell", "TM-Cell" },

	// Turks and Caicos Islands - TC

	{ 338, 50, "Digicel", "Digicel" },
	{ 376, 350, "C&W", "C&W" },
	{ 376, 352, "Islandcom", "Islandcom" },

	// Tuvalu - TV

	{ 553, 1, "TTC", "TTC" },

	// Uganda - UG

	{ 641, 1, "Airtel", "Airtel" },
	{ 641, 10, "MTN", "MTN" },
	{ 641, 11, "UTL ( Telecom Limited)", "UTL ( Telecom Limited)" },
	{ 641, 14, "Orange", "Orange" },
	{ 641, 22, "Warid Telecom", "Warid Telecom" },
	{ 641, 33, "Smile", "Smile" },
	{ 641, 44, "K2", "K2" },
	{ 641, 66, "i-Tel", "i-Tel" },

	// Ukraine - UA

	{ 255, 1, "MTS", "MTS" },
	{ 255, 2, "Beeline", "Beeline" },
	{ 255, 3, "Kyivstar", "Kyivstar" },
	{ 255, 4, "IT", "IT" },
	{ 255, 5, "Golden Telecom", "Golden Telecom" },
	{ 255, 6, "life:)", "life:)" },
	{ 255, 7, "3Mob", "3Mob" },
	{ 255, 21, "PEOPLEnet", "PEOPLEnet" },
	{ 255, 23, "CDMA Ukraine", "CDMA Ukraine" },
	{ 255, 25, "NEWTONE", "NEWTONE" },

	// United Arab Emirates - AE

	{ 424, 2, "Etisalat", "Etisalat" },
	{ 424, 3, "du", "du" },

	// United Kingdom - GB

	{ 234, 1, "Vectone Mobile", "Vectone Mobile" },
	{ 234, 2, "O2 (UK)", "O2 (UK)" },
	{ 234, 3, "Airtel-Vodafone", "Airtel-Vodafone" },
	{ 234, 4, "FMS Solutions Ltd", "FMS Solutions Ltd" },
	{ 234, 5, "COLT Mobile Telecommunications Limited", "COLT Mobile Telecommunications Limited" },
	{ 234, 6, "Internet Computer Bureau Limited", "Internet Computer Bureau Limited" },
	{ 234, 7, "Cable & Wireless Worldwide", "Cable & Wireless Worldwide" },
	{ 234, 8, "OnePhone (UK) Ltd", "OnePhone (UK) Ltd" },
	{ 234, 9, "Tismi BV", "Tismi BV" },
	{ 234, 10, "O2 (UK)", "O2 (UK)" },
	{ 234, 11, "O2 (UK)", "O2 (UK)" },
	{ 234, 12, "Railtrack", "Railtrack" },
	{ 234, 13, "Railtrack", "Railtrack" },
	{ 234, 14, "Hay Systems Ltd", "Hay Systems Ltd" },
	{ 234, 15, "Vodafone UK", "Vodafone UK" },
	{ 234, 16, "Talk Talk", "Talk Talk" },
	{ 234, 17, "FleXtel Limited", "FleXtel Limited" },
	{ 234, 18, "Cloud9", "Cloud9" },
	{ 234, 19, "Private Mobile Networks PMN", "Private Mobile Networks PMN" },
	{ 234, 20, "3", "3" },
	{ 234, 22, "RoutoMessaging", "RoutoMessaging" },
	{ 234, 23, "Icron Network Limited", "Icron Network Limited" },
	{ 234, 24, "Greenfone", "Greenfone" },
	{ 234, 25, "Truphone", "Truphone" },
	{ 234, 26, "Lycamobile", "Lycamobile" },
	{ 234, 27, "Teleena UK Limited", "Teleena UK Limited" },
	{ 234, 28, "Marathon Telecom Ltd", "Marathon Telecom Ltd" },
	{ 234, 29, "aql", "aql" },
	{ 234, 30, "T-Mobile UK", "T-Mobile UK" },
	{ 234, 31, "Virgin Mobile UK", "Virgin Mobile UK" },
	{ 234, 32, "Virgin Mobile UK", "Virgin Mobile UK" },
	{ 234, 33, "Orange (UK)", "Orange (UK)" },
	{ 234, 34, "Orange (UK)", "Orange (UK)" },
	{ 234, 35, "JSC Ingenium (UK) Limited", "JSC Ingenium (UK) Limited" },
	{ 234, 36, "Sure (Isle of Man) Limited", "Sure (Isle of Man) Limited" },
	{ 234, 37, "Synectiv Ltd", "Synectiv Ltd" },
	{ 234, 38, "Virgin Mobile UK", "Virgin Mobile UK" },
	{ 234, 39, "SSE Energy Supply Limited", "SSE Energy Supply Limited" },
	{ 234, 50, "JT", "JT" },
	{ 234, 51, "UK Broadband", "UK Broadband" },
	{ 234, 52, "Shyam Telecom UK Ltd", "Shyam Telecom UK Ltd" },
	{ 234, 53, "Limitless Mobile Ltd", "Limitless Mobile Ltd" },
	{ 234, 55, "Cable & Wireless Guernsey / Sure Mobile (Jersey)", "Cable & Wireless Guernsey / Sure Mobile (Jersey)" },
	{ 234, 58, "Manx Telecom", "Manx Telecom" },
	{ 234, 76, "BT", "BT" },
	{ 234, 78, "Airwave", "Airwave" },
	{ 234, 86, "EE", "EE" },
	{ 235, 1, "EE", "EE" },
	{ 235, 2, "EE", "EE" },
	{ 235, 3, "UK Broadband", "UK Broadband" },
	{ 235, 77, "BT", "BT" },
	{ 235, 91, "Vodafone United Kingdom", "Vodafone United Kingdom" },
	{ 235, 92, "Cable & Wireless UK", "Cable & Wireless UK" },
	{ 235, 94, "Hutchison 3G UK Ltd", "Hutchison 3G UK Ltd" },
	{ 235, 95, "Network Rail Infrastructure Limited", "Network Rail Infrastructure Limited" },

	// United States of America - US

	{ 310, 4, "Verizon", "Verizon" },
	{ 310, 5, "Verizon", "Verizon" },
	{ 310, 10, "MCI", "MCI" },
	{ 310, 13, "MobileTel", "MobileTel" },
	{ 310, 14, "Testing", "Testing" },
	{ 310, 16, "Cricket Communications", "Cricket Communications" },
	{ 310, 17, "North Sight Communications Inc.", "North Sight Communications Inc." },
	{ 310, 20, "Union Telephone Company", "Union Telephone Company" },
	{ 310, 26, "T-Mobile", "T-Mobile" },
	{ 310, 30, "AT&T", "AT&T" },
	{ 310, 34, "Airpeak", "Airpeak" },
	{ 310, 40, "Concho", "Concho" },
	{ 310, 46, "SIMMETRY", "SIMMETRY" },
	{ 310, 53, "Virgin Mobile US", "Virgin Mobile US" },
	{ 310, 54, "Alltel US", "Alltel US" },
	{ 310, 60, "Consolidated Telcom", "Consolidated Telcom" },
	{ 310, 66, "U.S. Cellular", "U.S. Cellular" },
	{ 310, 70, "Highland Cellular", "Highland Cellular" },
	{ 310, 80, "Corr", "Corr" },
	{ 310, 90, "Cricket Communications", "Cricket Communications" },
	{ 310, 100, "Plateau Wireless", "Plateau Wireless" },
	{ 310, 110, "PTI Pacifica", "PTI Pacifica" },
	{ 310, 120, "Sprint", "Sprint" },
	{ 310, 150, "AT&T", "AT&T" },
	{ 310, 160, "T-Mobile", "T-Mobile" },
	{ 310, 170, "AT&T", "AT&T" },
	{ 310, 180, "West Central", "West Central" },
	{ 310, 190, "Dutch Harbor", "Dutch Harbor" },
	{ 310, 200, "T-Mobile", "T-Mobile" },
	{ 310, 210, "T-Mobile", "T-Mobile" },
	{ 310, 220, "T-Mobile", "T-Mobile" },
	{ 310, 230, "T-Mobile", "T-Mobile" },
	{ 310, 240, "T-Mobile", "T-Mobile" },
	{ 310, 250, "T-Mobile", "T-Mobile" },
	{ 310, 260, "T-Mobile", "T-Mobile" },
	{ 310, 270, "T-Mobile", "T-Mobile" },
	{ 310, 280, "AT&T", "AT&T" },
	{ 310, 290, "T-Mobile", "T-Mobile" },
	{ 310, 300, "Big Sky Mobile", "Big Sky Mobile" },
	{ 310, 310, "T-Mobile", "T-Mobile" },
	{ 310, 311, "Farmers Wireless", "Farmers Wireless" },
	{ 310, 320, "Cellular One", "Cellular One" },
	{ 310, 330, "T-Mobile", "T-Mobile" },
	{ 310, 340, "Westlink", "Westlink" },
	{ 310, 350, "Carolina Phone", "Carolina Phone" },
	{ 310, 380, "AT&T", "AT&T" },
	{ 310, 390, "Cellular One of East Texas", "Cellular One of East Texas" },
	{ 310, 400, "i CAN_GSM", "i CAN_GSM" },
	{ 310, 410, "AT&T", "AT&T" },
	{ 310, 420, "Cincinnati Bell", "Cincinnati Bell" },
	{ 310, 430, "Alaska Digitel", "Alaska Digitel" },
	{ 310, 440, "Cellular One", "Cellular One" },
	{ 310, 450, "Viaero", "Viaero" },
	{ 310, 460, "Simmetry", "Simmetry" },
	{ 310, 470, "nTelos", "nTelos" },
	{ 310, 480, "Choice Phone", "Choice Phone" },
	{ 310, 490, "T-Mobile", "T-Mobile" },
	{ 310, 500, "Alltel", "Alltel" },
	{ 310, 510, "Airtel", "Airtel" },
	{ 310, 520, "VeriSign", "VeriSign" },
	{ 310, 530, "West Virginia Wireless", "West Virginia Wireless" },
	{ 310, 540, "Oklahoma Western", "Oklahoma Western" },
	{ 310, 560, "AT&T", "AT&T" },
	{ 310, 570, "Cellular One", "Cellular One" },
	{ 310, 580, "T-Mobile", "T-Mobile" },
	{ 310, 590, "Alltel", "Alltel" },
	{ 310, 610, "Epic Touch", "Epic Touch" },
	{ 310, 620, "Coleman County Telecom", "Coleman County Telecom" },
	{ 310, 630, "AmeriLink PCS", "AmeriLink PCS" },
	{ 310, 640, "Airadigm", "Airadigm" },
	{ 310, 650, "Jasper", "Jasper" },
	{ 310, 660, "T-Mobile", "T-Mobile" },
	{ 310, 670, "Northstar", "Northstar" },
	{ 310, 680, "AT&T", "AT&T" },
	{ 310, 690, "Immix", "Immix" },
	{ 310, 730, "SeaMobile", "SeaMobile" },
	{ 310, 740, "Convey", "Convey" },
	{ 310, 750, "Appalachian Wireless", "Appalachian Wireless" },
	{ 310, 760, "Panhandle", "Panhandle" },
	{ 310, 770, "i wireless", "i wireless" },
	{ 310, 780, "Airlink PCS", "Airlink PCS" },
	{ 310, 790, "PinPoint", "PinPoint" },
	{ 310, 800, "T-Mobile", "T-Mobile" },
	{ 310, 830, "Caprock", "Caprock" },
	{ 310, 840, "telna Mobile", "telna Mobile" },
	{ 310, 850, "Aeris", "Aeris" },
	{ 310, 870, "PACE", "PACE" },
	{ 310, 880, "Advantage", "Advantage" },
	{ 310, 890, "Unicel", "Unicel" },
	{ 310, 900, "Mid-Rivers Wireless", "Mid-Rivers Wireless" },
	{ 310, 910, "First Cellular", "First Cellular" },
	{ 310, 940, "Iris Wireless LLC", "Iris Wireless LLC" },
	{ 310, 950, "XIT Wireless", "XIT Wireless" },
	{ 310, 960, "Plateau Wireless", "Plateau Wireless" },
	{ 310, 970, "Globalstar", "Globalstar" },
	{ 310, 980, "AT&T", "AT&T" },
	{ 310, 990, "AT&T", "AT&T" },
	{ 311, 10, "Chariton Valley", "Chariton Valley" },
	{ 311, 12, "Verizon", "Verizon" },
	{ 311, 20, "Missouri RSA 5 Partnership", "Missouri RSA 5 Partnership" },
	{ 311, 30, "Indigo Wireless", "Indigo Wireless" },
	{ 311, 40, "Commnet Wireless", "Commnet Wireless" },
	{ 311, 50, "Wikes Cellular", "Wikes Cellular" },
	{ 311, 60, "Farmers Cellular", "Farmers Cellular" },
	{ 311, 70, "Easterbrooke", "Easterbrooke" },
	{ 311, 80, "Pine Cellular", "Pine Cellular" },
	{ 311, 90, "Long Lines Wireless", "Long Lines Wireless" },
	{ 311, 100, "High Plains Wireless", "High Plains Wireless" },
	{ 311, 110, "High Plains Wireless", "High Plains Wireless" },
	{ 311, 120, "Choice Phone", "Choice Phone" },
	{ 311, 130, "Cell One Amarillo", "Cell One Amarillo" },
	{ 311, 140, "Sprocket", "Sprocket" },
	{ 311, 150, "Wilkes Cellular", "Wilkes Cellular" },
	{ 311, 160, "Endless Mountains Wireless", "Endless Mountains Wireless" },
	{ 311, 170, "PetroCom", "PetroCom" },
	{ 311, 180, "Cingular Wireless", "Cingular Wireless" },
	{ 311, 190, "Cellular Properties", "Cellular Properties" },
	{ 311, 210, "Emery Telcom Wireless", "Emery Telcom Wireless" },
	{ 311, 220, "U.S. Cellular", "U.S. Cellular" },
	{ 311, 230, "C Spire Wireless", "C Spire Wireless" },
	{ 311, 330, "Bug Tussel Wireless", "Bug Tussel Wireless" },
	{ 311, 480, "Verizon", "Verizon" },
	{ 311, 481, "Verizon", "Verizon" },
	{ 311, 490, "Sprint Corporation", "Sprint Corporation" },
	{ 311, 580, "U.S. Cellular", "U.S. Cellular" },
	{ 311, 660, "metroPCS", "metroPCS" },
	{ 311, 870, "Boost", "Boost" },
	{ 311, 960, "Lycamobile", "Lycamobile" },
	{ 311, 970, "Big River Broadband", "Big River Broadband" },
	{ 312, 590, "NMU", "NMU" },
	{ 313, 100, "700&nbsp;MHz Public Safety Broadband", "700&nbsp;MHz Public Safety Broadband" },
	{ 313, 101, "700&nbsp;MHz Public Safety Broadband", "700&nbsp;MHz Public Safety Broadband" },
	{ 316, 10, "Nextel", "Nextel" },
	{ 316, 11, "Southern Communications Services", "Southern Communications Services" },

	// Uruguay - UY

	{ 748, 1, "Antel", "Antel" },
	{ 748, 7, "Movistar", "Movistar" },
	{ 748, 10, "Claro", "Claro" },

	// Uzbekistan - UZ

	{ 434, 1, "Buztel", "Buztel" },
	{ 434, 2, "Uzmacom", "Uzmacom" },
	{ 434, 4, "Beeline", "Beeline" },
	{ 434, 5, "Ucell", "Ucell" },
	{ 434, 6, "Perfectum Mobile", "Perfectum Mobile" },
	{ 434, 7, "MTS", "MTS" },
	{ 434, 3, "UzMobile", "UzMobile" },

	// Vanuatu - VU

	{ 541, 1, "SMILE", "SMILE" },
	{ 541, 5, "Digicel", "Digicel" },
	{ 541, 7, "WanTok", "WanTok" },

	// Vatican - VA


	// Venezuela - VE

	{ 734, 1, "Digitel", "Digitel" },
	{ 734, 2, "Digitel GSM", "Digitel GSM" },
	{ 734, 3, "Digitel", "Digitel" },
	{ 734, 4, "movistar", "movistar" },
	{ 734, 6, "Movilnet", "Movilnet" },

	// Vietnam - VN

	{ 452, 1, "MobiFone", "MobiFone" },
	{ 452, 2, "Vinaphone", "Vinaphone" },
	{ 452, 3, "S-Fone", "S-Fone" },
	{ 452, 4, "Viettel Mobile", "Viettel Mobile" },
	{ 452, 5, "Vietnamobile (HT Mobile )", "Vietnamobile (HT Mobile )" },
	{ 452, 6, "EVNTelecom", "EVNTelecom" },
	{ 452, 7, "G-Mobile", "G-Mobile" },
	{ 452, 8, "3G EVNTelecom", "3G EVNTelecom" },

	// Yemen - YE

	{ 421, 1, "SabaFon", "SabaFon" },
	{ 421, 2, "MTN", "MTN" },
	{ 421, 3, "Yemen Mobile", "Yemen Mobile" },
	{ 421, 4, "HiTS-UNITEL", "HiTS-UNITEL" },

	// Zambia - ZM

	{ 645, 1, "Airtel", "Airtel" },
	{ 645, 2, "MTN", "MTN" },
	{ 645, 3, "ZAMTEL", "ZAMTEL" },

	// Zimbabwe - ZW

	{ 648, 1, "Net*One", "Net*One" },
	{ 648, 3, "Telecel", "Telecel" },
	{ 648, 4, "Econet", "Econet" },
	{ 901, 1, "ICO", "ICO" },
	{ 901, 2, "''Unassigned''", "''Unassigned''" },
	{ 901, 3, "Iridium", "Iridium" },
	{ 901, 4, "''Unassigned''", "''Unassigned''" },
	{ 901, 5, "Thuraya RMSS Network", "Thuraya RMSS Network" },
	{ 901, 6, "Thuraya Satellite Telecommunications Company", "Thuraya Satellite Telecommunications Company" },
	{ 901, 7, "''Unassigned''", "''Unassigned''" },
	{ 901, 8, "''Unassigned''", "''Unassigned''" },
	{ 901, 9, "''Unassigned''", "''Unassigned''" },
	{ 901, 10, "ACeS", "ACeS" },
	{ 901, 11, "Inmarsat", "Inmarsat" },
	{ 901, 12, "Telenor", "Telenor" },
	{ 901, 13, "GSM.AQ", "GSM.AQ" },
	{ 901, 14, "AeroMobile AS", "AeroMobile AS" },
	{ 901, 15, "OnAir", "OnAir" },
	{ 901, 16, "Jasper Systems", "Jasper Systems" },
	{ 901, 17, "Navitas", "Navitas" },
	{ 901, 18, "Cellular @Sea", "Cellular @Sea" },
	{ 901, 19, "Vodafone Malta Maritime", "Vodafone Malta Maritime" },
	{ 901, 20, "Intermatica", "Intermatica" },
	{ 901, 21, "''Unassigned''", "''Unassigned''" },
	{ 901, 22, "MediaLincc Ltd", "MediaLincc Ltd" },
	{ 901, 23, "''Unassigned''", "''Unassigned''" },
	{ 901, 24, "iNum", "iNum" },
	{ 901, 25, "''Unassigned''", "''Unassigned''" },
	{ 901, 26, "TIM", "TIM" },
	{ 901, 27, "OnAir", "OnAir" },
	{ 901, 28, "Vodafone", "Vodafone" },
	{ 901, 29, "Telenor", "Telenor" },
	{ 901, 30, "''Unassigned''", "''Unassigned''" },
	{ 901, 31, "Orange", "Orange" },
	{ 901, 32, "Sky High", "Sky High" },
	{ 901, 33, "Smart Communications", "Smart Communications" },
	{ 901, 34, "tyntec GmbH", "tyntec GmbH" },
	{ 901, 35, "Globecomm Network Services", "Globecomm Network Services" },
	{ 901, 36, "Azerfon", "Azerfon" },
	{ 901, 39, "MTX Connect Ltd", "MTX Connect Ltd" },
	{ 901, 88, "UN Office for the Coordination of Humanitarian Affairs (OCHA)", "UN Office for the Coordination of Humanitarian Affairs (OCHA)" },
};

unsigned int plmn_list_count = sizeof(plmn_list) /
	sizeof(struct plmn_list_entry);

#endif
