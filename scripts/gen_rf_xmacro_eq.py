#the maximum number for which equality checks can happen
#the maximum number of arguments (int) for which equality should work
maxArgs = 64


#stuff to import to be able to open files in the parent directory
import os.path
import sys

print("Generating rf_xmacro_eq.h ...");
f = open(os.path.dirname(sys.argv[0]) + "/../include/Preprocessor/rf_xmacro_eq.h","w");


#put some required definitions at the top
f.write("/**\n** @author Lefteris\n** @date 13/02/2012\n**\n\
**This file contains token definition for emulating equality check with\n\
**the C preprocessor. It is automatically generated by the python script\n\
**gen_rf_xmacro_eq.py\n*/")

f.write("\n//! This is the macro from which equality checks will actually be achieved")
f.write("\n#define RP_EQ(v1__,v2__)  i_EQ_##v1__##_##v2__\n\n")

#the outermost while
res = 0;
i = maxArgs;
while(i > 0):
    f.write("//! These are the tokens for equality with "+str(i)+"\n");
    j = maxArgs;
    #this is the individual while for which we get each equality with i
    while(j>=0):
        res =  0 if (i!=j) else 1
        f.write("#define i_EQ_"+str(i)+"_"+str(j)+"   "+str(res)+"\n")
        j-=1
    i-=1
    f.write("\n")
    


#at the end close the file
print("rf_xmacro_eq.h has been generated!");
f.close();
