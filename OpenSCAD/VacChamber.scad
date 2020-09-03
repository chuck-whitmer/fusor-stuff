$fn = 100;
in = 25.4;

chamberOD = 6.5*in;
chamberID = 6.0*in;

chamberOuterHeight = 6.75*in;
chamberInnerHeight = 6.5*in;  // No top

flangeOD = 200;
t = 0.25*in;
passThroughOD = 0.75*in;

showCutaway = false;

module chamber()
{
    translate([0,0,-t-chamberInnerHeight/2])
    difference()
    {
        union()
        {
            cylinder(r=chamberOD/2,h=chamberOuterHeight);
            translate([0,0,chamberInnerHeight])
            cylinder(r=flangeOD/2,h=2*t);
        }
        translate([0,0,t])
        cylinder(r=chamberID/2,h=chamberInnerHeight);
        translate([0,0,chamberInnerHeight])
        cylinder(r=passThroughOD/2,h=3*t);
    }
}

if (showCutaway)
{
    intersection()
    {
        s = 2*chamberOuterHeight;
        translate([-s,-s/2,-s/2])
        cube(s);
        chamber();
    }
}
else
{
    chamber();
}