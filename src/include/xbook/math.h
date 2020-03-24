#ifndef _XBOOK_MATH_H
#define _XBOOK_MATH_H

/* max() & min() */
#define	MAX(a,b)	((a) > (b) ? (a) : (b))
#define	MIN(a,b)	((a) < (b) ? (a) : (b))

/* 除后上入 */
#define DIV_ROUND_UP(X, STEP) ((X + STEP - 1) / (STEP))

/* 除后下舍 */
#define DIV_ROUND_DOWN(X, STEP) ((X) / (STEP))

#define ALIGN_WITH(x, y) ((x + (y - 1)) & (~(y - 1)))

/* 判断一个数是否为2的次幂 */
#define is_power_of_2(n) (n != 0 && ((n & (n - 1)) == 0)) 

static inline long fls(long x)
{
    long position;
    long i;
    if (0 != x) {
        for (i = (x >> 1), position = 0; i != 0; ++position)
            i >>= 1;
    } else
        position = -1;
    return position + 1;
}

static inline unsigned long roundup_pow_of_two(unsigned long x)
{
    return 1UL << fls(x - 1);
}

#endif /* _XBOOK_MATH_H */
