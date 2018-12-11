//
// Created by marvinle on 2018/12/11 9:44 AM.
//

#define SIZE  256
#include <cstring>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>

using namespace std;

/**
 * 生成坏字符表
 * @param b
 * @param len
 * @param bc
 */
void generateBC(char *b, int len, int *bc) {
    memset(bc, 0, SIZE);
    // 简单hash
    for(int i = 0; i < len; i++){
        bc[(int)b[i]] = i;
    }
}


void generateGS(char *b, int m, int *suffix, bool *prefix) {
    for(int i = 0; i < m; i++) {
        suffix[i] = -1;
        prefix[i] = false;
    }

    for(int i = 0; i < m-1; i++){

        int j = i;
        int k = 0;
        while(j >= 0 && b[j] == b[m-k-1]){
            --j;
            ++k;
            suffix[k] = j+1;        // 和长度为k的后缀匹配的起始位置为 j+1
        }
        // 如果j小于0，也就是移动到了头，那么后缀也是前缀的字串了。
        if(j < 0){
            prefix[k] = true;
        }
    }
}

/**
 *  只使用坏字符规则
 * @param a  主串
 * @param n  主串长度
 * @param b  模式串
 * @param m  模式串长度
 * @return
 */
int bm_v1(char *a, char *b) {
    int m = strlen(b);
    int n = strlen(a);
    if(m > n)
        return -1;

    int *bc = new int[SIZE];
    generateBC(b, m, bc);
    int i = 0;
    while(i <= n-m){
        int j;
        for(j = m-1; j >=0; j--){
            if(a[j+i] != b[j])
                break;
        }
        if(j < 0){
            delete[] bc;
            return i;
        }
        // 查bc 表移动
        i += (j-(bc[(int)a[j+i]]));
    }
    delete[] bc;
    return -1;
}

int moveByGS(int j, int m, int *suffix, bool *prefix) {
    int k = m - 1 -j;   //好后缀长度

    if(suffix[k] != -1)
        return j-suffix[k]+1;

    // j+1 表示号好后缀的首字符， j+2表示好后缀的最长后缀字串的首字符
     for(int r = j + 2; r <= m-1; r++){
         if(prefix[m-r])
             return r;
     }
     return m;
}

/**
 * 完整版本bm
 * @param a
 * @param n
 * @param b
 * @param m
 * @return
 */
int bm(char *a, char *b) {
    int m = strlen(b);
    int n = strlen(a);
    if(m > n)
        return -1;

    int *bc = new int[SIZE];
    generateBC(b, m, bc);
    int *suffix = new int[m];
    bool *prefix = new bool[m];
    generateGS(b, m, suffix, prefix);
    int move_worst, move_best;


    int i = 0;
    while(i <= n-m){
        int j;
        for(j = m-1; j >=0; j--){
            if(a[j+i] != b[j])
                break;
        }
        if(j < 0){
            delete[] suffix;
            delete[] prefix;
            delete[] bc;
            return i;
        }
        // 查bc 表移动
        move_worst =  j-(bc[(int)a[j+i]]);
        move_best = 0;
        // j= m-1说明没有好后缀
        if(j < m-1){
            move_best = moveByGS(j, m, suffix, prefix);
        }
        i += max(move_best, move_worst);
    }
    delete[] suffix;
    delete[] prefix;
    delete[] bc;
    return -1;
}


/**
 * 暴力
 * @param main
 * @param reg
 * @return
 */
int bf(char* main, char* reg) {
    int n = strlen(main);
    int m = strlen(reg);

    if(m > n)
        return -1;

    for(int i = 0; i <= n - m ; i++){
        int j;
        for(j = 0; j < m; j++){
            if(main[j+i] != reg[j]){
                break;
            }
        }
        if(j == m){
            return i;
        }
    }
    return -1;
}

/**
 * KMP next数组
 * @param p
 * @param len
 * @return
 */
int* getNext(char *p , int len)
{
    int *next = (int*)malloc(sizeof(int) * len);
    *(next) = -1;
    int k = -1;
    int j = 0;
    while(j < len - 1)
    {
        if(k == -1 || *(p+j) == *(p+k))
        {
            next[++j] = ++k;
        } else {
            k = *(next+k);
        }
    }
    return next;
}

/**
 * KMP
 * @param ts
 * @param ps
 * @return
 */
int KMP(char *ts, char *ps)
{
    int j = 0;  // 模式串位置
    int i = 0;  // 主串位置

    int pl = strlen(ps);
    int tl = strlen(ts);
    if(tl <= 0 || pl <= 0)
    {
        return -1;
    }
    int  *next = getNext(ps, pl);

    while(i < tl && j < pl)
    {
        // 1. 如果j已经退到字符串首了，还是不匹配当前i，就要移动i了 ||  2. i和j匹配也要一起移动
        if(j == -1 || *(ts+i) == *(ps+j))
        {
            i++;  // 不管哪种情况i始终要移动
            j++;  // 第一种情况要将j归1，第二种情况j要增长
        } else {  //否则需要j回朔到next[j]
            j = *(next+j);
        }
    }
    free(next);
    if(j == pl) //匹配完全
    {
        return i-pl;
    } else {
        return -1;
    }
}


/**
 * 测试函数
 * @param fun
 * @param ts
 * @param ps
 */

void test(char *funname, int (*fun)(char*,char*), char *ts, char *ps)
{
    clock_t start, end;
    start = clock();
    int i = fun(ts, ps);
    end = clock();
    printf("function %s index=%d\t%f 秒\n",funname,i,(double)(end-start)/CLOCKS_PER_SEC);
}



/**
 * 获取随机字符串
 * @param len
 * @param isRandLen
 * @return
 */
char* getRandString(int *len, int isRandLen)
{
    srand((unsigned int)time(NULL));
    int tlen = 0;
    // 是否使用随机长度
    if(isRandLen){
        tlen  = rand();
        *len = tlen;
    } else {
        tlen = *len;
    }
    char *str = (char*)malloc((tlen+1)*sizeof(char));
    for(int i = 0; i < tlen; i++)
    {
        int c = rand()%26;
        *(str+i) = c+97;
    }
    *(str+tlen) = '\0';
    return str;
}

int main(){
    int tl = 0;
    int pl = 100000;
    char *ts = getRandString(&tl, 1);
    char *ps = getRandString(&pl, 0);

    printf("主串长度: %d\n", tl);
    printf("模式串长度: %d\n", pl);
    printf("开始测试查找\n");

    test("bm_a", bm, ts, ps);
    test("bf", bf, ts, ps);
    test("kmp", KMP, ts, ps);
    free(ts);
    free(ps);
    return 0;
}
