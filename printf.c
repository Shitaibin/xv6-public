#include "types.h"
#include "stat.h"
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
/**
 * How: Transform int into character array, but in
 * reverse order, add sign if need, to the tail,
 * and then write it into `fd`.
 *
 * Params:
 * fd: file description
 * xx: real value
 * base: d, o, x, p
 * sgn: signed or unsigned
 */
printint(int fd, int xx, int base, int sgn)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16]; /* int take up 2 bytes */
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putc(fd, buf[i]);
}

// Print to the given fd. Only understands %d, %x, %p, %s.
// Exercise 8: add %o.
void
printf(int fd, char *fmt, ...)
{
  char *s;
  /* c: current character in `fmt` */
  /* state: meet `%` or not */
  int c, i, state;
  /* ap: the address of the real values. why int ????? */
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1; /* why? */
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;  /* get current */
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);   /* case: "abc..." */
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++; /* move ap to next place */
      } else if(c == 'o'){
        printint(fd, *ap, 8, 1);  /* set `*ap` is a signed octal number */
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
        putc(fd, c);  /* "%%" */
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;  // reset state
    }
  }
}
