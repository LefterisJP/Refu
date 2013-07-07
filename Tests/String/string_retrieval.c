﻿#include <RFstring.h>
#include <RFprintf.h>
#include <refu.h>

#include "../refu_tests.h"
int main()
{
	RF_String s1,s2,s3,s4,s5,s6,s7;
	RF_String ret;

	EXPECT(rfInit(), true)

	
	EXPECT(true,rfString_Init(&s1,"This is the test for String retrievals. We are testing retrieval in various situations"))
	EXPECT(true,rfString_Init(&s2,"ABCDEFGHIJKLMNOPQRSTUVWXYZ"))
	EXPECT(true,rfString_Init(&s3,"abcdefghijklmnopqrstuvwxyz"))
	EXPECT(true,rfString_Init(&s4,"A record 4m (13ft) tidal surge sent seawater cascading into large parts of New York City's subway system"))
	EXPECT(true,rfString_Init(&s5,"東京電力福島第一原発の被曝（ひばく）隠し問題で、厚生労働省は３０日、データの誤入力や線量計の未装着など１９件の不適切な事例が新たに見つかったと発表した"))
	EXPECT(true,rfString_Init(&s6,"Στον πυρετό της έγκρισης των μέτρων και των προαπαιτούμενων ενεργειών μπαίνει από την Τετάρτη η Αθήνα"))
    EXPECT(true,rfString_Init(&s7,"【ワシントン＝行方史郎】ハリケーン「サンディ」の影響で、米ニューヨーク州とニュージャージー州で運転中だった原発３基が２９日から３０日にかけて停止した。米原子力規制委員会（ＮＲＣ）が発表した。いずれも原子炉の冷却機能は確保されており、\
いまのところ安全上の問題はないという。ニューヨーク州のナインマイルポイント原発１号機とインディアナポイント原発３号機では原子炉が自動的に緊急停止。いずれも送電網の障害が原因とみられる。ニュージャージー州のセーラム原発１号機は、川から水を取る循環水ポンプが故障し、\
手動で原子炉を止めた。　また、燃料交換のため停止中だったニュージャージー州オイスタークリーク原発では、取水口の水位が一時、警戒レベルを超えた。電力会社は、ＮＲＣが定めた４段階の緊急態勢のうち下から２番目の「警戒」を宣言して事態を注視している。"))

	//attempt all the following retrievals and check against expected output
	EXPECT(40,rfString_Find(&s1,RFS_("We are"),0))
	EXPECT(RF_FAILURE,rfString_Find(&s1,RFS_("I am not in the string"),0))
	EXPECT(8,rfString_Find(&s2,RFS_("ijklmnopqrstuv"),RF_CASE_IGNORE))
	EXPECT(3,rfString_Find(&s3,RFS_("DEFGHIJKL"),RF_CASE_IGNORE))
	EXPECT(RF_FAILURE,rfString_Find(&s3,RFS_("DEFGHIJKL"),RF_CASE_IGNORE|RF_MATCH_WORD))
	EXPECT(RF_FAILURE,rfString_Find_i(&s4,RFS_("New York"),2,30,0))
	EXPECT(55,rfString_Find_i(&s5,RFS_("不適切な事例"),34,120,0))
	EXPECT(true,rfString_BeginsWith(&s6,RFS_("Στον πυρετό")))
	EXPECT(true,rfString_EndsWith(&s6,RFS_("Αθήνα")))
	EXPECT(false,rfString_EndsWith(&s6,RFS_("Δεν ειμαι στο τελος")))
	EXPECT(false,rfString_BeginsWith(&s6,RFS_("Στο πυρετό")));
	EXPECT(2,rfString_Count(&s7,RFS_("ニューヨーク")))
	EXPECT(5,rfString_Count(&s7,RFS_("州")))
	EXPECT(false,rfString_Count(&s7,RFS_("東京")))
	
	//expect the strings to be equal
	EXPECT(true,rfString_Equal(&s7,RFS_("【ワシントン＝行方史郎】ハリケーン「サンディ」の影響で、米ニューヨーク州とニュージャージー州で運転中だった原発３基が２９日から３０日にかけて停止した。米原子力規制委員会（ＮＲＣ）が発表した。いずれも原子炉の冷却機能は確保されており、\
いまのところ安全上の問題はないという。ニューヨーク州のナインマイルポイント原発１号機とインディアナポイント原発３号機では原子炉が自動的に緊急停止。いずれも送電網の障害が原因とみられる。ニュージャージー州のセーラム原発１号機は、川から水を取る循環水ポンプが故障し、\
手動で原子炉を止めた。　また、燃料交換のため停止中だったニュージャージー州オイスタークリーク原発では、取水口の水位が一時、警戒レベルを超えた。電力会社は、ＮＲＣが定めた４段階の緊急態勢のうち下から２番目の「警戒」を宣言して事態を注視している。")))
	EXPECT(false,rfString_Equal(&s7,RFS_("I am not equal")))
	//expect both to be true : (the substring of s7 from character 308 until 318) and check to see if it is what was expected
	EXPECT(true,rfString_Substr(&s7,308,10,&ret))
	EXPECT(true,rfString_Equal(&ret,RFS_("警戒レベルを超えた。")))
	//expect false: (requesting a substring out of range)
	EXPECT(false,rfString_Substr(&s7,400,100,&ret))


	return 0;
}
