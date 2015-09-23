my s;
s = "    abc    ";
{
    my s = "  x  ";
    my a = s;
    trim s;
    print "s: *" + s + "*";
    print "a: *" + a + "*";
}
print "s: *" + s + "*";
