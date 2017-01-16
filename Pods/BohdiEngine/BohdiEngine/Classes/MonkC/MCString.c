#include "MCString.h"
#include "MCMath.h"

utility(MCString, size_t, replace, const char* str, const char* withstr, const char* instr, char (*buff)[])
{
    size_t count = strlen(str);
    size_t wcount = strlen(withstr);
    
    int i=0, b=0, o=0;
    while (instr[i] != NUL) {
        while (instr[i+o] != NUL && instr[i+o] == str[0+o]) {
            o++;
        }
        if (o == (int)count) {
            for (int k=0; k<wcount; k++) {
                (*buff)[b++] = withstr[k];
            }
            i += o;
        }
        o = 0;
        (*buff)[b++] = instr[i++];
    }
    (*buff)[b++] = NUL;
    
    return b;
}

utility(MCString, size_t, reverse, const char* str, char (*buff)[])
{
    size_t count = strlen(str);
    char* c = (char*)&str[count-1];
    for (int i=0; i<count; i++) {
        (*buff)[i] = *c;
        c--;
    }
    (*buff)[count] = NUL;
    
    return count;
}

utility(MCString, const char*, baseFromPath, const char* path, char (*buff)[])
{
    char reversebuff[PATH_MAX];
    size_t count = MCString_reverse(path, &reversebuff);
    
    char* head = &reversebuff[count-1];
    char* tail = &reversebuff[0];
    while (*tail != '/') {
        tail++;
    }
    
    int i=0;
    while (head != tail) {
        (*buff)[i++] = *head;
        head--;
    }
    (*buff)[i] = NUL;
    
    return &(*buff)[0];
}

utility(MCString, const char*, filenameFromPath, const char* path, char (*buff)[])
{
    char reversebuff[PATH_MAX];
    MCString_reverse(path, &reversebuff);
    
    char* head = &reversebuff[0];
    char* tail = &reversebuff[0];
    while (*head != '/') {
        head++;
    }
    head--;
    
    int i=0;
    while (head != tail) {
        (*buff)[i++] = *head;
        head--;
    }
    (*buff)[i] = *tail;
    
    return &(*buff)[0];
}

utility(MCString, const char*, filenameTrimExtension, const char* name, char* buff)
{
    int i=0;
    while (*name != '.' && *name != NUL) {
        buff[i++] = *name;
        name++;
    }
    buff[i] = NUL;
    return buff;
}

utility(MCString, const char*, extensionFromFilename, const char* name, char (*buff)[])
{
    while (*name != '.' && *name != NUL) name++;
    if (*name == NUL) {
        return null;
    }else{
        name++;//skip dot
        int i=0;
        while (*name != NUL) {
            (*buff)[i++] = *name;
            name++;
        }
        (*buff)[i] = NUL;
        
        return &(*buff)[0];
    }
}

utility(MCString, const char*, concate, const char** strings, size_t count, char (*buff)[])
{
    strcpy(*buff, strings[0]);
    for (int i=1; i<count; i++) {
        strcat(*buff, strings[i]);
    }
    return *buff;
}

utility(MCString, const char*, concateWith, const char* sp, const char* path1, const char* path2, char (*buff)[])
{
    return MCString_concate((const char* []){
        path1, sp, path2
    }, 3, buff);
}

utility(MCString, const char*, concatePath, const char* path1, const char* path2, char (*buff)[])
{
    return MCString_concateWith("/", path1, path2, buff);
}

utility(MCString, const char*, compressToCharCount, const char* source, char* buff)
{
    //assume buff is large enough
    if (source && buff) {
        //first char
        int cur = 0;
        buff[cur++] = *source;
        char last   = *source;
        
        char digits[LINE_MAX];
        size_t count = 1;
        
        for (int i=1; i<strlen(source); i++) {
            if(source[i] != last) {
                if (count > 1) {
                    sprintf(digits, "%ld", count);
                    for (int d=0; d<strlen(digits); d++)
                        buff[cur++] = digits[d];
                    count = 0;
                }
                buff[cur++] = source[i];
                last = source[i];
            } else {
                if (count > LINE_MAX)
                    count = LINE_MAX;
                else
                    count++;
            }
        }
        //end char
        buff[cur] = NUL;
    }
    return buff;
}

utility(MCString, const char*, extractFromCharCount, const char* source, char* buff)
{
    if (source && buff) {
        int cur = 0; int count=0; char last = NUL;
        char digits[LINE_MAX];

        for (int i=0; i<strlen(source); i++) {
            char c = source[i];
            if (c >= '0' && c <= '9') {
                digits[count++] = c;
                continue;
            } else {
                if (count > LINE_MAX)
                    count = LINE_MAX;
                digits[count] = NUL;
                if (count > 0) {
                    int number = atoi(digits);
                    for(int d=0; d<number; d++)
                        buff[cur++] = last;
                    count=0;
                }
                buff[cur++] = c;
                last = c;
            }
        }
        //end char
        buff[cur] = NUL;
    }
    return buff;
}

