my a = "   a";
my b = "b   ";
print a + b;
try {
    if (my x = b + b) {
        print trim x;
    }
    print "OK";
} catch () {
    print "EXC";
}
