
module Plate(){ color("grey")
        translate([-0.5, -0.5, -0.2]) cube([w0+1,l0+1, 0.2]);
}


module Walls2(){
    len0 = PowerCornerLen;
    translate([len0,0,0]) cube([w0-len0, wall, h0]);
    translate([0,len0,0]) cube([wall,l0-len0,h0]);
    translate([0, l0-wall, 0]) cube([w0-ToggleCornerLen, wall, h0]);
    translate([w0-wall, 0, 0]) cube([wall, l0-ToggleCornerLen, h0]);
    
    add0 = max( 0,(ThrottleDepth-wall)+0.6);
    thickbar = ThrottleSocketWidth+ThrottleLowering;
    translate([wall, l0-(thickbar+wall), 0]) cube([add0,thickbar,h0]);
    
    add1 = max( 0,(ToggleDepth-wall)+0.6);
    translate([w0-ToggleCornerLen, l0, 0.0]) rotate(180+45) cube([wall+add1, sqrt(2)*ToggleCornerLen, h0]);
    
    translate([0,len0,0]) rotate(-45) cube([len0*sqrt(2),wall,h0]);
}



module ToggleSocket() {
                 translate([0,1.5-ToggleDepth,0]) cube([ToggleSocketWidth,3,h0+1], center= true);
    color("red") translate([0,0-ToggleDepth,0]) cube([1.4,2,1.4], center = true);
}


module ThrottleSocket() {
    wid = ThrottleSocketWidth;
    spare = wid-(1.4*2);
    gap = spare/3;
    h = h0/2 - 1.4/2;
    color("red") translate([-1, gap, h]) cube([ThrottleDepth+1+wall+0.1, 1.4, 1.4]);
    color("red") translate([-1, gap+1.4+gap, h]) cube([ThrottleDepth+1+wall+0.1, 1.4, 1.4]);
    color("green") translate([-1, 0, -h0/2]) cube([ThrottleDepth+1, ThrottleSocketWidth , h0*2]);
}


module StrapClips() {
    translate([w0-(ToggleCornerLen+StrapWidth+0.2), l0-wall*5+0.1, h0-StrapH]) cube([StrapWidth, wall*5, StrapH+0.02]);
    translate([PowerCornerLen+0.7, -0.1, h0-StrapH]) cube([StrapWidth, wall+0.5, StrapH+0.02]);
}

module LedSocket() {
    r = ledRadius1;
    r2 = ledRadius2;
    dd = 0.1;
    l = wall*2;
    l2 = l-(0.4+dd);
    translate([0,-l+dd,0]) rotate([-90,90]) cylinder(l,r,r, $fn=35);
    translate([0,-l+dd,0]) rotate([-90,90]) cylinder(l2,r2,r2, $fn=35);
    //translate([0,-0.4,h0*0.65]) rotate([-90,90]) cylinder(0.4,r2/2,r2/2, $fn=25);
    
}

module ScrewHole() {
    translate([0,0,-0.05]) cylinder(h0+0.1, 0.94/2, 0.94/2, $fn = 6);
}




module Final() {
    difference(){
        Walls2();
        translate([w0-(ToggleCornerLen/2), l0-(ToggleCornerLen/2), h0/2]) rotate(-45) ToggleSocket();
        translate([0,l0-(ThrottleSocketWidth+ThrottleLowering),0]) ThrottleSocket();
        r = PowerButtonDiameter/2;
        l = PowerCornerLen*sqrt(2);
        translate([0,0,h0/2]) rotate([-45,90]) cylinder(l,r,r, $fn=45);
        StrapClips();
        translate([w0-(ToggleCornerLen+2), l0, (h0/12)*6]) LedSocket();
        translate([w0-(ToggleCornerLen+3), l0, (h0/12)*6]) LedSocket();
        translate([w0-(ToggleCornerLen+4), l0, (h0/12)*6]) LedSocket();
        translate([w0-wall,l0-(wall+0.4+ToggleCornerLen),0]) ScrewHole();
        //translate([w0-wall*1.2,wall*1.2,0]) ScrewHole();
        translate([wall+0.67,l0-wall-0.1,0]) ScrewHole();
        translate([w0*0.5,wall*1.1,0]) ScrewHole();
        
    }
    
}


wall = 0.75;
ToggleCornerLen = 2.4;
PowerCornerLen = 1.8;
h0 = 2.2;
w0 = 8.65;//8.3
l0 = 15;//14.25

ToggleDepth= 1.1;
ToggleSocketWidth = 2.05;

ThrottleDepth = 0.86;
ThrottleSocketWidth = 4.3;
ThrottleLowering = 1.4;

PowerButtonDiameter = 1.8;

ledRadius1 = 0.5/2;
ledRadius2 = 0.6/2;

StrapWidth = 2.9;
StrapH = 0.072;


//color("SlateGrey") Plate();

color([1.0,0.2,0.0]) Final();
//LedSocket();



