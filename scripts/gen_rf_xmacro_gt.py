#the maximum number for which greater checks can happen
maxArgs = 64

#stuff to import to be able to open files in the parent directory
import os.path
import sys

print("Generating rf_xmacro_gt.h ...");
f = open(os.path.dirname(sys.argv[0]) + "/../include/Preprocessor/rf_xmacro_gt.h","w");



#put some required definitions at the top
f.write("/**\n** @author Lefteris\n** @date 13/02/2012\n**\n\
**This file contains token definition for emulating great than check with\n\
**the C preprocessor. It is automatically generated by the python script\n\
**gen_rf_xmacro_gt.py\n*/")

f.write("\n//! This is the macro from which greater than checks will actually be achieved")
f.write("\n#define RP_GT(v1_,v2_)  i_GT_##v1_##_##v2_\n\n")

#the outermost while
res = 0;
i = maxArgs
while(i >= 0):
    f.write("//! These are the tokens for greater than "+str(i)+" check\n");
    j = maxArgs ;
    #this is the individual while for which we get each equality with i
    while(j>=0):
        res =  1 if (i>j) else 0
        f.write("#define i_GT_"+str(i)+"_"+str(j)+"   "+str(res)+"\n")
        j-=1
    i-=1
    f.write("\n")
    


#at the end close the file
print("rf_xmacro_gt.h has been generated!");
f.close();