static void swap(char* a, char* b)
{
    if (a != b) {
        char temp = *a;
        *a = *b;
        *b = temp;
    }
}

static void permutationOf(char str[], int index)
{
    if (index == strlen(str)) {
        printf("%s\n", str);
        return;
    }
    for (int i=index; i<strlen(str); i++) {
        swap(&str[index], &str[i]);
        permutationOf(str, index+1);
        swap(&str[index], &str[i]);
    }
}

utility(MCString, void, printPermutationOf, char str[])
{
    char buff[LINE_MAX];
    strcpy(buff, str);
    buff[strlen(str)] = NUL;
    permutationOf(buff, 0);
}

#define MCStringBlock LINE_MAX

oninit(MCString)
{
    if (init(MCObject)) {
        //nothing to init
        obj->buff = malloc(MCStringBlock * sizeof(char));
        return obj;
    }else{
        return null;
    }
}

method(MCString, void, bye, voida)
{
    //debug_log("MCString bye");
    free(obj->buff);
}

method(MCString, MCString*, initWithCString, const char* str)
{
    size_t len = strlen(str);
    if (len >= MCStringBlock) {
        free(obj->buff);
        obj->buff = malloc(len*sizeof(char));
    }
    
    obj->length = strlen(str);
	obj->size = strlen(str) + 1;
    strcpy(obj->buff, str);
	return obj;
}

MCString* MCString_newWithCString(const char* cstr)
{
    return MCString_initWithCString(0, new(MCString), cstr);
}

MCString* MCString_newWithMCString(MCString* mcstr)
{
    return MCString_initWithCString(0, new(MCString), mcstr->buff);
}

MCString* MCString_newForHttp(char* cstr, int isHttps)
{
	MCString* res;
	if (isHttps)
        res = MCString_newWithCString("https://");
	else
		res = MCString_newWithCString("http://");
    MCString_add(0, res, cstr);
	return res;
}

static char get_one_char()
{
    char cf = NUL;
    while(!isNewLine(&cf)) {
        cf = getchar();
    }//clear the buff
	return cf;
}

static void get_chars_until_enter(char resultString[])
{
	char tc = NUL;
	int i=0;
	while(!isNewLine(&tc)){
		resultString[i]=tc;
		i++;
	}
	resultString[i]=NUL;
}

method(MCString, void, add, char* str)
{
    if (MCStringBlock-obj->size < strlen(str)+1) {
        char* newbuff = malloc(sizeof(char) * (obj->size + MCStringBlock));
        strncpy(newbuff, obj->buff, obj->size-1);
        newbuff[obj->size-1]=NUL;
        free(obj->buff);
        obj->buff = newbuff;
        obj->size = obj->size + MCStringBlock;
    }
    strncat(obj->buff, str, strlen(str));
}

method(MCString, void, print, MCBool withNewline)
{
    if (withNewline)
        debug_log("%s\n", obj->buff);
    else
        debug_log("%s", obj->buff);
}

method(MCString, const char*, toCString, char const buff[])
{
	strcpy(cast(char*, buff), obj->buff);
	return buff;
}

method(MCString, int, equalTo, MCString* stringToComp)
{
	int res;
	res = strcmp(obj->buff, stringToComp->buff);
	if (res==0)
		return 1;
	else
		return 0;
}

method(MCString, char, getOneChar, voida)
{
	return get_one_char();
}

method(MCString, void, getCharsUntilEnter, char resultString[])
{
	get_chars_until_enter(resultString);
}

method(MCString, MCBool, startWith, const char* str)
{
    size_t len = strlen(str);
    if (len > obj->length) {
        return false;
    }else{
        if (strncmp(obj->buff, str, len) == 0) {
            return true;
        }else{
            return false;
        }
    }
}

method(MCString, double, toDoubleValue, char** endptr)
{
    return strtod(obj->buff, endptr);
}

method(MCString, MCString*, copyCompressedString, voida)
{
    MCString* string = new(MCString);
    MCString_compressToCharCount(obj->buff, string->buff);
    return string;
}

method(MCString, MCString*, copyExtractedString, voida)
{
    MCString* string = new(MCString);
    MCString_extractFromCharCount(obj->buff, string->buff);
    return string;
}

onload(MCString)
{
    if (load(MCObject)) {
        binding(MCString, MCString*, initWithCString, char* str);
        binding(MCString, void, add, char* str);
        binding(MCString, void, print, MCBool withNewline);
        binding(MCString, char*, toCString, char const buff[]);
        binding(MCString, int, equalTo, MCString* stringToComp);
        binding(MCString, char, getOneChar);
        binding(MCString, void, getCharsUntilEnter, char const resultString[]);
        binding(MCString, void, bye);
        binding(MCString, MCBool, startWith, const char* str);
        binding(MCString, double, toDoubleValue, char** endptr);
        binding(MCString, MCString*, copyCompressedString, voida);
        binding(MCString, MCString*, copyExtractedString, voida);
        return cla;
    }else{
        return null;
    }
}
