﻿#include <RFtextfile.h>
#include <RFstring.h>
#include <refu.h>


#include "../refu_tests.h"

//the lines against which we check each file type
static RF_String lines[5];

void writeTest(char encoding,RF_StringX* buff)
{
	RF_TextFile outF,inF;
	uint32_t i=0; 
	rfInit();
	
	EXPECT(RF_SUCCESS,rfTextFile_Init(&outF,RFS_("writefile"),RF_FILE_NEW,encoding,RF_EOL_LF))
	EXPECT(RF_SUCCESS,rfTextFile_Write(&outF,RFS_("फ्रांस की एक अदालत ने पूर्व राष्ट्रपति निकोला सार्कोज़ी से ग़ैर-क़ानूनी राजनीतिक चंदे के आरोप को लेकर बारह घंटे लंबी पूछताछ की है और कहा है कि वो संदेह के दायरे में हैं.\n")))
	EXPECT(RF_SUCCESS,rfTextFile_Write(&outF,RFS_("Οργή στους συγγενείς των θυμάτων προκάλεσε η αθωωτική απόφαση του Κακουργιοδικείου στους υπεύθυνους της αεροπορικής εταιρείας «Ήλιος», αεροσκάφος της οποίας συνετρίβη στις 14 Αυγούστου του 2005 στο Γραμματικό Αττικής, με 121 θύματα.\n")))
	EXPECT(RF_SUCCESS,rfTextFile_Write(&outF,RFS_("今回初めて「児童生徒の生命・身体の安全がおびやかされるような重大事案」について詳細な報告を求めたところ、全国で２７８件あった\n")))

	EXPECT(RF_SUCCESS,rfTextFile_Insert(&outF,1,RFS_("We are inserting a line before the first line of the file, replacing the previous first"),false))
	EXPECT(RF_SUCCESS,rfTextFile_Insert(&outF,3,RFS_("We are inserting a line after the third line"),true))
	rfTextFile_Deinit(&outF);
	
	//checking if the file was correctly written
	EXPECT(RF_SUCCESS,rfTextFile_Init(&inF,RFS_("writefile"),RF_FILE_READ,encoding,RF_EOL_LF))
	while(rfTextFile_ReadLine(&inF,buff) == RF_SUCCESS)
	{
		EXPECT(true,rfString_Equal(buff,&lines[i]))
		i++;
	}
	EXPECT(5,i)
	rfTextFile_Deinit(&inF);
}

int main()
{
	RF_StringX buffer;
	
	rfInit();
	//initialize the lines we are gonna use for checking
	EXPECT(true,rfString_Init(&lines[0],"We are inserting a line before the first line of the file, replacing the previous first"))
	EXPECT(true,rfString_Init(&lines[1],"फ्रांस की एक अदालत ने पूर्व राष्ट्रपति निकोला सार्कोज़ी से ग़ैर-क़ानूनी राजनीतिक चंदे के आरोप को लेकर बारह घंटे लंबी पूछताछ की है और कहा है कि वो संदेह के दायरे में हैं."))
	EXPECT(true,rfString_Init(&lines[2],"Οργή στους συγγενείς των θυμάτων προκάλεσε η αθωωτική απόφαση του Κακουργιοδικείου στους υπεύθυνους της αεροπορικής εταιρείας «Ήλιος», αεροσκάφος της οποίας συνετρίβη στις 14 Αυγούστου του 2005 στο Γραμματικό Αττικής, με 121 θύματα."))
	EXPECT(true,rfString_Init(&lines[3],"We are inserting a line after the third line"))
	EXPECT(true,rfString_Init(&lines[4],"今回初めて「児童生徒の生命・身体の安全がおびやかされるような重大事案」について詳細な報告を求めたところ、全国で２７８件あった"))
	//initialize the buffer
	EXPECT(true,rfStringX_Init_buff(&buffer,4096,""))
	
	//test each file type
	writeTest(RF_UTF8,&buffer);
	writeTest(RF_UTF16_LE,&buffer);
	writeTest(RF_UTF16_BE,&buffer);
	writeTest(RF_UTF32_LE,&buffer);
	writeTest(RF_UTF32_BE,&buffer);
	
	//cleanup
	rfStringX_Deinit(&buffer);
	EXPECT(RF_SUCCESS,rfDeleteFile(RFS_("writefile")))

	return 0;
}
