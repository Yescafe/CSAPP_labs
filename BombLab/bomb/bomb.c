/***************************************************************************
 * Dr. Evil's Insidious Bomb, Version 1.1
 * Copyright 2011, Dr. Evil Incorporated. All rights reserved.
 *
 * LICENSE:
 *
 * Dr. Evil Incorporated (the PERPETRATOR) hereby grants you (the
 * VICTIM) explicit permission to use this bomb (the BOMB).  This is a
 * time limited license, which expires on the death of the VICTIM.
 * The PERPETRATOR takes no responsibility for damage, frustration,
 * insanity, bug-eyes, carpal-tunnel syndrome, loss of sleep, or other
 * harm to the VICTIM.  Unless the PERPETRATOR wants to take credit,
 * that is.  The VICTIM may not distribute this bomb source code to
 * any enemies of the PERPETRATOR.  No VICTIM may debug,
 * reverse-engineer, run "strings" on, decompile, decrypt, or use any
 * other technique to gain knowledge of and defuse the BOMB.  BOMB
 * proof clothing may not be worn when handling this program.  The
 * PERPETRATOR will not apologize for the PERPETRATOR's poor sense of
 * humor.  This license is null and void where the BOMB is prohibited
 * by law.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "support.h"
#include "phases.h"

/* 
 * Note to self: Remember to erase this file so my victims will have no
 * idea what is going on, and so they will all blow up in a
 * spectaculary fiendish explosion. -- Dr. Evil 
 */

FILE *infile;

