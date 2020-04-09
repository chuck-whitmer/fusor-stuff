$fn=50;

function RotateOnAxis(x,axis,angle) =
    sin(angle)*cross(axis,x) + cos(angle)*(x-axis*(axis*x)) + axis*(axis*x);

showShape = false;

parallelWireOffset = 0.08*([0,1,1]-[-1,0,1]);
shapeEdge = 22;

// A list of vertices shows the order in which we traverse the edges
// of the embedded shape.

vlist = shapeEdge/sqrt(2)*[
[-6.1,0,6.1],
[-6,0,6],   // The long stem
[-1,0,1],
[0,-1,1],
[1,0,1],
[1,-1,0],
[1,0,-1],
[0,-1,-1],
[-1,0,-1],
[-1,-1,0],
[0,-1,-1], 
[1,-1,0],  
[0,-1,1], 
[-1,-1,0],
[-1,0,1],  //<--- Reflection symmetry here
[-1,1,0],
[0,1,1],  
[1,1,0],
[0,1,-1], 
[-1,1,0],
[-1,0,-1], 
[0,1,-1],
[1,0,-1], 
[1,1,0],
[1,0,1],
[0,1,1],
[-1,0,1] + parallelWireOffset,
[-3,0,3]+ parallelWireOffset,   // The short stem
[-3.1,0,3.1] + parallelWireOffset
];

g = 0.7;  // Wire gauge, radius.
r = 4.0;  // Radius of wire bend.
ra = 2.0;  // For acute angles.

rotate([0,45,0])
{
    // This is optional and displays an embedded trucated cube.
    // We do NOT have the ability to reconstruct the polyhedron from 
    // the vertex list. If you make a new vertex list, just delete this.
    if (showShape)
    {
        difference()
        {
            s = shapeEdge*sqrt(2)*0.95;
            cube(s,center=true);
            for (i=[0,90,180,270])
            rotate([0,0,i])
            translate(2/3*[s,s,s]/2)
            rotate([0,-acos(sqrt(2)/sqrt(3)),45])
            translate([0,-12.5,-12.5])
            cube(s);
            for (i=[0,90,180,270])
            rotate([0,0,i])
            translate(2/3*[s,s,-s]/2)
            rotate([0,acos(sqrt(2)/sqrt(3)),45])
            translate([0,-s/2,-s/2])
            cube(s);
        }
    }

    // This loop draws the wires and bends from the vertex list.
    for (i=[1:len(vlist)-3])
    {
        d0 = vlist[i] - vlist[i-1];
        d1 = vlist[i+1] - vlist[i];
        d2 = vlist[i+2] - vlist[i+1];
        n0 = d0 / norm(d0);
        len1 = norm(d1);
        n1 = d1 / len1;
        n2 = d2 / norm(d2);
        angle0 = acos(min(n0*n1,1));
        r0 = (angle0 > 100) ? ra : r;
        angle2 = acos(n2*n1);
        r2 = (angle2 > 100) ? ra : r;
        cut0 = r0 * tan(angle0/2);
        cut2 = r2 * tan(angle2/2);
        edgeLen = len1-cut0-cut2;
        axis = cross([0,0,1],n1);
        angle = (n1[2]>0) ? asin(norm(axis)) : 180-asin(norm(axis));
        n2prime = RotateOnAxis(n2,axis/norm(axis),-angle);
        phi = atan2(n2prime[1],n2prime[0]) - 180;
        color("silver")
        translate(vlist[i]+cut0*n1)
        rotate(angle,axis)
        {
            sphere(g);
            cylinder(r=g,h=edgeLen);
            translate([0,0,edgeLen])
            {
                sphere(g);
                rotate([90,0,phi])
                translate([-r2,0])
                rotate_extrude(angle=angle2)
                translate([r2,0])
                circle(g);
            }      
        }
    }
}
