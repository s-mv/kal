# kal - because I forget the spelling of 'kaleidoscope'

**kal** is a revolutionary new language that has shaken the roots of
programming by introducing new, immensely beneficial paradigms through its
syntax.

## Build and run

This should work. If it doesn't, question your past life choices and fix it.

```bash
cmake -B build -G "Ninja"
ninja build
./build/kal
```

## Example usage

```
~/temp/kal δ ./build/kal
herro wolld
ready> fn a() 1;
function def:
define double @a() {
entry:
  ret double 1.000000e+00
}

ready> fn b(x) x + 2;
ready> function def:
define double @b(double %x) {
entry:
  %addtmp = fadd double %x, 2.000000e+00
  ret double %addtmp
}

ready> a() + b(a());
ready> top-level expression ir:
define double @__sus() {
entry:
  %calltmp = call double @a()
  %calltmp1 = call double @a()
  %calltmp2 = call double @b(double %calltmp1)
  %addtmp = fadd double %calltmp, %calltmp2
  ret double %addtmp
}
```

Ignore the minor errors in presenting the output. The IR itself is IMMACULATE.
Unequivocally perfect.

## Bye

That's it for now.

Goodbye friend.