int main(int argc, char *argv[])
{
    // 这个 lab 的设定其实蛮有意思的，叫我们使用反汇编、解码等各种手段，去拆除这个“炸弹”。
    // 这个 lab 的解法将全部写在这个文件的注释里，使用 objdump 反汇编的代码放在 bomb.S 里。
    char *input;

    /* Note to self: remember to port this bomb to Windows and put a 
     * fantastic GUI on it. */

    /* When run with no arguments, the bomb reads its input lines 
     * from standard input. */
    if (argc == 1) {  
	infile = stdin;
    } 

    /* When run with one argument <file>, the bomb reads from <file> 
     * until EOF, and then switches to standard input. Thus, as you 
     * defuse each phase, you can add its defusing string to <file> and
     * avoid having to retype it. */
    else if (argc == 2) {
	if (!(infile = fopen(argv[1], "r"))) {
	    printf("%s: Error: Couldn't open %s\n", argv[0], argv[1]);
	    exit(8);
	}
    }

    /* You can't call the bomb with more than 1 command line argument. */
    else {
	printf("Usage: %s [<input_file>]\n", argv[0]);
	exit(8);
    }

    /* Do all sorts of secret stuff that makes the bomb harder to defuse. */
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have 6 phases with\n");
    printf("which to blow yourself up. Have a nice day!\n");

    /* Hmm...  Six phases must be more secure than one phase! */
    input = read_line();             /* Get input                   */
    phase_1(input);                  /* Run the phase               */
    /*
     * 第一个是 phase_1 函数，它接收一个输入的字符串。
     * 查看反汇编的代码之后我们可以看到，这个函数的主体其实是调用一个叫 strings_not_equal 的函数，
     * 使用的是它的返回值。
     *
     * 转到 strings_not_equal 的逻辑部分。
     * 首先是保存从 phase_1 中传进来的 edi = 0x402400, 进入 strings_not_equal 后 rdi 先被保存到 rbp 中，
     * 先用 string_length 计算了 phase_1 的第一个参数的字符串长度，而后将 rbp 中保存的值重新取出。
     * 观察发现 string_length 中使用的寄存器是 rdi. 所以第一步操作是有必要的。
     *
     * 于是下面就比较简单了，使用 gdb 打印出 0x402400 附近的内存区域就行了。
     * 因为我还使用 gdb 打印了执行完两次 string_length 之后的 eax 和 r12d 的值，所以容易得出两个结论：
     *   1. string_length 不计算末尾的 0;
     *   2. 0x402400 处的字符串长度为 52.
     * 结果我们能得到，需要输入的字符串是：
     *   Border relations with Canada have never been better.
     *   (与加拿大的边境关系再好不过了。)
     * */
    phase_defused();                 /* Drat!  They figured it out!
				      * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");

    /* The second phase is harder.  No one will ever figure out
     * how to defuse this... */
    input = read_line();
    phase_2(input);
    /*
     * 开始 rsp 就被减掉了 40，有点吓人。然后将 rsp 拷贝到 rsi 中备用。
     * 调用 read_six_numbers 函数。这个函数里有很多 lea 指令，而且很多看不明白的地方。
     * 但是 objdump 给我们为 callq 指令添加了注释，所以我们也很容易能注意到 sscanf 的调用。
     * 到 sscanf 的调用之前，可以看到一个 4025c3 的位置，使用 x/s 打印可以看到是 6 个 %d，
     * 最后的逻辑是，sscanf 的返回值大于 5 就可以正常退出了。
     * 回到 phase_2, 依次判断这 6 个整数到底是什么。
     * 首先比较 1 和 (%rsp) 的值，如果相等跳转到后面，经过两次加载有效地址后，跳回到前面，
     * 注意这句 add %eax,%eax, 所以我直接猜测输入可能是 1 2 4 8 16 32.
     * BINGO!
     * */
    phase_defused();
    printf("That's number 2.  Keep going!\n");

    /* I guess this is too easy so far.  Some more complex code will
     * confuse people. */
    input = read_line();
    phase_3(input);
    /*
     * 看下 0x4025cf 里的内容：%d %d。看来这次是输入两个整数。
     * 这个其实看一下就会明白了，其实应该是一个 switch 语句。
     * 上面其实很多不用看的，主要是这句 jmpq   *0x402470(,%rax,8)
     * 看来这题是多解的，使用 gdb 调试，
     * 0x400f7c(,%rax,8) 是一个比例变址寻址，值应该等于 *M[0x400f7c + R[rax] * 8]
     * 然后我们读取一下对应内存的内容。
     * (gdb) x/x 0x402470
     * 0x402470:	0x00400f7c
     * (gdb) x/x 0x402478
     * 0x402478:	0x00400fb9
     * (gdb) x/x 0x402480
     * 0x402480:	0x00400f83
     * (gdb) x/x 0x402488
     * 0x402488:	0x00400f8a
     * (gdb) x/x 0x402490
     * 0x402490:	0x00400f91
     * (gdb) x/x 0x402498
     * 0x402498:	0x00400f98
     * (gdb) x/x 0x4024A0
     * 0x4024a0:	0x00400f9f
     * (gdb) x/x 0x4024A8
     * 0x4024a8:	0x00400fa6
     * 于是这题就解出来了，多解：
     * 0 207
     * 1 311
     * 2 707
     * 3 256
     * 4 389
     * 5 206
     * 6 682
     * 7 327
     * 根据之前有个 if，如果 0x8(%rsp) 大于 7u 的话就直接 explode 了，所以应该就这些了。
     * */
    phase_defused();
    printf("Halfway there!\n");

    /* Oh yeah?  Well, how good is your math?  Try on this saucy problem! */
    input = read_line();
    phase_4(input);
    /*
     * saucy a.棘手的
     * 0x4025cf: %d %d
     * 如果 0x8(%rsp) 即第一个参数小于等于 14, 则 edx=14, esi=0, edi=scanf获取到的第一个值, 调用 func4.
     * 首先理一下寄存器的值，eax=edx, eax-=esi, ecx=eax => ecx=eax=edx=14. 
     * 对 ecx 进行逻辑右移 31, 简单说就是取它的符号位，1 是负数，0 是整数。
     * 将 ecx 加回 eax, eax 还等于 14，然后对 eax 算数右移 1, 即对整型数据除以 2. 
     * 加载了 (rax,rsi,1) 的有效地址后，ecx 等于 eax 等于 7.
     * 下面就是关键了，
     *   如果 ecx 小于等于 edi：eax=0, 如果 ecx 大于等于 edi, 函数就返回，换句话说就是如果 ecx 等于 edi，就直接返回；如果不等于，edx=ecx+1, 递归调用 func4，递归完后，rax=2*rax+1.
     *   如果 ecx 大于 edi：edx=ecx-1, 比如这里是 7，然后递归调用 func4. rax=2*rax. 递归结束后，eax=0. 如果 ecx 大于等于 edi 的话，函数就会结束；反之就会到上面所说的另外一种情况的递归里去。
     * 回到 phase_4 的逻辑可以看到，如果最终能让 eax 等于 0，那么才能让阶段四继续下去。所以这就名副其实是一个棘手的数学题了。
     * 这之后的逻辑就比较简单了，scanf获取到的第二个值等于 0 就可以了。
     * 好了，看起来逻辑很混乱，但是我们会发现当最初始的 ecx 就等于 edi 的时候，func4 就能直接以 eax=0 结束，测试之后也确实是这样。
     * 不过，按照代码的逻辑来看，这题很有可能是在 0~14 之间的二分搜索。
     * 经过测试，0, 1, 3, 7, 是可以的。这里面递归有点乱，原因不知道。
     * */
    phase_defused();
    printf("So you got that one.  Try this one.\n");
    
    /* Round and 'round in memory we go, where we stop, the bomb blows! */
    input = read_line();
    phase_5(input);
    /*
     * 最上面一段代码看的不是太懂，稍微往下一点是将 eax=0, 判断字符串长度等不等于 6.
     * edi 是指向参数字符串的指针。
     * 40108b 的代码是将 edi 的第一个字符的 ASCII 码读进 ecx, 然后存进 edx 再与与 0xf 求与运算，
     * 再将 0x4024b0+(%edx) 位置的值，读进 edx.
     * 读取看了一下 0x4024b0 位置后面的字符是 maduiersnfotvbyl
     * 后面一段看不太懂了，但是明白了 eax 是作为循环的计数器用的，整个算法应该是对这个字符串的 6 个字符，
     * 使用了某种方法进行加密。
     * 循环结束之后，与字符串 flyers 比较是否相等。到这里就大概明白了与 0xf 做与运算的作用，是取到字符的最后一个十六进制位，
     * 正好相当于在上面那个 maduiersnfotvbyl 不知名字符串的索引。
     * 于是要凑一个 0x9 0xf 0xe 0x5 0x6 0x7 为 ASCII 尾数的字符串就行了。因为我们知道 A 的 ASCII 码是 0x41，所以很容易推算出一个序列：IONEFG 当然，这个结果也不唯一，只是用字母比较方便。
     * */
    phase_defused();
    printf("Good work!  On to the next...\n");

    /* This phase will never be used, since no one will get past the
     * earlier ones.  But just in case, make this one extra hard. */
    input = read_line();
    phase_6(input);
    /*
     * 阶段 6 的代码非常非常非常多，好在只有一个函数，好像也没有递归啥的，要不然真的劝退了。
     * 
     * */
    phase_defused();

    /* Wow, they got it!  But isn't something... missing?  Perhaps
     * something they overlooked?  Mua ha ha ha ha! */
    
    return 0;
}
